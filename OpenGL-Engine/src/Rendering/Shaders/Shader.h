#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <unordered_map>
#include <vector>
#include "Core/Core.h"

class Shader
{
public:
    Shader(const std::vector<std::string>& shaderFiles, const std::string& outputName = "outColor");
    ~Shader();

    void Use() const { glUseProgram(m_ProgramId); }
    void Dispatch(const glm::uvec3& dimensions, GLenum barrier = GL_ALL_BARRIER_BITS) const;

    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, uint32_t value) const;
    void SetUniform(const std::string& name, float value) const;

    void SetUniform(const std::string& name, const glm::vec2& value) const;
    void SetUniform(const std::string& name, const glm::vec3& value) const;
    void SetUniform(const std::string& name, const glm::vec4& value) const;
    void SetUniform(const std::string& name, const glm::mat4& value) const;

    uint32_t GetProgramId() { return m_ProgramId; }
    const uint32_t GetProgramId() const { return m_ProgramId; }

    void ReCompile();
private:
    struct TextureUnit
    {
        std::string Name;
        uint32_t Slot;
        GLenum Type{};
    };

    // Per pipeline element
    struct ShaderInfo
    {
        std::string Path;
        std::string Source;
        GLenum Type{};
    };
private:
    void Compile(const std::vector<std::string>& shaderFiles);
    void ActivateTexUnits();
    void CheckCompiled(const GLuint shader) const;
    void CheckLinked();
    GLint getUniformLocation(const std::string& name) const;
    GLenum GetShaderType(const std::string& path) const;
    GLenum GetSamplerType(const std::string& typeStr) const;
    void TexUnit(const std::string& name, uint32_t slot) const;
    void ParseTexUnits(ShaderInfo& shaderInfo);
private:
    mutable std::unordered_map<std::string, GLint> m_UniformCache;
    std::vector<TextureUnit> m_TextureUnits;
    uint32_t m_ProgramId = 0;
    std::string m_OutputName;
    std::vector<ShaderInfo> m_ShaderInfos;
};

class ShaderLibrary
{
public:
    ShaderLibrary() = default;
    ShaderLibrary(const ShaderLibrary&) = delete;
    ShaderLibrary operator=(const ShaderLibrary&) = delete;
    ~ShaderLibrary() = default;
    
    void Add(const std::string name, const Ref<Shader> shader);
    const Ref<Shader> Get(const std::string& name);

    std::unordered_map<std::string, Ref<Shader>>::iterator begin() { return m_Shaders.begin(); }
    std::unordered_map<std::string, Ref<Shader>>::iterator end() { return m_Shaders.end(); }

    std::unordered_map<std::string, Ref<Shader>>::const_iterator begin() const { return m_Shaders.begin(); }
    std::unordered_map<std::string, Ref<Shader>>::const_iterator end()	const { return m_Shaders.end(); }
private:
    std::unordered_map<std::string, Ref<Shader>> m_Shaders;
};