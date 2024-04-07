#include "CameraControls.h"

#include <tuple>

CameraControls* CameraControls::instance = nullptr;

CameraControls* CameraControls::GetInstance()
{
    if(instance == nullptr)
    {
        instance = new CameraControls();
    }
    
    return instance;
}

void CameraControls::updateCameraRotation(float x, float y)
{
    constexpr float sensitivity = 0.1f;
    x *= sensitivity;
    y *= sensitivity;

    yaw += x;
    pitch += y;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_lookat = glm::normalize(direction);
}

void CameraControls::Lerp(float time) {
    time += lerp_speed;
    camera_position = (1.0f - time) * camera_position + time * target_position;
}

std::tuple<glm::mat4, glm::mat4> CameraControls::SetVP(const float x, const float y, const int width, const int height)
{
    // Create rotation matrix
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), x, glm::vec3(0.0f, 1.0f, 0.0f));
    rotation = glm::rotate(rotation, y, glm::vec3(1.0f, 0.0f, 0.0f));

    // Apply rotation to camera_lookat vector
    glm::vec4 rotated_lookat = rotation * glm::vec4(camera_lookat, 0.0f);
    glm::vec3 new_lookat(rotated_lookat);

    glm::mat4 view = glm::lookAt(target_position, target_position + new_lookat, camera_up);
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        1.0f * width / height, 0.1f,
        20.0f);

    return {view, projection};
}
