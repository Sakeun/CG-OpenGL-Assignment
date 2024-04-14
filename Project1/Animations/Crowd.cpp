#include "Crowd.h"
#include "../Buffers/BufferBinder.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>

#include "Animation/AmbientAnimation.h"
#include "../objloader.h"
#include "../texture.h"
#include "../JsonReader/JsonReader.h"

Crowd* Crowd::instance = nullptr;

Crowd* Crowd::get_instance()
{
    if (!instance)
    {
        instance = new Crowd();
    }
    return instance;
}

double Crowd::get_random_num(double min, double max)
{
    if (min > max) {
        std::swap(min, max);
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

void Crowd::init_crowd_buffers(GLuint program_id)
{
    // Load the crowd character once to re-use for all the crowd characters
    ObjectProperties* actor = new ObjectProperties();
    bool res = loadOBJ("Objects/CrowdCharacter.obj", actor->vertices, actor->uvs, actor->normals);
    actor->materials = JsonReader::read_materials("Character");
    actor->model = glm::mat4();
    actor->materials.diffuse_color = glm::vec3(0.01, 0.01, 0.01);
    actor->model = glm::scale(actor->model, glm::vec3(1.0f, 1.0f, 1.0f));
    
    // initialize x amount of crowd members
    for(int i = 0; i < CROWD_SIZE; i++)
    {
        // Add an actor at a random position within the crowd area and at a random height, to simulate a more realistic jumping crowd
        vao.push_back(0);
        crowd_y_positions.push_back(-1);
        ObjectProperties* newActor = new ObjectProperties(*actor);
        newActor->model = glm::translate(newActor->model, glm::vec3(get_random_num(-10.0, 13.0), get_random_num(1.13, 0.8) * -1, get_random_num(-7.0, 7.0)));
        newActor->model = glm::rotate(newActor->model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        newActor->animation = new AmbientAnimation();
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

void Crowd::draw_crowd(GLuint program_id, glm::mat4 view, glm::mat4 projection)
{
    // Initialize the crowd if it is not initialized
    if(crowd.empty()) init_crowd_buffers(program_id);

    // Update the position of the crowd and render them
    update_positions();
    int vao_index = 0;
    for(auto actor: crowd)
    {
        actor->animation->update_object(*actor);
        actor->mv = view * actor->model;
        rendering_handler->render(projection, actor, SingleColor);
        rendering_handler->draw_arrays(vao[vao_index], actor->vertices.size());
        
        vao_index++;
    }
}

// Update the position of the crowd to simulate jumping up and down
void Crowd::update_positions()
{
    int index = 0;
    for(auto actor : crowd)
    {
        float currentY = actor->model[3][1];

        if (currentY >= -0.8f) {
            crowd_y_positions[index] = -1;
        } else if (currentY <= -1.13f) {
            crowd_y_positions[index] = 1;
        }

        glm::vec3 translation(0.0f, crowd_y_positions[index] * 0.01f, 0.0f);
        actor->model = glm::translate(actor->model, translation);
        index++;
    }
}