#include "Rendering/Shader.h"

Shader::Shader(const char* vertexSrc, const char* fragmentSrc)
{
	uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSrc, NULL);
	glCompileShader(vertexShader);

	uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
	glCompileShader(fragmentShader);

	m_ProgramId = glCreateProgram();
	glAttachShader(m_ProgramId, vertexShader);
	glAttachShader(m_ProgramId, fragmentShader);
	glLinkProgram(m_ProgramId);
	glUseProgram(m_ProgramId);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Use() const
{
	glUseProgram(m_ProgramId);
}

void Shader::SetUniform(const std::string& name, int value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform1i(location, value);
	else
		printf("Uniform %s not found in shader!", name.c_str());
}

void Shader::SetUniform(const std::string& name, float value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform1f(location, value);
	else
		printf("Uniform %s not found in shader!", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::vec3& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform3f(location, value.x, value.y, value.z);
	else
		printf("Uniform %s not found in shader!", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::vec4& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform4f(location, value.x, value.y, value.z, value.w);
	else
		printf("Uniform %s not found in shader!", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::mat4& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	else
		printf("Uniform %s not found in shader!", name.c_str());
}
