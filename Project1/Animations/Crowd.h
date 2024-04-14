#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "../Importers/ObjectProperties.h"
#include "../Buffers/RenderingHandler.h"

class Crowd
{
private:
    const int CROWD_SIZE = 20;
    std::vector<ObjectProperties*> crowd;
    // Vector used to track the current y position of each crowd member so it can easily be reset mid animation
    std::vector<float> crowd_y_positions;
    static Crowd* instance;
    std::vector<GLuint> vao;
    RenderingHandler* rendering_handler = RenderingHandler::get_instance();

    double get_random_num(double min, double max);
    Crowd()
    {
    }
    
public:
    static Crowd* get_instance();
    void init_crowd_buffers(GLuint program_id);
    void draw_crowd(GLuint program_id, glm::mat4 view, glm::mat4 projection);
    void update_positions();
    
};
