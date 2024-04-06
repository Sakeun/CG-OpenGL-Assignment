#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "Importers/Object.h"
#include "Importers/ObjectProperties.h"

using namespace std;


//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 1900, HEIGHT = 1060;

const char* fragshader_name = "fragmentshader.frag";
const char* vertexshader_name = "vertexshader.vert";

unsigned const int DELTA_TIME = 10;

const int SIZE_CONSTANT = 3;

ObjectProperties* objects;
int objectAmount;


//--------------------------------------------------------------------------------
// Typedefs
//--------------------------------------------------------------------------------

struct LightSource
{
    glm::vec3 position;
};

//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

// ID's
GLuint program_id;
GLuint vao[SIZE_CONSTANT];

// Uniform ID's
GLuint uniform_mv[SIZE_CONSTANT];

// Matrices
glm::mat4 view, projection;
glm::mat4 mv[SIZE_CONSTANT];

// Material and light
LightSource light;
Material material[SIZE_CONSTANT];

// Mouse positions
bool firstMouse = true;
int mouse_x, mouse_y;

// Camera angles
float angle_x = 0.0f;
float angle_y = 0.0f;

// Camera positions
glm::vec3 camera_position = glm::vec3(0.0, 0.5, 3.0);
glm::vec3 camera_lookat = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 camera_up = glm::vec3(0.0, 1.0, 0.0);

// Times
float delta_time = 0.0f;
float last_frame = 0.0f;

glm::vec3 target_position = camera_position;
const float lerp_speed = 0.1f;
bool mouseActive = false;
int last_x = 950, last_y = 530;

float yaw = -90.0f;
float pitch;

glm::vec3 lerp(glm::vec3 start, glm::vec3 end, float time) {
    return (1.0f - time) * start + time * end;
}

//------------------------------------------------------------
// void InitMatrices()
//------------------------------------------------------------

void InitMatrices()
{
    // Create rotation matrix
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle_x, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around y-axis
    rotation = glm::rotate(rotation, angle_y, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around x-axis

    // Apply rotation to camera_lookat vector
    glm::vec4 rotated_lookat = rotation * glm::vec4(camera_lookat, 0.0f);
    glm::vec3 new_lookat(rotated_lookat);

    view = glm::lookAt(target_position, target_position + new_lookat, camera_up);
    projection = glm::perspective(
        glm::radians(45.0f),
        1.0f * WIDTH / HEIGHT, 0.1f,
        20.0f);

    for (int i = 0; i < objectAmount; i++)
    {
        objects[i].mv = view * objects[i].model;
    }
}

//--------------------------------------------------------------------------------
// Mouse handling
//--------------------------------------------------------------------------------

void mouseMotionHandler(int x, int y)
{
    if(!mouseActive) return;

    if(firstMouse)
    {
        last_x = x;
        last_y = y;
        firstMouse = false;
    }

    float xOffset = x - last_x;
    float yOffset = last_y - y;
    last_x = x;
    last_y = y;

    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_lookat = glm::normalize(direction);
}

void mouseClickHandler(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouseActive = !mouseActive;
        mouse_x = WIDTH / 2;
        mouse_y = HEIGHT / 2;
        glutWarpPointer(mouse_x, mouse_y);
    }
}

//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void keyboardHandler(unsigned char key, int a, int b)
{
    const float speed = 2.5f * delta_time;
    glm::vec3 camera_direction_xz = glm::normalize(glm::vec3(camera_lookat.x, 0.0f, camera_lookat.z));

    if (key == 27)
        glutExit();
    if(key == 'w')
    {
        target_position += speed * camera_direction_xz;
    }
    if(key == 's')
    {
        target_position -= speed * camera_direction_xz;
    }
    if(key == 'a')
    {
        target_position -= glm::normalize(glm::cross(camera_lookat, camera_up)) * speed;
    }
    if(key == 'd')
    {
        target_position += glm::normalize(glm::cross(camera_lookat, camera_up)) * speed;
    }
    if(key == 'i')
    {
        angle_y += speed;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), speed, glm::vec3(1.0f, 0.0f, 0.0f));
        camera_lookat = glm::vec3(rotation * glm::vec4(camera_lookat, 0.0f));
    }
    if(key == 'k')
    {
        angle_y -= speed;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), -speed, glm::vec3(1.0f, 0.0f, 0.0f));
        camera_lookat = glm::vec3(rotation * glm::vec4(camera_lookat, 0.0f));
    }
    if(key == 'j')
    {
        angle_x -= speed;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), -speed, glm::vec3(0.0f, 1.0f, 0.0f));
        camera_lookat = glm::vec3(rotation * glm::vec4(camera_lookat, 0.0f));
    }
    if(key == 'l')
    {
        angle_x += speed;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), speed, glm::vec3(0.0f, 1.0f, 0.0f));
        camera_lookat = glm::vec3(rotation * glm::vec4(camera_lookat, 0.0f));
    }

    glutPostRedisplay();
}

//------------------------------------------------------------
// void InitMaterialsLight()
//------------------------------------------------------------

void InitMaterialsLight(int i)
{
    light.position = glm::vec3(4.0, 4.0, 4.0);
    material[i].ambient_color = glm::vec3(0.2, 0.2, 0.1);
    material[i].diffuse_color = glm::vec3(0.5, 0.5, 0.3);
    material[i].specular_color = glm::vec3(1.0, 1.0, 1.0);
    material[i].power = 50.0;
}


//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------

void Render()
{
    // Define background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program_id);

    GLuint uniform_proj;
    GLuint uniform_light_pos;
    GLuint uniform_material_ambient;
    GLuint uniform_material_diffuse;
    GLuint uniform_specular;
    GLuint uniform_material_power;


    float current_frame = glutGet(GLUT_ELAPSED_TIME);
    delta_time = (current_frame - last_frame) / 1000.0f;
    last_frame = current_frame;
    camera_position = lerp(camera_position, target_position, lerp_speed * delta_time);

    InitMatrices();
    
    for (int i = 0; i < objectAmount; i++)
    {
        // Do transformation
        if(i != 0)
        {
            objects[i].model = glm::rotate(objects[i].model, 0.01f, glm::vec3(0.5f, 1.0f, 0.2f));
            objects[i].mv = view * objects[i].model;
        }

        // Attach to program_id

        // Send mv
        glUniformMatrix4fv(uniform_mv[i], 1, GL_FALSE, glm::value_ptr(objects[i].mv));
        glUseProgram(program_id);

        InitMaterialsLight(i);
        
        // Make uniform vars
        uniform_mv[i] = glGetUniformLocation(program_id, "mv");
        uniform_proj = glGetUniformLocation(program_id, "projection");
        uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
        uniform_material_ambient = glGetUniformLocation(program_id, "mat_ambient");
        uniform_material_diffuse = glGetUniformLocation(program_id, "mat_diffuse");
        uniform_specular = glGetUniformLocation(program_id, "mat_specular");
        uniform_material_power = glGetUniformLocation(program_id, "mat_power");

        // Attach to program (needed to fill uniform vars)
        //Bind Texture
        glBindTexture(GL_TEXTURE_2D, objects[i].texture);
        
        // Fill uniform vars
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(uniform_light_pos, 1, glm::value_ptr(light.position));
        glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(material[i].ambient_color));
        glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(material[i].diffuse_color));
        glUniform3fv(uniform_specular, 1, glm::value_ptr(material[i].specular_color));
        glUniform1f(uniform_material_power, material[i].power);

        // Send vao
        glBindVertexArray(vao[i]);
        glDrawArrays(GL_TRIANGLES, 0, objects[i].vertices.size());
    }

        glBindVertexArray(0);
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
    glutDisplayFunc(Render);
    glutKeyboardFunc(keyboardHandler);
    glutMouseFunc(mouseClickHandler);
    glutPassiveMotionFunc(mouseMotionHandler);
    glutTimerFunc(DELTA_TIME, Render, 0);

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
    
    vsh_id = glsl::makeVertexShader(vertexshader);
    fsh_id = glsl::makeFragmentShader(fragshader);
    program_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}

//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------

void InitBuffers()
{
    GLuint vbo_vertices;
    GLuint vbo_normals;
    GLuint vbo_uvs;
    for (int i = 0; i < objectAmount; i++)
    {
        // vbo for vertices
        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, objects[i].vertices.size() * sizeof(glm::vec3),
            &objects[i].vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo for normals
        glGenBuffers(1, &vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, objects[i].normals.size() * sizeof(glm::vec3),
            &objects[i].normals[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo for uvs
        glGenBuffers(1, &vbo_uvs);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
        glBufferData(GL_ARRAY_BUFFER, objects[i].uvs.size() * sizeof(glm::vec2),
            &objects[i].uvs[0], GL_STATIC_DRAW);   
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Get vertex attributes
        const GLuint position_id = glGetAttribLocation(program_id, "position");
        const GLuint normal_id = glGetAttribLocation(program_id, "normal");
        const GLuint uv_id = glGetAttribLocation(program_id, "uv");

        // Allocate memory for vao
        glGenVertexArrays(1, &vao[i]);

        // Bind to vao
        glBindVertexArray(vao[i]);

        // Bind vertices to vao
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Bind normals to vao
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normal_id);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Bind uvs to vao
        glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
        glVertexAttribPointer(uv_id, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(uv_id);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Stop bind to vao
        glBindVertexArray(0);
    }
}


int main(int argc, char** argv)
{
    InitGlutGlew(argc, argv);
    InitShaders();
    tie(objects, objectAmount) = Object::get_objects();
    InitMatrices();
    InitBuffers();

    // Hide console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Main loop
    glutMainLoop();

    return 0;
}
