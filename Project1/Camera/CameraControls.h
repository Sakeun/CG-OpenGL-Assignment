#pragma once
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class CameraControls
{
private:
    static CameraControls* instance;
    CameraControls()
    {
        yaw = -90.0f;
        pitch = 0.0f;
        camera_position = glm::vec3(0.0, 0.5, 3.0);
        camera_lookat = glm::vec3(0.0, 0.0, -1.0);
        camera_up = glm::vec3(0.0, 1.0, 0.0);
        target_position = camera_position;
    }
    float yaw;
    float pitch;
    const float lerp_speed = 0.1f;
    
public:
    glm::vec3 camera_position;
    glm::vec3 camera_lookat;
    glm::vec3 camera_up;
    glm::vec3 target_position;
    
    CameraControls(CameraControls const&) = delete;
    void operator=(CameraControls const&) = delete;
    
    static CameraControls* GetInstance();
    void updateCameraRotation(float x, float y);
    std::tuple<glm::mat4, glm::mat4> SetVP(const float x, const float y, const int width, const int height);
    void Lerp(float time);

};
