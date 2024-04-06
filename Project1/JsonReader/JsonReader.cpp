#include "JsonReader.h"
#include <fstream>

#include "../json.hpp"
#include "../Importers/ObjectProperties.h"

std::vector<JsonProperties*> JsonReader::ReadObjects() {
    std::ifstream file("Objects.json");
    nlohmann::json jsonObject;
    file >> jsonObject;
    std::vector<JsonProperties*> objects;

    for (auto& object : jsonObject["Objects"]) {
        JsonProperties* obj = new JsonProperties();
        obj->path = object["Path"];
        obj->texture = object["Texture"];
        obj->position = glm::vec3(object["Pos"]["x"], object["Pos"]["y"], object["Pos"]["z"]);
        obj->scale = glm::vec3(object["Scale"]["x"], object["Scale"]["y"], object["Scale"]["z"]);
        obj->rotation = glm::vec3(object["Pos"]["x"], object["Pos"]["y"], object["Pos"]["z"]);
        objects.push_back(obj);
    }

    return objects;
}
