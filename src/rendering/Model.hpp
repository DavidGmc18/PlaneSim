#pragma once

#include <vector>
#include "Mesh.hpp"
#include <string>
#include "TextureCache.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

// TODO optimize materials to share, rather than one per mesh
class Model {
    public:
    std::vector<Mesh> opaque_meshes;
    std::vector<Mesh> transparent_meshes;
    std::string directory;

public:
    Model(std::string path, TextureCache& cache, bool flip_UVs = false);

    void drawOpaque(GLuint shader, const glm::dmat4& model, const glm::dmat4& view, const glm::mat4& projection);
    void drawTransparent(GLuint shader, const glm::dmat4& model, const glm::dmat4& view, const glm::mat4& projection);

private:
    void processNode(aiNode *node, const aiScene *scene, TextureCache& cache);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene, TextureCache& cache);
    Material loadMaterial(aiMaterial* mat, TextureCache& cache);
    GLuint getTexture(aiMaterial* mat, TextureCache& cache, aiTextureType type);
};