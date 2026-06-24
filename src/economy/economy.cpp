#include "./economy.h"

namespace Engine {
bool Economy::addMoney(Player *player, int value) {
  if (player == nullptr) {
    return false;
  }
  player->addPlrMoney(value);
  return true;
}
void Economy::removeMoney(Player *player, int value) {
  if (player == nullptr) {
    return;
  }
  player->removePlrMoney(value);
}

} // namespace Engine
