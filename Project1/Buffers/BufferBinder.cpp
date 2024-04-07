#include "BufferBinder.h"

void BufferBinder::bind_vbo3d(GLuint &vbo, std::vector<glm::vec3> prop)
{
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, prop.size() * sizeof(glm::vec3),
        &prop[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BufferBinder::bind_vbo2d(GLuint &vbo, std::vector<glm::vec2> prop)
{
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, prop.size() * sizeof(glm::vec2),
        &prop[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void BufferBinder::bind_vao3d(GLuint id, std::vector<glm::vec3> prop)
{
    GLuint vbo;
    bind_vbo3d(vbo, prop);


    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(id);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BufferBinder::bind_vao2d(GLuint id, std::vector<glm::vec2> prop)
{
    GLuint vbo;
    bind_vbo2d(vbo, prop);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(id);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}