#include "Beer.h"
#include "../Buffers/BufferBinder.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>
#include "../Importers//Object.h"
#include "../objloader.h"
#include "../texture.h"
#include "../JsonReader/JsonReader.h"

Beer* Beer::instance = nullptr;

Beer* Beer::GetInstance()
{
    if (!instance)
    {
        instance = new Beer();
    }
    return instance;
}

void Beer::InitBeerBuffers(GLuint program_id)
{
    for (auto particle : beer_particles)
    {
        vao.push_back(0);
        
        // Get vertex attributes
        const GLuint position_id = glGetAttribLocation(program_id, "position");
        const GLuint normal_id = glGetAttribLocation(program_id, "normal");
        const GLuint uv_id = glGetAttribLocation(program_id, "uv");

        // Allocate memory for vao
        glGenVertexArrays(1, &vao[vao.size() - 1]);
        
        glBindVertexArray(vao[vao.size() - 1]);

        BufferBinder::bind_vao3d(position_id, particle->triangles);
        BufferBinder::bind_vao3d(normal_id, particle->normals);
        BufferBinder::bind_vao2d(uv_id, particle->uvs);

        glBindVertexArray(0);
    }
}

void Beer::DrawBeer(GLuint program_id, glm::mat4 view, glm::mat4 projection)
{
    for (int i = 0; i < beer_particles.size(); i++) {
        glm::mat4 mv = view * beer_particles[i]->model;

        // Send mv
        GLint uniform_mv = 0;
        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));
        glUseProgram(program_id);
        Material material;
        Object::InitMaterialLights(material);
        
        // Make uniform vars
        uniform_mv = glGetUniformLocation(program_id, "mv");
        const GLuint uniform_proj = glGetUniformLocation(program_id, "projection");
        const GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
        GLint fragColLocation = glGetUniformLocation(program_id, "FragCol");
        
        // Fill uniform vars
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(fragColLocation, 0.89f, 0.91f, 0.15f);

        rendering_handler->DrawArrays(vao[i], beer_particles[i]->triangles.size());
    }
}

// Update the position of the beer particles at a random Z and X to simulate falling
void Beer::UpdatePositions() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disX(-2.09, -2.03);
    std::uniform_real_distribution<> disZ(8.29, 8.35);
    int vaoIndex = 0;

    for (int i = 0; i < beer_particles.size(); i++) {
        float randX = disX(gen);
        float randZ = disZ(gen);
        float yPos = this->yPos[i];

        // Reset the model matrix and apply the translation
        beer_particles[i]->model = glm::mat4(1.0f);
        beer_particles[i]->model = glm::translate(beer_particles[i]->model, glm::vec3(randX, yPos, randZ));
        beer_particles[i]->model = glm::scale(beer_particles[i]->model, glm::vec3(0.01, 0.01, 0.01));

        this->yPos[i] -= 0.001f;
        if (this->yPos[i] < minY)
        {
            this->yPos[i] = maxY;
        }
    }
}

void Beer::GrabBeer(GLuint program_id)
{
    if (!isGrabbed && !cup)
    {
        cup = new ObjectProperties();
        bool res = loadOBJ("Objects/Cup.obj", cup->vertices, cup->uvs, cup->normals);
        cup->texture = loadDDS("Textures/Hand.dds");
        cup->materials = JsonReader::ReadMaterial("Metallic");

        cup->model = glm::mat4();

        isGrabbed = true;
        
        cup_vao = 0;
        glGenVertexArrays(1, &cup_vao);
        glBindVertexArray(cup_vao);

        // Get vertex attributes
        const GLuint position_id = glGetAttribLocation(program_id, "position");
        const GLuint normal_id = glGetAttribLocation(program_id, "normal");
        const GLuint uv_id = glGetAttribLocation(program_id, "uv");

        BufferBinder::bind_vao3d(position_id, cup->vertices);
        BufferBinder::bind_vao3d(normal_id, cup->normals);
        BufferBinder::bind_vao2d(uv_id, cup->uvs);

        glBindVertexArray(0);
    }
}

void Beer::UpdateCupPosition(glm::vec3 position, GLuint program_id, glm::mat4 view, glm::mat4 projection)
{
    if(!isGrabbed && !cup_animation) return;

    glm::mat4 cameraRotation = glm::mat4(glm::mat3(view));
    cup->model = glm::mat4(1.0f);

    cup->model = glm::scale(cup->model, glm::vec3(1.0f, 1.0f, 1.0f));
    cup->model = glm::translate(cup->model, position);
    cup->model *= glm::inverse(cameraRotation);
    cup->model = glm::rotate(cup->model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    if(!isGrabbed && cup_animation)
    {
        glm::mat4 animationMatrix = glm::mat4(1.0f);
        cup_animation->Execute(animationMatrix);
        if (cup_animation->IsCompleted())
        {
            delete cup_animation;
            cup_animation = nullptr;
        } else
        {
            cup->model *= animationMatrix;
        }
    }

    cup->mv = view * cup->model;
    rendering_handler->Render(projection, cup, Phong);
    rendering_handler->DrawArrays(cup_vao, cup->vertices.size());
}

void Beer::DrinkBeer()
{
    if (isGrabbed)
    {
        cup_animation = new Animation( 0.0f, 0.0f, -90.0f, false);
        isGrabbed = false;
    }
}