#pragma once
#include <glm/glm.hpp>

namespace Engine {

class InputManager;

struct GameSettings {
  float renderDistance = 200.0f;
  float fogStart = 60.0f;
  float fogEnd = 200.0f;
  glm::vec3 fogColor{0.5f};
};

class MenuPause {
public:
  MenuPause();

  void update(InputManager &input);
  void render();

  bool isPaused() const;
  bool wasPaused() const;
  bool shouldQuit() const;
  const GameSettings &settings() const;

private:
  void renderPauseMenu();
  void renderSettings();

  bool m_paused = false;
  bool m_wasPaused = false;
  bool m_quitRequested = false;
  bool m_showSettings = false;
  bool m_prevEscapeState = false;
  GameSettings m_settings;
};

} // namespace Engine
