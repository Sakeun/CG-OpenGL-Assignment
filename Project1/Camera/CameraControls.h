#pragma once
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct CameraPositions {
    glm::vec3 camera_position;
    glm::vec3 camera_lookat;
    glm::vec3 camera_up;
    glm::vec3 target_position;
};

class CameraControls
{
private:
    static CameraControls* instance;
    CameraControls()
    {
        yaw = -90.0f;
        pitch = 0.0f;
        walk_mode_cam.camera_position = glm::vec3(0.0, 0.5, 3.0);
        walk_mode_cam.camera_lookat = glm::vec3(3.0, 0.0, -1.0);
        walk_mode_cam.camera_up = glm::vec3(0.0, 1.0, 0.0);
        walk_mode_cam.target_position = walk_mode_cam.camera_position;

        drone_mode_cam.camera_position = glm::vec3(15.0, 2.5, 10.0);
        drone_mode_cam.camera_lookat = glm::vec3(-5.0, -1.0, -3.0);
        drone_mode_cam.camera_up = glm::vec3(0.0, 1.0, 0.0);
        drone_mode_cam.target_position = drone_mode_cam.camera_position;

        upstairs_cam.camera_position = glm::vec3(-10.0, 4.5, 10.0);
        upstairs_cam.camera_lookat = glm::vec3(3.0, 0.0, -1.0);
        upstairs_cam.camera_up = glm::vec3(0.0, 1.0, 0.0);
        upstairs_cam.target_position = upstairs_cam.camera_position;
        isWalk = true;
        isUpstairs = false;
    }
    float yaw;
    float pitch;
    const float lerp_speed = 0.1f;
    CameraPositions walk_mode_cam;
    CameraPositions drone_mode_cam;
    CameraPositions upstairs_cam;
    
public:
    bool isWalk;
    bool isUpstairs;
    
    CameraControls(CameraControls const&) = delete;
    void operator=(CameraControls const&) = delete;
    
    static CameraControls* GetInstance();
    void updateCameraRotation(float x, float y);
    void updateTargetPosition(glm::vec3 targetPos);
    std::tuple<glm::mat4, glm::mat4> SetVP(const float x, const float y, const int width, const int height);
    void Lerp(float time);

    glm::vec3 getTargetPosition();
    glm::vec3 getCameraUp();
    glm::vec3 getCameraLookat();
    glm::vec3 getCameraPosition();

    void setCameraUp(glm::vec3 newValue);
    void setCameraLookat(glm::vec3 newValue);
    void setCameraPosition(glm::vec3 newValue);
    void setTargetPosition(glm::vec3 newValue);
};
