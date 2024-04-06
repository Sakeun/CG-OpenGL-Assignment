#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "objloader.h"
#include "texture.h"
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
GLuint texture_id[SIZE_CONSTANT];

// Uniform ID's
GLuint uniform_mv[SIZE_CONSTANT];

// Matrices
glm::mat4 model[SIZE_CONSTANT], view, projection;
glm::mat4 mv[SIZE_CONSTANT];

// Material and light
LightSource light;
Material material[SIZE_CONSTANT];


//--------------------------------------------------------------------------------
// Mesh variables
//--------------------------------------------------------------------------------

vector<glm::vec3> vertices[SIZE_CONSTANT];
vector<glm::vec3> normals[SIZE_CONSTANT];
vector<glm::vec2> uvs[SIZE_CONSTANT];


//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void keyboardHandler(unsigned char key, int a, int b)
{
    if (key == 27)
        glutExit();
}

//------------------------------------------------------------
// void InitMaterialsLight()
//------------------------------------------------------------

void InitMaterialsLight(int i)
{
    light.position = glm::vec3(4.0, 4.0, 4.0);
    material[i].ambient_color = glm::vec3(0.2, 0.2, 0.1);
    //material.diffuse_color = glm::vec3(0.5, 0.5, 0.3);
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
    
    for (int i = 0; i < 2; i++)
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
// void InitMatrices()
//------------------------------------------------------------

void InitMatrices()
{
    view = glm::lookAt(
        glm::vec3(0.0, 2.0, 4.0),
        glm::vec3(0.0, 0.5, 0.0),
        glm::vec3(0.0, 1.0, 0.0));
    projection = glm::perspective(
        glm::radians(45.0f),
        1.0f * WIDTH / HEIGHT, 0.1f,
        20.0f);
    
    for (int i = 0; i < 2; i++)
    {
        objects[i].model = glm::mat4();
        objects[i].mv = view * objects[i].model;
    }
}


//------------------------------------------------------------
// void InitObjects()
//------------------------------------------------------------

void InitObjects()
{
    bool res;

    // Objects

    res = loadOBJ("Objects/Axes.obj", vertices[0], uvs[0], normals[0]);
    texture_id[0] = loadDDS("Textures/AxesTexture.dds");

    res = loadOBJ("Objects/box.obj", vertices[1], uvs[1], normals[1]);
    texture_id[1] = loadBMP("Textures/Yellobrk.bmp");

    res = loadOBJ("Objects/box.obj", vertices[2], uvs[2], normals[2]);
    texture_id[2] = texture_id[1];

    model[1] = glm::translate(model[1], glm::vec3(1.0, 2.0, -1.0));
    model[2] = glm::translate(model[2], glm::vec3(-1.0, 2.0, -1.0));
}

//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------

void InitBuffers()
{
    // GLuint position_id[2];
    // GLuint normal_id[2];
    // GLuint uv_id[2];
    GLuint vbo_vertices;
    GLuint vbo_normals;
    GLuint vbo_uvs;
    for (int i = 0; i < 2; i++)
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
    objects = Object::get_objects();
    InitMatrices();
    InitBuffers();

    // Hide console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Main loop
    glutMainLoop();

    return 0;
}
