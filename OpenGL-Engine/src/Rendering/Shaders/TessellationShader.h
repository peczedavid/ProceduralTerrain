#pragma once

#include "Rendering/Shaders/Shader.h"

class TessellationShader : public Shader
{
public:
    TessellationShader(const char* vertexPath,
        const char* tessControlPath, const char* tessEvalPath,
        const char* fragmentPath, const char* outputName = "outColor");
    ~TessellationShader();

    void Compile() override;
private:
    std::string m_VertexPath, m_VertexSrc;
    std::string m_TessControlPath, m_TessControlSrc;
    std::string m_TessEvalPath, m_TessEvalSrc;
    std::string m_FragmentPath, m_FragmentSrc;
    std::string m_OutputName;
};