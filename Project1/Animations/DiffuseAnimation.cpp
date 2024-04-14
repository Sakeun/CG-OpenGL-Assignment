#include "DiffuseAnimation.h"

#include "../Structs.h"

int DiffuseAnimation::color_index = 0;
std::mutex DiffuseAnimation::color_index_mutex;
std::chrono::steady_clock::time_point DiffuseAnimation::last_color_change;

glm::vec3 DiffuseAnimation::get_next_color()
{
    switch(color_index)
    {
    case 0:
        return {0.95, 0.62, 0.88};
    case 1:
        return {1.00, 0.57, 0.57};
    case 2:
        return {0.59, 0.98, 0.62};
    case 3:
        return {0.57, 0.59, 1.00};
    case 4:
        return {0.84, 0.57, 1.00};
    default:
        return {1.0f, 1.0f, 1.0f};
    }
}

void DiffuseAnimation::update_object(ObjectProperties& object)
{
    std::lock_guard<std::mutex> lock(color_index_mutex);
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_color_change);

    if(elapsed.count() > 3)
    {
        if(completed) return;

        last_color_change = now;

        color_index++;
        if(color_index > 4)
            color_index = 0;
    }
    object.materials.ambient_color = get_next_color();
}