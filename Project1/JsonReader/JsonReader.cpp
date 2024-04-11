#include "JsonReader.h"
#include <fstream>

#include "../json.hpp"
#include "../texture.h"
#include "../Importers/ObjectProperties.h"
#include "../Meshes/Cube.h"
#include <glm/gtc/matrix_transform.hpp>

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
                glm::vec3 position = getVec3(jsonMesh["position"]) + getVec3(object["Props"]["position"]);
                Cube mesh = Cube(position, getVec3(jsonMesh["scale"]), getVec3(jsonMesh["rotation"]), 20.0f, MeshType::Cube);
                obj->vertices = mesh.triangles;
                obj->normals = mesh.normals;
                obj->uvs = mesh.uvs;
                obj->model = mesh.model;
            }
            std::string path = object["Props"]["texture"];
            obj->texture = loadDDS(("Textures/" + path + ".dds").c_str());
            objects.push_back(obj);
        }
    }

    file.close();

    return objects;
}
