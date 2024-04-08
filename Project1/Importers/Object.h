#pragma once
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ObjectProperties.h"

class Object
{
private:
    
    
public:
    static std::tuple<ObjectProperties*, int> get_objects();
    static void InitMaterialLights(Material &materials);
    
};
