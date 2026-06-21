#pragma once
#include "../shader/shader.h"
#include "../texture/texture.h"
#include "mesh.h"
#include "model.h"
#include "transform.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {

using ObjectID = uint32_t;
using Tag = std::string;

struct SceneObject {
  ObjectID id = 0;
  Mesh *mesh = nullptr;
  std::unique_ptr<Model> model;
  Texture *texture = nullptr;
  Transform transform;
  bool active = true;
  std::string name;
  Tag tag;

  SceneObject() = default;
  SceneObject(ObjectID id, Mesh *mesh, Texture *texture = nullptr,
              const std::string &name = "", const Tag &tag = "")
      : id(id), mesh(mesh), texture(texture), name(name), tag(tag) {}
  SceneObject(ObjectID id, std::unique_ptr<Model> model,
              Texture *texture = nullptr, const std::string &name = "",
              const Tag &tag = "")
      : id(id), model(std::move(model)), texture(texture), name(name),
        tag(tag) {}
};

class SceneManager {
public:
  ~SceneManager();

  ObjectID addObject(Mesh *mesh, Texture *texture = nullptr,
                     const std::string &name = "", const Tag &tag = "",
                     const glm::vec3 &position = glm::vec3(0.0f),
                     const glm::vec3 &scale = glm::vec3(1.0f),
                     const glm::vec3 &rotation = glm::vec3(0.0f));

  ObjectID addModel(const std::string &filename, const std::string &name = "",
                    const Tag &tag = "",
                    const glm::vec3 &position = glm::vec3(0.0f),
                    const glm::vec3 &scale = glm::vec3(1.0f),
                    const glm::vec3 &rotation = glm::vec3(0.0f));

  SceneObject *getObject(ObjectID id);
  bool removeObject(ObjectID id);
  void clear();
  void clearByTag(const Tag &tag);

  void render(ShaderProgram &shader);

  std::unordered_map<ObjectID, SceneObject> &objects();
  std::vector<ObjectID> getIDsByTag(const Tag &tag);
  size_t objectCount() const;

private:
  ObjectID nextID();
  std::unordered_map<ObjectID, SceneObject> m_objects;
  ObjectID m_nextID = 1;
};

} // namespace Engine
