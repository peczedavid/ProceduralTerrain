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
private:
    Texture2D* m_Texture;
    uint32_t m_ProgramId;
};