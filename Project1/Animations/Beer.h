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
        float currY = 1.0f;
        while(currY >= 0.0f)
        {
            beer_particles.push_back(new Cube(glm::vec3(-0.5f, currY, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f)));
            yPos.push_back(currY);
            beer_particles.push_back(new Cube(glm::vec3(0.5f, currY, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f)));
            yPos.push_back(currY);
            beer_particles.push_back(new Cube(glm::vec3(0.0f, currY, -0.5f), glm::vec3(0.01f, 0.01f, 0.01f)));
            yPos.push_back(currY);
            beer_particles.push_back(new Cube(glm::vec3(0.0f, currY, 0.5f), glm::vec3(0.01f, 0.01f, 0.01f)));
            yPos.push_back(currY);
            currY -= 0.05f;
        }
    }
    
public:
    static Beer* GetInstance();
    void InitBeerBuffers(GLuint program_id);
    void DrawBeer(GLuint program_id, glm::mat4 view, glm::mat4 projection);
};
