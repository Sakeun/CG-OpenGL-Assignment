#include "Instructions.h"

#include <glm/gtc/type_ptr.inl>

#include "../Buffers/BufferBinder.h"

Instructions* Instructions::instance = nullptr;

Instructions* Instructions::GetInstance()
{
    if (!instance)
    {
        instance = new Instructions();
    }
    return instance;
}

void Instructions::UpdateInstructionsPosition(glm::vec3 position, GLuint program_id, glm::mat4 view, glm::mat4 projection)
{
    if(!isGrabbed) return;

    if(!instructions)
    {
        InitInstructionsBuffers(program_id);
    }

    glm::mat4 cameraRotation = glm::mat4(glm::mat3(view));
    
    instructions->model = glm::mat4(1.0f);
    instructions->model = glm::translate(instructions->model, position);
    instructions->model = glm::scale(instructions->model, glm::vec3(0.3f, 0.3f, 0.3f));
    instructions->model *= glm::inverse(cameraRotation);
    instructions->model = glm::rotate(instructions->model, glm::radians(-70.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    instructions->mv = view * instructions->model;

    // Send mv
    glUseProgram(program_id);

    GLint uniform_mv = 0;
    glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(instructions->mv));
    
    // Make uniform vars
    uniform_mv = glGetUniformLocation(program_id, "mv");
    const GLuint uniform_proj = glGetUniformLocation(program_id, "projection");
    const GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
    const GLuint uniform_material_ambient = glGetUniformLocation(program_id, "mat_ambient");
    const GLuint uniform_material_diffuse = glGetUniformLocation(program_id, "mat_diffuse");
    const GLuint uniform_specular = glGetUniformLocation(program_id, "mat_specular");
    const GLuint uniform_material_power = glGetUniformLocation(program_id, "mat_power");
    const GLuint uniform_reflection_ambient = glGetUniformLocation(program_id, "reflection_ambient");
    const GLuint uniform_reflection_diffuse = glGetUniformLocation(program_id, "reflection_diffuse");
    const GLuint uniform_reflection_specular = glGetUniformLocation(program_id, "reflection_specular");

    //Bind Texture
    glBindTexture(GL_TEXTURE_2D, instructions->texture);

    glUniform1i(glGetUniformLocation(program_id, "texsampler"), 0);
    // Fill uniform vars
    glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(instructions->materials.ambient_color));
    glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(instructions->materials.diffuse_color));
    glUniform3fv(uniform_specular, 1, glm::value_ptr(instructions->materials.specular_color));
    glUniform1f(uniform_material_power, instructions->materials.power);
    glUniform1f(uniform_reflection_ambient, instructions->materials.ambient_strength);
    glUniform1f(uniform_reflection_diffuse, instructions->materials.diffuse_strength);
    glUniform1f(uniform_reflection_specular, instructions->materials.specular_strength);

    // Send vao
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, instructions->vertices.size());
    glBindVertexArray(0);
}

void Instructions::GrabInstructions()
{
    isGrabbed = !isGrabbed;
}

void Instructions::InitInstructionsBuffers(GLuint program_id)
{
    instructions = new ObjectProperties();
    bool res = loadOBJ("Objects/Instructions.obj", instructions->vertices, instructions->uvs, instructions->normals);
    instructions->texture = loadDDS("Textures/Controls.dds");
    instructions->materials = JsonReader::ReadMaterial("Metallic");
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