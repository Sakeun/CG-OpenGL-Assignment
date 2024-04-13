#include "Crowd.h"
#include "../Buffers/BufferBinder.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include "../objloader.h"
#include "../texture.h"
#include "../JsonReader/JsonReader.h"

Crowd* Crowd::instance = nullptr;

Crowd* Crowd::GetInstance()
{
    if (!instance)
    {
        instance = new Crowd();
    }
    return instance;
}

double Crowd::GetRandomNum(double min, double max)
{
    if (min > max) {
        std::swap(min, max);
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

void Crowd::InitCrowdBuffers(GLuint program_id)
{
    ObjectProperties* actor = new ObjectProperties();
    bool res = loadOBJ("Objects/CrowdCharacter.obj", actor->vertices, actor->uvs, actor->normals);
    actor->texture = loadDDS("Textures/Controls.dds");
    actor->materials = JsonReader::ReadMaterial("Metallic");
    actor->model = glm::mat4();
    actor->model = glm::scale(actor->model, glm::vec3(1.0f, 1.0f, 1.0f));

    for(int i = 0; i < 20; i++)
    {
        vao.push_back(0);
        yPos.push_back(-1);
        ObjectProperties* newActor = new ObjectProperties(*actor);
        newActor->model = glm::translate(newActor->model, glm::vec3(GetRandomNum(-10.0, 13.0), GetRandomNum(1.13, 0.8) * -1, GetRandomNum(-7.0, 7.0)));
        newActor->model = glm::rotate(newActor->model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        crowd.push_back(newActor);
        // Get vertex attributes
        const GLuint position_id = glGetAttribLocation(program_id, "position");
        const GLuint normal_id = glGetAttribLocation(program_id, "normal");
        const GLuint uv_id = glGetAttribLocation(program_id, "uv");

        // Allocate memory for vao
        glGenVertexArrays(1, &vao[i]);
        glBindVertexArray(vao[i]);

        BufferBinder::bind_vao3d(position_id, crowd[i]->vertices);
        BufferBinder::bind_vao3d(normal_id, crowd[i]->normals);
        BufferBinder::bind_vao2d(uv_id, crowd[i]->uvs);

        glBindVertexArray(0);
    }
}

void Crowd::DrawCrowd(GLuint program_id, glm::mat4 view, glm::mat4 projection)
{
    
    if(crowd.empty()) InitCrowdBuffers(program_id);

    UpdatePositions();
    int vaoindex = 0;
    for(auto actor: crowd)
    {
        actor->mv = view * actor->model;

        // Send mv
        glUseProgram(program_id);

        GLint uniform_mv = 0;
        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(actor->mv));
        
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
        glBindTexture(GL_TEXTURE_2D, actor->texture);

        glUniform1i(glGetUniformLocation(program_id, "texsampler"), 0);
        // Fill uniform vars
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(actor->materials.ambient_color));
        glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(actor->materials.diffuse_color));
        glUniform3fv(uniform_specular, 1, glm::value_ptr(actor->materials.specular_color));
        glUniform1f(uniform_material_power, actor->materials.power);
        glUniform1f(uniform_reflection_ambient, actor->materials.ambient_strength);
        glUniform1f(uniform_reflection_diffuse, actor->materials.diffuse_strength);
        glUniform1f(uniform_reflection_specular, actor->materials.specular_strength);

        // Send vao
        glBindVertexArray(vao[vaoindex]);
        glDrawArrays(GL_TRIANGLES, 0, actor->vertices.size());
        glBindVertexArray(0);
        vaoindex++;
    }
}

void Crowd::UpdatePositions()
{
    int index = 0;
    for(auto actor : crowd)
    {
        // Get the current y position
        float currentY = actor->model[3][1];

        // Check if the actor has reached the upper or lower limit
        if (currentY >= -0.8f) {
            yPos[index] = -1; // Change direction to down
        } else if (currentY <= -1.13f) {
            yPos[index] = 1; // Change direction to up
        }

        glm::vec3 translation(0.0f, yPos[index] * 0.01f, 0.0f);
        actor->model = glm::translate(actor->model, translation);
        index++;
    }
}