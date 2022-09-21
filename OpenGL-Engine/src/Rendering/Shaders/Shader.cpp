#include "Rendering/Shaders/Shader.h"

void Shader::TexUnit(const std::string& name, uint32_t slot) const
{
	uint32_t textureUnit = glGetUniformLocation(m_ProgramId, name.c_str());
	glUniform1i(textureUnit, slot);
}


void Shader::SetUniform(const std::string& name, int value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform1i(location, value);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, float value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform1f(location, value);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, uint32_t value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform1ui(location, value);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::vec2& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform2f(location, value.x, value.y);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::vec3& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform3f(location, value.x, value.y, value.z);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::vec4& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniform4f(location, value.x, value.y, value.z, value.w);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::mat4& value) const
{
	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	if (location >= 0)
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}