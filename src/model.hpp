#pragma once

#include <vector>
#include "mesh.hpp"
#include <string>
#include <glad/glad.h>
#include "TextureCache.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// TODO optimize materials to share, rather than one per mesh
class Model {
    public:
    std::vector<Mesh> meshes;
    std::string directory;

public:
    Model(std::string path, TextureCache& texture_cache, bool flip_UVs = false) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | (flip_UVs * aiProcess_FlipUVs) | aiProcess_CalcTangentSpace); 

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene, texture_cache);
    }

    void draw(GLuint shader, glm::mat4& model) {
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        for (Mesh& mesh : meshes) {
            mesh._draw(shader, model, normalMatrix);
        }
    }

private:
    void processNode(aiNode *node, const aiScene *scene, TextureCache& texture_cache) {
        // Process all node meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
            meshes.push_back(processMesh(mesh, scene, texture_cache));			
        }

        // Process all children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, texture_cache);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene, TextureCache& texture_cache) {
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
            material = loadMaterial(scene->mMaterials[mesh->mMaterialIndex], texture_cache);
        }

        return Mesh(vertices, indices, material);
    }

    Material loadMaterial(aiMaterial* mat, TextureCache& texture_cache) {
        Material material;

        material.diffuse = getTexture(mat, texture_cache, aiTextureType_DIFFUSE, TextureCache::DEFAULT_DIFFUSE);
        material.specular = getTexture(mat, texture_cache, aiTextureType_SPECULAR, TextureCache::DEFAULT_SPECULAR);
        material.normal = getTexture(mat, texture_cache, aiTextureType_NORMALS, TextureCache::DEFAULT_NORMAL);
        material.shininess = getTexture(mat, texture_cache, aiTextureType_SHININESS, TextureCache::DEFAULT_SHININESS);

        float opacity = 1.0f;
        float transparencyFactor = 0.0f;
        mat->Get(AI_MATKEY_OPACITY, opacity);
        mat->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparencyFactor);
        material.opacity = opacity * (1.0f - transparencyFactor);

        return material;
    }

    GLuint getTexture(aiMaterial* mat, TextureCache& texture_cache, aiTextureType type, TextureCache::DefaultTex fallback) {
        if (mat->GetTextureCount(type) == 0) {
            std::cout << "Found no " << type << " texture for material!\n";
            return texture_cache.getDefault(fallback);
        }

        if (mat->GetTextureCount(type) > 1)
            std::cout << "Found more than one " << type << " texture for material!\n";

        aiString str;
        mat->GetTexture(type, 0, &str);
        std::string path = directory + '/' + std::string(str.C_Str());
        return texture_cache.get(path);
    }
};