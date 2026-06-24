#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace Engine {
class Player;
class SceneObject;
using ObjectID = uint32_t;
using EventCallback = std::function<void(Player &, SceneObject &)>;
class EventSystem {
public:
  void onCollision(ObjectID id, EventCallback callback);
  void emitCollision(ObjectID id, Player &player, SceneObject &obj);
  void remove(ObjectID id);

private:
  std::unordered_map<ObjectID, EventCallback> m_callbacks;
};
} // namespace Engine
