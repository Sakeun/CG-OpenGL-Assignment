#pragma once

#include "../json.hpp"
#include "../Structs.h"

class JsonReader
{
private:
    static ShaderType get_shader_type(std::string shader);
public:
    static std::vector<ImportProperties*> read_objects();
    static std::vector<ObjectProperties*> read_meshes();
    static Material read_materials(std::string shader);
};
