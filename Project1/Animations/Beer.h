#pragma once
#include <GL/glew.h>

#include "../Meshes/Cube.h"

class Beer
{
private:
    std::vector<Meshes*> beer_particles;
    std::vector<float> yPos;
    static Beer* instance;
    std::vector<GLuint> vao;
    Beer()
    {
        beer_particles = {
            new Cube(glm::vec3(-0.5, 1.0, 0.0), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.5, 1.0, 0.0), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.0, 1.0, -0.5), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.0, 1.0, 0.5), glm::vec3(0.01, 0.01, 0.01)),

            new Cube(glm::vec3(0.0, 0.9, 0.5), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.0, 0.9, -0.5), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.5, 0.9, 0.0), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(-0.5, 0.9, 0.0), glm::vec3(0.01, 0.01, 0.01)),

            new Cube(glm::vec3(0.5, 0.8, 0.0), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(-0.5, 0.8, 0.0), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.0, 0.8, 0.5), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.0, 0.8, -0.5), glm::vec3(0.01, 0.01, 0.01)),

            new Cube(glm::vec3(0.0, 0.7, 0.5), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.0, 0.7, -0.5), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.5, 0.7, 0.0), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(-0.5, 0.7, 0.0), glm::vec3(0.01, 0.01, 0.01)),

            new Cube(glm::vec3(-0.5, 0.6, 0.0), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.5, 0.6, 0.0), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.0, 0.6, -0.5), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.0, 0.6, 0.5), glm::vec3(0.01, 0.01, 0.01)),

            new Cube(glm::vec3(0.0, 0.5, 0.5), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.0, 0.5, -0.5), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(0.5, 0.5, 0.0), glm::vec3(0.01, 0.01, 0.01)),
            new Cube(glm::vec3(-0.5, 0.5, 0.0), glm::vec3(0.01, 0.01, 0.01))
        };
        yPos = { 1.0f, 1.0f, 1.0f, 1.0f, 0.9f, 0.9f, 0.9f, 0.9f, 0.8f, 0.8f, 0.8f, 0.8f, 0.7f, 0.7f, 0.7f, 0.7f, 0.6f, 0.6f, 0.6f, 0.6f, 0.5f, 0.5f, 0.5f, 0.5f };
    }
    
public:
    static Beer* GetInstance();
    void InitBeerBuffers(GLuint program_id);
    void DrawBeer(GLuint program_id, glm::mat4 view, glm::mat4 projection);
};
