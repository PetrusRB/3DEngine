#pragma once
#include "../utils/arena_allocator.h"
#include "model.h"
#include "transform.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {

class ShaderProgram;
class Mesh;
class Texture;
class SpatialGrid;
class Frustum;

using ObjectID = uint32_t;
using Tag = std::string;

struct SceneObject {
  ObjectID id = 0;
  Mesh *mesh = nullptr;
  Model *model = nullptr;
  Texture *texture = nullptr;
  Transform transform;
  bool active = true;
  std::string name;
  Tag tag;
  glm::vec2 uvTiling{1.0f, 1.0f};
  glm::vec2 uvOffset{0.0f, 0.0f};

  SceneObject() = default;
  SceneObject(ObjectID id, Mesh *mesh, Texture *texture = nullptr,
              const std::string &name = "", const Tag &tag = "")
      : id(id), mesh(mesh), texture(texture), name(name), tag(tag) {}
  SceneObject(ObjectID id, Model *model, Texture *texture = nullptr,
              const std::string &name = "", const Tag &tag = "")
      : id(id), model(model), texture(texture), name(name), tag(tag) {}
};

class SceneManager {
public:
  ~SceneManager();

  ObjectID addObject(Mesh *mesh, Texture *texture = nullptr,
                     const std::string &name = "", const Tag &tag = "",
                     const glm::vec3 &position = glm::vec3(0.0f),
                     const glm::vec3 &scale = glm::vec3(1.0f),
                     const glm::vec3 &rotation = glm::vec3(0.0f),
                     const glm::vec2 &uvTiling = glm::vec3(1.0f));

  ObjectID addModel(const std::string &filename, const std::string &name = "",
                    Texture *texture = nullptr, const Tag &tag = "",
                    const glm::vec3 &position = glm::vec3(0.0f),
                    const glm::vec3 &scale = glm::vec3(1.0f),
                    const glm::vec3 &rotation = glm::vec3(0.0f));

  SceneObject *getObject(ObjectID id);
  bool removeObject(ObjectID id);
  void clear();
  void clearByTag(const Tag &tag);

  void render(ShaderProgram &shader, const Frustum *frustum = nullptr,
              float renderDistance = 0.0f,
              const glm::vec3 &viewPos = glm::vec3(0.0f),
              t_arena *frameArena = nullptr);
  void buildSpatialGrid(SpatialGrid &grid) const;

  std::unordered_map<ObjectID, SceneObject> &objects();
  std::vector<ObjectID> getIDsByTag(const Tag &tag);
  size_t objectCount() const;

private:
  ObjectID nextID();
  std::unordered_map<ObjectID, SceneObject> m_objects;
  std::unordered_map<std::string, std::shared_ptr<Model>> m_modelCache;
  ObjectID m_nextID = 1;
};

} // namespace Engine
