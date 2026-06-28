#include "ui.h"
#include "../renderer/scene.h"
#include "../utils/utils.h"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>

namespace Engine {

static void zincStyleColors() {
  ImVec4 *colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.14f, 0.94f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
  colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.27f, 0.50f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.30f, 1.00f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.38f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.11f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.09f, 0.11f, 0.50f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.53f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.33f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.43f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.50f, 0.53f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.45f, 0.56f, 0.96f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.56f, 0.96f, 0.78f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.56f, 0.96f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.45f, 0.56f, 0.96f, 0.86f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.56f, 0.96f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.56f, 0.96f, 0.86f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.45f, 0.56f, 0.96f, 1.00f);
  colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.27f, 0.50f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.45f, 0.56f, 0.96f, 0.78f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.45f, 0.56f, 0.96f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.45f, 0.56f, 0.96f, 0.25f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.45f, 0.56f, 0.96f, 0.67f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.45f, 0.56f, 0.96f, 0.95f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.56f, 0.96f, 0.86f);
  colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.16f, 0.86f);
  colors[ImGuiCol_TabSelected] = ImVec4(0.20f, 0.22f, 0.28f, 1.00f);
  colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.45f, 0.56f, 0.96f, 1.00f);
  colors[ImGuiCol_TabDimmed] = ImVec4(0.12f, 0.12f, 0.14f, 0.97f);
  colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(0.22f, 0.22f, 0.24f, 1.00f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
  colors[ImGuiCol_TextLink] = ImVec4(0.45f, 0.56f, 0.96f, 1.00f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.45f, 0.56f, 0.96f, 0.35f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.45f, 0.56f, 0.96f, 0.95f);
  colors[ImGuiCol_NavCursor] = ImVec4(0.45f, 0.56f, 0.96f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

bool UI::init(GLFWwindow *window) {
  m_window = window;
  float main_scale =
      ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();
  applyStyle(main_scale);

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  addTab("Scene", [this] { renderTabScene(); });
  addTab("Lighting", [this] { renderTabLighting(); });
  addTab("Debug", [this] { renderTabDebug(); });
  addTab("Output", [this] { renderTabOutput(); });
  addTab("Maze", [this] { renderTabMaze(); });

  return true;
}

void UI::addTab(const std::string &label, std::function<void()> render) {
  m_tabs.push_back({label, std::move(render)});
}

void UI::render() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  renderHeader();
  if (_main_toggled)
    renderMain();
}

void UI::destroy() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void UI::renderHeader() {
  ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(260, 0), ImGuiCond_FirstUseEver);

  ImGui::Begin("##Header", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  ImGui::TextColored(ImVec4(0.45f, 0.56f, 0.96f, 1.0f), "Necros");
  ImGui::SameLine();
  ImGui::TextDisabled("(v1.0)");
  ImGui::End();
}

void UI::renderMain() {
  ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(260, 200), ImGuiCond_FirstUseEver);

  ImGui::Begin("Home");

  if (ImGui::BeginTabBar("##tabs")) {
    for (auto &tab : m_tabs) {
      if (ImGui::BeginTabItem(tab.label.c_str())) {
        tab.render();
        ImGui::EndTabItem();
      }
    }
    ImGui::EndTabBar();
  }

  ImGui::End();
}

void UI::renderTabScene() {
  if (!m_sceneObjects || m_sceneObjects->empty()) {
    ImGui::TextDisabled("No objects in scene");
    return;
  }

  static ImGuiTextFilter filter;
  filter.Draw("Search");

  for (auto &[id, obj] : *m_sceneObjects) {
    if (!filter.PassFilter(obj.name.c_str()))
      continue;

    char treeLabel[128];
    if (obj.name.empty())
      snprintf(treeLabel, sizeof(treeLabel), "Object %u", obj.id);
    else
      snprintf(treeLabel, sizeof(treeLabel), "%s [#%u]", obj.name.c_str(),
               obj.id);

    if (ImGui::TreeNode(treeLabel)) {
      obj.renderUI();
      ImGui::TreePop();
    }
    ImGui::Separator();
  }
}

void UI::renderTabLighting() {
  if (m_shininess)
    ImGui::SliderFloat("Shininess", m_shininess, 2.0f, 256.0f, "%.0f");

  if (zenith)
    ImGui::ColorEdit3("PS Zenith", &zenith->x);
  if (horizon)
    ImGui::ColorEdit3("PS Horizon", &horizon->x);
  if (ground)
    ImGui::ColorEdit3("PS Ground", &ground->x);

  ImGui::Separator();

  if (!m_lights)
    return;

  for (size_t i = 0; i < m_lights->size(); i++) {
    auto &light = (*m_lights)[i];
    ImGui::PushID(static_cast<int>(i));

    bool enabled = light->enabled;
    ImGui::Checkbox("##en", &enabled);
    light->enabled = enabled;
    ImGui::SameLine();

    if (ImGui::TreeNode(light->name.c_str())) {
      light->renderUI();
      ImGui::TreePop();
    }

    ImGui::SameLine(ImGui::GetWindowWidth() - 30);
    if (ImGui::Button("X")) {
      m_lights->erase(m_lights->begin() + i);
      i--;
    }

    ImGui::PopID();
    ImGui::Separator();
  }

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.45f, 0.25f, 1.0f));
  if (ImGui::Button("+ Directional"))
    m_lights->push_back(std::make_unique<DirectionalLight>());
  ImGui::SameLine();
  if (ImGui::Button("+ Point"))
    m_lights->push_back(std::make_unique<PointLight>());
  ImGui::SameLine();
  if (ImGui::Button("+ Spot"))
    m_lights->push_back(std::make_unique<SpotLight>());
  ImGui::PopStyleColor();
}

void UI::renderTabDebug() {
  ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
  ImGui::SameLine(ImGui::GetWindowWidth() - 80);
  ImGui::Text("%.2f ms", 1000.0f / ImGui::GetIO().Framerate);
  ImGui::Separator();

  ImGui::Text("Fly: %s", u_playerFlying ? "ON" : "OFF");
  ImGui::Text("Ground: %s", u_playerOnGround ? "YES" : "NO");
  ImGui::Text("Pos: (%.1f, %.1f, %.1f)", m_playerPos.x, m_playerPos.y,
              m_playerPos.z);
  ImGui::Separator();

  if (m_frustumEnabled) {
    ImGui::Checkbox("Frustum Culling", m_frustumEnabled);
    ImGui::SliderFloat("Margin", m_frustumMargin, 0.0f, 5.0f, "%.1f");
    ImGui::Text("Visible: %d", *m_visibleObjects);
    ImGui::Text("Culled:  %d", *m_culledObjects);
  }
  ImGui::Separator();

  if (m_proceduralSky)
    ImGui::Checkbox("Procedural Sky", m_proceduralSky);

  if (m_collisionDebug)
    ImGui::Checkbox("Collision Shapes", m_collisionDebug);
}

void UI::renderTabOutput() {
  if (!m_output) {
    ImGui::TextDisabled("No output");
    return;
  }

  int write = m_output->getWriteBuffer();
  std::string *buffer = m_output->getBuffer();
  int maxCapacity = m_output->getMaxCapacity();
  bool hasContent = false;

  for (int i = 0; i < maxCapacity; i++) {
    int idx = (write + i) % 5;
    if (!buffer[idx].empty()) {
      ImGui::Text("%s", buffer[idx].c_str());
      hasContent = true;
    }
  }

  if (!hasContent)
    ImGui::Text("Buffer vazio");
}

void UI::renderTabMaze() {
  if (m_mazeWidth)
    ImGui::SliderInt("Width", m_mazeWidth, 3, 51, "%d");
  if (m_mazeHeight)
    ImGui::SliderInt("Height", m_mazeHeight, 3, 51, "%d");
  if (m_cellSize)
    ImGui::SliderFloat("Cell Size", m_cellSize, 0.5f, 5.0f);
  if (m_wallHeight)
    ImGui::SliderFloat("Wall Height", m_wallHeight, 0.1f, 15.0f);
  if (m_floorThickness)
    ImGui::SliderFloat("Floor Thickness", m_floorThickness, 0.01f, 1.0f);

  if (ImGui::Button("Rebuild Maze") && m_onMazeRebuild)
    m_onMazeRebuild();
}

void UI::setShininess(float *val) { m_shininess = val; }
void UI::setOutputTerm(OutputTerm *output) { m_output = output; }
void UI::setLights(std::vector<std::unique_ptr<Light>> *lights) {
  m_lights = lights;
}
void UI::setSceneObjects(std::unordered_map<ObjectID, SceneObject> *objects) {
  m_sceneObjects = objects;
}
void UI::setMazeWidth(int *w) { m_mazeWidth = w; }
void UI::setMazeHeight(int *h) { m_mazeHeight = h; }
void UI::setMazeWallHeight(float *h) { m_wallHeight = h; }
void UI::setMazeFloorThickness(float *t) { m_floorThickness = t; }
void UI::setMazeCellSize(float *s) { m_cellSize = s; }
void UI::setOnMazeRebuild(std::function<void()> callback) {
  m_onMazeRebuild = callback;
}
void UI::setUIPlayerState(bool flying, bool onGround, const glm::vec3 &pos) {
  u_playerFlying = flying;
  u_playerOnGround = onGround;
  m_playerPos = pos;
}
void UI::setFrustumDebug(bool *enabled, float *margin, int *visible,
                         int *culled) {
  m_frustumEnabled = enabled;
  m_frustumMargin = margin;
  m_visibleObjects = visible;
  m_culledObjects = culled;
}

ImVec2 UI::getAnchorPos(Anchor anchor, const ImVec2 &windowSize,
                        const ImVec2 &padding) {
  ImVec2 display = ImGui::GetIO().DisplaySize;
  switch (anchor) {
  case Anchor::TopLeft:
    return padding;
  case Anchor::TopCenter:
    return ImVec2((display.x - windowSize.x) * 0.5f, padding.y);
  case Anchor::TopRight:
    return ImVec2(display.x - windowSize.x - padding.x, padding.y);
  case Anchor::Left:
    return ImVec2(padding.x, (display.y - windowSize.y) * 0.5f);
  case Anchor::Center:
    return ImVec2((display.x - windowSize.x) * 0.5f,
                  (display.y - windowSize.y) * 0.5f);
  case Anchor::Right:
    return ImVec2(display.x - windowSize.x - padding.x,
                  (display.y - windowSize.y) * 0.5f);
  case Anchor::BottomLeft:
    return ImVec2(padding.x, display.y - windowSize.y - padding.y);
  case Anchor::BottomCenter:
    return ImVec2((display.x - windowSize.x) * 0.5f,
                  display.y - windowSize.y - padding.y);
  case Anchor::BottomRight:
    return ImVec2(display.x - windowSize.x - padding.x,
                  display.y - windowSize.y - padding.y);
  }
  return padding;
}

void UI::renderHUD() {
  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
      ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav;

  ImVec2 pad(20.0f, 20.0f);

  std::string moneyText = Utils::formatCoin(m_money, Currency::USD);
  ImVec2 moneySize = ImGui::CalcTextSize(moneyText.c_str());
  ImVec2 moneyWinSize = ImVec2(moneySize.x + 24.0f, moneySize.y + 16.0f);

  ImGui::SetNextWindowPos(getAnchorPos(Anchor::TopRight, moneyWinSize, pad),
                          ImGuiCond_Always);
  ImGui::SetNextWindowSize(moneyWinSize, ImGuiCond_Always);
  ImGui::Begin("##hud_money", nullptr, flags);
  ImGui::SetCursorPosY((moneyWinSize.y - moneySize.y) * 0.5f);
  ImGui::Text("%s", moneyText.c_str());
  ImGui::End();

  for (auto &comp : m_components) {
    if (comp->visible) {
      ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
      ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
      ImGui::Begin("##hud_layer", nullptr, flags);
      comp->render();
      ImGui::End();
    }
  }
}

void UI::applyStyle(float main_scale) {
  ImGuiStyle &style = ImGui::GetStyle();

  style.WindowRounding = 12.0f;
  style.ChildRounding = 8.0f;
  style.FrameRounding = 8.0f;
  style.PopupRounding = 8.0f;
  style.ScrollbarRounding = 12.0f;
  style.GrabRounding = 8.0f;
  style.TabRounding = 8.0f;

  style.WindowBorderSize = 1.0f;
  style.ChildBorderSize = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.PopupBorderSize = 1.0f;

  style.WindowPadding = ImVec2(12.0f, 12.0f);
  style.FramePadding = ImVec2(10.0f, 6.0f);
  style.ItemSpacing = ImVec2(8.0f, 8.0f);
  style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
  style.ScrollbarSize = 14.0f;
  style.GrabMinSize = 10.0f;

  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;

  zincStyleColors();
}

} // namespace Engine
