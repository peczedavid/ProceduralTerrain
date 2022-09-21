#pragma once

#include "Rendering/Shaders/Shader.h"

class BasicShader : public Shader
{
public:
    BasicShader(const char* vertexPath, const char* fragmentPath, const char* outputName = "outColor");
    ~BasicShader();

    void Compile() override;
private:
    std::string m_VertexPath, m_VertexSrc;
    std::string m_FragmentPath, m_FragmentSrc;
    std::string m_OutputName;
};