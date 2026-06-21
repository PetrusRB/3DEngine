#include "app.h"
#include "../algorithm/maze.h"
#include "../camera/camera.h"
#include "../cubemap/cubemap.h"
#include "../player/player.h"
#include "../renderer/mesh.h"
#include "../renderer/scene.h"
#include "../shader/shader.h"
#include "../texture/texture.h"
#include "../ui/ui.h"
#include "../utils/input.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <stdexcept>

namespace Engine {

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

Application::Application(WindowConfig config) : m_config(std::move(config)) {
  init();
}

Application::~Application() { destroy(); }

void Application::init() {
  if (!glfwInit())
    throw std::runtime_error("Falha ao inicializar o GLFW");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_window = glfwCreateWindow(m_config.width, m_config.height,
                              m_config.title.c_str(), nullptr, nullptr);
  if (!m_window) {
    glfwTerminate();
    throw std::runtime_error("Falha ao criar a janela");
  }
  glfwMakeContextCurrent(m_window);
  glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw std::runtime_error("Falha ao inicializar GLAD");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  m_ui = std::make_unique<UI>();
  m_ui->init(m_window);

  m_input = std::make_unique<InputManager>(m_window);
  m_cubemap = std::make_unique<Cubemap>();
  m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 1.0f, 3.0f));
  m_player = std::make_unique<Player>(*m_camera, *m_input);

  m_shader = std::make_unique<ShaderProgram>("src/assets/shaders/color.vs",
                                             "src/assets/shaders/color.fs");
  m_skyboxShader = std::make_unique<ShaderProgram>(
      "src/assets/shaders/cubemap.vs", "src/assets/shaders/cubemap.fs");

  m_ui->setLightPos(&m_lightPos);
  m_ui->setLightColor(&m_lightColor);
  m_ui->setAmbientStrength(&m_ambientStrength);

  // maze
  m_ui->setMazeWidth(&m_mazeWidth);
  m_ui->setMazeHeight(&m_mazeHeight);
  m_ui->setMazeWallHeight(&m_wallHeight);
  m_ui->setMazeFloorThickness(&m_floorThickness);
  m_ui->setMazeCellSize(&m_cellSize);
  m_ui->setOnMazeRebuild([this]() { buildMaze(); });

  m_scene = std::make_unique<SceneManager>();
  m_ui->setSceneObjects(&m_scene->objects());

  std::vector<std::string> faces{
      "src/assets/cubemap/right.png", "src/assets/cubemap/left.png",
      "src/assets/cubemap/top.png",   "src/assets/cubemap/bottom.png",
      "src/assets/cubemap/front.png", "src/assets/cubemap/back.png"};
  m_cubemap->load(faces);

  std::vector<Vertex> cubeVertices = {
      // back (z=-0.5)  normal -z
      {{0.5f, -0.5f, -0.5f},
       {1.0f, 0.0f, 0.0f},
       {0.0f, 0.0f, -1.0f},
       {0.0f, 0.0f}},
      {{-0.5f, -0.5f, -0.5f},
       {1.0f, 0.0f, 0.0f},
       {0.0f, 0.0f, -1.0f},
       {1.0f, 0.0f}},
      {{-0.5f, 0.5f, -0.5f},
       {1.0f, 0.0f, 0.0f},
       {0.0f, 0.0f, -1.0f},
       {1.0f, 1.0f}},
      {{-0.5f, 0.5f, -0.5f},
       {1.0f, 0.0f, 0.0f},
       {0.0f, 0.0f, -1.0f},
       {1.0f, 1.0f}},
      {{0.5f, 0.5f, -0.5f},
       {1.0f, 0.0f, 0.0f},
       {0.0f, 0.0f, -1.0f},
       {0.0f, 1.0f}},
      {{0.5f, -0.5f, -0.5f},
       {1.0f, 0.0f, 0.0f},
       {0.0f, 0.0f, -1.0f},
       {0.0f, 0.0f}},

      // front (z=0.5)  normal +z
      {{-0.5f, -0.5f, 0.5f},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f, 1.0f},
       {0.0f, 0.0f}},
      {{0.5f, -0.5f, 0.5f},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f, 1.0f},
       {1.0f, 0.0f}},
      {{0.5f, 0.5f, 0.5f},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f, 1.0f},
       {1.0f, 1.0f}},
      {{0.5f, 0.5f, 0.5f},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f, 1.0f},
       {1.0f, 1.0f}},
      {{-0.5f, 0.5f, 0.5f},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f, 1.0f},
       {0.0f, 1.0f}},
      {{-0.5f, -0.5f, 0.5f},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f, 1.0f},
       {0.0f, 0.0f}},

      // left (x=-0.5)  normal -x
      {{-0.5f, 0.5f, 0.5f},
       {0.0f, 0.0f, 1.0f},
       {-1.0f, 0.0f, 0.0f},
       {0.0f, 1.0f}},
      {{-0.5f, 0.5f, -0.5f},
       {0.0f, 0.0f, 1.0f},
       {-1.0f, 0.0f, 0.0f},
       {1.0f, 1.0f}},
      {{-0.5f, -0.5f, -0.5f},
       {0.0f, 0.0f, 1.0f},
       {-1.0f, 0.0f, 0.0f},
       {1.0f, 0.0f}},
      {{-0.5f, -0.5f, -0.5f},
       {0.0f, 0.0f, 1.0f},
       {-1.0f, 0.0f, 0.0f},
       {1.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f},
       {0.0f, 0.0f, 1.0f},
       {-1.0f, 0.0f, 0.0f},
       {0.0f, 0.0f}},
      {{-0.5f, 0.5f, 0.5f},
       {0.0f, 0.0f, 1.0f},
       {-1.0f, 0.0f, 0.0f},
       {0.0f, 1.0f}},

      // right (x=0.5)  normal +x
      {{0.5f, -0.5f, 0.5f},
       {1.0f, 1.0f, 0.0f},
       {1.0f, 0.0f, 0.0f},
       {0.0f, 0.0f}},
      {{0.5f, -0.5f, -0.5f},
       {1.0f, 1.0f, 0.0f},
       {1.0f, 0.0f, 0.0f},
       {1.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f},
       {1.0f, 1.0f, 0.0f},
       {1.0f, 0.0f, 0.0f},
       {1.0f, 1.0f}},
      {{0.5f, 0.5f, -0.5f},
       {1.0f, 1.0f, 0.0f},
       {1.0f, 0.0f, 0.0f},
       {1.0f, 1.0f}},
      {{0.5f, 0.5f, 0.5f},
       {1.0f, 1.0f, 0.0f},
       {1.0f, 0.0f, 0.0f},
       {0.0f, 1.0f}},
      {{0.5f, -0.5f, 0.5f},
       {1.0f, 1.0f, 0.0f},
       {1.0f, 0.0f, 0.0f},
       {0.0f, 0.0f}},

      // bottom (y=-0.5)  normal -y
      {{-0.5f, -0.5f, -0.5f},
       {1.0f, 0.0f, 1.0f},
       {0.0f, -1.0f, 0.0f},
       {0.0f, 1.0f}},
      {{0.5f, -0.5f, -0.5f},
       {1.0f, 0.0f, 1.0f},
       {0.0f, -1.0f, 0.0f},
       {1.0f, 1.0f}},
      {{0.5f, -0.5f, 0.5f},
       {1.0f, 0.0f, 1.0f},
       {0.0f, -1.0f, 0.0f},
       {1.0f, 0.0f}},
      {{0.5f, -0.5f, 0.5f},
       {1.0f, 0.0f, 1.0f},
       {0.0f, -1.0f, 0.0f},
       {1.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f},
       {1.0f, 0.0f, 1.0f},
       {0.0f, -1.0f, 0.0f},
       {0.0f, 0.0f}},
      {{-0.5f, -0.5f, -0.5f},
       {1.0f, 0.0f, 1.0f},
       {0.0f, -1.0f, 0.0f},
       {0.0f, 1.0f}},

      // top (y=0.5)  normal +y
      {{-0.5f, 0.5f, 0.5f},
       {0.0f, 1.0f, 1.0f},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f}},
      {{0.5f, 0.5f, 0.5f},
       {0.0f, 1.0f, 1.0f},
       {0.0f, 1.0f, 0.0f},
       {1.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f},
       {0.0f, 1.0f, 1.0f},
       {0.0f, 1.0f, 0.0f},
       {1.0f, 1.0f}},
      {{0.5f, 0.5f, -0.5f},
       {0.0f, 1.0f, 1.0f},
       {0.0f, 1.0f, 0.0f},
       {1.0f, 1.0f}},
      {{-0.5f, 0.5f, -0.5f},
       {0.0f, 1.0f, 1.0f},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 1.0f}},
      {{-0.5f, 0.5f, 0.5f},
       {0.0f, 1.0f, 1.0f},
       {0.0f, 1.0f, 0.0f},
       {0.0f, 0.0f}},
  };

  m_cubeMesh = std::make_unique<Mesh>(cubeVertices);

  m_textures.push_back(
      std::make_unique<Texture>("src/assets/textures/brick/brickwall003.png"));
  m_textures.push_back(std::make_unique<Texture>(
      "src/assets/textures/building_template/building_template003a.png"));

  // buildMaze();

  m_rotatingCube = m_scene->addObject(
      m_cubeMesh.get(), nullptr, "Rotating Cube", "persistent",
      glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.5f));

  Texture *brick = m_textures[0].get();
  ObjectID teapot = m_scene->addModel(
      "src/assets/objs/teapot/teapot.obj", "Teapot", "persistent",
      glm::vec3(25.0f, 1.0f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

  m_input->disableCursor();
}

void Application::buildMaze() {
  m_scene->clearByTag(maze_prefix);

  MazeConfig cfg;
  cfg.width = m_mazeWidth;
  cfg.height = m_mazeHeight;
  auto grid = MazeGenerator::generate(cfg);

  Texture *wallTex = m_textures[0].get();
  Texture *floorTex = m_textures[1].get();

  for (uint32_t y = 0; y < grid.size(); y++) {
    for (uint32_t x = 0; x < grid[y].size(); x++) {
      float px = x * m_cellSize;
      float pz = y * m_cellSize;
      if (grid[y][x] == 1) {
        m_scene->addObject(m_cubeMesh.get(), wallTex, "Wall", maze_prefix,
                           glm::vec3(px, m_wallHeight * 0.5f, pz),
                           glm::vec3(1.0f, m_wallHeight, 1.0f));
      } else {
        m_scene->addObject(m_cubeMesh.get(), floorTex, "Floor", maze_prefix,
                           glm::vec3(px, 0.0f, pz),
                           glm::vec3(1.0f, m_floorThickness, 1.0f));
      }
    }
  }
}

void Application::run() {
  float lastFrame = 0.0f;

  while (!glfwWindowShouldClose(m_window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (m_input->isKeyHeld(GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(m_window, true);

    if (m_input->isKeyJustPressed(GLFW_KEY_Q)) {
      m_cursorEnabled = !m_cursorEnabled;
      m_ui->set_main_visible(m_cursorEnabled);
      if (m_cursorEnabled)
        m_input->enableCursor();

      else
        m_input->disableCursor();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_cursorEnabled)
      m_player->update(deltaTime);

    SceneObject *rotCube = m_scene->getObject(m_rotatingCube);
    if (rotCube && rotCube->active)
      rotCube->transform.rotate(glm::vec3(0.0f, 90.0f * deltaTime, 0.0f));

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    float aspect = (height > 0)
                       ? static_cast<float>(width) / static_cast<float>(height)
                       : 1.0f;

    // Skybox
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    m_skyboxShader->use();
    m_skyboxShader->setMat4("view", m_camera->getViewMatrix());
    m_skyboxShader->setMat4("projection",
                            m_camera->getProjectionMatrix(aspect));
    m_skyboxShader->setInt("skybox", 0);
    m_cubemap->render();
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);

    // Objetos da cena
    m_shader->use();

    m_shader->setVec3("lightPos", m_lightPos);
    m_shader->setVec3("lightColor", m_lightColor);
    m_shader->setFloat("ambientStrength", m_ambientStrength);
    m_shader->setMat4("view", m_camera->getViewMatrix());
    m_shader->setMat4("projection", m_camera->getProjectionMatrix(aspect));

    m_scene->render(*m_shader);

    m_ui->render();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
  }
}

void Application::destroy() {
  m_ui.reset();
  m_scene.reset();
  m_cubeMesh.reset();
  m_textures.clear();
  m_shader.reset();
  m_player.reset();
  m_camera.reset();
  m_input.reset();
  m_cubemap.reset();

  if (m_window) {
    glfwDestroyWindow(m_window);
    m_window = nullptr;
  }
  glfwTerminate();
}

} // namespace Engine
