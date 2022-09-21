#include "Rendering/Shaders/Shader.h"
#include <fstream>
#include <iostream>

std::string ReadSource(const char* fileName) {
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
	std::cout << "Error reading: " + std::string(fileName) << std::endl;
	throw(errno);
}

void Shader::TexUnit(const std::string& name, uint32_t slot) const
{
	uint32_t textureUnit = getUniformLocation(name);
	glUniform1i(textureUnit, slot);
}


void Shader::SetUniform(const std::string& name, int value) const
{
	GLint location = getUniformLocation(name);
	if (location >= 0)
		glUniform1i(location, value);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, float value) const
{
	GLint location = getUniformLocation(name);
	if (location >= 0)
		glUniform1f(location, value);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, uint32_t value) const
{
	GLint location = getUniformLocation(name);
	if (location >= 0)
		glUniform1ui(location, value);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::vec2& value) const
{
	GLint location = getUniformLocation(name);
	if (location >= 0)
		glUniform2f(location, value.x, value.y);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::vec3& value) const
{
	GLint location = getUniformLocation(name);
	if (location >= 0)
		glUniform3f(location, value.x, value.y, value.z);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::vec4& value) const
{
	GLint location = getUniformLocation(name);
	if (location >= 0)
		glUniform4f(location, value.x, value.y, value.z, value.w);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::SetUniform(const std::string& name, const glm::mat4& value) const
{
	GLint location = getUniformLocation(name);
	if (location >= 0)
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	else
		printf("Uniform %s not found in shader!\n", name.c_str());
}

void Shader::CheckCompile(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		for (uint32_t i = 0; i < maxLength; i++)
			printf("%c", errorLog[i]);

		glDeleteShader(shader);
	}
}

GLint Shader::getUniformLocation(const std::string& name) const
{
	if (m_UniformCache.find(name) != m_UniformCache.end())
		return m_UniformCache[name];

	GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
	m_UniformCache[name] = location;
	return location;
}
