#include "Beer.h"
#include "../Buffers/BufferBinder.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "RotateAnimation.h"
#include "../Importers//Object.h"
#include "../objloader.h"
#include "../texture.h"
#include "../JsonReader/JsonReader.h"

Beer* Beer::instance = nullptr;

Beer* Beer::get_instance()
{
    if (!instance)
    {
        instance = new Beer();
    }
    return instance;
}

void Beer::init_beer_buffers(GLuint program_id)
{
    for (const auto particle : beer_particles)
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

void Beer::draw_beer_particles(GLuint program_id, glm::mat4 view, glm::mat4 projection)
{
    for (int i = 0; i < beer_particles.size(); i++) {
        glm::mat4 mv = view * beer_particles[i]->model;

        // Send mv
        GLint uniform_mv = 0;
        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));
        glUseProgram(program_id);
        
        Material material;
        Object::init_material_lights(material);
        
        // Make uniform vars
        uniform_mv = glGetUniformLocation(program_id, "mv");
        const GLuint uniform_proj = glGetUniformLocation(program_id, "projection");
        const GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
        GLint fragColLocation = glGetUniformLocation(program_id, "FragCol");
        
        // Fill uniform vars
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(fragColLocation, 0.89f, 0.91f, 0.15f);

        rendering_handler->draw_arrays(vao[i], beer_particles[i]->triangles.size());
    }
}

// Update the position of the beer particles at a random Z and X to simulate falling
void Beer::update_particle_positions() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distributed_x(-2.09, -2.03);
    std::uniform_real_distribution<> distributed_z(8.29, 8.35);

    for (int i = 0; i < beer_particles.size(); i++) {
        float random_x = distributed_x(gen);
        float random_z = distributed_z(gen);
        float y_position = this->particle_y_positions[i];

        // Reset the model matrix and apply the translation
        beer_particles[i]->model = glm::mat4(1.0f);
        beer_particles[i]->model = glm::translate(beer_particles[i]->model, glm::vec3(random_x, y_position, random_z));
        beer_particles[i]->model = glm::scale(beer_particles[i]->model, glm::vec3(0.01, 0.01, 0.01));

        this->particle_y_positions[i] -= 0.001f;
        if (this->particle_y_positions[i] < minimum_y)
        {
            this->particle_y_positions[i] = maximum_y;
        }
    }
}

void Beer::grab_beer(GLuint program_id)
{
    // If it's the first time grabbing a beer, initialize the cup
    if (!is_grabbed && !cup)
    {
        cup = new ObjectProperties();
        bool res = loadOBJ("Objects/Cup.obj", cup->vertices, cup->uvs, cup->normals);
        cup->texture = loadDDS("Textures/Hand.dds");
        cup->materials = JsonReader::read_materials("Props");

        cup->model = glm::mat4();
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

    // Activate beer cup
    is_grabbed = true;
}

void Beer::update_cup_position(glm::vec3 position, GLuint program_id, glm::mat4 view, glm::mat4 projection)
{
    if(!is_grabbed && !cup_animation) return;

    glm::mat4 damera_rotation = glm::mat4(glm::mat3(view));
    cup->model = glm::mat4(1.0f);

    // Set the cup to the correct position in the view of the camera
    cup->model = glm::scale(cup->model, glm::vec3(1.0f, 1.0f, 1.0f));
    cup->model = glm::translate(cup->model, position);
    cup->model *= glm::inverse(damera_rotation);
    cup->model = glm::rotate(cup->model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // if the "beer is being drank" add cup animation
    if(!is_grabbed && cup_animation)
    {
        glm::mat4 animation_matrix = glm::mat4(1.0f);
        cup_animation->update_model(animation_matrix);
        if (cup_animation->is_completed())
        {
            delete cup_animation;
            cup_animation = nullptr;
        } else
        {
            cup->model *= animation_matrix;
        }
    }

    cup->mv = view * cup->model;
    rendering_handler->render(projection, cup, Phong);
    rendering_handler->draw_arrays(cup_vao, cup->vertices.size());
}

void Beer::drink_beer()
{
    if (is_grabbed)
    {
        cup_animation = new RotateAnimation( 0.0f, 0.0f, -90.0f, false);
        is_grabbed = false;
    }
}