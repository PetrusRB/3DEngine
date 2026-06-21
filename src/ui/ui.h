#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <functional>
#include <string>
#include <unordered_map>

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

  void setLightPos(glm::vec3 *pos);
  void setLightColor(glm::vec3 *color);
  void setAmbientStrength(float *val);
  void setSceneObjects(std::unordered_map<ObjectID, SceneObject> *objects);

  void setMazeWidth(int *w);
  void setMazeHeight(int *h);
  void setMazeWallHeight(float *h);
  void setMazeFloorThickness(float *t);
  void setMazeCellSize(float *s);
  void setOnMazeRebuild(std::function<void()> callback);

private:
  void applyStyle(float main_scale);

  GLFWwindow *m_window = nullptr;
  glm::vec3 *m_lightPos = nullptr;
  glm::vec3 *m_lightColor = nullptr;
  float *m_ambientStrength = nullptr;
  std::unordered_map<ObjectID, SceneObject> *m_sceneObjects = nullptr;
  int *m_mazeWidth = nullptr;
  int *m_mazeHeight = nullptr;
  float *m_wallHeight = nullptr;
  float *m_floorThickness = nullptr;
  float *m_cellSize = nullptr;
  std::function<void()> m_onMazeRebuild;
  bool m_headerOpen = true;
  bool _main_toggled;
};

} // namespace Engine
