#include "JsonReader.h"
#include <fstream>

#include "../json.hpp"
#include "../texture.h"
#include "../Importers/ObjectProperties.h"
#include "../Meshes/Cube.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../Importers/Object.h"
#include "../objloader.h"
#include "../Animations/Animation.h"

std::vector<ImportProperties*> JsonReader::ReadObjects() {
    std::ifstream file("Objects.json");
    nlohmann::json jsonObject;
    file >> jsonObject;
    std::vector<ImportProperties*> objects;

    for (auto& object : jsonObject["Objects"]) {
        ImportProperties* obj = new ImportProperties();
        obj->path = object["Path"];
        obj->texture = object["Texture"];
        obj->position = glm::vec3(object["Pos"]["x"], object["Pos"]["y"], object["Pos"]["z"]);
        obj->scale = glm::vec3(object["Scale"]["x"], object["Scale"]["y"], object["Scale"]["z"]);
        obj->rotation = glm::vec3(object["Rotation"]["x"], object["Rotation"]["y"], object["Rotation"]["z"]);
        obj->radius = object["Rotation"]["radius"];
        obj->shader = object["Shader"];
        if(object["Animation"])
        {
            obj->isAnimated = true;
            obj->xDegrees = object["AnimationProps"]["xDeg"];
            obj->yDegrees = object["AnimationProps"]["yDeg"];
            obj->zDegrees = object["AnimationProps"]["zDeg"];
        }
        if (object["Repeated"]) {
            for (int i = 1; i < object["RepeatOffset"]["amount"]; i++) {
                ImportProperties* newObj = new ImportProperties(*obj);
                newObj->position += glm::vec3(object["RepeatOffset"]["xOffset"] * i, object["RepeatOffset"]["yOffset"] * i, object["RepeatOffset"]["zOffset"] * i);
                objects.push_back(newObj);
            }
        }
        objects.push_back(obj);
    }

    file.close();

    return objects;
}

glm::vec3 getVec3(nlohmann::basic_json<> json)
{
    return glm::vec3(json[0], json[1], json[2]);
}

std::vector<ObjectProperties*> JsonReader::ReadMeshes()
{
    std::ifstream file("Meshes.json");
    nlohmann::json jsonObject;
    file >> jsonObject;
    std::vector<ObjectProperties*> objects;

    for (auto& object : jsonObject["Objects"]) {
        for (auto& jsonMesh : object["Obj"]) {
            ObjectProperties* obj = new ObjectProperties();
            if (jsonMesh["mesh"] == "Cube")
            {
                // Each mesh has it's own position, so the object can be "built" at the center. Additionally, the object (every mesh combined) has a position, to move everything together
                // Add both positions together to get the final position of the mesh.
                glm::vec3 position = getVec3(jsonMesh["position"]) + getVec3(object["Props"]["position"]);

                Cube mesh = Cube(position, getVec3(jsonMesh["scale"]), getVec3(jsonMesh["rotation"]), jsonMesh["radians"], MeshType::Cube);
                obj->vertices = mesh.triangles;
                obj->normals = mesh.normals;
                obj->uvs = mesh.uvs;
                obj->model = mesh.model;
            }
            std::string path = object["Props"]["texture"];
            obj->texture = 0;
            obj->materials = ReadMaterial(object["Props"]["Shader"]);
            if (path.find("color") != std::string::npos) {
                std::string subS = path.substr(6);
                obj->materials.diffuse_color = Object::get_color(subS);
                
            } else {
                obj->texture = loadDDS(("Textures/" + path + ".dds").c_str());
            }
            objects.push_back(obj);
        }
    }

    file.close();

    return objects;
}

ShaderType JsonReader::GetShaderType(std::string shader)
{
    if (shader == "Glossy")
        return ShaderType::Glossy;
    if (shader == "SlightReflection")
        return ShaderType::SlightReflection;
    if (shader == "Shiny")
        return ShaderType::Shiny;
    if (shader == "Matt")
        return ShaderType::Matt;
    
    return ShaderType::Glossy;
}

Material JsonReader::ReadMaterial(std::string shader)
{
    std::ifstream file("Shaders.json");
    nlohmann::json object;
    file >> object;
    Material mat;
    mat.ambient_color = getVec3(object[shader]["ambient"]);
    mat.specular_color = getVec3(object[shader]["specular"]);
    mat.power = object[shader]["shininess"];
    mat.ambient_strength = object[shader]["ambientStrength"];
    mat.specular_strength = object[shader]["specularStrength"];
    mat.diffuse_strength = object[shader]["diffuseStrength"];
    file.close();
    return mat;
}
