#pragma once
#include <string>

struct GLFWwindow;

namespace Engine {

struct WindowConfig {
  int width = 800;
  int height = 600;
  std::string title = "Engine";
};

class Window {
public:
  explicit Window(const WindowConfig &config);
  ~Window();

  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  void update();
  bool shouldClose() const;

  GLFWwindow *getNativeWindow() const { return m_window; }
  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }

private:
  GLFWwindow *m_window = nullptr;
  int m_width;
  int m_height;
};

} // namespace Engine
