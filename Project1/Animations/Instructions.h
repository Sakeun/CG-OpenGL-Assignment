#pragma once
#include "../Importers/ObjectProperties.h"
#include "../objloader.h"
#include "../texture.h"
#include "../JsonReader/JsonReader.h"
#include "../Buffers/RenderingHandler.h"

class Instructions
{
private:
    GLuint vao;
    ObjectProperties* instructions;
    static Instructions* instance;
    RenderingHandler* rendering_handler = RenderingHandler::GetInstance();

    void init_instructions_buffers(GLuint program_id);
    
    Instructions()
    {
        is_grabbed = true;
    }
    
public:
    bool is_grabbed;
    static Instructions* get_instance();
    void grab_instructions();
    void update_instructions_position(glm::vec3 position, GLuint program_id, glm::mat4 view, glm::mat4 projection);
};
