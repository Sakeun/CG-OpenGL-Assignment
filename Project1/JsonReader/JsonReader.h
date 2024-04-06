#pragma once
#include <map>
#include <glm/glm.hpp>
#include <string>

class JsonReader
{
public:
    static std::map<std::string, std::pair<std::string, glm::vec3>> ReadObjects();
};
