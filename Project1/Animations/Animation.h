#pragma once
#include <glm/gtc/matrix_transform.hpp>

class Animation
{
private:
    const float x_degrees;
    const float y_degrees;
    const float z_degrees;

    float current_x_degrees;
    float current_y_degrees;
    float current_z_degrees;

    float x_direction = 1.0f;
    float y_direction = 1.0f;
    float z_direction = 1.0f;

    void RotateX(glm::mat4 &model);
    void RotateY(glm::mat4 &model);
    void RotateZ(glm::mat4 &model);
    
public:
    Animation(float x = 0.0f, float y = 0.0f, float z = 0.0f);
    void Execute(glm::mat4 &model);
};
