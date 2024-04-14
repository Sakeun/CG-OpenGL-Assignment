#include "CameraControls.h"

#include <tuple>

#include "../glsl.h"

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
    float* yaw = getYaw();
    float* pitch = getPitch();
    
    constexpr float sensitivity = 0.1f;
    float deltaYaw = x * sensitivity;
    float deltaPitch = y * sensitivity;

    if (abs(deltaYaw) > 45.0f || abs(deltaPitch) > 45.0f)
        return;

    *yaw += deltaYaw;
    *pitch += deltaPitch;

    if(*pitch > 89.0f)
        *pitch = 89.0f;
    if(*pitch < -89.0f)
        *pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(*yaw)) * cos(glm::radians(*pitch));
    direction.y = sin(glm::radians(*pitch));
    direction.z = sin(glm::radians(*yaw)) * cos(glm::radians(*pitch));

    setCameraLookat(glm::normalize(direction));
}

void CameraControls::Lerp(float time) {
    time += lerp_speed;
    glm::vec3 newPosition = (1.0f - time) * getCameraPosition() + time * getTargetPosition();
    setCameraPosition(newPosition);
}

void CameraControls::SetVP(glm::mat4& view, glm::mat4& projection, const float x, const float y, const int width, const int height)
{
    glm::vec3 camera_lookat = getCameraLookat();
    glm::vec3 target_position = getTargetPosition();
    glm::vec3 camera_up = getCameraUp();
    
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), x, glm::vec3(0.0f, 1.0f, 0.0f));
    rotation = glm::rotate(rotation, y, glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec4 rotated_lookat = rotation * glm::vec4(camera_lookat, 0.0f);
    glm::vec3 new_lookat(rotated_lookat);

    view = glm::lookAt(target_position, target_position + new_lookat, camera_up);
    projection = glm::perspective(
        glm::radians(45.0f),
        1.0f * width / height, 0.1f,
        100.0f);
}

void CameraControls::ToggleCameraMode(unsigned char key)
{
    if(key == '1' && getActiveMode() == CameraMode::Upstairs)
    {
        active_mode = CameraMode::Downstairs;
        active_cam = &downstairs_cam;
    }
    else if(key == '2' && getActiveMode() == CameraMode::Downstairs)
    {
        active_mode = CameraMode::Upstairs;
        active_cam = &upstairs_cam;
    }
    else if(key == 'v')
    {
        active_mode = CameraMode::Drone;
        active_cam = &drone_mode_cam;
    }
}

void CameraControls::SetYawPitch(float& yaw, float& pitch, glm::vec3 newValue)
{
    yaw = glm::degrees(atan2(newValue.z, newValue.x));
    pitch = glm::degrees(asin(newValue.y));
}

CameraMode CameraControls::getActiveMode() {
    return active_mode;
}

// Getters and Setters based on active camera mode
void CameraControls::updateTargetPosition(glm::vec3 targetPos) {
    active_cam->target_position = targetPos;
}

glm::vec3 CameraControls::getTargetPosition() {
    return active_cam->target_position;
}

glm::vec3 CameraControls::getCameraUp() {
    return active_cam->camera_up;
}

glm::vec3 CameraControls::getCameraLookat() {
    return active_cam->camera_lookat;
}

glm::vec3 CameraControls::getCameraPosition() {
    return active_cam->camera_position;
}

void CameraControls::setCameraUp(glm::vec3 newValue) {
    active_cam->camera_up = newValue;
}
void CameraControls::setCameraLookat(glm::vec3 newValue) {
    active_cam->camera_lookat = newValue;

}
void CameraControls::setCameraPosition(glm::vec3 newValue) {
    active_cam->camera_position = newValue;
}

void CameraControls::setTargetPosition(glm::vec3 newValue) {
    active_cam->target_position = newValue;
}

float* CameraControls::getYaw()
{
    return &active_cam->yaw;
}

float* CameraControls::getPitch()
{
    return &active_cam->pitch;
}