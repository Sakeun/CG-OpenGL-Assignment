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
    // Calculate the change in yaw and pitch
    float deltaYaw = x * sensitivity;
    float deltaPitch = y * sensitivity;

    if (abs(deltaYaw) > 45.0f || abs(deltaPitch) > 45.0f)
        return;

    // Add the change in yaw and pitch to the current yaw and pitch
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

std::tuple<glm::mat4, glm::mat4> CameraControls::SetVP(const float x, const float y, const int width, const int height)
{
    glm::vec3 camera_lookat = getCameraLookat();
    glm::vec3 target_position = getTargetPosition();
    glm::vec3 camera_up = getCameraUp();
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
        100.0f);

    return {view, projection};
}

void CameraControls::updateTargetPosition(glm::vec3 targetPos) {
    if (isWalk) {
        if(isUpstairs)
            upstairs_cam.target_position = targetPos;
        else
            walk_mode_cam.target_position = targetPos;
    }
    else {
        drone_mode_cam.target_position = targetPos;
    }
}

glm::vec3 CameraControls::getTargetPosition() {
    if(isUpstairs && isWalk)
        return upstairs_cam.target_position;
    if (isWalk)
        return walk_mode_cam.target_position;
    return drone_mode_cam.target_position;
}

glm::vec3 CameraControls::getCameraUp() {
    if(isUpstairs && isWalk)
        return upstairs_cam.camera_up;
    if (isWalk)
        return walk_mode_cam.camera_up;
    return drone_mode_cam.camera_up;
}

glm::vec3 CameraControls::getCameraLookat() {
    if(isUpstairs && isWalk)
        return upstairs_cam.camera_lookat;
    if (isWalk)
        return walk_mode_cam.camera_lookat;
    return drone_mode_cam.camera_lookat;
}

glm::vec3 CameraControls::getCameraPosition() {
    if(isUpstairs && isWalk)
        return upstairs_cam.camera_position;
    if (isWalk)
        return walk_mode_cam.camera_position;
    return drone_mode_cam.camera_position;
}

void CameraControls::setCameraUp(glm::vec3 newValue) {
    if (isWalk) {
        if(isUpstairs)
            upstairs_cam.camera_up = newValue;
        else
            walk_mode_cam.camera_up = newValue;
    }
    else {
        drone_mode_cam.camera_up = newValue;
    }
}
void CameraControls::setCameraLookat(glm::vec3 newValue) {
    if (isWalk) {
        if(isUpstairs)
            upstairs_cam.camera_lookat = newValue;
        else
            walk_mode_cam.camera_lookat = newValue;
    }
    else {
        drone_mode_cam.camera_lookat = newValue;
    }
}
void CameraControls::setCameraPosition(glm::vec3 newValue) {
    if (isWalk) {
        if(isUpstairs)
            upstairs_cam.camera_position = newValue;
        else
            walk_mode_cam.camera_position = newValue;
    }
    else {
        drone_mode_cam.camera_position = newValue;
    }
}

void CameraControls::setTargetPosition(glm::vec3 newValue) {
    if (isWalk) {
        newValue.y = getTargetPosition().y;
        if(isUpstairs)
            upstairs_cam.target_position = newValue;
        else
            walk_mode_cam.target_position = newValue;
    }
    else {
        drone_mode_cam.target_position = newValue;
    }
}

void CameraControls::ToggleCameraMode(unsigned char key)
{
    if(key == '1' && isWalk)
    {
        isUpstairs = false;
    }
    else if(key == '2' && isWalk)
    {
        isUpstairs = true;
    }
    else if(key == 'v')
    {
        isWalk = !isWalk;
    }
}

void CameraControls::SetYawPitch(float& yaw, float& pitch, glm::vec3 newValue)
{
    yaw = glm::degrees(atan2(newValue.z, newValue.x));
    pitch = glm::degrees(asin(newValue.y));
}

float* CameraControls::getYaw()
{
    if(isUpstairs && isWalk)
        return &upstairs_cam.yaw;
    if (isWalk)
        return &walk_mode_cam.yaw;
    return &drone_mode_cam.yaw;
}

float* CameraControls::getPitch()
{
    if(isUpstairs && isWalk)
        return &upstairs_cam.pitch;
    if (isWalk)
        return &walk_mode_cam.pitch;
    return &drone_mode_cam.pitch;
}
