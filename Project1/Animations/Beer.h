#pragma once
#include <GL/glew.h>

#include "../Buffers/RenderingHandler.h"
#include "../Importers/ObjectProperties.h"
#include "../Meshes/Cube.h"

class Beer
{
private:
    std::vector<Meshes*> beer_particles;
    std::vector<float> yPos;
    static Beer* instance;
    std::vector<GLuint> vao;
    ObjectProperties* cup;
    GLuint cup_vao;

    Animation* cup_animation;
    RenderingHandler* rendering_handler = RenderingHandler::GetInstance();
    
    float minY = -0.1f;
    float maxY = 0.18f;
    Beer()
    {
        float currY = maxY;
        // Initialize 3 particles per Y position until there's 3 particles at the bottom
        while(currY >= minY)
        {
            beer_particles.push_back(new Cube(glm::vec3(-0.5f, currY, 0.0f), glm::vec3(0.005f, 0.005f, 0.005f)));
            yPos.push_back(currY);
            beer_particles.push_back(new Cube(glm::vec3(0.5f, currY, 0.0f), glm::vec3(0.005f, 0.005f, 0.005f)));
            yPos.push_back(currY);
            beer_particles.push_back(new Cube(glm::vec3(0.0f, currY, -0.5f), glm::vec3(0.005f, 0.005f, 0.005f)));
            yPos.push_back(currY);
            beer_particles.push_back(new Cube(glm::vec3(0.0f, currY, 0.5f), glm::vec3(0.005f, 0.005f, 0.005f)));
            yPos.push_back(currY);
            currY -= 0.01f;
        }
        isGrabbed = false;
    }
    
public:
    bool isGrabbed;
    static Beer* GetInstance();
    void InitBeerBuffers(GLuint program_id);
    void DrawBeer(GLuint program_id, glm::mat4 view, glm::mat4 projection);
    void UpdatePositions();
    void GrabBeer(GLuint program_id);
    void UpdateCupPosition(glm::vec3 position, GLuint program_id, glm::mat4 view, glm::mat4 projection);
    void DrinkBeer();
};
