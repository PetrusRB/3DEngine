#include "scene.h"
#include "../physics/spatial_grid.h"
#include "../shader/shader.h"
#include "../texture/texture.h"
#include "frustum.h"
#include "mesh.h"
#include "model.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <memory>

namespace Engine {

SceneManager::~SceneManager() { clear(); }

ObjectID SceneManager::addObject(Mesh *mesh, Texture *texture,
                                 const std::string &name, const Tag &tag,
                                 const glm::vec3 &position,
                                 const glm::vec3 &scale,
                                 const glm::vec3 &rotation) {
  ObjectID id = nextID();
  SceneObject obj(id, mesh, texture, name, tag);
  obj.transform.setPosition(position);
  obj.transform.setRotation(rotation);
  obj.transform.setScale(scale);
  m_objects.emplace(id, std::move(obj));
  return id;
}

ObjectID SceneManager::addModel(const std::string &filename,
                                const std::string &name, Texture *texture,
                                const Tag &tag, const glm::vec3 &position,
                                const glm::vec3 &scale,
                                const glm::vec3 &rotation) {
  ObjectID id = nextID();
  auto mdl = std::make_unique<Model>(filename);
  if (!mdl->valid())
    return 0;

  SceneObject obj(id, std::move(mdl), texture, name, tag);
  obj.transform.setPosition(position);
  obj.transform.setRotation(rotation);
  obj.transform.setScale(scale);
  m_objects.emplace(id, std::move(obj));
  return id;
}

SceneObject *SceneManager::getObject(ObjectID id) {
  auto it = m_objects.find(id);
  if (it != m_objects.end())
    return &it->second;
  return nullptr;
}

bool SceneManager::removeObject(ObjectID id) { return m_objects.erase(id) > 0; }
void SceneManager::clear() { m_objects.clear(); }
void SceneManager::clearByTag(const Tag &tag) {
  for (auto it = m_objects.begin(); it != m_objects.end();) {
    if (it->second.tag == tag)
      it = m_objects.erase(it);
    else
      ++it;
  }
}

void SceneManager::render(ShaderProgram &shader, const Frustum *frustum) {
  struct BatchKey {
    Mesh *mesh;
    Texture *texture;
    bool operator==(const BatchKey &o) const {
      return mesh == o.mesh && texture == o.texture;
    }
  };
  struct BatchKeyHash {
    size_t operator()(const BatchKey &k) const {
      size_t h1 = std::hash<void *>()(k.mesh);
      size_t h2 = std::hash<void *>()(k.texture);
      return h1 ^ (h2 * 0x9e3779b97f4a7c15ULL);
    }
  };

  auto isVisible = [&](const SceneObject &obj) -> bool {
    if (!obj.active)
      return false;
    if (!frustum)
      return true;
    glm::vec3 pos = obj.transform.getPosition();
    glm::vec3 scale = obj.transform.getScale();
    AABB box{pos - scale * 0.5f, pos + scale * 0.5f};
    return frustum->isAABBInside(box);
  };

  std::unordered_map<BatchKey, std::vector<glm::mat4>, BatchKeyHash> batches;
  std::vector<SceneObject *> soloObjects;

  for (auto &[id, obj] : m_objects) {
    if (!isVisible(obj))
      continue;
    if (obj.mesh && !obj.model)
      batches[{obj.mesh, obj.texture}].push_back(
          obj.transform.getModelMatrix());
    else
      soloObjects.push_back(&obj);
  }

  if (!batches.empty()) {
    shader.setInt("useInstancing", 1);
    for (auto &[key, matrices] : batches) {
      key.mesh->uploadInstances(matrices);
      if (key.texture) {
        key.texture->bind(0);
        shader.setInt("texture1", 0);
        shader.setInt("useTexture", 1);
      } else {
        shader.setInt("useTexture", 0);
      }
      key.mesh->drawInstanced(static_cast<int>(matrices.size()));
    }
    shader.setInt("useInstancing", 0);
  }

  for (auto *obj : soloObjects) {
    shader.setInt("useInstancing", 0);
    shader.setMat4("model", obj->transform.getModelMatrix());
    if (obj->model) {
      obj->model->draw(shader);
    } else if (obj->mesh) {
      if (obj->texture) {
        obj->texture->bind(0);
        shader.setInt("texture1", 0);
        shader.setInt("useTexture", 1);
      } else {
        shader.setInt("useTexture", 0);
      }
      obj->mesh->draw();
    }
  }
}

void SceneManager::buildSpatialGrid(SpatialGrid &grid) const {
  for (auto &[id, obj] : m_objects) {
    if (!obj.active)
      continue;
    glm::vec3 pos = obj.transform.getPosition();
    glm::vec3 scale = obj.transform.getScale();
    AABB box;
    box.min = pos - scale * 0.5f;
    box.max = pos + scale * 0.5f;
    grid.insert(id, box);
  }
}

std::unordered_map<ObjectID, SceneObject> &SceneManager::objects() {
  return m_objects;
}

std::vector<ObjectID> SceneManager::getIDsByTag(const Tag &tag) {
  std::vector<ObjectID> ids;
  for (auto &[id, obj] : m_objects) {
    if (obj.tag == tag)
      ids.push_back(id);
  }
  return ids;
}

size_t SceneManager::objectCount() const { return m_objects.size(); }

ObjectID SceneManager::nextID() { return m_nextID++; }

} // namespace Engine
