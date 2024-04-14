#pragma once
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraMode {
    Downstairs,
    Upstairs,
    Drone
};

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
    CameraPositions downstairs_cam;
    CameraPositions drone_mode_cam;
    CameraPositions upstairs_cam;

    CameraPositions* active_cam;
    CameraMode active_mode;
    
    CameraControls()
    {
        // Set the default positions per camera mode and initialize the yaw and pitch to match the view direction
        downstairs_cam.camera_position = glm::vec3(0.0, 0.5, 3.0);
        downstairs_cam.camera_lookat = glm::normalize(glm::vec3(3.0, 0.0, -1.0));
        downstairs_cam.camera_up = glm::vec3(0.0, 1.0, 0.0);
        downstairs_cam.target_position = downstairs_cam.camera_position;
        set_yaw_pitch(downstairs_cam.yaw, downstairs_cam.pitch, glm::normalize(downstairs_cam.camera_lookat));

        drone_mode_cam.camera_position = glm::vec3(15.0, 2.5, 10.0);
        drone_mode_cam.camera_lookat = glm::normalize(glm::vec3(-5.0, -1.0, -3.0));
        drone_mode_cam.camera_up = glm::vec3(0.0, 1.0, 0.0);
        drone_mode_cam.target_position = drone_mode_cam.camera_position;
        set_yaw_pitch(drone_mode_cam.yaw, drone_mode_cam.pitch, glm::normalize(drone_mode_cam.camera_lookat));

        upstairs_cam.camera_position = glm::vec3(-10.0, 4.5, 10.0);
        upstairs_cam.camera_lookat = glm::normalize(glm::vec3(3.0, 0.0, -1.0));
        upstairs_cam.camera_up = glm::vec3(0.0, 1.0, 0.0);
        upstairs_cam.target_position = upstairs_cam.camera_position;
        set_yaw_pitch(upstairs_cam.yaw, upstairs_cam.pitch, glm::normalize(upstairs_cam.camera_lookat));

        active_cam = &downstairs_cam;
        active_mode = CameraMode::Downstairs;
    }
    
public:
    CameraControls(CameraControls const&) = delete;
    void operator=(CameraControls const&) = delete;
    
    static CameraControls* get_instance();
    void update_camera_rotation(float x, float y);
    void update_target_position(glm::vec3 targetPos);
    void set_vp(glm::mat4&view, glm::mat4& projection, const float x, const float y, const int width, const int height);
    void lerp(float time);

    glm::vec3 get_target_position();
    glm::vec3 get_camera_up();
    glm::vec3 get_camera_lookat();
    glm::vec3 get_camera_position();
    CameraMode get_active_mode();
    
    void set_camera_up(glm::vec3 newValue);
    void set_camera_lookat(glm::vec3 newValue);
    void set_camera_position(glm::vec3 newValue);
    void set_target_position(glm::vec3 newValue);
    void toggle_camera_mode(unsigned char key);

    void set_yaw_pitch(float& yaw, float& pitch, glm::vec3 newValue);
    float* get_yaw();
    float* get_pitch();
};
