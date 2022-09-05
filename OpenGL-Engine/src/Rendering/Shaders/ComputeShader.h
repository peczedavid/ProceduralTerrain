#pragma once

#include "Rendering/Texture2D.h"
#include <glad/glad.h>
#include <glm/vec3.hpp>

class ComputeShader
{
public:

    ComputeShader(const char* computePath);
    ~ComputeShader();

    void Dispatch(GLenum barrier = GL_ALL_BARRIER_BITS);

    inline uint32_t GetProgramId() { return m_ProgramId; }
    inline const uint32_t GetProgramId() const { return m_ProgramId; }

    inline Texture2D* GetTexture() { return m_Texture; }
    inline const Texture2D* GetTexture() const { return m_Texture; }

    void Use() const;

    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, uint32_t value) const;
    void SetUniform(const std::string& name, float value) const;

    void SetUniform(const std::string& name, const glm::vec2& value) const;
    void SetUniform(const std::string& name, const glm::vec3& value) const;
    void SetUniform(const std::string& name, const glm::vec4& value) const;
    void SetUniform(const std::string& name, const glm::mat4& value) const;
private:
    Texture2D* m_Texture;
    uint32_t m_ProgramId;
};