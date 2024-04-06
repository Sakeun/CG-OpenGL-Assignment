#pragma once

#include "../Importers/ObjectProperties.h"

class JsonReader
{
public:
    static std::vector<JsonProperties*> ReadObjects();
};
