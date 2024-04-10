#pragma once
#include "Meshes.h"

class Cube : public Meshes
{
public:
    explicit Cube(glm::vec3 position = glm::vec3(), glm::vec3 scale = glm::vec3(), glm::vec3 rotation = glm::vec3(), float radians = 0.0f, MeshType type = MeshType::Cube);
    ~Cube() = default;
    
private:
    std::vector<glm::vec3> vertices = {
        glm::vec3(-1.0f, -1.0f, 1.0f), // 0: Front bottom left
        glm::vec3(1.0f, -1.0f, 1.0f), // 1: Front bottom right
        glm::vec3(1.0f, 1.0f, 1.0f), // 2: Front top right
        glm::vec3(-1.0f, 1.0f, 1.0f), // 3: Front top left
        glm::vec3(-1.0f, -1.0f, -1.0f), // 4: Back bottom left
        glm::vec3(1.0f, -1.0f, -1.0f), // 5: Back bottom right
        glm::vec3(1.0f, 1.0f, -1.0f), // 6: Back top right
        glm::vec3(-1.0f, 1.0f, -1.0f) // 7: Back top left
    };

    std::vector<glm::vec3> normalPoints = {
        glm::vec3(0.0f, 0.0f, 1.0f), // 0: Front
        glm::vec3(0.0f, 0.0f, -1.0f), // 1: Back
        glm::vec3(1.0f, 0.0f, 0.0f), // 2: Right
        glm::vec3(-1.0f, 0.0f, 0.0f), // 3: Left
        glm::vec3(0.0f, 1.0f, 0.0f), // 4: Top
        glm::vec3(0.0f, -1.0f, 0.0f) // 5: Bottom
    };

    std::vector<glm::vec2> uvPositions = {
        glm::vec2(0.0f, 0.0f), // 0: bottom left
        glm::vec2(1.0f, 0.0f), // 1: bottom right
        glm::vec2(1.0f, 1.0f), // 2: top right
        glm::vec2(0.0f, 0.0f), // 3: bottom left
        glm::vec2(1.0f, 1.0f), // 4: top right
        glm::vec2(0.0f, 1.0f), // 5: top left
    };

    void set_triangles(MeshType type) override;
    void set_normals() override;
    void set_uvs() override;
    void set_model(glm::mat4 newModel) override;
};
