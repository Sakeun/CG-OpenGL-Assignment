#include "CameraControls.h"

#include <tuple>

#include "../glsl.h"

CameraControls* CameraControls::instance = nullptr;

CameraControls* CameraControls::get_instance()
{
    if(instance == nullptr)
    {
        instance = new CameraControls();
    }
    
    return instance;
}

void CameraControls::update_camera_rotation(float x, float y)
{
    float* yaw = get_yaw();
    float* pitch = get_pitch();
    
    constexpr float sensitivity = 0.1f;
    float delta_yaw = x * sensitivity;
    float delta_pitch = y * sensitivity;

    if (abs(delta_yaw) > 45.0f || abs(delta_pitch) > 45.0f)
        return;

    *yaw += delta_yaw;
    *pitch += delta_pitch;

    if(*pitch > 89.0f)
        *pitch = 89.0f;
    if(*pitch < -89.0f)
        *pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(*yaw)) * cos(glm::radians(*pitch));
    direction.y = sin(glm::radians(*pitch));
    direction.z = sin(glm::radians(*yaw)) * cos(glm::radians(*pitch));

    set_camera_lookat(glm::normalize(direction));
}

void CameraControls::lerp(float time) {
    time += lerp_speed;
    glm::vec3 new_position = (1.0f - time) * get_camera_position() + time * get_target_position();
    set_camera_position(new_position);
}

void CameraControls::set_vp(glm::mat4& view, glm::mat4& projection, const float x, const float y, const int width, const int height)
{
    glm::vec3 camera_lookat = get_camera_lookat();
    glm::vec3 target_position = get_target_position();
    glm::vec3 camera_up = get_camera_up();
    
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

void CameraControls::toggle_camera_mode(unsigned char key)
{
    if(key == '1' && get_active_mode() == CameraMode::Upstairs)
    {
        active_mode = CameraMode::Downstairs;
        active_cam = &downstairs_cam;
    }
    else if(key == '2' && get_active_mode() == CameraMode::Downstairs)
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

void CameraControls::set_yaw_pitch(float& yaw, float& pitch, glm::vec3 newValue)
{
    yaw = glm::degrees(atan2(newValue.z, newValue.x));
    pitch = glm::degrees(asin(newValue.y));
}

CameraMode CameraControls::get_active_mode() {
    return active_mode;
}

// Getters and Setters based on active camera mode
void CameraControls::update_target_position(glm::vec3 targetPos) {
    active_cam->target_position = targetPos;
}

glm::vec3 CameraControls::get_target_position() {
    return active_cam->target_position;
}

glm::vec3 CameraControls::get_camera_up() {
    return active_cam->camera_up;
}

glm::vec3 CameraControls::get_camera_lookat() {
    return active_cam->camera_lookat;
}

glm::vec3 CameraControls::get_camera_position() {
    return active_cam->camera_position;
}

void CameraControls::set_camera_up(glm::vec3 newValue) {
    active_cam->camera_up = newValue;
}
void CameraControls::set_camera_lookat(glm::vec3 newValue) {
    active_cam->camera_lookat = newValue;

}
void CameraControls::set_camera_position(glm::vec3 newValue) {
    active_cam->camera_position = newValue;
}

void CameraControls::set_target_position(glm::vec3 newValue) {
    active_cam->target_position = newValue;
}

float* CameraControls::get_yaw()
{
    return &active_cam->yaw;
}

float* CameraControls::get_pitch()
{
    return &active_cam->pitch;
}