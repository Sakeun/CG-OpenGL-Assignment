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
    float yaw;
    float pitch;
};

class CameraControls
{
private:
    static CameraControls* instance;
    const float lerp_speed = 0.1f;
    CameraPositions walk_mode_cam;
    CameraPositions drone_mode_cam;
    CameraPositions upstairs_cam;
    
    CameraControls()
    {
        // Set the default positions per camera mode and initialize the yaw and pitch to match the view direction
        walk_mode_cam.camera_position = glm::vec3(0.0, 0.5, 3.0);
        walk_mode_cam.camera_lookat = glm::normalize(glm::vec3(3.0, 0.0, -1.0));
        walk_mode_cam.camera_up = glm::vec3(0.0, 1.0, 0.0);
        walk_mode_cam.target_position = walk_mode_cam.camera_position;
        SetYawPitch(walk_mode_cam.yaw, walk_mode_cam.pitch, glm::normalize(walk_mode_cam.camera_lookat));

        drone_mode_cam.camera_position = glm::vec3(15.0, 2.5, 10.0);
        drone_mode_cam.camera_lookat = glm::normalize(glm::vec3(-5.0, -1.0, -3.0));
        drone_mode_cam.camera_up = glm::vec3(0.0, 1.0, 0.0);
        drone_mode_cam.target_position = drone_mode_cam.camera_position;
        SetYawPitch(drone_mode_cam.yaw, drone_mode_cam.pitch, glm::normalize(drone_mode_cam.camera_lookat));

        upstairs_cam.camera_position = glm::vec3(-10.0, 4.5, 10.0);
        upstairs_cam.camera_lookat = glm::normalize(glm::vec3(3.0, 0.0, -1.0));
        upstairs_cam.camera_up = glm::vec3(0.0, 1.0, 0.0);
        upstairs_cam.target_position = upstairs_cam.camera_position;
        SetYawPitch(upstairs_cam.yaw, upstairs_cam.pitch, glm::normalize(upstairs_cam.camera_lookat));
        
        isWalk = true;
        isUpstairs = false;
    }
    
public:
    bool isWalk;
    bool isUpstairs;
    
    CameraControls(CameraControls const&) = delete;
    void operator=(CameraControls const&) = delete;
    
    static CameraControls* GetInstance();
    void updateCameraRotation(float x, float y);
    void updateTargetPosition(glm::vec3 targetPos);
    void SetVP(glm::mat4&view, glm::mat4& projection, const float x, const float y, const int width, const int height);
    void Lerp(float time);

    glm::vec3 getTargetPosition();
    glm::vec3 getCameraUp();
    glm::vec3 getCameraLookat();
    glm::vec3 getCameraPosition();
    
    void setCameraUp(glm::vec3 newValue);
    void setCameraLookat(glm::vec3 newValue);
    void setCameraPosition(glm::vec3 newValue);
    void setTargetPosition(glm::vec3 newValue);
    void ToggleCameraMode(unsigned char key);

    void SetYawPitch(float& yaw, float& pitch, glm::vec3 newValue);
    float* getYaw();
    float* getPitch();
};
