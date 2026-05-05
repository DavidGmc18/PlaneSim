#pragma once

#include <vector>
#include "Mesh.hpp"
#include <string>
#include <glad/glad.h>
#include "TextureCache.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// TODO optimize materials to share, rather than one per mesh
class Model {
    public:
    std::vector<Mesh> opaque_meshes;
    std::vector<Mesh> transparent_meshes;
    std::string directory;

public:
    Model(std::string path, TextureCache& cache, bool flip_UVs = false) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | (flip_UVs * aiProcess_FlipUVs) | aiProcess_CalcTangentSpace); 

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene, cache);
    }

    void drawOpaque(GLuint shader, glm::mat4& model) {
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        for (const Mesh& mesh : opaque_meshes) {
            mesh.render(shader, model, normalMatrix);
        }
    }

    void drawTransparent(GLuint shader, glm::mat4& model) {
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        for (const Mesh& mesh : transparent_meshes) {
            mesh.render(shader, model, normalMatrix);
        }
    }

private:
    void processNode(aiNode *node, const aiScene *scene, TextureCache& cache) {
        // Process all node meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *aiMesh = scene->mMeshes[node->mMeshes[i]];
            Mesh mesh = processMesh(aiMesh, scene, cache);
            if (mesh.material.opacity < 1.0f) {
                transparent_meshes.push_back(std::move(mesh));
            } else {
                opaque_meshes.push_back(std::move(mesh));
            }			
        }

        // Process all children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, cache);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene, TextureCache& cache) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Material material;

        // std::cout << mesh->mName.C_Str() << '\n';

        // vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
           
            vertex.pos.x = mesh->mVertices[i].x;
            vertex.pos.y = mesh->mVertices[i].y;
            vertex.pos.z = mesh->mVertices[i].z;

            vertex.norm.x = mesh->mNormals[i].x;
            vertex.norm.y = mesh->mNormals[i].y;
            vertex.norm.z = mesh->mNormals[i].z;

            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vertex.uv.x = mesh->mTextureCoords[0][i].x; 
                vertex.uv.y = mesh->mTextureCoords[0][i].y;
            } else {
                vertex.uv = glm::vec2(0.0f, 0.0f);
            }

            if (mesh->mTangents) {
                glm::vec3 T = { mesh->mTangents[i].x,  mesh->mTangents[i].y,  mesh->mTangents[i].z };
                glm::vec3 B = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
                glm::vec3 N = vertex.norm;
                float w = glm::dot(glm::cross(N, T), B) < 0.0f ? -1.0f : 1.0f;
                vertex.tan = glm::vec4(T, w);
            } // TODO fallback

            vertices.push_back(vertex);
        }

        // Indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Materials
        if (mesh->mMaterialIndex >= 0) {
            material = loadMaterial(scene->mMaterials[mesh->mMaterialIndex], cache);
        }

        return Mesh(vertices, indices, material);
    }

    Material loadMaterial(aiMaterial* mat, TextureCache& cache) {
        Material material;

        material.diffuse = getTexture(mat, cache, aiTextureType_DIFFUSE);
        if (!material.diffuse) {
            glm::vec3 color(0, 0, 0);
            aiColor3D ai_color;
            if (AI_SUCCESS ==  mat->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color)) {
                color.r = ai_color.r;
                color.g = ai_color.g;
                color.b = ai_color.b;
            }
            material.diffuse = cache.getColor(color);
        }

        material.specular = getTexture(mat, cache, aiTextureType_SPECULAR);
        if (!material.specular) {
            glm::vec3 color(0, 0, 0);
            aiColor3D ai_color;
            if (AI_SUCCESS ==  mat->Get(AI_MATKEY_COLOR_SPECULAR, ai_color)) {
                color.r = ai_color.r;
                color.g = ai_color.g;
                color.b = ai_color.b;
            }
            material.specular = cache.getColor(color);
        }

        material.normal = getTexture(mat, cache, aiTextureType_NORMALS);
        if (!material.normal) {
            material.normal =cache.getColor(glm::vec3(0.5f, 0.5f, 1.0f));
        }

        material.shininess = getTexture(mat, cache, aiTextureType_SHININESS);
        if (!material.shininess) {
            glm::vec3 color(0, 0, 0);
            float ai_float;
            if (AI_SUCCESS ==  mat->Get(AI_MATKEY_SHININESS, ai_float)) {
                color.r = ai_float;
                color.g = ai_float;
                color.b = ai_float;
            }
            material.shininess = cache.getColor(color);
        }

        float opacity = 1.0f;
        float transparencyFactor = 0.0f;
        mat->Get(AI_MATKEY_OPACITY, opacity);
        mat->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparencyFactor);
        material.opacity = opacity * (1.0f - transparencyFactor);

        return material;
    }

    GLuint getTexture(aiMaterial* mat, TextureCache& cache, aiTextureType type) {
        if (mat->GetTextureCount(type) == 0) {
            std::cout << "Found no " << type << " texture for material!\n";
            return 0;
        }

        if (mat->GetTextureCount(type) > 1)
            std::cout << "Found more than one " << type << " texture for material!\n";

        aiString str;
        mat->GetTexture(type, 0, &str);
        std::string path = directory + '/' + std::string(str.C_Str());
        return cache.get(path);
    }
};