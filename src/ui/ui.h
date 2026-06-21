#pragma once

#define GLFW_INCLUDE_NONE
#include "../light/light.h"
#include <GLFW/glfw3.h>
#include <functional>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {

using ObjectID = uint32_t;
struct SceneObject;

class UI {
public:
  bool init(GLFWwindow *window);
  void render();
  void destroy();

  void renderHeader();
  void renderMain();

  void set_main_visible(bool val) { _main_toggled = val; }

  void setShininess(float *val);
  void setLights(std::vector<std::unique_ptr<Light>> *lights);
  void setSceneObjects(std::unordered_map<ObjectID, SceneObject> *objects);

  void setMazeWidth(int *w);
  void setMazeHeight(int *h);
  void setMazeWallHeight(float *h);
  void setMazeFloorThickness(float *t);
  void setMazeCellSize(float *s);
  void setOnMazeRebuild(std::function<void()> callback);

  void setUIPlayerState(bool flying, bool onGround, const glm::vec3 &pos);

  void setFrustumDebug(bool *enabled, float *margin, int *visible, int *culled);

private:
  void applyStyle(float main_scale);

  GLFWwindow *m_window = nullptr;
  float *m_shininess = nullptr;
  std::vector<std::unique_ptr<Light>> *m_lights = nullptr;
  std::unordered_map<ObjectID, SceneObject> *m_sceneObjects = nullptr;
  int *m_mazeWidth = nullptr;
  int *m_mazeHeight = nullptr;
  float *m_wallHeight = nullptr;
  float *m_floorThickness = nullptr;
  float *m_cellSize = nullptr;
  std::function<void()> m_onMazeRebuild;
  bool m_headerOpen = true;
  bool _main_toggled;

  bool u_playerFlying = false;
  bool u_playerOnGround = false;
  glm::vec3 m_playerPos{0.0f};

  bool *m_frustumEnabled = nullptr;
  float *m_frustumMargin = nullptr;
  int *m_visibleObjects = nullptr;
  int *m_culledObjects = nullptr;
};

} // namespace Engine
