#pragma once
#include <glm/glm.hpp>
#include <vector>

enum class MeshType
{
    Cube,
    ReverseCube,
    Sphere,
    Cylinder,
    Cone,
    Torus
};

class Meshes
{
public:
    Meshes(const std::vector<glm::vec3>& triangles, const std::vector<glm::vec3>& normals,
           const std::vector<glm::vec2>& uvs, const glm::mat4& model) : triangles(triangles), normals(normals), uvs(uvs), model(model)
    {
    }
    virtual void set_triangles(MeshType type) = 0;
    virtual void set_normals() = 0;
    virtual void set_uvs() = 0;
    virtual void set_model(glm::mat4 newModel) = 0;
    
    std::vector<glm::vec3> triangles;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    glm::mat4 model;

protected:
    Meshes() = default;
};
