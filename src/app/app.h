#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Engine {

using ObjectID = uint32_t;
class Camera;
class Cubemap;
class InputManager;
class Player;
class ShaderProgram;
class SceneManager;
class Mesh;
class UI;
class Texture;

struct WindowConfig {
  int width = 800;
  int height = 600;
  std::string title = "Engine";
};

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

  GLFWwindow *m_window = nullptr;
  WindowConfig m_config;

  std::vector<std::unique_ptr<Texture>> m_textures;
  std::unique_ptr<Cubemap> m_cubemap;

  std::unique_ptr<Camera> m_camera;
  std::unique_ptr<InputManager> m_input;
  std::unique_ptr<Player> m_player;
  std::unique_ptr<ShaderProgram> m_shader;
  std::unique_ptr<ShaderProgram> m_skyboxShader;
  std::unique_ptr<SceneManager> m_scene;
  std::unique_ptr<Mesh> m_cubeMesh;
  std::unique_ptr<UI> m_ui;

  glm::vec3 m_lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
  std::string maze_prefix = "maze";

  float m_ambientStrength = 0.1f;
  bool m_cursorEnabled = false;
  int m_mazeWidth = 15;
  int m_mazeHeight = 15;
  float m_wallHeight = 1.0f;
  float m_floorThickness = 0.1f;
  float m_cellSize = 2.0f;
  ObjectID m_rotatingCube = 0;

  void buildMaze();
};

} // namespace Engine
