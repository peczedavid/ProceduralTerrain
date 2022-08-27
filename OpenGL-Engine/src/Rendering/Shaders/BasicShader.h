#pragma once

#include "Rendering/Shaders/Shader.h"

class BasicShader : public Shader
{
public:

    BasicShader(const char* vertexPath, const char* fragmentPath, const char* outputName = "outColor");
    ~BasicShader();

    void Use() const override;

    inline uint32_t GetProgramId() override { return m_ProgramId; }
    inline const uint32_t GetProgramId() const override { return m_ProgramId; }

    void TexUnit(const std::string& name, uint32_t slot) const override;

    void SetUniform(const std::string& name, int value) const override;
    void SetUniform(const std::string& name, float value) const override;
    void SetUniform(const std::string& name, uint32_t value) const override;

    void SetUniform(const std::string& name, const glm::vec3& value) const override;
    void SetUniform(const std::string& name, const glm::vec4& value) const override;
    void SetUniform(const std::string& name, const glm::mat4& value) const override;
private:
    uint32_t m_ProgramId;
};