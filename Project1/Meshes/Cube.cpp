#include "Cube.h"

#include <glm/gtc/matrix_transform.hpp>

Cube::Cube(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float radians, MeshType type) : Meshes()
{
    glm::mat4 model = glm::mat4();
    if(position != glm::vec3())
        model = glm::translate(model, position);
    if(scale != glm::vec3())
        model = glm::scale(model, scale);
    if(radians != 0.0f)
        model = glm::rotate(model, radians, rotation);

    Cube::set_triangles(type);
    Cube::set_normals();
    Cube::set_uvs();
    Cube::set_model(model);
}

void Cube::set_triangles(MeshType type)
{
    if(type == MeshType::Cube)
    {
        triangles = {
            // Front face
            vertices[0], vertices[1], vertices[2],
            vertices[0], vertices[2], vertices[3],
            
            // Right face
            vertices[1], vertices[5], vertices[6],
            vertices[1], vertices[6], vertices[2],

            // Back face
            vertices[5], vertices[4], vertices[7],
            vertices[5], vertices[7], vertices[6],

            // Left face
            vertices[4], vertices[0], vertices[3],
            vertices[4], vertices[3], vertices[7],

            // Top face
            vertices[3], vertices[2], vertices[6],
            vertices[3], vertices[6], vertices[7],
            
            // Bottom face
            vertices[4], vertices[5], vertices[1],
            vertices[4], vertices[1], vertices[0]
        };
    }
    if(type == MeshType::ReverseCube)
    {
        triangles = {
            // Front face
            vertices[2], vertices[1], vertices[0],
            vertices[3], vertices[2], vertices[0],

            // Right face
            vertices[6], vertices[5], vertices[1],
            vertices[2], vertices[6], vertices[1],

            // Back face
            vertices[7], vertices[4], vertices[5],
            vertices[6], vertices[7], vertices[5],

            // Left face
            vertices[3], vertices[0], vertices[4],
            vertices[7], vertices[3], vertices[4],

            // Top face
            vertices[6], vertices[2], vertices[3],
            vertices[7], vertices[6], vertices[3],

            // Bottom face
            vertices[1], vertices[5], vertices[4],
            vertices[0], vertices[1], vertices[4]
        };
    }
}

void Cube::set_normals()
{
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            normals.push_back(normalPoints[i]);
        }
    }
}

void Cube::set_uvs()
{
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            uvs.push_back(uvPositions[j]);
        }
    }
}

void Cube::set_model(glm::mat4 newModel)
{
    model = newModel;
}