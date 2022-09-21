#include "Rendering/Shaders/ComputeShader.h"
#include <vector>

ComputeShader::ComputeShader(const char* computePath)
	: m_ComputePath(computePath)
{
	Compile();
	glUseProgram(m_ProgramId);
}

ComputeShader::~ComputeShader()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);
}

void ComputeShader::Compile()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);

	m_ComputeSrc = ReadSource(m_ComputePath.c_str());

	const char* computeSrc = m_ComputeSrc.c_str();

	m_ProgramId = glCreateProgram();

	uint32_t computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &computeSrc, NULL);
	glCompileShader(computeShader);

	GLint isCompiled = 0;
	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(computeShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(computeShader, maxLength, &maxLength, &errorLog[0]);

		for (uint32_t i = 0; i < maxLength; i++)
			printf("%c", errorLog[i]);

		glDeleteShader(computeShader);
		return;
	}

	glAttachShader(m_ProgramId, computeShader);
	glLinkProgram(m_ProgramId);

	glDeleteShader(computeShader);
}

void ComputeShader::Dispatch(const glm::uvec3& dimensions, GLenum barrier) const
{
	glUseProgram(m_ProgramId);
	glDispatchCompute(dimensions.x, dimensions.y, dimensions.z);
	glMemoryBarrier(barrier);
}