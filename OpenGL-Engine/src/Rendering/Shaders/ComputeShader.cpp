#include "Rendering/Shaders/ComputeShader.h"
#include <fstream>
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
	m_Texture = new Texture2D(1024, 1024, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGBA32F);

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

void ComputeShader::Use() const
{
	glUseProgram(m_ProgramId);
}

void ComputeShader::SetUniform(const std::string& name, int value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform1i(location, value);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void ComputeShader::SetUniform(const std::string& name, float value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform1f(location, value);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void ComputeShader::SetUniform(const std::string& name, uint32_t value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform1ui(location, value);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void ComputeShader::SetUniform(const std::string& name, const glm::vec2& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform2f(location, value.x, value.y);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void ComputeShader::SetUniform(const std::string& name, const glm::vec3& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform3f(location, value.x, value.y, value.z);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void ComputeShader::SetUniform(const std::string& name, const glm::vec4& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform4f(location, value.x, value.y, value.z, value.w);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void ComputeShader::SetUniform(const std::string& name, const glm::mat4& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}