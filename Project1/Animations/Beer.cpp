#include "Beer.h"
#include "../Buffers/BufferBinder.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>
#include "../Importers//Object.h"

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

        // Send vao
        glBindVertexArray(vao[i]);
        glDrawArrays(GL_TRIANGLES, 0, beer_particles[i]->triangles.size());
        glBindVertexArray(0);
    }
}

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
