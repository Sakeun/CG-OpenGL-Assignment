#pragma once
#include "../Importers/ObjectProperties.h"
#include "../objloader.h"
#include "../texture.h"
#include "../JsonReader/JsonReader.h"

class Instructions
{
private:
    GLuint vao;
    ObjectProperties* instructions;
    static Instructions* instance;
    void InitInstructionsBuffers(GLuint program_id);
    
    Instructions()
    {
        isGrabbed = true;
    }
    
public:
    bool isGrabbed;
    static Instructions* GetInstance();
    void GrabInstructions();
    void UpdateInstructionsPosition(glm::vec3 position, GLuint program_id, glm::mat4 view, glm::mat4 projection);
};
