#pragma once
#include <glm/gtc/matrix_transform.hpp>

class Animation
{
private:
    const float x_degrees;
    const float y_degrees;
    const float z_degrees;
    bool isLoop;
    bool completed;

    float current_x_degrees;
    float current_y_degrees;
    float current_z_degrees;

    glm::vec3 x_rotation;
    glm::vec3 y_rotation;
    glm::vec3 z_rotation;

    float x_direction = 1.0f;
    float y_direction = 1.0f;
    float z_direction = 1.0f;

    void RotateX(glm::mat4 &model);
    void RotateY(glm::mat4 &model);
    void RotateZ(glm::mat4 &model);
    
public:
    Animation(float x = 0.0f, float y = 0.0f, float z = 0.0f, bool isLoop = true);
    void Execute(glm::mat4 &model);
    bool IsCompleted() const { return completed; }
};
