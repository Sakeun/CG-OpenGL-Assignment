#pragma once
#include <GL/glew.h>

#include "../Buffers/RenderingHandler.h"
#include "../Structs.h"
#include "../Meshes/Cube.h"

class Beer
{
private:
    // Variables for the beer particles
    std::vector<Meshes*> beer_particles;
    // Vector to keep track of the current y position of each particle so it can easily be reset
    std::vector<float> particle_y_positions;
    static Beer* instance;
    std::vector<GLuint> vao;
    // Minimum and Maximum Y positions the particles can be in. To prevent it going on top of the tap or below the cup
    float minimum_y = -0.1f;
    float maximum_y = 0.18f;

    // Variables for the cup
    ObjectProperties* cup;
    GLuint cup_vao;
    Animation* cup_animation;

    RenderingHandler* rendering_handler = RenderingHandler::get_instance();
    
    Beer()
    {
        float currY = maximum_y;
        // Initialize 3 particles per Y position until there's 3 particles at the bottom
        while(currY >= minimum_y)
        {
            beer_particles.push_back(new Cube(glm::vec3(-0.5f, currY, 0.0f), glm::vec3(0.005f, 0.005f, 0.005f)));
            particle_y_positions.push_back(currY);
            beer_particles.push_back(new Cube(glm::vec3(0.5f, currY, 0.0f), glm::vec3(0.005f, 0.005f, 0.005f)));
            particle_y_positions.push_back(currY);
            beer_particles.push_back(new Cube(glm::vec3(0.0f, currY, -0.5f), glm::vec3(0.005f, 0.005f, 0.005f)));
            particle_y_positions.push_back(currY);
            beer_particles.push_back(new Cube(glm::vec3(0.0f, currY, 0.5f), glm::vec3(0.005f, 0.005f, 0.005f)));
            particle_y_positions.push_back(currY);
            currY -= 0.01f;
        }
        is_grabbed = false;
    }
    
public:
    bool is_grabbed;
    static Beer* get_instance();
    void init_beer_buffers(GLuint program_id);
    void draw_beer_particles(GLuint program_id, glm::mat4 view, glm::mat4 projection);
    void update_particle_positions();
    void grab_beer(GLuint program_id);
    void update_cup_position(glm::vec3 position, GLuint program_id, glm::mat4 view, glm::mat4 projection);
    void drink_beer();
};
