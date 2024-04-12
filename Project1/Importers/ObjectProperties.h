#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "../Meshes/Meshes.h"

class Animation;

struct Material
{
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
    float power;
};

struct ObjectProperties
{
    glm::mat4 model;
    glm::mat4 mv;
    GLuint texture;
    glm::vec3 color;
    Material materials;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    Animation* animation;
};

struct ImportProperties
{
    std::string path;
    std::string texture;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    float radius;
    bool isAnimated;
    float xDegrees;
    float yDegrees;
    float zDegrees;
};

struct ObjectMeshes
{
    std::vector<Meshes> meshes;
    Material materials;
    GLuint texture;
    glm::mat4 model;
    glm::mat4 mv;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
};