#pragma once
#include <chrono>
#include <mutex>

#include "Animation.h"

class AmbientAnimation : public Animation
{
private:
    static int color_index;
    static std::mutex color_index_mutex;
    static std::chrono::steady_clock::time_point last_color_change;

    glm::vec3 get_next_color();
    
public:
    AmbientAnimation(bool isLoop = true) : Animation(isLoop)
    {
        last_color_change = std::chrono::steady_clock::now();
    }
    virtual void update_object(ObjectProperties& object) override;
    virtual void update_model(glm::mat4& model) override {}
};