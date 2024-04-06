#include "Object.h"
#include "../JsonReader/JsonReader.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>

#include "ObjectProperties.h"
#include "../objloader.h"
#include "../texture.h"

std::tuple<ObjectProperties*, int> Object::get_objects()
{
    std::vector<JsonProperties*> objects = JsonReader::ReadObjects();
    ObjectProperties* objectProperties = new ObjectProperties[objects.size()];
    int i = 0;
    for(auto properties : objects)
    {
        bool res = loadOBJ(("Objects/" + properties->path + ".obj").c_str(), objectProperties[i].vertices, objectProperties[i].uvs, objectProperties[i].normals);
        if(!res) continue;
        objectProperties[i].texture = loadDDS(("Textures/" + properties->texture + ".dds").c_str());
        objectProperties[i].model = glm::mat4();
        objectProperties[i].model = glm::scale(objectProperties[i].model, properties->scale);
        objectProperties[i].model = glm::translate(objectProperties[i].model, properties->position);
        delete properties;
        i++;
    }

    return {objectProperties, i};
}
