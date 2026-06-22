#include "menu_pause.h"
#include "../utils/input.h"
#include <imgui/imgui.h>

namespace Engine {

MenuPause::MenuPause() = default;

void MenuPause::update(InputManager &input) {
  bool escapeDown = input.isKeyHeld(GLFW_KEY_ESCAPE);
  bool justPressed = escapeDown && !m_prevEscapeState;
  m_prevEscapeState = escapeDown;

  m_wasPaused = m_paused;

  if (justPressed) {
    if (m_showSettings) {
      m_showSettings = false;
    } else {
      m_paused = !m_paused;
    }
  }
}

void MenuPause::render() {
  if (!m_paused)
    return;

  ImGuiIO &io = ImGui::GetIO();
  ImGui::SetNextWindowPos(
      ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
      ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_FirstUseEver);

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoCollapse;

  if (m_showSettings) {
    renderSettings();
  } else {
    renderPauseMenu();
  }
}

void MenuPause::renderPauseMenu() {
  ImGui::Begin("Pausado", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse);

  float buttonWidth = ImGui::GetContentRegionAvail().x;

  if (ImGui::Button("Continuar", ImVec2(buttonWidth, 35))) {
    m_paused = false;
  }

  if (ImGui::Button("Configuracoes", ImVec2(buttonWidth, 35))) {
    m_showSettings = true;
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button("Sair", ImVec2(buttonWidth, 35))) {
    m_quitRequested = true;
  }

  ImGui::End();
}

void MenuPause::renderSettings() {
  ImGui::Begin("Configuracoes", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse);

  ImGui::Text("Graphics");
  ImGui::Separator();
  ImGui::SliderFloat("Render Distance", &m_settings.renderDistance, 20.0f,
                     500.0f, "%.0f m");
  ImGui::SliderFloat("Fog Start", &m_settings.fogStart, 10.0f, 400.0f);
  ImGui::SliderFloat("Fog End", &m_settings.fogEnd, 20.0f, 500.0f);
  ImGui::ColorEdit3("Fog Color", &m_settings.fogColor.x,
                    ImGuiColorEditFlags_NoInputs);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button("Voltar", ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
    m_showSettings = false;
  }

  ImGui::End();
}

bool MenuPause::isPaused() const { return m_paused; }

bool MenuPause::wasPaused() const { return m_wasPaused; }

bool MenuPause::shouldQuit() const { return m_quitRequested; }

const GameSettings &MenuPause::settings() const { return m_settings; }

} // namespace Engine
