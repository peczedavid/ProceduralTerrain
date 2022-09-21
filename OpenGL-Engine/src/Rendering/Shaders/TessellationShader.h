#pragma once

#include "Rendering/Shaders/Shader.h"

class TessellationShader : public Shader
{
public:

    TessellationShader(const char* vertexPath,
        const char* tessControlPath, const char* tessEvalPath,
        const char* fragmentPath, const char* outputName = "outColor");
    ~TessellationShader();
};