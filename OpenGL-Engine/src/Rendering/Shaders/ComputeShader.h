#pragma once

#include "Rendering/Texture2D.h"
#include "Rendering/Shaders/ShaderBase.h"
#include <glad/glad.h>
#include <glm/vec3.hpp>

class ComputeShader : public ShaderBase
{
public:
    ComputeShader(const char* computePath);
    ~ComputeShader();

    void Compile() override;
    void Dispatch(const glm::uvec3& dimensions, GLenum barrier = GL_ALL_BARRIER_BITS) const override;
private:
    std::string m_ComputePath, m_ComputeSrc;
};