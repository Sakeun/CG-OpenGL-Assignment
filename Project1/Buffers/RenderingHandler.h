#pragma once
#include <GL/glew.h>
#include <glm/detail/type_mat.hpp>

#include "../Structs.h"

enum FragmentShader
{
    Phong,
    SingleColor
};

class RenderingHandler
{
private:
    GLuint phong_program_id;
    GLuint singlecolor_program_id;
    static RenderingHandler* instance;
    
    RenderingHandler() = delete;
    RenderingHandler(GLuint phong_program_id, GLuint singlecolor_program_id)
    {
        this->phong_program_id = phong_program_id;
        this->singlecolor_program_id = singlecolor_program_id;
    }

    // Render based on the defined fragment shader
    void render_phong(glm::mat4 projection, ObjectProperties* object);
    void render_single_color(glm::mat4 projection, ObjectProperties* object);
    
public:
    static RenderingHandler* get_instance(GLuint phong_program_id, GLuint singlecolor_program_id);
    static RenderingHandler* get_instance()
    {
        if(!instance)
            instance = new RenderingHandler(0, 0);
        
        return instance;
    }

    void render(glm::mat4 projection, ObjectProperties* object, FragmentShader shader);
    void draw_arrays(GLuint vao, int size);
};
