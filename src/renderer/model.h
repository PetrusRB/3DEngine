#pragma once
#include "mesh.h"
#include "../texture/texture.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <memory>
#include <string>
#include <vector>

namespace Engine {

class ShaderProgram;

class Model {
public:
  Model(const std::string &path);
  ~Model();

  Model(const Model &) = delete;
  Model &operator=(const Model &) = delete;

  void draw(ShaderProgram &shader) const;
  bool valid() const;

  struct SubMesh {
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Texture> diffuseTexture;
  };

  const std::vector<SubMesh> &subMeshes() const { return m_submeshes; }

private:
  void loadModel(const std::string &path);
  void processNode(aiNode *node, const aiScene *scene);
  void processMesh(aiMesh *mesh, const aiScene *scene);

  std::vector<SubMesh> m_submeshes;
  bool m_valid = false;
};

} // namespace Engine
