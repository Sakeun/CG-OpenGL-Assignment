#include "RenderingHandler.h"

#include <glm/gtc/type_ptr.inl>

RenderingHandler* RenderingHandler::instance = nullptr;

RenderingHandler* RenderingHandler::get_instance(GLuint phong_program_id, GLuint singlecolor_program_id)
{
    if (instance == nullptr)
    {
        instance = new RenderingHandler(phong_program_id, singlecolor_program_id);
    }
    if(instance->phong_program_id != phong_program_id || instance->singlecolor_program_id != singlecolor_program_id)
    {
        instance->phong_program_id = phong_program_id;
        instance->singlecolor_program_id = singlecolor_program_id;
    }
    return instance;
}

// Render with the specified fragment shader
void RenderingHandler::render(glm::mat4 projection, ObjectProperties* object, FragmentShader shader)
{
    switch (shader)
    {
    case Phong:
        render_phong(projection, object);
        break;
    case SingleColor:
        render_single_color(projection, object);
        break;
    default:
        break;
    }
}

void RenderingHandler::draw_arrays(GLuint vao, int size)
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, size);
    glBindVertexArray(0);
}

void RenderingHandler::render_phong(glm::mat4 projection, ObjectProperties* object)
{
    glUseProgram(phong_program_id);
    // Send mv
    GLint uniform_mv = 0;
    glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(object->mv));

    glUseProgram(phong_program_id);

    // Make uniform vars
    uniform_mv = glGetUniformLocation(phong_program_id, "mv");
    const GLuint uniform_proj = glGetUniformLocation(phong_program_id, "projection");
    const GLuint uniform_light_pos = glGetUniformLocation(phong_program_id, "light_pos");
    const GLuint uniform_material_ambient = glGetUniformLocation(phong_program_id, "mat_ambient");
    const GLuint uniform_material_diffuse = glGetUniformLocation(phong_program_id, "mat_diffuse");
    const GLuint uniform_specular = glGetUniformLocation(phong_program_id, "mat_specular");
    const GLuint uniform_material_power = glGetUniformLocation(phong_program_id, "mat_power");
    const GLuint uniform_reflection_ambient = glGetUniformLocation(phong_program_id, "reflection_ambient");
    const GLuint uniform_reflection_diffuse = glGetUniformLocation(phong_program_id, "reflection_diffuse");
    const GLuint uniform_reflection_specular = glGetUniformLocation(phong_program_id, "reflection_specular");

    //Bind Texture
    glBindTexture(GL_TEXTURE_2D, object->texture);
    glUniform1i(glGetUniformLocation(phong_program_id, "texsampler"), 0);
    
    // Fill uniform vars
    glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(object->materials.ambient_color));
    glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(object->materials.diffuse_color));
    glUniform3fv(uniform_specular, 1, glm::value_ptr(object->materials.specular_color));
    glUniform1f(uniform_material_power, object->materials.power);
    glUniform1f(uniform_reflection_ambient, object->materials.ambient_strength);
    glUniform1f(uniform_reflection_diffuse, object->materials.diffuse_strength);
    glUniform1f(uniform_reflection_specular, object->materials.specular_strength);
}

void RenderingHandler::render_single_color(glm::mat4 projection, ObjectProperties* object)
{
    glUseProgram(singlecolor_program_id);

    // Send mv
    GLint uniform_mv = glGetUniformLocation(singlecolor_program_id, "mv");
    glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(object->mv));
    glUseProgram(singlecolor_program_id);

    // Make uniform vars
    uniform_mv = glGetUniformLocation(singlecolor_program_id, "mv");
    const GLuint uniform_proj = glGetUniformLocation(singlecolor_program_id, "projection");
    const GLuint uniform_light_pos = glGetUniformLocation(singlecolor_program_id, "light_pos");
    GLint fragColLocation = glGetUniformLocation(singlecolor_program_id, "FragCol");
    const GLuint uniform_material_ambient = glGetUniformLocation(phong_program_id, "mat_ambient");
    const GLuint uniform_specular = glGetUniformLocation(phong_program_id, "mat_specular");
    const GLuint uniform_material_power = glGetUniformLocation(phong_program_id, "mat_power");
    const GLuint uniform_reflection_ambient = glGetUniformLocation(phong_program_id, "reflection_ambient");
    const GLuint uniform_reflection_diffuse = glGetUniformLocation(phong_program_id, "reflection_diffuse");
    const GLuint uniform_reflection_specular = glGetUniformLocation(phong_program_id, "reflection_specular");

    // Fill uniform vars
    glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
    glm::vec3 color = object->materials.diffuse_color;
    glUniform3f(fragColLocation, color.r, color.g, color.b);
    glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(object->materials.ambient_color));
    glUniform3fv(uniform_specular, 1, glm::value_ptr(object->materials.specular_color));
    glUniform1f(uniform_material_power, object->materials.power);
    glUniform1f(uniform_reflection_ambient, object->materials.ambient_strength);
    glUniform1f(uniform_reflection_diffuse, 1.0f);
    glUniform1f(uniform_reflection_specular, object->materials.specular_strength);
}