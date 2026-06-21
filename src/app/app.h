#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

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
class Light;
class DirectionalLight;

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

  std::vector<std::unique_ptr<Light>> m_lights;
  std::string maze_prefix = "maze";

  float m_shininess = 32.0f;
  bool m_cursorEnabled = false;
  int m_mazeWidth = 15;
  int m_mazeHeight = 15;
  float m_wallHeight = 4.0f;
  float m_floorThickness = 0.1f;
  float m_cellSize = 2.0f;
  ObjectID m_rotatingCube = 0;
  bool m_frustumEnabled = true;
  float m_frustumMargin = 0.1f;
  int m_visibleObjects = 0;
  int m_culledObjects = 0;

  void buildMaze();
};

} // namespace Engine
