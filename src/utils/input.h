#pragma once
#define GLFW_INCLUDE_NONE
#include "../../include/GLFW/glfw3.h"
#include <glm/glm.hpp>

namespace Engine {

class InputManager {
public:
    explicit InputManager(GLFWwindow* window);

    bool isKeyPressed(int key) const;
    bool isKeyHeld(int key) const;
    bool isKeyJustPressed(int key);
    bool isMouseButtonPressed(int button) const;

    glm::dvec2 getMousePosition() const;
    void updateMouseDelta();
    glm::dvec2 getMouseDelta() const;

    void disableCursor() const;
    void enableCursor() const;

private:
    GLFWwindow* m_window;
    glm::dvec2 m_lastMouse = {0.0, 0.0};
    glm::dvec2 m_mouseDelta = {0.0, 0.0};
    bool m_firstMouse = true;
    bool m_prevKeyState[GLFW_KEY_LAST + 1] = {};
};

} // namespace Engine
