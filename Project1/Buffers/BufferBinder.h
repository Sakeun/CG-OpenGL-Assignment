#pragma once
#include <vector>
#include <GL/glew.h>

#include "../Importers/ObjectProperties.h"

class BufferBinder
{
private:
    static void bind_vbo3d(GLuint &vbo, std::vector<glm::vec3> prop);
    static void bind_vbo2d(GLuint &vbo, std::vector<glm::vec2> prop);
public:
    static void bind_vao3d(GLuint id, std::vector<glm::vec3> prop);
    static void bind_vao2d(GLuint id, std::vector<glm::vec2> prop);
};
