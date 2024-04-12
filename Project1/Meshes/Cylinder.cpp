#include "Cylinder.h"

#include <glm/gtc/matrix_transform.hpp>

Cylinder::Cylinder(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float radians, MeshType type) : Meshes()
{
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    glm::mat4 translationMatrix = glm::mat4(1.0f);
    glm::mat4 scaleMatrix = glm::mat4(1.0f);

    if (rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f)
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(radians), rotation);

    if (position != glm::vec3())
        translationMatrix = glm::translate(translationMatrix, position);

    if (scale != glm::vec3())
        scaleMatrix = glm::scale(scaleMatrix, scale);

    glm::mat4 model = translationMatrix * rotationMatrix * scaleMatrix;
    
    Cylinder::set_triangles(type);
    Cylinder::set_normals();
    Cylinder::set_uvs();
    Cylinder::set_model(model);
}

void Cylinder::set_model(glm::mat4 newModel)
{
    model = newModel;
}

void Cylinder::set_triangles(MeshType type)
{
    if (type == MeshType::Cylinder)
    {
        float angleIncrement = 2 * glm::pi<float>() / segments;

        for (int i = 0; i < segments; ++i) {
            float theta1 = i * angleIncrement;
            float theta2 = ((i + 1) % segments) * angleIncrement;

            glm::vec3 top1(radius * cos(theta1), height / 2, radius * sin(theta1));
            glm::vec3 top2(radius * cos(theta2), height / 2, radius * sin(theta2));
            glm::vec3 bottom1(radius * cos(theta1), -height / 2, radius * sin(theta1));
            glm::vec3 bottom2(radius * cos(theta2), -height / 2, radius * sin(theta2));

            triangles.push_back(bottom1);
            triangles.push_back(top1);
            triangles.push_back(top2);

            triangles.push_back(bottom1);
            triangles.push_back(top2);
            triangles.push_back(bottom2);
        }
    }
}

void Cylinder::set_normals()
{
    float angleIncrement = 2 * glm::pi<float>() / segments;

    for (int i = 0; i < segments; ++i) {
        float theta1 = i * angleIncrement;
        float theta2 = ((i + 1) % segments) * angleIncrement;

        normals.push_back(glm::normalize(glm::vec3(cos(theta1), 0, sin(theta1))));
        normals.push_back(glm::normalize(glm::vec3(cos(theta1), 0, sin(theta1))));
        normals.push_back(glm::normalize(glm::vec3(cos(theta2), 0, sin(theta2))));

        normals.push_back(glm::normalize(glm::vec3(cos(theta1), 0, sin(theta1))));
        normals.push_back(glm::normalize(glm::vec3(cos(theta2), 0, sin(theta2))));
        normals.push_back(glm::normalize(glm::vec3(cos(theta2), 0, sin(theta2))));
    }
}

void Cylinder::set_uvs()
{
    float angleIncrement = 2 * glm::pi<float>() / segments;

    for (int i = 0; i < segments; ++i) {
        float theta1 = i * angleIncrement;
        float theta2 = ((i + 1) % segments) * angleIncrement;

        uvs.push_back(glm::vec2(theta1 / (2 * glm::pi<float>()), 0.0f));
        uvs.push_back(glm::vec2(theta1 / (2 * glm::pi<float>()), 1.0f));
        uvs.push_back(glm::vec2(theta2 / (2 * glm::pi<float>()), 1.0f));

        uvs.push_back(glm::vec2(theta1 / (2 * glm::pi<float>()), 0.0f));
        uvs.push_back(glm::vec2(theta2 / (2 * glm::pi<float>()), 1.0f));
        uvs.push_back(glm::vec2(theta2 / (2 * glm::pi<float>()), 0.0f));
    }
}


