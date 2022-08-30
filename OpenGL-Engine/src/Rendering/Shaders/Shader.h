#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Shader
{
public:
    virtual void Use() const = 0;

    virtual uint32_t GetProgramId() = 0;
    virtual const uint32_t GetProgramId() const = 0;

    virtual void TexUnit(const std::string& name, uint32_t slot) const = 0;

    virtual void SetUniform(const std::string& name, int value) const = 0;
    virtual void SetUniform(const std::string& name, uint32_t value) const = 0;
    virtual void SetUniform(const std::string& name, float value) const = 0;

    virtual void SetUniform(const std::string& name, const glm::vec2& value) const = 0;
    virtual void SetUniform(const std::string& name, const glm::vec3& value) const = 0;
    virtual void SetUniform(const std::string& name, const glm::vec4& value) const = 0;
    virtual void SetUniform(const std::string& name, const glm::mat4& value) const = 0;
};