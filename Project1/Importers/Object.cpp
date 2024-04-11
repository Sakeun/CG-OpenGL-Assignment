#include "Object.h"
#include "../JsonReader/JsonReader.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>

#include "ObjectProperties.h"
#include "../objloader.h"
#include "../texture.h"

std::tuple<ObjectProperties*, int> Object::get_objects()
{
    std::vector<ImportProperties*> objects = JsonReader::ReadObjects();
    std::vector<ObjectProperties*> meshes = JsonReader::ReadMeshes();
    ObjectProperties* objectProperties = new ObjectProperties[objects.size() + meshes.size()];
    int i = 0;
    for(auto properties : objects)
    {
        bool res = loadOBJ(("Objects/" + properties->path + ".obj").c_str(), objectProperties[i].vertices, objectProperties[i].uvs, objectProperties[i].normals);
        if(!res) continue;
        objectProperties[i].texture = loadDDS(("Textures/" + properties->texture + ".dds").c_str());
        objectProperties[i].model = glm::mat4();
        if (properties->radius != 0) {
            objectProperties[i].model = glm::rotate(objectProperties[i].model, glm::radians(properties->radius), properties->rotation);
        }
        objectProperties[i].model = glm::scale(objectProperties[i].model, properties->scale);
        objectProperties[i].model = glm::translate(objectProperties[i].model, properties->position);
        
        delete properties;
        i++;
    }

    for(auto properties : meshes)
    {
        objectProperties[i] = *properties;
        i++;
    }

    return {objectProperties, i};
}

void Object::InitMaterialLights(Material &materials)
{
    materials.ambient_color = glm::vec3(0.2, 0.2, 0.1);
    materials.diffuse_color = glm::vec3(0.5, 0.5, 0.3);
    materials.specular_color = glm::vec3(1.0, 1.0, 1.0);
    materials.power = 30.0;
}