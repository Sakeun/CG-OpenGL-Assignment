#pragma once
#include <glm/gtc/matrix_transform.hpp>

struct ObjectProperties;

class Animation
{
public:
    Animation(bool isLoop = true) : is_loop(isLoop), completed(false) {}

    // Execute animations, either on the object or on the model depending on what's necessary for the specific animation
    virtual void update_object(ObjectProperties& object) = 0;
    virtual void update_model(glm::mat4& model) = 0;
    virtual bool is_completed() const { return completed; }
    virtual void set_degrees(float x, float y, float z) = 0;

protected:
    bool is_loop;
    bool completed;
    Animation() = default;
};
