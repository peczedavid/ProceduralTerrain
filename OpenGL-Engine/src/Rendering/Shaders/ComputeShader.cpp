#include "Rendering/Shaders/ComputeShader.h"
#include <vector>

ComputeShader::ComputeShader(const char* computePath)
{
	std::string computeStr = ReadSource(computePath);

	const char* computeSrc = computeStr.c_str();

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

	m_ProgramId = glCreateProgram();
	glAttachShader(m_ProgramId, computeShader);
	glLinkProgram(m_ProgramId);
	glUseProgram(m_ProgramId);

	glDeleteShader(computeShader);
}

ComputeShader::~ComputeShader()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);
}

void ComputeShader::Dispatch(const glm::uvec3& dimensions, GLenum barrier) const
{
	glUseProgram(m_ProgramId);
	glDispatchCompute(dimensions.x, dimensions.y, dimensions.z);
	glMemoryBarrier(barrier);
}