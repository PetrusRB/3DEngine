#include "event_sys.h"

namespace Engine {
void EventSystem::onCollision(ObjectID coinID, EventCallback callback) {
  m_callbacks[coinID] = callback;
}
void EventSystem::emitCollision(ObjectID id, Player &player, SceneObject &obj) {
  auto it = m_callbacks.find(id);
  if (it != m_callbacks.end()) {
    it->second(player, obj);
  }
}
void EventSystem::remove(ObjectID id) { m_callbacks.erase(id); }

} // namespace Engine
