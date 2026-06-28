#pragma once

#define GLFW_INCLUDE_NONE
#include "../light/light.h"
#include "../output/output.h"
#include "ui_comps/ui_component.h"
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

enum class Anchor {
  TopLeft,
  TopCenter,
  TopRight,
  Left,
  Center,
  Right,
  BottomLeft,
  BottomCenter,
  BottomRight
};

struct UITab {
  std::string label;
  std::function<void()> render;
};

class UI {
public:
  bool init(GLFWwindow *window);
  void render();
  void destroy();

  void renderHeader();
  void renderMain();

  void set_main_visible(bool val) { _main_toggled = val; }

  void setShininess(float *val);
  void setZenith(glm::vec3 *zen) { zenith = zen; }
  void setHorizon(glm::vec3 *hor) { horizon = hor; }
  void setGround(glm::vec3 *gro) { ground = gro; }

  void setLights(std::vector<std::unique_ptr<Light>> *lights);
  void setOutputTerm(OutputTerm *output);
  void setSceneObjects(std::unordered_map<ObjectID, SceneObject> *objects);

  void setMazeWidth(int *w);
  void setMazeHeight(int *h);
  void setMazeWallHeight(float *h);
  void setMazeFloorThickness(float *t);
  void setMazeCellSize(float *s);
  void setOnMazeRebuild(std::function<void()> callback);

  void setUIPlayerState(bool flying, bool onGround, const glm::vec3 &pos);
  void setFrustumDebug(bool *enabled, float *margin, int *visible, int *culled);
  void setProceduralSky(bool *val) { m_proceduralSky = val; }
  void setCollisionDebug(bool *val) { m_collisionDebug = val; }
  void setMoney(int cents) { m_money = cents; }
  void renderHUD();

  template <typename T, typename... Args> T &addUIComponent(Args &&...args) {
    auto comp = std::make_unique<T>(std::forward<Args>(args)...);
    T &ref = *comp;
    m_components.push_back(std::move(comp));
    return ref;
  }

  template <typename T> T *getUIComponent() {
    for (auto &c : m_components) {
      T *p = dynamic_cast<T *>(c.get());
      if (p)
        return p;
    }
    return nullptr;
  }

private:
  void addTab(const std::string &label, std::function<void()> render);
  void renderTabScene();
  void renderTabLighting();
  void renderTabDebug();
  void renderTabOutput();
  void renderTabMaze();

  ImVec2 getAnchorPos(Anchor anchor, const ImVec2 &windowSize,
                      const ImVec2 &padding);
  int m_money = 0;
  void applyStyle(float main_scale);

  GLFWwindow *m_window = nullptr;
  float *m_shininess = nullptr;
  glm::vec3 *zenith = nullptr;
  glm::vec3 *horizon = nullptr;
  glm::vec3 *ground = nullptr;

  std::vector<UITab> m_tabs;
  std::vector<std::unique_ptr<Light>> *m_lights = nullptr;
  std::unordered_map<ObjectID, SceneObject> *m_sceneObjects = nullptr;
  OutputTerm *m_output = nullptr;

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
  bool *m_proceduralSky = nullptr;
  bool *m_collisionDebug = nullptr;
  std::vector<std::unique_ptr<UIComponent>> m_components;
};

} // namespace Engine
