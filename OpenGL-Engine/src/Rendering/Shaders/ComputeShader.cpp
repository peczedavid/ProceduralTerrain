#include "Rendering/Shaders/ComputeShader.h"
#include<fstream>
#include <vector>

std::string ReadComputeSource(const char* fileName) {
	std::ifstream in(fileName, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

ComputeShader::ComputeShader(const char* computePath)
{
	m_Texture = new Texture2D(512, 512, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGBA32F);

	std::string computeStr = ReadComputeSource(computePath);

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

void ComputeShader::Dispatch(GLenum barrier)
{
	glUseProgram(m_ProgramId);
	glDispatchCompute(ceil(m_Texture->GetWidth() / 8), ceil(m_Texture->GetHeight() / 4), 1);
	glMemoryBarrier(barrier);
}
