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

ObjectID
SceneManager::addObject(Mesh *mesh, Texture *texture, const std::string &name,
                        const Tag &tag, const glm::vec3 &position,
                        const glm::vec3 &scale, const glm::vec3 &rotation,
                        const glm::vec2 &uvTiling, const float &reflect) {
  ObjectID id = nextID();
  SceneObject obj(id, mesh, texture, name, tag);
  obj.transform.setPosition(position);
  obj.transform.setRotation(rotation);
  obj.transform.setScale(scale);
  obj.uvTiling = uvTiling;
  obj.reflectStrength = reflect;
  m_objects.emplace(id, std::move(obj));
  return id;
}

ObjectID SceneManager::addModel(const std::string &filename,
                                const std::string &name, Texture *texture,
                                const Tag &tag, const glm::vec3 &position,
                                const glm::vec3 &scale,
                                const glm::vec3 &rotation,
                                const float &reflect) {
  auto it = m_modelCache.find(filename);
  if (it == m_modelCache.end()) {
    auto mdl = std::make_shared<Model>(filename);
    if (!mdl->valid())
      return 0;
    m_modelCache.emplace(filename, std::move(mdl));
    it = m_modelCache.find(filename);
  }

  ObjectID id = nextID();
  SceneObject obj(id, it->second.get(), texture, name, tag);
  obj.transform.setPosition(position);
  obj.transform.setRotation(rotation);
  obj.transform.setScale(scale);
  obj.reflectStrength = reflect;
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
void SceneManager::clear() {
  m_objects.clear();
  m_modelCache.clear();
}
void SceneManager::clearByTag(const Tag &tag) {
  for (auto it = m_objects.begin(); it != m_objects.end();) {
    if (it->second.tag == tag)
      it = m_objects.erase(it);
    else
      ++it;
  }
}

void SceneManager::render(ShaderProgram &shader, const Frustum *frustum,
                          float renderDistance, const glm::vec3 &viewPos,
                          t_arena *frameArena) {
  struct BatchKey {
    Mesh *mesh;
    Texture *texture;
    glm::vec2 uvTiling;
    glm::vec2 uvOffset;
    float reflectStrength;
    bool operator==(const BatchKey &o) const {
      return mesh == o.mesh && texture == o.texture && uvTiling == o.uvTiling &&
             uvOffset == o.uvOffset && reflectStrength == o.reflectStrength;
    }
  };
  struct BatchKeyHash {
    size_t operator()(const BatchKey &k) const {
      size_t h1 = std::hash<void *>()(k.mesh);
      size_t h2 = std::hash<void *>()(k.texture);
      size_t h3 = std::hash<float>()(k.uvTiling.x) ^
                  (std::hash<float>()(k.uvTiling.y) * 0x9e3779b97f4a7c15ULL);
      size_t h4 = std::hash<float>()(k.uvOffset.x) ^
                  (std::hash<float>()(k.uvOffset.y) * 0x9e3779b97f4a7c15ULL);
      size_t h5 =
          std::hash<int>()(*reinterpret_cast<const int *>(&k.reflectStrength));
      return h1 ^ (h2 * 0x9e3779b97f4a7c15ULL) ^ (h3 * 0x7feb352d) ^ (h4) ^
             (h5 * 0x517cc1b7);
    }
  };

  auto isVisible = [&](const SceneObject &obj) -> bool {
    if (!obj.active)
      return false;
    glm::vec3 pos = obj.transform.getPosition();
    glm::vec3 scale = obj.transform.getScale();
    AABB box{pos - scale * 0.5f, pos + scale * 0.5f};
    if (renderDistance > 0.0f) {
      glm::vec3 closest = glm::clamp(viewPos, box.min, box.max);
      float dist = glm::length(viewPos - closest);
      if (dist > renderDistance)
        return false;
    }
    if (!frustum)
      return true;

    return frustum->isAABBInside(box);
  };

  using MatVec = std::vector<glm::mat4, ArenaAllocator<glm::mat4>>;
  ArenaAllocator<glm::mat4> matAlloc(frameArena);
  std::unordered_map<BatchKey, MatVec, BatchKeyHash> batches;

  for (auto &[id, obj] : m_objects) {
    if (!isVisible(obj))
      continue;
    glm::mat4 modelMatrix = obj.transform.getModelMatrix();
    if (obj.model) {
      for (const auto &sub : obj.model->subMeshes()) {
        Texture *tex = sub.diffuseTexture.get();
        if (!tex)
          tex = obj.texture;
        BatchKey key{sub.mesh.get(), tex, obj.uvTiling, obj.uvOffset,
                     obj.reflectStrength};
        auto [it, _] = batches.try_emplace(key, MatVec(matAlloc));
        it->second.push_back(modelMatrix);
      }
    } else if (obj.mesh) {
      BatchKey key{obj.mesh, obj.texture, obj.uvTiling, obj.uvOffset,
                   obj.reflectStrength};
      auto [it, _] = batches.try_emplace(key, MatVec(matAlloc));
      it->second.push_back(modelMatrix);
    }
  }

  shader.setInt("useInstancing", 1);
  for (auto &[key, matrices] : batches) {
    key.mesh->uploadInstances(matrices);
    shader.setVec2("uvTiling", key.uvTiling);
    shader.setVec2("uvOffset", key.uvOffset);
    shader.setFloat("reflectStrength", key.reflectStrength);
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
