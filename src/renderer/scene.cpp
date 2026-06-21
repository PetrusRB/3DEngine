#include "scene.h"
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
                                const std::string &name, const Tag &tag,
                                const glm::vec3 &position,
                                const glm::vec3 &scale,
                                const glm::vec3 &rotation) {
  ObjectID id = nextID();
  auto mdl = std::make_unique<Model>(filename);
  if (!mdl->valid())
    return 0;

  SceneObject obj(id, std::move(mdl), nullptr, name, tag);
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

void SceneManager::render(ShaderProgram &shader) {
  for (auto &[id, obj] : m_objects) {
    if (!obj.active)
      continue;

    shader.setMat4("model", obj.transform.getModelMatrix());

    if (obj.model) {
      obj.model->draw(shader);
    } else if (obj.mesh) {
      if (obj.texture) {
        obj.texture->bind(0);
        shader.setInt("texture1", 0);
        shader.setInt("useTexture", 1);
      } else {
        shader.setInt("useTexture", 0);
      }
      obj.mesh->draw();
    }
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
