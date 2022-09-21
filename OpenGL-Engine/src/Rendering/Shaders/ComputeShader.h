#pragma once

#include "Rendering/Texture2D.h"
#include "Rendering/Shaders/Shader.h"
#include <glad/glad.h>
#include <glm/vec3.hpp>

class ComputeShader : public Shader
{
public:
    ComputeShader(const char* computePath);
    ~ComputeShader();

    void Dispatch(const glm::uvec3& dimensions, GLenum barrier = GL_ALL_BARRIER_BITS);

    inline uint32_t GetProgramId() override { return m_ProgramId; }
    inline const uint32_t GetProgramId() const override { return m_ProgramId; }

    void Use() const override;

    void SetUniform(const std::string& name, int value) const override;
    void SetUniform(const std::string& name, uint32_t value) const override;
    void SetUniform(const std::string& name, float value) const override;

    void SetUniform(const std::string& name, const glm::vec2& value) const override;
    void SetUniform(const std::string& name, const glm::vec3& value) const override;
    void SetUniform(const std::string& name, const glm::vec4& value) const override;
    void SetUniform(const std::string& name, const glm::mat4& value) const override;

    virtual void TexUnit(const std::string& name, uint32_t slot) const override { }
private:
    uint32_t m_ProgramId;
};