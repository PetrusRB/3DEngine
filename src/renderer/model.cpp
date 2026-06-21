#include "model.h"
#include "../shader/shader.h"
#include <iostream>

namespace Engine {

Model::Model(const std::string &path) { loadModel(path); }

Model::~Model() = default;

bool Model::valid() const { return m_valid; }

void Model::loadModel(const std::string &path) {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_FlipUVs |
                aiProcess_CalcTangentSpace | aiProcess_GenNormals);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
    return;
  }

  processNode(scene->mRootNode, scene);
  m_valid = !m_submeshes.empty();
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    processMesh(mesh, scene);
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex{};

    vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
                                 mesh->mVertices[i].z);

    if (mesh->HasNormals())
      vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
                                mesh->mNormals[i].z);

    if (mesh->mTextureCoords[0]) {
      vertex.texCoords =
          glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    }

    vertex.color = glm::vec3(0.8f, 0.8f, 0.8f);

    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  SubMesh sub;
  sub.mesh = std::make_unique<Mesh>(vertices, indices);

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
      aiString str;
      material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
      auto tex = std::make_unique<Texture>(std::string(str.C_Str()));
      if (tex->id() != 0)
        sub.diffuseTexture = std::move(tex);
    }
  }

  m_submeshes.push_back(std::move(sub));
}

void Model::draw(ShaderProgram &shader) const {
  for (const auto &sub : m_submeshes) {
    if (sub.diffuseTexture) {
      sub.diffuseTexture->bind(0);
      shader.setInt("texture1", 0);
      shader.setInt("useTexture", 1);
    } else {
      shader.setInt("useTexture", 0);
    }
    shader.setInt("useInstancing", 0);
    sub.mesh->draw();
  }
}

} // namespace Engine
