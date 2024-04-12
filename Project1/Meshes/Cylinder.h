#pragma once
#include "Meshes.h"

class Cylinder : public Meshes
{
private:
    int segments = 16;
    float radius = 1.0f;
    float height = 2.0f;
    
public:
    Cylinder(glm::vec3 position = glm::vec3(), glm::vec3 scale = glm::vec3(), glm::vec3 rotation = glm::vec3(0, 0, 0), float radians = 0.0f, MeshType type = MeshType::Cylinder);

    void set_triangles(MeshType type) override;
    void set_model(glm::mat4 newModel) override;
    void set_normals() override;
    void set_uvs() override;
};
