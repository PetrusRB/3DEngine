#include "input.h"

namespace Engine {

InputManager::InputManager(GLFWwindow* window) : m_window(window) {
    glfwGetCursorPos(m_window, &m_lastMouse.x, &m_lastMouse.y);
}

bool InputManager::isKeyPressed(int key) const {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool InputManager::isKeyHeld(int key) const {
    return glfwGetKey(m_window, key) != GLFW_RELEASE;
}

bool InputManager::isKeyJustPressed(int key) {
    bool pressed = glfwGetKey(m_window, key) == GLFW_PRESS;
    bool justPressed = pressed && !m_prevKeyState[key];
    m_prevKeyState[key] = pressed;
    return justPressed;
}

bool InputManager::isMouseButtonPressed(int button) const {
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

glm::dvec2 InputManager::getMousePosition() const {
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return {x, y};
}

void InputManager::updateMouseDelta() {
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);

    if (m_firstMouse) {
        m_lastMouse = {x, y};
        m_firstMouse = false;
    }

    m_mouseDelta = {x - m_lastMouse.x, m_lastMouse.y - y};
    m_lastMouse = {x, y};
}

glm::dvec2 InputManager::getMouseDelta() const {
    return m_mouseDelta;
}

void InputManager::disableCursor() const {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputManager::enableCursor() const {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

} // namespace Engine
