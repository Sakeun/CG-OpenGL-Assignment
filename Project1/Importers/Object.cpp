#include "Object.h"
#include "../JsonReader/JsonReader.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "../objloader.h"
#include "../texture.h"
#include "../Animations/Animation/Animation.h"
#include "../Animations/Animation/RotateAnimation.h"
#include "../Animations/Animation/AmbientAnimation.h"

struct Colors {
    glm::vec3 Black = glm::vec3(0, 0, 0);
    glm::vec3 Red = glm::vec3(1.00, 0.12, 0.14);
};

std::tuple<ObjectProperties*, int> Object::get_objects()
{
    std::vector<ImportProperties*> objects = JsonReader::read_objects();
    std::vector<ObjectProperties*> meshes = JsonReader::read_meshes();

    // Map used to store objects once they've already been loaded, to prevent having to load an object again if it's used multiple times.
    std::map<std::string, ObjectElements> loadedObjs;

    ObjectProperties* objectProperties = new ObjectProperties[objects.size() + meshes.size()];
    int i = 0;
    for (auto properties : objects)
    {
        // Check if object has already been loaded
        if (loadedObjs.find(properties->path) == loadedObjs.end()) {
            ObjectElements obj;
            bool res = loadOBJ(("Objects/" + properties->path + ".obj").c_str(), obj.vertices, obj.uvs, obj.normals);
            if (!res) continue;
            loadedObjs[properties->path] = obj;
        }

        objectProperties[i].vertices = loadedObjs[properties->path].vertices;
        objectProperties[i].uvs = loadedObjs[properties->path].uvs;
        objectProperties[i].normals = loadedObjs[properties->path].normals;

        objectProperties[i].materials = JsonReader::read_materials(properties->shader);

        std::string path = properties->texture;
        if (path.find("color") != std::string::npos) {
            std::string subS = path.substr(6);
            objectProperties[i].materials.diffuse_color = Object::get_color(subS);

        }
        else {
            objectProperties[i].texture = loadDDS(("Textures/" + path + ".dds").c_str());
        }
        // Translate the object to the predefined position
        objectProperties[i].model = glm::mat4();
        if (properties->radius != 0) {
            objectProperties[i].model = glm::rotate(objectProperties[i].model, glm::radians(properties->radius), properties->rotation);
        }
        objectProperties[i].model = glm::scale(objectProperties[i].model, properties->scale);
        objectProperties[i].model = glm::translate(objectProperties[i].model, properties->position);

        // Add animation to the object
        if (properties->isAnimated)
        {
            objectProperties[i].animation = get_animation_type(properties->animation_type);
            if (properties->animation_type != "diffuse")
                objectProperties[i].animation->set_degrees(properties->xDegrees, properties->yDegrees, properties->zDegrees);

        }
        else
        {
            objectProperties[i].animation = nullptr;
        }

        delete properties;
        i++;
    }

    // Add all the custom made meshes to the object array for rendering
    for (auto properties : meshes)
    {
        objectProperties[i] = *properties;
        i++;
    }

    return { objectProperties, i };
}

glm::vec3 Object::get_color(std::string color) {
    if (color == "Red")
        return glm::vec3(0.76, 0.01, 0.04);
    if (color == "Grey")
        return glm::vec3(0.5, 0.5, 0.5);

    return glm::vec3(0.00, 0.00, 0.00);
}

void Object::init_material_lights(Material& materials)
{
    materials.ambient_color = glm::vec3(0.2, 0.07, 0.07);
    materials.diffuse_color = glm::vec3(0.5, 0.0, 0.0);
    materials.specular_color = glm::vec3(1.0, 1.0, 1.0);
    materials.power = 128.0;
}

Animation* Object::get_animation_type(std::string type)
{
    if (type == "rotate")
    {
        return new RotateAnimation();
    }
    if (type == "diffuse")
    {
        return new AmbientAnimation();
    }
    return nullptr;
}