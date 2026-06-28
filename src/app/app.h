#pragma once
#include "../utils/arena_wrapper.h"
#include "window.h"
#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Engine {

using ObjectID = uint32_t;
class Camera;
class Cubemap;
class OutputTerm;
class Utils;
class InputManager;
class Economy;
class Player;
class EventSystem;
class ShaderProgram;
class SceneManager;
class Mesh;
class UI;
class Texture;
class Light;
class DirectionalLight;
class MenuPause;
class AudioSystem;

class Application {
public:
  explicit Application(WindowConfig config = {});
  ~Application();

  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

  void run();

private:
  void init();
  void destroy();
  void recreate_coins();

  WindowConfig m_config;
  std::unique_ptr<Window> m_window;

  std::vector<std::unique_ptr<Texture>> m_textures;
  std::unique_ptr<Cubemap> m_cubemap;
  std::unique_ptr<Camera> m_camera;
  std::unique_ptr<InputManager> m_input;
  std::unique_ptr<EventSystem> m_event;
  std::unique_ptr<Utils> m_utils;
  std::unique_ptr<Economy> m_economy;
  std::unique_ptr<OutputTerm> m_output;
  std::unique_ptr<Player> m_player;
  std::unique_ptr<ShaderProgram> m_shader;
  std::unique_ptr<ShaderProgram> m_skyboxShader;
  std::unique_ptr<SceneManager> m_scene;
  std::unique_ptr<Mesh> m_cubeMesh;
  std::unique_ptr<UI> m_ui;
  std::unique_ptr<MenuPause> m_menuPause;

  std::vector<std::unique_ptr<Light>> m_lights;
  std::string maze_prefix = "maze";

  std::array<glm::vec3, 5> coinPositions = {
      glm::vec3(5.0f, 0.5f, 5.0f),  glm::vec3(10.0f, 0.5f, 3.0f),
      glm::vec3(3.0f, 0.5f, 10.0f), glm::vec3(7.0f, 0.5f, 7.0f),
      glm::vec3(1.0f, 0.5f, 1.0f),
  };

  float m_shininess = 32.0f;
  bool m_cursorEnabled = false;
  int m_mazeWidth = 15;
  int m_mazeHeight = 15;
  float m_wallHeight = 4.0f;
  float m_floorThickness = 0.1f;
  float m_cellSize = 2.0f;
  std::vector<ObjectID> m_coins;
  t_arena *m_frameArena = nullptr;
  bool m_frustumEnabled = true;
  float m_frustumMargin = 0.1f;
  int m_visibleObjects = 0;
  int m_culledObjects = 0;

  void buildMaze();
};

} // namespace Engine
