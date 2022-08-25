#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class TessellationShader
{
public:

    TessellationShader(const char* vertexPath,
        const char* tessControlPath, const char* tessEvalPath,
        const char* fragmentPath, const char* outputName = "outColor");
    ~TessellationShader();

    void Use() const;

    inline uint32_t GetProgramId() { return m_ProgramId; }
    inline const uint32_t GetProgramId() const { return m_ProgramId; }

    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, float value) const;
    void SetUniform(const std::string& name, uint32_t value) const;

    void SetUniform(const std::string& name, const glm::vec3& value) const;
    void SetUniform(const std::string& name, const glm::vec4& value) const;
    void SetUniform(const std::string& name, const glm::mat4& value) const;
private:
    uint32_t m_ProgramId;

};