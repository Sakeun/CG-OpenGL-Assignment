#pragma once
#include <glm/gtc/matrix_transform.hpp>

#include "Animation.h"
#include "../../Structs.h"

class RotateAnimation : public Animation
{
private:
    // Variable to check the max degrees the animmation can reach
    float x_degrees;
    float y_degrees;
    float z_degrees;

    // Variables to keep track of the current degrees of the animatoin
    float current_x_degrees;
    float current_y_degrees;
    float current_z_degrees;

    // Variables used to determine which axis to rotate on
    glm::vec3 x_rotation;
    glm::vec3 y_rotation;
    glm::vec3 z_rotation;

    float x_direction = 1.0f;
    float y_direction = 1.0f;
    float z_direction = 1.0f;

    void rotate_x(glm::mat4& model);
    void rotate_y(glm::mat4& model);
    void rotate_z(glm::mat4& model);

public:
    RotateAnimation(float x = 0.0f, float y = 0.0f, float z = 0.0f, bool isLoop = true);
    virtual void set_degrees(float x, float y, float z) override;
    virtual void update_object(ObjectProperties& object) override;
    virtual void update_model(glm::mat4& model) override;
};
