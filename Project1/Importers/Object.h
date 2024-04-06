#pragma once
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ObjectProperties.h"

class Object
{
private:
    static glm::mat4 import_object();
    static GLuint import_texture();
    
public:
    static std::tuple<ObjectProperties*, int> get_objects();
    
};
