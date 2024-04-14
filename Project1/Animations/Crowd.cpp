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
    // Load the crowd character once to re-use for all the crowd characters
    ObjectProperties* actor = new ObjectProperties();
    bool res = loadOBJ("Objects/CrowdCharacter.obj", actor->vertices, actor->uvs, actor->normals);
    actor->texture = loadDDS("Textures/Controls.dds");
    actor->materials = JsonReader::ReadMaterial("Metallic");
    actor->model = glm::mat4();
    actor->model = glm::scale(actor->model, glm::vec3(1.0f, 1.0f, 1.0f));

    // initialize x amount of crowd members
    for(int i = 0; i < CROWD_SIZE; i++)
    {
        // Add an actor at a random position within the crowd area and at a random height, to simulate a more realistic jumping crowd
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
    // Initialize the crowd if it is not initialized
    if(crowd.empty()) InitCrowdBuffers(program_id);

    // Update the position of the crowd and render them
    UpdatePositions();
    int vaoindex = 0;
    for(auto actor: crowd)
    {
        actor->mv = view * actor->model;
        rendering_handler->Render(projection, actor, Phong);
        rendering_handler->DrawArrays(vao[vaoindex], actor->vertices.size());
        
        vaoindex++;
    }
}

// Update the position of the crowd to simulate jumping up and down
void Crowd::UpdatePositions()
{
    int index = 0;
    for(auto actor : crowd)
    {
        float currentY = actor->model[3][1];

        if (currentY >= -0.8f) {
            yPos[index] = -1;
        } else if (currentY <= -1.13f) {
            yPos[index] = 1;
        }

        glm::vec3 translation(0.0f, yPos[index] * 0.01f, 0.0f);
        actor->model = glm::translate(actor->model, translation);
        index++;
    }
}