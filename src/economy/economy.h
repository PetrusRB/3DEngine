#pragma once
#include "../player/player.h"

namespace Engine {
class Economy {
public:
  bool addMoney(Player *player, int value);
  void removeMoney(Player *player, int value);
};
} // namespace Engine
