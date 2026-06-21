#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

enum CameraMovement {
    CAM_FORWARD,
    CAM_BACKWARD,
    CAM_LEFT,
    CAM_RIGHT,
    CAM_UP,
    CAM_DOWN
};

class Camera {
public:
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f));

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect, float near = 0.1f, float far = 100.0f) const;

    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouse(float xoffset, float yoffset);

    void setPosition(const glm::vec3 &position);

    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    glm::vec3 getRight() const;
    float getFov() const;

private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;

    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    float m_speed = 5.0f;
    float m_sensitivity = 0.1f;
    float m_fov = 45.0f;

    void updateVectors();
};

} // namespace Engine
