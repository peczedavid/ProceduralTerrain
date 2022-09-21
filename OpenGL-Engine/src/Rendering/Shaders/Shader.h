#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <unordered_map>

class Shader
{
public:
    virtual void Use() const { glUseProgram(m_ProgramId); }

    virtual uint32_t GetProgramId() { return m_ProgramId; }
    virtual const uint32_t GetProgramId() const { return m_ProgramId; }

    virtual void Dispatch(const glm::uvec3& dimensions, GLenum barrier = GL_ALL_BARRIER_BITS) const {}

    virtual void TexUnit(const std::string& name, uint32_t slot) const;

    virtual void SetUniform(const std::string& name, int value) const;
    virtual void SetUniform(const std::string& name, uint32_t value) const;
    virtual void SetUniform(const std::string& name, float value) const;

    virtual void SetUniform(const std::string& name, const glm::vec2& value) const;
    virtual void SetUniform(const std::string& name, const glm::vec3& value) const;
    virtual void SetUniform(const std::string& name, const glm::vec4& value) const;
    virtual void SetUniform(const std::string& name, const glm::mat4& value) const;
protected:
    mutable std::unordered_map<std::string, GLint> m_UniformCache;
    uint32_t m_ProgramId;
private:
    GLint getUniformLocation(const std::string& name) const;
};