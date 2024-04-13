#include "Animation.h"

Animation::Animation(const float x, const float y, const float z, const bool isLoop): x_degrees(x), y_degrees(y), z_degrees(z), isLoop(isLoop)
{
    current_x_degrees = 0.0f;
    current_y_degrees = 0.0f;
    current_z_degrees = 0.0f;
    x_rotation = x > 0 ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(-1.0f, 0.0f, 0.0f);
    y_rotation = y > 0 ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, -1.0f, 0.0f);
    z_rotation = z > 0 ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 0.0f, -1.0f);
    completed = false;
}

// Execute animation
void Animation::Execute(glm::mat4& model)
{
    // Extract scale and translation from model matrix
    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(model[0]));
    scale.y = glm::length(glm::vec3(model[1]));
    scale.z = glm::length(glm::vec3(model[2]));
    glm::vec3 translation = glm::vec3(model[3]);
    
    glm::mat4 x_matrix = glm::mat4(1.0f);
    glm::mat4 y_matrix = glm::mat4(1.0f);
    glm::mat4 z_matrix = glm::mat4(1.0f);
    
    if(x_degrees != 0.0f)
        RotateX(x_matrix);
    if(y_degrees != 0.0f)
        RotateY(y_matrix);
    if(z_degrees != 0.0f)
        RotateZ(z_matrix);
    if(completed) return;
    model = z_matrix * y_matrix * x_matrix;
    model = glm::scale(model, scale);
    model[3] = glm::vec4(translation, 1.0f);
}

void Animation::RotateX(glm::mat4& model)
{
    current_x_degrees += x_direction;
    if(abs(current_x_degrees) >= abs(x_degrees) && x_degrees != 360.0f)
    {
        if(isLoop) {
            x_direction *= -1;
        }
        else {
            completed = true;
        }
    }
    model = glm::rotate(model, glm::radians(current_x_degrees), x_rotation);
}

void Animation::RotateY(glm::mat4& model)
{
    current_y_degrees += y_direction;
    if(abs(current_y_degrees) >= abs(y_degrees) && y_degrees != 360.0f)
    {
        if(isLoop) {
            y_direction *= -1;
        }
        else {
            completed = true;
        }
    }
    model = glm::rotate(model, glm::radians(current_y_degrees), y_rotation);
}

void Animation::RotateZ(glm::mat4& model)
{
    current_z_degrees += z_direction;
    if(abs(current_z_degrees) >= abs(z_degrees) && z_degrees != 360.0f)
    {
        if(isLoop) {
            z_direction *= -1;
        }
        else {
            completed = true;
        }
    }
    model = glm::rotate(model, glm::radians(current_z_degrees), z_rotation);
}