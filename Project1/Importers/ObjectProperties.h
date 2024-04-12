#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "../Animations/Animation.h"
#include "../Meshes/Meshes.h"

enum ShaderType
{
    Matt,
    Glossy,
    SlightReflection,
    Shiny
};

struct Material
{
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
    float power;
    float ambient_strength;
    float specular_strength;
    float diffuse_strength;
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
    Animation* animation;
};

struct ImportProperties
{
    std::string path;
    std::string texture;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    std::string shader;
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

struct ObjectElements {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
};