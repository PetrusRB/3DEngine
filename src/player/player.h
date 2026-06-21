#pragma once
#include "../camera/camera.h"
#include "../utils/input.h"

namespace Engine {

class Player {
public:
  explicit Player(Camera &camera, InputManager &input);

  void update(float deltaTime);
  glm::vec3 getPosition() const;

  Camera getCamera() { return m_camera; }
  InputManager getInput() { return m_input; }

private:
  Camera &m_camera;
  InputManager &m_input;
};

} // namespace Engine
