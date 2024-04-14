#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "../Importers/ObjectProperties.h"
#include "../Buffers/RenderingHandler.h"

class Crowd
{
private:
    std::vector<ObjectProperties*> crowd;
    std::vector<float> yPos;
    static Crowd* instance;
    std::vector<GLuint> vao;
    RenderingHandler* rendering_handler = RenderingHandler::GetInstance();

    double GetRandomNum(double min, double max);
    Crowd()
    {
    }
    
public:
    static Crowd* GetInstance();
    void InitCrowdBuffers(GLuint program_id);
    void DrawCrowd(GLuint program_id, glm::mat4 view, glm::mat4 projection);
    void UpdatePositions();
    
};
