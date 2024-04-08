#include "JsonReader.h"
#include <fstream>

#include "../json.hpp"
#include "../texture.h"
#include "../Importers/ObjectProperties.h"

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
        obj->rotation = glm::vec3(object["Pos"]["x"], object["Pos"]["y"], object["Pos"]["z"]);
        objects.push_back(obj);
    }

    file.close();

    return objects;
}

MeshProperties JsonReader::ReadMesh(nlohmann::json object, std::string meshType)
{
    MeshProperties meshProperties;
    auto& meshObject = object["Meshes"][0][meshType];

    for (auto& element : meshObject["elements"]) {
        meshProperties.elements.push_back(element);
    }
    for (auto& vertex : meshObject["vertices"]) {
        meshProperties.vertices.push_back(vertex);
    }
    for (auto& textCoord : meshObject["uvs"]) {
        meshProperties.textCoords.push_back(textCoord);
    }

    return meshProperties;
}

std::vector<ObjectMeshes*> JsonReader::ReadMeshes()
{
    std::ifstream file("Meshes.json");
    nlohmann::json jsonObject;
    file >> jsonObject;
    std::vector<ObjectMeshes*> objects;

    for (auto& object : jsonObject["Objects"]) {
        ObjectMeshes* obj = new ObjectMeshes();
        obj->model = glm::mat4(1.0f);
        obj->meshes.push_back(ReadMesh(jsonObject, object["Cube"]["mesh"]));
        obj->texture = loadDDS("Textures/uvmap.DDS");
        obj->position = glm::vec3(object["Cube"]["position"][0], object["Cube"]["position"][1], object["Cube"]["position"][2]);
        obj->scale = glm::vec3(object["Cube"]["scale"][0], object["Cube"]["scale"][1], object["Cube"]["scale"][2]);
        obj->rotation = glm::vec3(object["Cube"]["rotation"][0], object["Cube"]["rotation"][1], object["Cube"]["rotation"][2]);
        objects.push_back(obj);
    }

    file.close();

    return objects;
}
