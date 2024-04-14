#pragma once
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../Structs.h"

class Object
{
private:
    
public:
    static std::tuple<ObjectProperties*, int> get_objects();
    static void init_material_lights(Material &materials);
    static glm::vec3 get_color(std::string color);
    
};
