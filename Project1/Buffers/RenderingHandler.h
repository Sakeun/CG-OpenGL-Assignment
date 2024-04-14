#pragma once
#include <GL/glew.h>
#include <glm/detail/type_mat.hpp>

#include "../Importers/ObjectProperties.h"

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

    void RenderPhong(glm::mat4 projection, ObjectProperties* object);
    void RenderSingleColor(glm::mat4 projection, ObjectProperties* object);
public:
    static RenderingHandler* GetInstance(GLuint phong_program_id, GLuint singlecolor_program_id);
    static RenderingHandler* GetInstance()
    {
        if(!instance)
            instance = new RenderingHandler(0, 0);
        
        return instance;
    }

    void Render(glm::mat4 projection, ObjectProperties* object, FragmentShader shader);
    void DrawArrays(GLuint vao, int size);
};
