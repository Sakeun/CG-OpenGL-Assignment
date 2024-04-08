#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "Buffers/BufferBinder.h"
#include "Camera/CameraControls.h"
#include "Importers/Object.h"
#include "Importers/ObjectProperties.h"
#include "JsonReader/JsonReader.h"

using namespace std;

//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 1900, HEIGHT = 1060;

const char* fragshader_name = "fragmentshader.frag";
const char* vertexshader_name = "vertexshader.vert";

unsigned const int DELTA_TIME = 10;

bool cubeRendered = false;

// Indices
std::vector<int> Indices = {
    1, 0, 0,    0, 1, 0,    3, 2, 0, // Front face
    1, 0, 0,    3, 2, 0,    2, 3, 0,

    1, 0, 1,    5, 1, 1,    6, 2, 1, // Right face
    6, 2, 1,    2, 3, 1,    1, 0, 1,

    6, 2, 2,    5, 1, 2,    4, 0, 2, // Top face
    4, 0, 2,    7, 3, 2,    6, 2, 2,

    5, 1, 3,    1, 0, 3,    0, 1, 3, // Left face
    0, 1, 3,    4, 0, 3,    5, 1, 3,

    2, 3, 4,    6, 2, 4,    7, 3, 4, // Back face
    7, 3, 4,    3, 2, 4,    2, 3, 4,

    5, 1, 5,    4, 0, 5,    0, 1, 5, // Bottom face
    0, 1, 5,    1, 0, 5,    5, 1, 5
};
GLuint ibo;

//--------------------------------------------------------------------------------
// Typedefs
//--------------------------------------------------------------------------------

struct LightSource
{
    glm::vec3 position = glm::vec3(1.0f, 0.5f, -1.5f);
};

//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

// ID's
GLuint program_id;
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
int last_x = 950, last_y = 530;

// Object properties
ObjectProperties* objects;
int objectAmount;
std::vector<ObjectMeshes*> objectMeshes;

CameraControls* camera = CameraControls::GetInstance();

//--------------------------------------------------------------------------------
// Mouse handling
//--------------------------------------------------------------------------------

void mouseMotionHandler(int x, int y)
{
    if(!mouseActive || firstMouse) {
        firstMouse = false;
        return;
    }

    float xOffset = x - WIDTH / 2;
    float yOffset = HEIGHT / 2 - y;
    last_x = x;
    last_y = y;

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
            glutWarpPointer(last_x, last_y);
        }
    }
}
//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void keyboardHandler(unsigned char key, int a, int b)
{
    const float speed = 2.5f * delta_time;
    glm::vec3 camera_direction_xz = glm::normalize(glm::vec3(camera->camera_lookat.x, 0.0f, camera->camera_lookat.z));

    if (key == 27)
        glutExit();
    if(key == 'w')
    {
        camera->target_position += speed * camera_direction_xz;
    }
    if(key == 's')
    {
        camera->target_position -= speed * camera_direction_xz;
    }
    if(key == 'a')
    {
        camera->target_position -= glm::normalize(glm::cross(camera->camera_lookat, camera->camera_up)) * speed;
    }
    if(key == 'd')
    {
        camera->target_position += glm::normalize(glm::cross(camera->camera_lookat, camera->camera_up)) * speed;
    }
    if(key == 'i')
    {
        angle_y += speed;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), speed, glm::vec3(1.0f, 0.0f, 0.0f));
        camera->camera_lookat = glm::vec3(rotation * glm::vec4(camera->camera_lookat, 0.0f));
    }
    if(key == 'k')
    {
        angle_y -= speed;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), -speed, glm::vec3(1.0f, 0.0f, 0.0f));
        camera->camera_lookat = glm::vec3(rotation * glm::vec4(camera->camera_lookat, 0.0f));
    }
    if(key == 'j')
    {
        angle_x += speed;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), speed, glm::vec3(0.0f, 1.0f, 0.0f));
        camera->camera_lookat = glm::vec3(rotation * glm::vec4(camera->camera_lookat, 0.0f));
    }
    if(key == 'l')
    {
        angle_x -= speed;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), -speed, glm::vec3(0.0f, 1.0f, 0.0f));
        camera->camera_lookat = glm::vec3(rotation * glm::vec4(camera->camera_lookat, 0.0f));
    }

    glutPostRedisplay();
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

    float current_frame = glutGet(GLUT_ELAPSED_TIME);
    delta_time = (current_frame - last_frame) / 1000.0f;
    last_frame = current_frame;
    camera->Lerp(delta_time);

    GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
    glUniform3fv(uniform_light_pos, 1, glm::value_ptr(light.position));

    tie(view, projection) = camera->SetVP(angle_x, angle_y, WIDTH, HEIGHT);
    
    for (int i = 0; i < objectAmount; i++)
    {
        objects[i].mv = view * objects[i].model;
        // Do transformation
        if(i != 0)
        {
            objects[i].model = glm::rotate(objects[i].model, 0.01f, glm::vec3(0.5f, 1.0f, 0.2f));
            objects[i].mv = view * objects[i].model;
        }

        // Send mv
        GLint uniform_mv = 0;
        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(objects[i].mv));
        glUseProgram(program_id);

        Object::InitMaterialLights(objects[i].materials);
        
        // Make uniform vars
        uniform_mv = glGetUniformLocation(program_id, "mv");
        const GLuint uniform_proj = glGetUniformLocation(program_id, "projection");
        const GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
        const GLuint uniform_material_ambient = glGetUniformLocation(program_id, "mat_ambient");
        const GLuint uniform_material_diffuse = glGetUniformLocation(program_id, "mat_diffuse");
        const GLuint uniform_specular = glGetUniformLocation(program_id, "mat_specular");
        const GLuint uniform_material_power = glGetUniformLocation(program_id, "mat_power");

        //Bind Texture
        glBindTexture(GL_TEXTURE_2D, objects[i].texture);
        
        // Fill uniform vars
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(objects[i].materials.ambient_color));
        glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(objects[i].materials.diffuse_color));
        glUniform3fv(uniform_specular, 1, glm::value_ptr(objects[i].materials.specular_color));
        glUniform1f(uniform_material_power, objects[i].materials.power);

        // Send vao
        glBindVertexArray(vao[i]);
        glDrawArrays(GL_TRIANGLES, 0, objects[i].vertices.size());
        glBindVertexArray(0);
    }

    for (auto& object : objectMeshes)
    {
        object->model = glm::translate(object->model, object->position);
        if(!scaled)
            //object->model = glm::scale(object->model, object->scale);
        scaled = true;
        //object->model = glm::rotate(object->model, 0.01f, glm::vec3(0.5f, 1.0f, 0.2f));
        object->mv = view * object->model;

        // Send mv
        GLint uniform_mv = 0;
        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(object->mv));
        glUseProgram(program_id);

        Object::InitMaterialLights(object->materials);

        const GLuint uniform_proj = glGetUniformLocation(program_id, "projection");
        const GLuint uniform_material_ambient = glGetUniformLocation(program_id, "mat_ambient");
        const GLuint uniform_material_diffuse = glGetUniformLocation(program_id, "mat_diffuse");
        const GLuint uniform_specular = glGetUniformLocation(program_id, "mat_specular");
        const GLuint uniform_material_power = glGetUniformLocation(program_id, "mat_power");

        //Bind Texture
        glBindTexture(GL_TEXTURE_2D, object->texture);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set the texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Fill uniform vars
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(object->materials.ambient_color));
        glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(object->materials.diffuse_color));
        glUniform3fv(uniform_specular, 1, glm::value_ptr(object->materials.specular_color));
        glUniform1f(uniform_material_power, object->materials.power);

        // Bind VAO and draw elements
        glBindVertexArray(vao[objectAmount]); // Assuming the VAO for the cube is stored at index objectAmount
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }

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
    //glutFullScreen();
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
    
    for(auto object : objectMeshes)
    {
        vao.push_back(0);

        std::vector<glm::vec3> vertices = {
            glm::vec3(-1.0f, -1.0f, 1.0f),  // 0 Bottom left front
            glm::vec3(1.0f, -1.0f, 1.0f),  // 1 Bottom right front
            glm::vec3(1.0f, 1.0f, 1.0f),  // 2 Top right front
            glm::vec3(-1.0f, 1.0f, 1.0f),  // 3 Top left front
            glm::vec3(-1.0f, -1.0f, -1.0f),  // 4 Bottom left back
            glm::vec3(1.0f, -1.0f, -1.0f),  // 5 Bottom right back
            glm::vec3(1.0f, 1.0f, -1.0f),  // 6 Top right back
            glm::vec3(-1.0f, 1.0f, -1.0f),  // 7 Top left back
        };

        // depth = 1, height = 0.5 and width = 0.5

        std::vector<glm::vec2> uvs = {
            // Front face
            glm::vec2(0.5f, 0.0f), // Bottom left
            glm::vec2(0.5f, 0.5f), // Bottom right
            glm::vec2(0.0f, 0.5f), // Top right
            glm::vec2(0.0f, 0.0f), // Top left
            glm::vec2(1.0f, 0.0f), // Bottom left
            glm::vec2(1.0f, 0.5f), // Bottom right
            glm::vec2(0.5f, 1.0f), // Top right
            glm::vec2(0.0f, 1.0f), // Top left
        };
        
        std::vector<glm::vec3> normals = {
            glm::vec3(0.0f, 0.0f, 1.0f),  // 0 Front face
            glm::vec3(1.0f, 0.0f, 0.0f),  // 1 Right face
            glm::vec3(0.0f, 1.0f, 0.0f),  // 2 Top face
            glm::vec3(-1.0f, 0.0f, 0.0f),  // 3 Left face
            glm::vec3(0.0f, 0.0f, -1.0f), // 4 Back face
            glm::vec3(0.0f, -1.0f, 0.0f), // 5 Bottom face
        };

        std::vector<GLushort> indices = {
            0, 1, 2,  2, 3, 0,   // Front face
            1, 5, 6,  6, 2, 1,   // Right face
            7, 6, 5,  5, 4, 7,   // Back face
            4, 0, 3,  3, 7, 4,   // Left face
            3, 2, 6,  6, 7, 3,   // Top face
            4, 5, 1,  1, 0, 4,   // Bottom face
        };

        std::vector<glm::vec3> finalVertices;
        std::vector<glm::vec2> finalUVs;
        std::vector<glm::vec3> finalNormals;

        for(unsigned int i = 0; i < Indices.size(); i += 3)
        {
            finalVertices.push_back(vertices[Indices[i]]);
            finalUVs.push_back(uvs[Indices[i + 1]]);
            finalNormals.push_back(normals[Indices[i + 2]]);
        }

        const GLuint position_id = glGetAttribLocation(program_id, "position");
        const GLuint normal_id = glGetAttribLocation(program_id, "normal");
        const GLuint uv_id = glGetAttribLocation(program_id, "uv");
        
        // Allocate memory for vao
        glBindVertexArray(vao[vao.size() - 1]);

        BufferBinder::bind_vao3d(position_id, vertices);
        BufferBinder::bind_vao3d(normal_id, normals);
        BufferBinder::bind_vao2d(uv_id, uvs);
        
        // Generate index buffer
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }
}


int main(int argc, char** argv)
{
    InitGlutGlew(argc, argv);
    InitShaders();
    tie(objects, objectAmount) = Object::get_objects();
    objectMeshes = JsonReader::ReadMeshes();
    tie(view, projection) = camera->SetVP(angle_x, angle_y, WIDTH, HEIGHT);
    InitBuffers();

    // Hide console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Main loop
    glutMainLoop();

    return 0;
}
