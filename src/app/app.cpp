#include "app.h"
#define GLFW_INCLUDE_NONE
#include "../algorithm/maze.h"
#include "../camera/camera.h"
#include "../cubemap/cubemap.h"
#include "../light/light.h"
#include "../physics/spatial_grid.h"
#include "../player/player.h"
#include "../renderer/frustum.h"
#include "../renderer/mesh.h"
#include "../renderer/model.h"
#include "../renderer/scene.h"
#include "../shader/shader.h"
#include "../texture/texture.h"
#include "../ui/ui.h"
#include "../utils/input.h"
#include <GLFW/glfw3.h>
#include <cstdio>
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
  m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 1.7f, 0.0f));
  m_player = std::make_unique<Player>(*m_camera, *m_input);

  m_shader = std::make_unique<ShaderProgram>("src/assets/shaders/color.vs",
                                             "src/assets/shaders/color.fs");
  m_skyboxShader = std::make_unique<ShaderProgram>(
      "src/assets/shaders/cubemap.vs", "src/assets/shaders/cubemap.fs");

  m_ui->setShininess(&m_shininess);
  m_ui->setLights(&m_lights);
  m_ui->setFrustumDebug(&m_frustumEnabled, &m_frustumMargin, &m_visibleObjects,
                        &m_culledObjects);

  m_ui->setMazeWidth(&m_mazeWidth);
  m_ui->setMazeHeight(&m_mazeHeight);
  m_ui->setMazeWallHeight(&m_wallHeight);
  m_ui->setMazeFloorThickness(&m_floorThickness);
  m_ui->setMazeCellSize(&m_cellSize);
  m_ui->setOnMazeRebuild([this]() { buildMaze(); });

  m_scene = std::make_unique<SceneManager>();
  m_player->setScene(m_scene.get());
  m_ui->setSceneObjects(&m_scene->objects());

  auto sun = std::make_unique<DirectionalLight>();
  sun->name = "Sun";
  sun->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
  sun->ambient = glm::vec3(0.1f);
  sun->diffuse = glm::vec3(0.4f);
  sun->specular = glm::vec3(0.5f);
  m_lights.push_back(std::move(sun));

  auto point = std::make_unique<PointLight>();
  point->name = "Point Light";
  point->position = glm::vec3(1.0f, 3.0f, 1.0f);
  point->ambient = glm::vec3(0.1f);
  point->diffuse = glm::vec3(0.8f);
  point->specular = glm::vec3(1.0f);
  m_lights.push_back(std::move(point));

  std::vector<std::string> faces{
      "src/assets/cubemap/right.png", "src/assets/cubemap/left.png",
      "src/assets/cubemap/top.png",   "src/assets/cubemap/bottom.png",
      "src/assets/cubemap/front.png", "src/assets/cubemap/back.png"};
  m_cubemap->load(faces);

  std::vector<Vertex> cubeVertices = {
      {{0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0, 0, -1}, {0, 0}},
      {{-0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0, 0, -1}, {1, 0}},
      {{-0.5f, 0.5f, -0.5f}, {1, 0, 0}, {0, 0, -1}, {1, 1}},
      {{-0.5f, 0.5f, -0.5f}, {1, 0, 0}, {0, 0, -1}, {1, 1}},
      {{0.5f, 0.5f, -0.5f}, {1, 0, 0}, {0, 0, -1}, {0, 1}},
      {{0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0, 0, -1}, {0, 0}},

      {{-0.5f, -0.5f, 0.5f}, {0, 1, 0}, {0, 0, 1}, {0, 0}},
      {{0.5f, -0.5f, 0.5f}, {0, 1, 0}, {0, 0, 1}, {1, 0}},
      {{0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0, 0, 1}, {1, 1}},
      {{0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0, 0, 1}, {1, 1}},
      {{-0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0, 0, 1}, {0, 1}},
      {{-0.5f, -0.5f, 0.5f}, {0, 1, 0}, {0, 0, 1}, {0, 0}},

      {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}, {-1, 0, 0}, {0, 1}},
      {{-0.5f, 0.5f, -0.5f}, {0, 0, 1}, {-1, 0, 0}, {1, 1}},
      {{-0.5f, -0.5f, -0.5f}, {0, 0, 1}, {-1, 0, 0}, {1, 0}},
      {{-0.5f, -0.5f, -0.5f}, {0, 0, 1}, {-1, 0, 0}, {1, 0}},
      {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}, {-1, 0, 0}, {0, 0}},
      {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}, {-1, 0, 0}, {0, 1}},

      {{0.5f, -0.5f, 0.5f}, {1, 1, 0}, {1, 0, 0}, {0, 0}},
      {{0.5f, -0.5f, -0.5f}, {1, 1, 0}, {1, 0, 0}, {1, 0}},
      {{0.5f, 0.5f, -0.5f}, {1, 1, 0}, {1, 0, 0}, {1, 1}},
      {{0.5f, 0.5f, -0.5f}, {1, 1, 0}, {1, 0, 0}, {1, 1}},
      {{0.5f, 0.5f, 0.5f}, {1, 1, 0}, {1, 0, 0}, {0, 1}},
      {{0.5f, -0.5f, 0.5f}, {1, 1, 0}, {1, 0, 0}, {0, 0}},

      {{-0.5f, -0.5f, -0.5f}, {1, 0, 1}, {0, -1, 0}, {0, 1}},
      {{0.5f, -0.5f, -0.5f}, {1, 0, 1}, {0, -1, 0}, {1, 1}},
      {{0.5f, -0.5f, 0.5f}, {1, 0, 1}, {0, -1, 0}, {1, 0}},
      {{0.5f, -0.5f, 0.5f}, {1, 0, 1}, {0, -1, 0}, {1, 0}},
      {{-0.5f, -0.5f, 0.5f}, {1, 0, 1}, {0, -1, 0}, {0, 0}},
      {{-0.5f, -0.5f, -0.5f}, {1, 0, 1}, {0, -1, 0}, {0, 1}},

      {{-0.5f, 0.5f, 0.5f}, {0, 1, 1}, {0, 1, 0}, {0, 0}},
      {{0.5f, 0.5f, 0.5f}, {0, 1, 1}, {0, 1, 0}, {1, 0}},
      {{0.5f, 0.5f, -0.5f}, {0, 1, 1}, {0, 1, 0}, {1, 1}},
      {{0.5f, 0.5f, -0.5f}, {0, 1, 1}, {0, 1, 0}, {1, 1}},
      {{-0.5f, 0.5f, -0.5f}, {0, 1, 1}, {0, 1, 0}, {0, 1}},
      {{-0.5f, 0.5f, 0.5f}, {0, 1, 1}, {0, 1, 0}, {0, 0}},
  };

  m_cubeMesh = std::make_unique<Mesh>(cubeVertices);

  m_textures.push_back(std::make_unique<Texture>(
      "src/assets/textures/liminal/backroom-wall.png"));
  m_textures.push_back(
      std::make_unique<Texture>("src/assets/textures/liminal/carpet.png"));

  buildMaze();

  Texture *wallTex = m_textures[0].get();

  m_rotatingCube = m_scene->addObject(
      m_cubeMesh.get(), nullptr, "Rotating Cube", "persistent",
      glm::vec3(5.0f, 0.5f, 5.0f), glm::vec3(0.5f));

  m_scene->addModel("src/assets/models/teapot/teapot.obj", "Teapot", nullptr,
                    "persistent", glm::vec3(8.0f, 7.0f, 3.0f),
                    glm::vec3(0.05f, 0.05f, 0.05f));

  m_scene->addModel("src/assets/models/radio/radio.fbx", "Radio", wallTex,
                    "persistent", glm::vec3(2.0f, 7.0f, 1.0f),
                    glm::vec3(0.2f, 0.2f, 0.2f));

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

  uint32_t gridHeight = grid.size();
  uint32_t gridWidth = grid[0].size();

  float totalWidth = gridWidth * m_cellSize;
  float totalDepth = gridHeight * m_cellSize;
  float centerX = (gridWidth - 1) * m_cellSize * 0.5f;
  float centerZ = (gridHeight - 1) * m_cellSize * 0.5f;

  m_scene->addObject(m_cubeMesh.get(), floorTex, "Floor", maze_prefix,
                     glm::vec3(centerX, -m_floorThickness * 0.5f, centerZ),
                     glm::vec3(totalWidth, m_floorThickness, totalDepth));

  for (uint32_t y = 0; y < gridHeight; y++) {
    for (uint32_t x = 0; x < gridWidth; x++) {
      if (grid[y][x] == 1) {
        float px = x * m_cellSize;
        float pz = y * m_cellSize;
        m_scene->addObject(m_cubeMesh.get(), wallTex, "Wall", maze_prefix,
                           glm::vec3(px, m_wallHeight * 0.5f, pz),
                           glm::vec3(1.0f, m_wallHeight, 1.0f));
      }
    }
  }
}

void Application::run() {
  float lastFrame = 0.0f;
  Frustum frustum;
  SpatialGrid spatialGrid(m_cellSize * 2.0f);

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

    m_ui->setUIPlayerState(m_player->isFlying(), m_player->isOnGround(),
                           m_player->transform.getPosition());

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

    // Frustum
    glm::mat4 vp =
        m_camera->getProjectionMatrix(aspect) * m_camera->getViewMatrix();
    frustum.update(vp, m_frustumMargin);

    m_visibleObjects = 0;
    m_culledObjects = 0;
    if (m_frustumEnabled) {
      for (auto &[id, obj] : m_scene->objects()) {
        if (!obj.active)
          continue;
        glm::vec3 pos = obj.transform.getPosition();
        glm::vec3 scale = obj.transform.getScale();
        AABB box{pos - scale * 0.5f, pos + scale * 0.5f};
        if (frustum.isAABBInside(box))
          m_visibleObjects++;
        else
          m_culledObjects++;
      }
    } else {
      for (auto &[id, obj] : m_scene->objects()) {
        if (obj.active)
          m_visibleObjects++;
      }
    }

    // Scene objects
    m_shader->use();

    m_shader->setFloat("shininess", m_shininess);
    m_shader->setVec3("viewPos", m_camera->getPosition());

    char p[64];
    int dirCount = 0, pointCount = 0, spotCount = 0;
    for (auto &light : m_lights) {
      if (!light->enabled)
        continue;
      switch (light->type) {
      case Light::DIRECTIONAL: {
        auto *dl = static_cast<DirectionalLight *>(light.get());
        snprintf(p, sizeof(p), "dirLights[%d]", dirCount);
        std::string prefix(p);
        m_shader->setVec3(prefix + ".direction", dl->direction);
        m_shader->setVec3(prefix + ".ambient", dl->ambient);
        m_shader->setVec3(prefix + ".diffuse", dl->diffuse);
        m_shader->setVec3(prefix + ".specular", dl->specular);
        dirCount++;
        break;
      }
      case Light::POINT: {
        auto *pl = static_cast<PointLight *>(light.get());
        snprintf(p, sizeof(p), "pointLights[%d]", pointCount);
        std::string prefix(p);
        m_shader->setVec3(prefix + ".position", pl->position);
        m_shader->setFloat(prefix + ".constant", pl->constant);
        m_shader->setFloat(prefix + ".linear", pl->linear);
        m_shader->setFloat(prefix + ".quadratic", pl->quadratic);
        m_shader->setVec3(prefix + ".ambient", pl->ambient);
        m_shader->setVec3(prefix + ".diffuse", pl->diffuse);
        m_shader->setVec3(prefix + ".specular", pl->specular);
        pointCount++;
        break;
      }
      case Light::SPOT: {
        auto *sl = static_cast<SpotLight *>(light.get());
        snprintf(p, sizeof(p), "spotLights[%d]", spotCount);
        std::string prefix(p);
        m_shader->setVec3(prefix + ".position", sl->position);
        m_shader->setVec3(prefix + ".direction", sl->direction);
        m_shader->setFloat(prefix + ".cutOff", sl->cutOff);
        m_shader->setFloat(prefix + ".outerCutOff", sl->outerCutOff);
        m_shader->setFloat(prefix + ".constant", sl->constant);
        m_shader->setFloat(prefix + ".linear", sl->linear);
        m_shader->setFloat(prefix + ".quadratic", sl->quadratic);
        m_shader->setVec3(prefix + ".ambient", sl->ambient);
        m_shader->setVec3(prefix + ".diffuse", sl->diffuse);
        m_shader->setVec3(prefix + ".specular", sl->specular);
        spotCount++;
        break;
      }
      }
    }
    m_shader->setInt("numDirLights", dirCount);
    m_shader->setInt("numPointLights", pointCount);
    m_shader->setInt("numSpotLights", spotCount);

    m_shader->setMat4("view", m_camera->getViewMatrix());
    m_shader->setMat4("projection", m_camera->getProjectionMatrix(aspect));

    m_scene->render(*m_shader, m_frustumEnabled ? &frustum : nullptr);

    // Rebuild spatial grid
    spatialGrid.clear();
    m_scene->buildSpatialGrid(spatialGrid);
    m_player->setSpatialGrid(&spatialGrid);

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
