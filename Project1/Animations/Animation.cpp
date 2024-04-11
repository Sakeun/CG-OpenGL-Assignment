#include "Animation.h"

Animation::Animation(const float x, const float y, const float z): x_degrees(x), y_degrees(y), z_degrees(z)
{
    current_x_degrees = 0.0f;
    current_y_degrees = 0.0f;
    current_z_degrees = 0.0f;
}

void Animation::Execute(glm::mat4& model)
{
    glm::mat4 x_matrix = glm::mat4(1.0f);
    glm::mat4 y_matrix = glm::mat4(1.0f);
    glm::mat4 z_matrix = glm::mat4(1.0f);
    
    if(x_degrees != 0.0f)
        RotateX(x_matrix);
    if(y_degrees != 0.0f)
        RotateY(y_matrix);
    if(z_degrees != 0.0f)
        RotateZ(z_matrix);

    model = x_matrix * y_matrix * z_matrix;
}

void Animation::RotateX(glm::mat4& model)
{
    current_x_degrees += x_direction;
    if(abs(current_x_degrees) >= abs(x_degrees))
    {
        x_direction *= -1;
    }
    model = glm::rotate(model, glm::radians(current_x_degrees), glm::vec3(1.0f, 0.0f, 0.0f));
}

void Animation::RotateY(glm::mat4& model)
{
    current_y_degrees += y_direction;
    if(abs(current_y_degrees) >= abs(y_degrees))
    {
        y_direction *= -1;
    }
    model = glm::rotate(model, glm::radians(current_y_degrees), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Animation::RotateZ(glm::mat4& model)
{
    current_z_degrees += z_direction;
    if(abs(current_z_degrees) >= abs(z_degrees))
    {
        z_direction *= -1;
    }
    model = glm::rotate(model, glm::radians(current_z_degrees), glm::vec3(0.0f, 0.0f, 1.0f));
}