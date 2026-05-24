#include "Model.hpp"
#include <assimp/postprocess.h>

Model::Model(std::string path, TextureCache& cache, bool flip_UVs) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path,
        aiProcess_Triangulate
        | (flip_UVs * aiProcess_FlipUVs)
        | aiProcess_CalcTangentSpace
        | aiProcess_JoinIdenticalVertices
    ); 

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene, cache);
}

void Model::drawOpaque(GLuint shader, const glm::dmat4& model, const glm::dmat4& view, const glm::mat4& projection) {
    for (const Mesh& mesh : opaque_meshes) {
        mesh.render(shader, model, view, projection);
    }
}

void Model::drawTransparent(GLuint shader, const glm::dmat4& model, const glm::dmat4& view, const glm::mat4& projection) {
    for (const Mesh& mesh : transparent_meshes) {
        mesh.render(shader, model, view, projection);
    }
}

void Model::processNode(aiNode *node, const aiScene *scene, TextureCache& cache) {
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

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene, TextureCache& cache) {
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    Material material;

    // vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        
        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        if (mesh->mNormals) {
            vertex.norm.x = mesh->mNormals[i].x;
            vertex.norm.y = mesh->mNormals[i].y;
            vertex.norm.z = mesh->mNormals[i].z;
        } else {
            vertex.norm = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (mesh->mTextureCoords[0]) {
            vertex.uv.x = mesh->mTextureCoords[0][i].x; 
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.uv = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->mTangents && mesh->mBitangents) {
            glm::vec3 T = { mesh->mTangents[i].x,  mesh->mTangents[i].y,  mesh->mTangents[i].z };
            glm::vec3 B = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
            glm::vec3 N = vertex.norm;

            if (glm::length(T) > 0.00001f && glm::length(B) > 0.00001) {
                float w = glm::dot(glm::cross(N, T), B) < 0.0f ? -1.0f : 1.0f;
                vertex.tan = glm::vec4(T, w);
            } else {
                vertex.tan = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        } else {
            vertex.tan = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }

        vertices.push_back(vertex);
    }

    // Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j <= (face.mNumIndices - 3); j += 3) {
            triangles.push_back(Triangle(face.mIndices[j], face.mIndices[j+1], face.mIndices[j+2]));
        }
    }

    // Materials
    material = loadMaterial(scene->mMaterials[mesh->mMaterialIndex], cache);

    return Mesh(vertices, triangles, material);
}

Material Model::loadMaterial(aiMaterial* mat, TextureCache& cache) {
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

    material.glossiness = getTexture(mat, cache, aiTextureType_SHININESS);
    if (!material.glossiness) {
        glm::vec3 color(0, 0, 0);
        float ai_float;
        if (AI_SUCCESS ==  mat->Get(AI_MATKEY_SHININESS, ai_float)) {
            color.r = ai_float;
            color.g = ai_float;
            color.b = ai_float;
        }
        material.glossiness = cache.getColor(color);
    }

    float opacity = 1.0f;
    float transparencyFactor = 0.0f;
    mat->Get(AI_MATKEY_OPACITY, opacity);
    mat->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparencyFactor);
    material.opacity = opacity * (1.0f - transparencyFactor);

    return material;
}

GLuint Model::getTexture(aiMaterial* mat, TextureCache& cache, aiTextureType type) {
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