#include "Object.h"
#include "../JsonReader/JsonReader.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>

#include "ObjectProperties.h"
#include "../objloader.h"
#include "../texture.h"
#include "../Animations/Animation.h"

struct Colors {
    glm::vec3 Black = glm::vec3(0, 0, 0);
    glm::vec3 Red = glm::vec3(1.00, 0.12, 0.14);
};

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
        objectProperties[i].texture = 0;
        if (properties->texture.find("color") != std::string::npos) {
            std::string subS = properties->texture.substr(6);
            objectProperties[i].color = get_color(subS);
        }
        else {
            objectProperties[i].texture = loadDDS(("Textures/" + properties->texture + ".dds").c_str());
        }
        objectProperties[i].model = glm::mat4();
        if (properties->radius != 0) {
            objectProperties[i].model = glm::rotate(objectProperties[i].model, glm::radians(properties->radius), properties->rotation);
        }
        objectProperties[i].model = glm::scale(objectProperties[i].model, properties->scale);
        objectProperties[i].model = glm::translate(objectProperties[i].model, properties->position);
        if(properties->isAnimated)
        {
            objectProperties[i].animation = new Animation(properties->xDegrees, properties->yDegrees, properties->zDegrees);
        } else
        {
            objectProperties[i].animation = nullptr;
        }
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

glm::vec3 Object::get_color(std::string color) {
    if (color == "Red")
        return glm::vec3(0.76, 0.01, 0.04);

    return glm::vec3(0.01, 0.01, 0.01);
}

void Object::InitMaterialLights(Material &materials)
{
    materials.ambient_color = glm::vec3(0.2, 0.2, 0.1);
    materials.diffuse_color = glm::vec3(1.0, 1.0, 1.0);
    materials.specular_color = glm::vec3(1.0, 1.0, 1.0);
    materials.power = 100.0;
}