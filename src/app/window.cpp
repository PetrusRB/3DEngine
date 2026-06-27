#include "./window.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdexcept>

namespace Engine {

static void framebufferSizeCallback(GLFWwindow * /*window*/, int width,
                                     int height) {
  glViewport(0, 0, width, height);
}

Window::Window(const WindowConfig &config)
    : m_width(config.width), m_height(config.height) {
  if (!glfwInit())
    throw std::runtime_error("Falha ao inicializar o GLFW");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_window = glfwCreateWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
  if (!m_window) {
    glfwTerminate();
    throw std::runtime_error("Falha ao criar a janela");
  }

  glfwMakeContextCurrent(m_window);
  glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwDestroyWindow(m_window);
    glfwTerminate();
    throw std::runtime_error("Falha ao inicializar GLAD");
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void Window::update() {
  glfwSwapBuffers(m_window);
  glfwPollEvents();
}

bool Window::shouldClose() const {
  return glfwWindowShouldClose(m_window);
}

Window::~Window() {
  if (m_window) {
    glfwDestroyWindow(m_window);
    m_window = nullptr;
  }
  glfwTerminate();
}

} // namespace Engine
