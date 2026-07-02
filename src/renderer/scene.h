#pragma once
#include "../components/component.h"
#include "../components/transform/transform.h"
#include "../utils/arena_allocator.h"
#include "model.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <memory>
#include <string>
#include <typeindex>
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

  std::vector<std::unique_ptr<Component>> components;

  bool active = true;
  std::string name;
  Tag tag;
  glm::vec2 uvTiling{1.0f, 1.0f};
  glm::vec2 uvOffset{0.0f, 0.0f};
  float reflectStrength = 1.0f;

  SceneObject() = default;
  SceneObject(ObjectID id, Mesh *mesh, Texture *texture = nullptr,
              const std::string &name = "", const Tag &tag = "")
      : id(id), mesh(mesh), texture(texture), name(name), tag(tag) {}
  SceneObject(ObjectID id, Model *model, Texture *texture = nullptr,
              const std::string &name = "", const Tag &tag = "")
      : id(id), model(model), texture(texture), name(name), tag(tag) {}

  template <typename T, typename... Args> T &addComponent(Args &&...args) {
    auto comp = std::make_unique<T>(std::forward<Args>(args)...);
    T &ref = *comp;
    components.push_back(std::move(comp));
    return ref;
  }

  template <typename T> T *getComponent() {
    for (auto &c : components) {
      T *p = dynamic_cast<T *>(c.get());
      if (p)
        return p;
    }
    return nullptr;
  }

  template <typename T> const T *getComponent() const {
    for (auto &c : components) {
      const T *p = dynamic_cast<const T *>(c.get());
      if (p)
        return p;
    }
    return nullptr;
  }

  template <typename T> bool hasComponent() const {
    for (auto &c : components) {
      if (dynamic_cast<const T *>(c.get()))
        return true;
    }
    return false;
  }

  void renderUI() {
    ImGui::Checkbox("Active", &active);
    transform.renderUI();
    ImGui::SliderFloat("Reflect", &reflectStrength, 0.0f, 1.0f, "%.2f");
    if (texture || model) {
      if (ImGui::TreeNode("UV")) {
        ImGui::DragFloat2("Tiling", &uvTiling.x, 0.01f, 0.01f, 100.0f);
        ImGui::DragFloat2("Offset", &uvOffset.x, 0.01f);
        ImGui::TreePop();
      }
    }
    for (auto &comp : components) {
      if (ImGui::TreeNode(comp->name())) {
        comp->renderUI();
        ImGui::TreePop();
      }
    }
  }
};

class SceneManager {
public:
  ~SceneManager();

  ObjectID addObject(Mesh *mesh, Texture *texture = nullptr,
                     const std::string &name = "", const Tag &tag = "",
                     const glm::vec3 &position = glm::vec3(0.0f),
                     const glm::vec3 &scale = glm::vec3(1.0f),
                     const glm::vec3 &rotation = glm::vec3(0.0f),
                     const glm::vec2 &uvTiling = glm::vec3(1.0f),
                     const float &reflect = 1.0f);

  ObjectID addModel(const std::string &filename, const std::string &name = "",
                    Texture *texture = nullptr, const Tag &tag = "",
                    const glm::vec3 &position = glm::vec3(0.0f),
                    const glm::vec3 &scale = glm::vec3(1.0f),
                    const glm::vec3 &rotation = glm::vec3(0.0f),
                    const float &reflect = 1.0f);

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

  int getVisibleObjects() const { return m_visibleObjects; }
  int getCulledObjects() const { return m_culledObjects; }

private:
  ObjectID nextID();
  std::unordered_map<ObjectID, SceneObject> m_objects;
  std::unordered_map<std::string, std::shared_ptr<Model>> m_modelCache;
  ObjectID m_nextID = 1;
  int m_visibleObjects = 0;
  int m_culledObjects = 0;
};

} // namespace Engine
