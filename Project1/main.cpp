#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "Animations/Animation.h"
#include "Animations/Beer.h"
#include "Animations/Crowd.h"
#include "Animations/Instructions.h"
#include "Buffers/BufferBinder.h"
#include "Camera/CameraControls.h"
#include "Importers/Object.h"
#include "Importers/ObjectProperties.h"
#include "JsonReader/JsonReader.h"
#include "Meshes/Cube.h"

using namespace std;

//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 1900, HEIGHT = 1060;

const char* fragshader_name = "fragmentshader.frag";
const char* vertexshader_name = "vertexshader.vert";
const char* singlecolor_fragshader_name = "singlecolor.frag";

unsigned const int DELTA_TIME = 10;
unsigned const int PARTICLE_DELTA_TIME = 50;

bool cubeRendered = false;

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

bool scaled = false;

// Material and light
LightSource light;

// Mouse positions
bool firstMouse = true;
int mouse_x, mouse_y;

// Camera angles
float angle_x = 0.0f;
float angle_y = 0.0f;

// Times
float delta_time = 0.0f;
float last_frame = 0.0f;

// Mouse tracking
bool mouseActive = false;
int last_x = WIDTH / 2, last_y = HEIGHT / 2;

// Object properties
ObjectProperties* objects;
int objectAmount;
std::vector<ObjectMeshes*> objectMeshes;

CameraControls* camera = CameraControls::GetInstance();
Beer* beer = Beer::GetInstance();
Instructions* instructions = Instructions::GetInstance();
Crowd* crowd = Crowd::GetInstance();

Meshes* cube = nullptr;

//--------------------------------------------------------------------------------
// Mouse handling
//--------------------------------------------------------------------------------

void mouseMotionHandler(int x, int y)
{
    if (firstMouse)
    {
        last_x = x;
        last_y = y;
        firstMouse = false;
    }

    if (!mouseActive) {
        return;
    }

    float xOffset = x - last_x;
    float yOffset = last_y - y;

    last_x = WIDTH / 2;
    last_y = HEIGHT / 2;

    camera->updateCameraRotation(xOffset, yOffset);
    glutWarpPointer(WIDTH / 2, HEIGHT / 2);
    
}

void mouseClickHandler(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouseActive = !mouseActive;
        if (mouseActive) {
            firstMouse = true;
            glutWarpPointer(WIDTH / 2, HEIGHT / 2);
        }
    }
}
//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void keyboardHandler(unsigned char key, int a, int b)
{
    const float speed = 10.0f * delta_time;
    glm::vec3 target_position = camera->getTargetPosition();
    glm::vec3 camera_lookat = camera->getCameraLookat();
    glm::vec3 camera_up = camera->getCameraUp();

    glm::vec3 camera_direction_xz = glm::normalize(glm::vec3(camera_lookat.x, 0.0f, camera_lookat.z));

    if (key == 27)
        glutExit();
    if(key == 'w')
    {
        camera->setTargetPosition(target_position + speed * camera_lookat);
    }
    if(key == 's')
    {
        camera->setTargetPosition(target_position - speed * camera_lookat);
    }
    if(key == 'a')
    {
        camera->setTargetPosition(target_position - glm::normalize(glm::cross(camera_lookat, camera_up)) * speed);
    }
    if(key == 'd')
    {
        camera->setTargetPosition(target_position + glm::normalize(glm::cross(camera_lookat, camera_up)) * speed);
    }
    if(key == 'i')
    {
        camera->updateCameraRotation(0.0f, 10.0f);
    }
    if(key == 'k')
    {
        camera->updateCameraRotation(0.0f, -10.0f);
    }
    if(key == 'j')
    {
        camera->updateCameraRotation(-10.0f, 0.0f);
    }
    if(key == 'l')
    {
        camera->updateCameraRotation(10.0f, 0.0f);
    }
    if (key == 'v') {
        firstMouse = true;
        mouseActive = !mouseActive;
        camera->isWalk = !camera->isWalk;
    }
    if (key == 'q' && !camera->isWalk) {
        target_position.y -= speed;
        camera->setTargetPosition(target_position);
    }
    if (key == 'e' && !camera->isWalk) {
        target_position.y += speed;
        camera->setTargetPosition(target_position);
    }
    if(key == '1' && camera->isWalk)
    {
        camera->isUpstairs = false;
    }
    if(key == '2' && camera->isWalk)
    {
        camera->isUpstairs = true;
    }
    if(key == 'b' && camera->isWalk)
    {
        if(beer->isGrabbed)
        {
            beer->DrinkBeer();
        }
        else
        {
            camera->isUpstairs = false;
            camera->setCameraLookat(glm::vec3(-0.4f, -0.17f, 0.9f));
            camera->setTargetPosition(glm::vec3(-1.35f, 0.5f, 6.92f));
            camera->SetYawPitch(*camera->getYaw(), *camera->getPitch(), glm::normalize(camera->getCameraLookat()));
            beer->GrabBeer(program_id);
            
        }
    }
    if(key == 'm')
    {
        instructions->GrabInstructions();
    }

    glutPostRedisplay();
}

//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------
void RenderParticles() {

    beer->UpdatePositions();
    glutSwapBuffers();

}

void Render()
{
    // Define background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float current_frame = glutGet(GLUT_ELAPSED_TIME);
    delta_time = (current_frame - last_frame) / 1000.0f;
    last_frame = current_frame;
    camera->Lerp(delta_time);

    GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
    glUniform3fv(uniform_light_pos, 1, glm::value_ptr(light.position));

    tie(view, projection) = camera->SetVP(angle_x, angle_y, WIDTH, HEIGHT);
    
    for (glm::uint i = 0; i < objectAmount; i++)
    {
        // Do transformation
        if(objects[i].animation)
        {
            objects[i].animation->Execute(objects[i].model);
            if (objects[i].animation->IsCompleted())
            {
                delete objects[i].animation;
                objects[i].animation = nullptr;
            }
        }

        objects[i].mv = view * objects[i].model;

        if (objects[i].texture == 0) {
            glUseProgram(singlecolor_program_id);

            // Send mv
            GLint uniform_mv = glGetUniformLocation(singlecolor_program_id, "mv");
            glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(objects[i].mv));

            // Make uniform vars
            uniform_mv = glGetUniformLocation(singlecolor_program_id, "mv");
            const GLuint uniform_proj = glGetUniformLocation(singlecolor_program_id, "projection");
            const GLuint uniform_light_pos = glGetUniformLocation(singlecolor_program_id, "light_pos");
            GLint fragColLocation = glGetUniformLocation(singlecolor_program_id, "FragCol");
            const GLuint uniform_material_ambient = glGetUniformLocation(program_id, "mat_ambient");
            const GLuint uniform_specular = glGetUniformLocation(program_id, "mat_specular");
            const GLuint uniform_material_power = glGetUniformLocation(program_id, "mat_power");
            const GLuint uniform_reflection_ambient = glGetUniformLocation(program_id, "reflection_ambient");
            const GLuint uniform_reflection_diffuse = glGetUniformLocation(program_id, "reflection_diffuse");
            const GLuint uniform_reflection_specular = glGetUniformLocation(program_id, "reflection_specular");

            // Fill uniform vars
            glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
            glm::vec3 color = objects[i].materials.diffuse_color;
            glUniform3f(fragColLocation, color.r, color.g, color.b);
            glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(objects[i].materials.ambient_color));
            glUniform3fv(uniform_specular, 1, glm::value_ptr(objects[i].materials.specular_color));
            glUniform1f(uniform_material_power, objects[i].materials.power);
            glUniform1f(uniform_reflection_ambient, objects[i].materials.ambient_strength);
            glUniform1f(uniform_reflection_diffuse, 1.0f);
            glUniform1f(uniform_reflection_specular, objects[i].materials.specular_strength);
        }
        else {
            glUseProgram(program_id);
            // Send mv
            GLint uniform_mv = 0;
            glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(objects[i].mv));
            glUseProgram(program_id);

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
            glBindTexture(GL_TEXTURE_2D, objects[i].texture);

            glUniform1i(glGetUniformLocation(program_id, "texsampler"), 0);
            // Fill uniform vars
            glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(objects[i].materials.ambient_color));
            glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(objects[i].materials.diffuse_color));
            glUniform3fv(uniform_specular, 1, glm::value_ptr(objects[i].materials.specular_color));
            glUniform1f(uniform_material_power, objects[i].materials.power);
            glUniform1f(uniform_reflection_ambient, objects[i].materials.ambient_strength);
            glUniform1f(uniform_reflection_diffuse, objects[i].materials.diffuse_strength);
            glUniform1f(uniform_reflection_specular, objects[i].materials.specular_strength);
        }

        // Send vao
        glBindVertexArray(vao[i]);
        glDrawArrays(GL_TRIANGLES, 0, objects[i].vertices.size());
        glBindVertexArray(0);
    }
    beer->DrawBeer(singlecolor_program_id, view, projection);

    glm::vec3 cameraPos = camera->getTargetPosition();
    glm::vec3 cameraLookat = camera->getCameraLookat();
    glm::vec3 cameraUp = camera->getCameraUp();

    // Calculate the camera's right and down vectors
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraLookat, cameraUp));
    glm::vec3 cameraDown = glm::normalize(glm::cross(cameraRight, cameraLookat));

    // Define the offset from the camera position
    float rightOffset = 0.6f; // Adjust this value as needed
    float leftOffset = -0.5f;
    float downOffset = -0.2f; // Adjust this value as needed
    glm::vec3 offsetRight = rightOffset * cameraRight + downOffset * cameraDown;
    glm::vec3 offsetLeft = leftOffset * cameraRight + downOffset * cameraDown;

    if(camera->isWalk)
        beer->UpdateCupPosition(cameraPos + cameraLookat + offsetRight, program_id, view, projection);

    instructions->UpdateInstructionsPosition(cameraPos + cameraLookat + offsetLeft, program_id, view, projection);

    crowd->DrawCrowd(program_id, view, projection);
    
    // Swap buffers
    glutSwapBuffers();
}


//------------------------------------------------------------
// void Render(int n)
// Render method that is called by the timer function
//------------------------------------------------------------

void Render(int n)
{
    Render();
    glutTimerFunc(DELTA_TIME, Render, 0);
}

void UpdateBeerAnimation(int value) {
    RenderParticles();
    glutTimerFunc(PARTICLE_DELTA_TIME, UpdateBeerAnimation, 0);
}


//------------------------------------------------------------
// void InitGlutGlew(int argc, char **argv)
// Initializes Glut and Glew
//------------------------------------------------------------

void InitGlutGlew(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("OpenGL Sietse van der Zee");
    //glutFullScreen();
    glutDisplayFunc(Render);
    glutKeyboardFunc(keyboardHandler);
    glutMouseFunc(mouseClickHandler);
    glutPassiveMotionFunc(mouseMotionHandler);
    glutTimerFunc(DELTA_TIME, Render, 0);
    glutTimerFunc(PARTICLE_DELTA_TIME, UpdateBeerAnimation, 0);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    glewInit();
}


//------------------------------------------------------------
// void InitShaders()
// Initializes the fragmentshader and vertexshader
//------------------------------------------------------------

void InitShaders()
{
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
}

//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------

void InitBuffers()
{
    for (int i = 0; i < objectAmount; i++)
    {
        vao.push_back(0);
        
        // Get vertex attributes
        const GLuint position_id = glGetAttribLocation(program_id, "position");
        const GLuint normal_id = glGetAttribLocation(program_id, "normal");
        const GLuint uv_id = glGetAttribLocation(program_id, "uv");

        // Allocate memory for vao
        glGenVertexArrays(1, &vao[i]);
        
        glBindVertexArray(vao[i]);

        BufferBinder::bind_vao3d(position_id, objects[i].vertices);
        BufferBinder::bind_vao3d(normal_id, objects[i].normals);
        BufferBinder::bind_vao2d(uv_id, objects[i].uvs);

        glBindVertexArray(0);
    }

    beer->InitBeerBuffers(program_id);
}


int main(int argc, char** argv)
{
    InitGlutGlew(argc, argv);
    InitShaders();
    tie(objects, objectAmount) = Object::get_objects();
    tie(view, projection) = camera->SetVP(angle_x, angle_y, WIDTH, HEIGHT);
    InitBuffers();

    // Hide console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Main loop
    glutMainLoop();

    return 0;
}
