#include "JsonReader.h"
#include <fstream>

#include "../json.hpp"

std::map<std::string, std::pair<std::string, glm::vec3>> JsonReader::ReadObjects() {
    std::ifstream file("Objects.json");
    nlohmann::json jsonObject;
    file >> jsonObject;
    std::map<std::string, std::pair<std::string, glm::vec3>> objects;

    for (auto& object : jsonObject["Objects"]) {
        std::string path = object["Path"];
        std::string texture = object["Texture"];
        glm::vec3 pos = glm::vec3(object["Pos"]["x"], object["Pos"]["y"], object["Pos"]["z"]);
        std::pair<std::string, glm::vec3> objectPair = std::make_pair(texture, pos);
        objects.insert({path, objectPair});
    }

    return objects;
}