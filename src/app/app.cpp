#include "app.h"
#include "../components/audio/audio.h"
#include "../utils/arena_wrapper.h"
#include <glm/ext/vector_float3.hpp>
#define GLFW_INCLUDE_NONE
#include "../algorithm/maze.h"
#include "../camera/camera.h"
#include "../components/collision/collision.h"
#include "../cubemap/cubemap.h"
#include "../debug/debug_draw.h"
#include "../economy/economy.h"
#include "../events/event_sys.h"
#include "../light/light.h"
#include "../output/output.h"
#include "../physics/spatial_grid.h"
#include "../player/player.h"
#include "../renderer/frustum.h"
#include "../renderer/mesh.h"
#include "../renderer/model.h"
#include "../renderer/scene.h"
#include "../renderer/shadow.h"
#include "../shader/shader.h"
#include "../texture/texture.h"
#include "../ui/menu_pause.h"
#include "../ui/ui.h"
#include "../ui/ui_comps/health_bar.h"
#include "../utils/input.h"
#include "../utils/utils.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <fmt/format.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <memory>
#include <stdexcept>

namespace Engine {

Application::Application(WindowConfig config) : m_config(std::move(config)) {
  init();
}

Application::~Application() { destroy(); }

void Application::init() {
  m_frameArena = arena_wrap_create(4 * 1024 * 1024);

  m_window = std::make_unique<Window>(m_config);
  GLFWwindow *native = m_window->getNativeWindow();

  AudioSystem::init();

  m_output = std::make_unique<OutputTerm>();
  m_ui = std::make_unique<UI>();
  m_ui->init(native);
  m_ui->setOutputTerm(m_output.get());

  m_menuPause = std::make_unique<MenuPause>();

  m_input = std::make_unique<InputManager>(native);
  m_cubemap = std::make_unique<Cubemap>();
  m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 1.7f, 0.0f));
  m_event = std::make_unique<EventSystem>();
  m_player = std::make_unique<Player>(*m_camera, *m_input, m_event.get());

  m_shader = std::make_unique<ShaderProgram>("src/assets/shaders/color.vs",
                                             "src/assets/shaders/color.fs");
  m_skyboxShader = std::make_unique<ShaderProgram>(
      "src/assets/shaders/cubemap.vs", "src/assets/shaders/cubemap.fs");
  m_proceduralSkyShader = std::make_unique<ShaderProgram>(
      "src/assets/shaders/cubemap.vs", "src/assets/shaders/procedural_sky.fs");

  m_depthShader =
      std::make_unique<ShaderProgram>("src/assets/shaders/shadow_depth.vs",
                                      "src/assets/shaders/shadow_depth.fs");

  m_shadowMap = std::make_unique<ShadowMap>();
  m_shadowMap->init(2048);

  m_ui->setShininess(&m_shininess);
  m_ui->setLights(&m_lights);
  m_ui->setZenith(&ps_zenith);
  m_ui->setHorizon(&ps_horizon);
  m_ui->setGround(&ps_ground);
  m_ui->setFrustumDebug(&m_frustumEnabled, &m_frustumMargin, &m_visibleObjects,
                        &m_culledObjects);
  m_ui->setProceduralSky(&m_useProceduralSky);

  m_ui->setMazeWidth(&m_mazeWidth);
  m_ui->setMazeHeight(&m_mazeHeight);
  m_ui->setMazeWallHeight(&m_wallHeight);
  m_ui->setMazeFloorThickness(&m_floorThickness);
  m_ui->setMazeCellSize(&m_cellSize);
  m_ui->setOnMazeRebuild([this]() {
    buildMaze();
    if (m_coins.empty()) {
      recreate_coins();
    }
  });
  m_utils = std::make_unique<Utils>();

  m_scene = std::make_unique<SceneManager>();
  m_player->setScene(m_scene.get());
  m_ui->setSceneObjects(&m_scene->objects());

  m_debugDraw = std::make_unique<DebugDraw>();
  m_debugDraw->init();
  m_ui->setCollisionDebug(&m_collisionDebug);

  auto &hpBar = m_ui->addUIComponent<HealthBar>(m_player.get());

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
  m_economy = std::make_unique<Economy>();

  // registra as texturas na lista
  // cria objetos do tipo textura
  m_textures.push_back(std::make_unique<Texture>(
      "src/assets/textures/liminal/backroom-wall.png"));

  m_textures.push_back(
      std::make_unique<Texture>("src/assets/textures/liminal/carpet.png"));

  m_textures.push_back(
      std::make_unique<Texture>("src/assets/textures/nature/lava002.jpg"));

  m_textures.push_back(std::make_unique<Texture>(
      "src/assets/textures/building_template/gray_grid.png"));

  m_textures.push_back(std::make_unique<Texture>(
      "src/assets/textures/liminal/celling.jpg")); // muito grande

  Texture *lava = m_textures[2].get();
  Texture *gray = m_textures[3].get();

  buildMaze();
  recreate_coins();

  ObjectID m_lava =
      m_scene->addObject(m_cubeMesh.get(), lava, "Kill", "persistent",
                         glm::vec3(9.0f, 0.5f, 7.0f), glm::vec3(0.5f));
  ObjectID teapotId = m_scene->addModel(
      "src/assets/models/teapot/teapot.obj", "Teapot", gray, "persistent",
      glm::vec3(8.0f, 7.0f, 3.0f), glm::vec3(0.05f, 0.05f, 0.05f));

  ObjectID ambientId =
      m_scene->addObject(m_cubeMesh.get(), nullptr, "AmbientSound",
                         "persistent", glm::vec3(0.0f), glm::vec3(0.0f));

  SceneObject *teapot = m_scene->getObject(teapotId);
  if (teapot) {
    auto &audio = teapot->addComponent<Audio>();
    audio.load("src/assets/audio/teapot.mp3");
    audio.setType(AudioType::Self);
    audio.setLoop(true);
    audio.setMaxDistance(50.0f);
    audio.setReferenceDistance(1.0f);
    audio.setGain(0.5f);
    audio.bindTransform(&teapot->transform.getPosition());
    audio.play();
  }

  // global
  SceneObject *ambient = m_scene->getObject(ambientId);
  if (ambient) {
    auto &audio = ambient->addComponent<Audio>();
    audio.load("src/assets/audio/ambient.mp3");
    audio.setType(AudioType::Global);
    audio.setLoop(true);
    audio.setGain(0.1f);
    audio.play();
  }

  // quando tocar na lava
  m_event->onCollision(m_lava, [this](Player &player, SceneObject &obj) {
    float now = static_cast<float>(glfwGetTime());
    if (now - m_lastLavaKill < m_coolKill) {
      return;
    }
    m_lastLavaKill = static_cast<float>(glfwGetTime());
    glm::vec3 curr_pos = player.transform.getPosition();
    std::string randomJk = m_utils->getRandomJoke();
    m_output->addLog(randomJk, static_cast<float>(glfwGetTime()));
    player.take_damage(15);
    // player.setTeleportPending(
    //     glm::vec3(curr_pos.x, curr_pos.y + 15.0f, curr_pos.z));
  });
  m_input->disableCursor();
}

void Application::recreate_coins() {
  for (auto &pos : coinPositions) {
    ObjectID coin = m_scene->addObject(m_cubeMesh.get(), nullptr, "Coin",
                                       "coin", pos, glm::vec3(0.5f));
    SceneObject *obj = m_scene->getObject(coin);
    if (obj) {
      auto &audio = obj->addComponent<Audio>();
      audio.load("src/assets/audio/drop-coin.mp3");
      audio.setType(AudioType::Global);
      audio.setLoop(false);
      audio.setGain(0.5f);
      audio.bindTransform(&obj->transform.getPosition());
    }
    m_coins.push_back(coin);
    m_event->onCollision(coin, [this](Player &player, SceneObject &obj) {
      if (auto *audio = obj.getComponent<Audio>())
        audio->play();
      obj.active = false;
      m_economy->addMoney(&player, 50);
      m_output->addLog(fmt::format("Pegou a moeda {}, o jogador ganhou {}",
                                   obj.id, player.getMoney()),
                       static_cast<float>(glfwGetTime()));
    });
  }
}

void Application::buildMaze() {
  m_scene->clearByTag(maze_prefix);

  MazeConfig cfg;
  cfg.width = m_mazeWidth;
  cfg.height = m_mazeHeight;
  auto grid = MazeGenerator::generate(cfg);

  Texture *wallTex = m_textures[0].get();
  Texture *floorTex = m_textures[1].get();
  Texture *ceelingTex = m_textures[4].get();

  uint32_t gridHeight = grid.size();   // quantidade de linhas
  uint32_t gridWidth = grid[0].size(); // quantidade de colunas

  // calcular o tamanho fisico do labirinto
  float totalWidth = gridWidth * m_cellSize;
  float totalDepth = gridHeight * m_cellSize;

  // encontra o centro do labirinto
  float centerX = (gridWidth - 1) * m_cellSize * 0.5f;
  float centerZ = (gridHeight - 1) * m_cellSize * 0.5f;

  m_scene->addObject(
      m_cubeMesh.get(), floorTex, "Floor", maze_prefix,
      glm::vec3(centerX, -m_floorThickness * 0.5f - 0.001f, centerZ),
      glm::vec3(totalWidth, m_floorThickness, totalDepth), glm::vec3(0.0f),
      glm::vec2(totalWidth / m_cellSize, totalWidth / m_cellSize), 0.0f);

  m_scene->addObject(
      m_cubeMesh.get(), ceelingTex, "Roof", maze_prefix,
      glm::vec3(centerX, m_wallHeight + m_floorThickness * 0.5f - 0.001f,
                centerZ),
      glm::vec3(totalWidth, m_floorThickness, totalDepth), glm::vec3(0.0f),
      glm::vec2(totalWidth / m_cellSize, totalWidth / m_cellSize), 0.0f);

  std::vector<std::vector<bool>> visited(gridHeight,
                                         std::vector<bool>(gridWidth, false));

  for (uint32_t y = 0; y < gridHeight; y++) {
    for (uint32_t x = 0; x < gridWidth; x++) {
      if (grid[y][x] != 1 || visited[y][x])
        continue;

      uint32_t runX = x;
      while (runX < gridWidth && grid[y][runX] == 1 && !visited[y][runX])
        runX++;
      uint32_t lenX = runX - x;

      uint32_t runY = y + 1;
      bool canGrowY = true;
      while (canGrowY && runY < gridHeight) {
        for (uint32_t cx = x; cx < runX; cx++) {
          if (grid[runY][cx] != 1 || visited[runY][cx]) {
            canGrowY = false;
            break;
          }
        }
        if (canGrowY)
          runY++;
      }
      uint32_t lenY = runY - y;

      for (uint32_t cy = y; cy < y + lenY; cy++)
        for (uint32_t cx = x; cx < x + lenX; cx++)
          visited[cy][cx] = true;

      float px = (x + (lenX - 1) * 0.5f) * m_cellSize;
      float pz = (y + (lenY - 1) * 0.5f) * m_cellSize;

      m_scene->addObject(m_cubeMesh.get(), wallTex, "Wall", maze_prefix,
                         glm::vec3(px, m_wallHeight * 0.5f, pz),
                         glm::vec3(lenX * 1.0f, m_wallHeight, lenY * 1.0f),
                         glm::vec3(0.0f), glm::vec2(lenX, lenY));
    }
  }
}

void Application::run() {
  float lastFrame = 0.0f;
  Frustum frustum;
  SpatialGrid spatialGrid(m_cellSize * 2.0f);

  while (!m_window->shouldClose()) {
    GLFWwindow *win = m_window->getNativeWindow();

    bool hasFocus = glfwGetWindowAttrib(win, GLFW_FOCUSED) &&
                    !glfwGetWindowAttrib(win, GLFW_ICONIFIED);

    if (hasFocus != !m_wasAFK) {
      bool isPaused = !hasFocus;
      m_wasAFK = isPaused;
      for (auto *a : Audio::activeSources())
        if (isPaused)
          a->pause();
        else
          a->play();
    }

    if (!hasFocus) {
      glfwWaitEventsTimeout(0.01);
      lastFrame = static_cast<float>(glfwGetTime());
      continue;
    }

    arena_wrap_reset(m_frameArena);

    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    m_menuPause->update(*m_input);

    if (m_menuPause->shouldQuit())
      glfwSetWindowShouldClose(m_window->getNativeWindow(), true);

    bool menuJustOpened = m_menuPause->isPaused() && !m_menuPause->wasPaused();
    bool menuJustClosed = !m_menuPause->isPaused() && m_menuPause->wasPaused();

    if (menuJustOpened) {
      m_input->enableCursor();
      m_cursorEnabled = true;
    } else if (menuJustClosed) {
      m_cursorEnabled = false;
      m_input->disableCursor();
    }

    if (m_input->isKeyJustPressed(GLFW_KEY_Q) && !m_menuPause->isPaused()) {
      m_cursorEnabled = !m_cursorEnabled;
      m_ui->set_main_visible(m_cursorEnabled);
      if (m_cursorEnabled)
        m_input->enableCursor();
      else
        m_input->disableCursor();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_cursorEnabled && !m_menuPause->isPaused())
      m_player->update(deltaTime);

    AudioSystem::updateListener(m_camera->getPosition(), m_camera->getFront(),
                                glm::vec3(0.0f, 1.0f, 0.0f));

    m_ui->setUIPlayerState(m_player->isFlying(), m_player->isOnGround(),
                           m_player->transform.getPosition());

    m_ui->setMoney(static_cast<int>(m_player->getMoney()));

    for (auto it = Audio::activeSources().begin();
         it != Audio::activeSources().end();) {
      Audio *audio = *it;
      audio->update();
      if (!audio->isPlaying()) {
        it = Audio::activeSources().erase(it);
        continue;
      }
      ++it;
    }

    for (auto it = m_coins.begin(); it != m_coins.end();) {
      SceneObject *coin = m_scene->getObject(*it);
      if (!coin) {
        it = m_coins.erase(it);
        continue;
      }
      if (coin->active) {
        coin->transform.rotate(glm::vec3(0.0f, 90.0f * deltaTime, 0.0f));
        ++it;
        continue;
      }
      auto *audio = coin->getComponent<Audio>();
      if (audio && !audio->isPlaying()) {
        m_event->remove(*it);
        m_scene->removeObject(*it);
        it = m_coins.erase(it);
        continue;
      }
      ++it;
    }

    int width, height;
    glfwGetFramebufferSize(m_window->getNativeWindow(), &width, &height);
    float aspect = (height > 0)
                       ? static_cast<float>(width) / static_cast<float>(height)
                       : 1.0f;

    // Skybox
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);

    if (m_useProceduralSky) {
      m_proceduralSkyShader->use();
      m_proceduralSkyShader->setMat4("view", m_camera->getViewMatrix());
      m_proceduralSkyShader->setMat4("projection",
                                     m_camera->getProjectionMatrix(aspect));
      for (auto &light : m_lights) {
        if (light->enabled && light->type == Light::DIRECTIONAL) {
          m_proceduralSkyShader->setVec3(
              "sunDir",
              static_cast<DirectionalLight *>(light.get())->direction);
          break;
        }
      }
      m_proceduralSkyShader->setFloat("time", currentFrame);
      m_proceduralSkyShader->setVec3("fogColor",
                                     m_menuPause->settings().fogColor);
      m_proceduralSkyShader->setVec3("zenith", ps_zenith);
      m_proceduralSkyShader->setVec3("horizon", ps_horizon);
      m_proceduralSkyShader->setVec3("ground", ps_ground);

      m_cubemap->render();
    } else {
      m_skyboxShader->use();
      m_skyboxShader->setMat4("view", m_camera->getViewMatrix());
      m_skyboxShader->setMat4("projection",
                              m_camera->getProjectionMatrix(aspect));
      m_skyboxShader->setInt("skybox", 0);
      m_cubemap->render();
    }

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);

    // Shadow pass
    glm::vec3 lightDir(0.0f);
    for (auto &light : m_lights) {
      if (light->enabled && light->type == Light::DIRECTIONAL) {
        lightDir = static_cast<DirectionalLight *>(light.get())->direction;
        break;
      }
    }
    m_shadowMap->begin(*m_depthShader, lightDir);
    m_scene->render(*m_depthShader, nullptr, 0.0f, glm::vec3(0.0f),
                    m_frameArena);
    m_shadowMap->end();

    glViewport(0, 0, width, height);

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
        glm::vec3 cameraPos = m_camera->getPosition();
        glm::vec3 baseDiffuse = pl->diffuse;
        float start = m_menuPause->settings().fadeStart;
        float end = m_menuPause->settings().fadeEnd;

        float maxLightDistance = end + 10.0f;
        float dist = glm::length(cameraPos - pl->position);
        if (dist > maxLightDistance) {
          continue;
        }
        float fade =
            1.0f - glm::clamp((dist - start) / (end - start), 0.0f, 1.0f);
        glm::vec3 mixedDiffuse = baseDiffuse * fade;
        std::string prefix(p);
        m_shader->setVec3(prefix + ".position", pl->position);
        m_shader->setFloat(prefix + ".constant", pl->constant);
        m_shader->setFloat(prefix + ".linear", pl->linear);
        m_shader->setFloat(prefix + ".quadratic", pl->quadratic);
        m_shader->setVec3(prefix + ".ambient", pl->ambient);
        m_shader->setVec3(prefix + ".diffuse", mixedDiffuse);
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

    m_shader->setFloat("fogStart", m_menuPause->settings().fogStart);
    m_shader->setFloat("fogEnd", m_menuPause->settings().fogEnd);
    m_shader->setVec3("fogColor", m_menuPause->settings().fogColor);

    m_shader->setMat4("lightSpaceMatrix", m_shadowMap->getLightSpaceMatrix());
    m_shadowMap->bind(1);
    m_shader->setInt("shadowMap", 1);
    m_shader->setFloat("shadowBias", 0.005f);

    m_scene->render(*m_shader, m_frustumEnabled ? &frustum : nullptr,
                    m_menuPause->settings().fogEnd + 20.0f,
                    m_camera->getPosition(), m_frameArena);

    // Debug collision shapes
    if (m_collisionDebug) {
      glDisable(GL_DEPTH_TEST);
      m_debugDraw->begin();

      for (auto &[id, obj] : m_scene->objects()) {
        if (!obj.active)
          continue;
        if (auto *col = obj.getComponent<Collision>()) {
          glm::vec3 c = col->isTrigger() ? glm::vec3(1.0f, 0.0f, 1.0f)
                                         : glm::vec3(0.0f, 1.0f, 0.0f);
          m_debugDraw->drawCollision(*col, obj.transform, c);
        }
      }

      m_debugDraw->drawSphere(m_player->transform.getPosition() +
                                  m_player->getCollider().getPosition(),
                              m_player->getPlayerRadius(), glm::vec3(1.0f));

      glm::mat4 vp =
          m_camera->getProjectionMatrix(aspect) * m_camera->getViewMatrix();
      m_debugDraw->end(m_camera->getViewMatrix(),
                       m_camera->getProjectionMatrix(aspect));
      glEnable(GL_DEPTH_TEST);
    }

    // Rebuild spatial grid
    spatialGrid.clear();
    m_scene->buildSpatialGrid(spatialGrid);
    m_player->setSpatialGrid(&spatialGrid);

    m_ui->render();
    m_ui->renderHUD();
    m_menuPause->render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    m_window->update();
  }
}

void Application::destroy() {
  m_ui.reset();
  m_menuPause.reset();
  m_output.reset();
  m_scene.reset();
  m_cubeMesh.reset();
  m_textures.clear();
  m_skyboxShader.reset();
  m_proceduralSkyShader.reset();
  m_shader.reset();
  m_depthShader.reset();
  m_shadowMap.reset();
  m_debugDraw.reset();
  m_player.reset();
  m_utils.reset();
  m_economy.reset();
  m_camera.reset();
  m_input.reset();
  m_cubemap.reset();

  arena_wrap_destroy(m_frameArena);

  AudioSystem::shutdown();

  m_window.reset();
}

} // namespace Engine
