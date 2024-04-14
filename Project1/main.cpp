#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "Animations/Animation/Animation.h"
#include "Animations/Beer.h"
#include "Animations/Crowd.h"
#include "Animations/Instructions.h"
#include "Buffers/BufferBinder.h"
#include "Buffers/RenderingHandler.h"
#include "Camera/CameraControls.h"
#include "Importers/Object.h"
#include "Structs.h"
#include "JsonReader/JsonReader.h"

using namespace std;

//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 1900, HEIGHT = 1060;

unsigned const int DELTA_TIME = 10;
unsigned const int PARTICLE_DELTA_TIME = 50;

//--------------------------------------------------------------------------------
// Typedefs
//--------------------------------------------------------------------------------

struct LightSource
{
    glm::vec3 position = glm::vec3(4.0f, 2.0f, 2.0f);
};

//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

// ID's
GLuint program_id;
GLuint singlecolor_fsh_id;
GLuint singlecolor_program_id;

std::vector<GLuint> vao;

// Matrices
glm::mat4 view, projection;

// Material and light
LightSource light;

// Camera angles
float angle_x = 0.0f;
float angle_y = 0.0f;

// Times
float delta_time = 0.0f;
float last_frame = 0.0f;

// Mouse tracking
bool mouse_active = false;
int last_x = WIDTH / 2, last_y = HEIGHT / 2;

// Mouse positions
bool first_mouse = true;
int mouse_x, mouse_y;

// Object properties
ObjectProperties* objects;
int object_amount;

// Singletons
CameraControls* camera = CameraControls::get_instance();
Beer* beer = Beer::get_instance();
Instructions* instructions = Instructions::get_instance();
Crowd* crowd = Crowd::get_instance();
RenderingHandler* rendering_handler;

//--------------------------------------------------------------------------------
// Mouse handling
//--------------------------------------------------------------------------------

void mouse_motion_handler(int x, int y)
{
    if (first_mouse)
    {
        last_x = x;
        last_y = y;
        first_mouse = false;
    }

    if (!mouse_active) {
        return;
    }

    float xOffset = x - last_x;
    float yOffset = last_y - y;

    last_x = WIDTH / 2;
    last_y = HEIGHT / 2;

    camera->update_camera_rotation(xOffset, yOffset);
    glutWarpPointer(WIDTH / 2, HEIGHT / 2);
    
}

void mouse_click_handler(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouse_active = !mouse_active;
        if (mouse_active) {
            first_mouse = true;
            glutWarpPointer(WIDTH / 2, HEIGHT / 2);
        }
    }
}

//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void keyboard_handler(unsigned char key, int a, int b)
{
    const float speed = 10.0f * delta_time;
    glm::vec3 target_position = camera->get_target_position();
    const glm::vec3 camera_lookat = camera->get_camera_lookat();
    const glm::vec3 camera_up = camera->get_camera_up();

    CameraMode current_camera_mode = camera->get_active_mode();

    if (key == 27)
        glutExit();
    if(key == 'w')
    {
        glm::vec3 updated_position = target_position + speed * camera_lookat;
        updated_position.y = target_position.y;
        camera->set_target_position(updated_position);
    }
    if(key == 's')
    {
        glm::vec3 updated_position = target_position - speed * camera_lookat;
        updated_position.y = target_position.y;
        camera->set_target_position(updated_position);
    }
    if(key == 'a')
    {
        glm::vec3 updated_position = target_position - glm::normalize(glm::cross(camera_lookat, camera_up)) * speed;
        updated_position.y = target_position.y;
        camera->set_target_position(updated_position);
    }
    if(key == 'd')
    {
        glm::vec3 updated_position = target_position + glm::normalize(glm::cross(camera_lookat, camera_up)) * speed;
        updated_position.y = target_position.y;
        camera->set_target_position(updated_position);
    }
    if(key == 'i')
    {
        camera->update_camera_rotation(0.0f, 10.0f);
    }
    if(key == 'k')
    {
        camera->update_camera_rotation(0.0f, -10.0f);
    }
    if(key == 'j')
    {
        camera->update_camera_rotation(-10.0f, 0.0f);
    }
    if(key == 'l')
    {
        camera->update_camera_rotation(10.0f, 0.0f);
    }
    if (key == 'v' || key == '1' || key == '2') {
        camera->toggle_camera_mode(key);
    }
    if (key == 'q' && current_camera_mode == Drone) {
        target_position.y -= speed;
        camera->set_target_position(target_position);
    }
    if (key == 'e' && current_camera_mode == Drone) {
        target_position.y += speed;
        camera->set_target_position(target_position);
    }
    if(key == 'b' && current_camera_mode != Drone)
    {
        if(beer->is_grabbed)
        {
            beer->drink_beer();
        }
        else
        {
            // if the character is upstairs, force it to go downstairs to the bar to "grab" a beer at the bar.
            camera->toggle_camera_mode('1');
            camera->set_camera_lookat(glm::vec3(-0.4f, -0.17f, 0.9f));
            camera->set_target_position(glm::vec3(-1.35f, 0.5f, 6.92f));
            camera->set_yaw_pitch(*camera->get_yaw(), *camera->get_pitch(), glm::normalize(camera->get_camera_lookat()));
            beer->grab_beer(program_id);
            
        }
    }
    if(key == 'm')
    {
        instructions->grab_instructions();
    }
}

//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------
void render_particles() {

    beer->update_particle_positions();
    glutSwapBuffers();

}

void render()
{
    // Lerp camera for smooth movement
    float current_frame = glutGet(GLUT_ELAPSED_TIME);
    delta_time = (current_frame - last_frame) / 1000.0f;
    last_frame = current_frame;
    camera->lerp(delta_time);

    // Get the lighting position
    GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
    glUniform3fv(uniform_light_pos, 1, glm::value_ptr(light.position));

    camera->set_vp(view, projection, angle_x, angle_y, WIDTH, HEIGHT);
    
    for (glm::uint i = 0; i < object_amount; i++)
    {
        // Check if the object has an animation and execute it
        if(objects[i].animation)
        {
            objects[i].animation->update_object(objects[i]);

            // Delete the animation in case it is completed to prevent memory leak
            if (objects[i].animation->is_completed())
            {
                delete objects[i].animation;
                objects[i].animation = nullptr;
            }
        }
        objects[i].mv = view * objects[i].model;

        // Render based on what type of shader it uses
        if (objects[i].texture == 0) {
            rendering_handler->render(projection, &objects[i], SingleColor);
        }
        else {
            rendering_handler->render(projection, &objects[i], Phong);
        }
        
        rendering_handler->draw_arrays(vao[i], objects[i].vertices.size());
    }

    // Calculate the arm positions for the FPS view
    glm::vec3 cameraPos = camera->get_target_position();
    glm::vec3 cameraLookat = camera->get_camera_lookat();
    glm::vec3 cameraUp = camera->get_camera_up();

    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraLookat, cameraUp));
    glm::vec3 cameraDown = glm::normalize(glm::cross(cameraRight, cameraLookat));

    float rightOffset = 0.6f;
    float leftOffset = -0.5f;
    float downOffset = -0.2f;
    glm::vec3 offsetRight = rightOffset * cameraRight + downOffset * cameraDown;
    glm::vec3 offsetLeft = leftOffset * cameraRight + downOffset * cameraDown;

    // Update and render the cup and instructions
    if(camera->get_active_mode() != Drone)
        beer->update_cup_position(cameraPos + cameraLookat + offsetRight, program_id, view, projection);
    instructions->update_instructions_position(cameraPos + cameraLookat + offsetLeft, program_id, view, projection);

    // Render the crowd and beer
    beer->draw_beer_particles(singlecolor_program_id, view, projection);
    crowd->draw_crowd(program_id, view, projection);
    
    // Swap buffers
    glutSwapBuffers();
}


//------------------------------------------------------------
// void Render(int n)
// Render method that is called by the timer function
//------------------------------------------------------------

void render(int n)
{
    // Define background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    render();
    glutTimerFunc(DELTA_TIME, render, 0);
}

// Slower render function for the beer tap animation
void update_beer_animation(int value) {
    render_particles();
    glutTimerFunc(PARTICLE_DELTA_TIME, update_beer_animation, 0);
}


//------------------------------------------------------------
// void InitGlutGlew(int argc, char **argv)
// Initializes Glut and Glew
//------------------------------------------------------------

void init_glut_glew(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("OpenGL Sietse van der Zee");
    //glutFullScreen();
    glutDisplayFunc(render);
    glutKeyboardFunc(keyboard_handler);
    glutMouseFunc(mouse_click_handler);
    glutPassiveMotionFunc(mouse_motion_handler);
    glutTimerFunc(DELTA_TIME, render, 0);
    glutTimerFunc(PARTICLE_DELTA_TIME, update_beer_animation, 0);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    glewInit();
}


//------------------------------------------------------------
// void InitShaders()
// Initializes the fragmentshader and vertexshader
//------------------------------------------------------------

void init_shaders()
{
    const char* fragshader_name = "fragmentshader.frag";
    const char* vertexshader_name = "vertexshader.vert";
    const char* singlecolor_fragshader_name = "singlecolor.frag";
    
    GLuint vsh_id;
    GLuint fsh_id;
    char* vertexshader = glsl::readFile(vertexshader_name);

    char* fragshader = glsl::readFile(fragshader_name);
    char* singlecolor_fragshader = glsl::readFile(singlecolor_fragshader_name);
    
    vsh_id = glsl::makeVertexShader(vertexshader);
    fsh_id = glsl::makeFragmentShader(fragshader);
    singlecolor_fsh_id = glsl::makeFragmentShader(singlecolor_fragshader);
    singlecolor_program_id = glsl::makeShaderProgram(vsh_id, singlecolor_fsh_id);
    program_id = glsl::makeShaderProgram(vsh_id, fsh_id);

    rendering_handler = RenderingHandler::get_instance(program_id, singlecolor_program_id);
}

//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------

void init_buffers()
{
    for (int i = 0; i < object_amount; i++)
    {
        vao.push_back(0);
        
        // Get vertex attributes
        const GLuint position_id = glGetAttribLocation(program_id, "position");
        const GLuint normal_id = glGetAttribLocation(program_id, "normal");
        const GLuint uv_id = glGetAttribLocation(program_id, "uv");

        // Allocate memory for vao
        glGenVertexArrays(1, &vao[i]);
        
        glBindVertexArray(vao[i]);

        // Bind vao and vbo buffers
        BufferBinder::bind_vao3d(position_id, objects[i].vertices);
        BufferBinder::bind_vao3d(normal_id, objects[i].normals);
        BufferBinder::bind_vao2d(uv_id, objects[i].uvs);

        glBindVertexArray(0);
    }

    beer->init_beer_buffers(program_id);
}


int main(int argc, char** argv)
{
    init_glut_glew(argc, argv);
    init_shaders();
    tie(objects, object_amount) = Object::get_objects();
    init_buffers();

    // Hide console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Main loop
    glutMainLoop();

    return 0;
}
