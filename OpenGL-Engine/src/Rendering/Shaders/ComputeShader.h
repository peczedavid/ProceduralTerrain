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
};