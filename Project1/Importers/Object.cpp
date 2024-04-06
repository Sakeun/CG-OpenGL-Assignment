#include "Object.h"
#include "../JsonReader/JsonReader.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>

#include "ObjectProperties.h"
#include "../objloader.h"
#include "../texture.h"

ObjectProperties* Object::get_objects()
{
    std::map<std::string, std::pair<std::string, glm::vec3>> objects = JsonReader::ReadObjects();
    ObjectProperties* objectProperties = new ObjectProperties[objects.size()];
    int i = 0;
    for(auto pair : objects)
    {
        bool res = loadOBJ(("Objects/" + pair.first + ".obj").c_str(), objectProperties[i].vertices, objectProperties[i].uvs, objectProperties[i].normals);
        if(!res) continue;
        objectProperties[i].texture = loadDDS(("Textures/" + pair.second.first + ".dds").c_str());
        objectProperties[i].model = glm::mat4();
        //objectProperties[i].model = glm::translate(objectProperties[i].model, pair.second.second);
        i++;
    }

    return objectProperties;
}
