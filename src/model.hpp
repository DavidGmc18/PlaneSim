#pragma once

#include <vector>
#include "mesh.hpp"
#include <string>
#include <glad/glad.h>
#include "TextureCache.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// TODO figureout how texture NORMS work
// TODO resolve glb model textures
// TODO optimize materials to share, rather than one per mesh
class Model {
    public:
    std::vector<Mesh> meshes;
    std::string directory;

public:
    Model(std::string path, TextureCache& texture_cache, bool flip_UVs = false) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | (flip_UVs * aiProcess_FlipUVs));

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene, texture_cache);
    }

    void draw(GLuint shader) {
        for (Mesh& mesh : meshes) {
            mesh.draw(shader);
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

    Material loadMaterial(aiMaterial *mat, TextureCache& texture_cache) {
        Material material;

        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 1) std::cerr << "Too many DIFFUSE textures for material!\n";
            aiString str;
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            std::string path = directory + '/' + std::string(str.C_Str());
            material.diffuse = texture_cache.get(path);
        } else {
            material.diffuse = texture_cache.getDefault(TextureCache::DEFAULT_DIFFUSE);
            std::cout << "No DIFFUSE texture for material, using DEFAULT_DIFFUSE!\n";
        }

        if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            if (mat->GetTextureCount(aiTextureType_SPECULAR) > 1) std::cerr << "Too many SPECULAR textures for material!\n";
            aiString str;
            mat->GetTexture(aiTextureType_SPECULAR, 0, &str);
            std::string path = directory + '/' + std::string(str.C_Str());
            material.specular = texture_cache.get(path);
        } else {
            material.specular = texture_cache.getDefault(TextureCache::DEFAULT_SPECULAR);
            std::cout << "No SPECULAR texture for material, using DEFAULT_SPECULAR!\n";
        }

        // TODO
        material.normal = texture_cache.getDefault(TextureCache::DEFAULT_NORMAL);

        mat->Get(AI_MATKEY_SHININESS, material.shininess);

        float opacity = 1.0f;
        float transparencyFactor = 0.0f;
        mat->Get(AI_MATKEY_OPACITY, opacity);
        mat->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparencyFactor);
        material.opacity = opacity * (1.0f - transparencyFactor);

        return material;
    }
};