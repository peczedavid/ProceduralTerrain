#pragma once

#include "Rendering/Shaders/Shader.h"

class BasicShader : public Shader
{
public:
    BasicShader(const char* vertexPath, const char* fragmentPath, const char* outputName = "outColor");
    ~BasicShader();
};