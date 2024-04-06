#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

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
    Material materials;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
};

struct JsonProperties
{
    std::string path;
    std::string texture;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
};
