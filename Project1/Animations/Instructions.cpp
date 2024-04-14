#include "Instructions.h"

#include <glm/gtc/type_ptr.inl>

#include "../Buffers/BufferBinder.h"

Instructions* Instructions::instance = nullptr;

Instructions* Instructions::get_instance()
{
    if (!instance)
    {
        instance = new Instructions();
    }
    return instance;
}

// Update the position of the instructions to the position of the hand and render it
void Instructions::update_instructions_position(glm::vec3 position, GLuint program_id, glm::mat4 view, glm::mat4 projection)
{
    if(!is_grabbed) return;

    // if the instructions are not initialized, initialize them
    if(!instructions)
    {
        init_instructions_buffers(program_id);
    }

    glm::mat4 cameraRotation = glm::mat4(glm::mat3(view));
    
    instructions->model = glm::mat4(1.0f);
    instructions->model = glm::translate(instructions->model, position);
    instructions->model = glm::scale(instructions->model, glm::vec3(0.3f, 0.3f, 0.3f));
    instructions->model *= glm::inverse(cameraRotation);
    instructions->model = glm::rotate(instructions->model, glm::radians(-70.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    instructions->mv = view * instructions->model;

    rendering_handler->render(projection, instructions, Phong);
    rendering_handler->draw_arrays(vao, instructions->vertices.size());
}

void Instructions::grab_instructions()
{
    is_grabbed = !is_grabbed;
}

void Instructions::init_instructions_buffers(GLuint program_id)
{
    instructions = new ObjectProperties();
    bool res = loadOBJ("Objects/Instructions.obj", instructions->vertices, instructions->uvs, instructions->normals);
    instructions->texture = loadDDS("Textures/Controls.dds");
    instructions->materials = JsonReader::read_materials("Props");
    instructions->model = glm::mat4();
    
    // Get vertex attributes
    const GLuint position_id = glGetAttribLocation(program_id, "position");
    const GLuint normal_id = glGetAttribLocation(program_id, "normal");
    const GLuint uv_id = glGetAttribLocation(program_id, "uv");

    // Allocate memory for vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    BufferBinder::bind_vao3d(position_id, instructions->vertices);
    BufferBinder::bind_vao3d(normal_id, instructions->normals);
    BufferBinder::bind_vao2d(uv_id, instructions->uvs);

    glBindVertexArray(0);
}