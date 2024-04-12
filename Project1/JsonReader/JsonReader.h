#pragma once

#include "../json.hpp"
#include "../Importers/ObjectProperties.h"

class JsonReader
{
private:
    //static MeshProperties ReadMesh(nlohmann::json object, std::string meshType);
    static ShaderType GetShaderType(std::string shader);
public:
    static std::vector<ImportProperties*> ReadObjects();
    static std::vector<ObjectProperties*> ReadMeshes();
    static Material ReadMaterial(std::string shader);
};
