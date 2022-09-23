#include "Rendering/Shaders/Shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

std::string ReadFile(const char* fileName)
{
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

Shader::Shader(const std::vector<std::string>& shaderFiles, const std::string& outputName)
	: m_OutputName(outputName)
{
	Compile(shaderFiles);
	glUseProgram(m_ProgramId);
}

Shader::~Shader()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);
}

void Shader::Compile(const std::vector<std::string>& shaderFiles)
{
	m_UniformCache.clear();
	m_ProgramId = glCreateProgram();

	m_ShaderInfos.resize(shaderFiles.size());
	for (int i = 0; i < m_ShaderInfos.size(); i++)
	{
		auto& shaderInfo = m_ShaderInfos[i];
		shaderInfo.Path = shaderFiles[i];
		shaderInfo.Source = ReadFile(shaderInfo.Path.c_str());
		shaderInfo.Type = GetShaderType(shaderInfo.Path);
		ParseTexUnits(shaderInfo);

		const char* src = shaderInfo.Source.c_str();

		GLuint shader = glCreateShader(GetShaderType(shaderInfo.Path));
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		CheckCompiled(shader);
		glAttachShader(m_ProgramId, shader);
		glDeleteShader(shader);
	}

	glLinkProgram(m_ProgramId);

	glUseProgram(m_ProgramId);
	ActivateTexUnits();
}

void Shader::ActivateTexUnits()
{
	for (auto& shaderInfo : m_ShaderInfos)
	{
		for (auto& texUnit : m_TextureUnits)
		{
			TexUnit(texUnit.Name, texUnit.Slot);
		}
	}
}

void Shader::ReCompile()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);

	m_UniformCache.clear();
	m_ProgramId = glCreateProgram();

	m_TextureUnits.clear();
	for (int i = 0; i < m_ShaderInfos.size(); i++)
	{
		auto& shaderInfo = m_ShaderInfos[i];
		shaderInfo.Source = ReadFile(shaderInfo.Path.c_str());
		ParseTexUnits(shaderInfo);

		const char* src = shaderInfo.Source.c_str();

		GLuint shader = glCreateShader(GetShaderType(shaderInfo.Path));
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		CheckCompiled(shader);
		glAttachShader(m_ProgramId, shader);
		glDeleteShader(shader);
	}

	glLinkProgram(m_ProgramId);
	glUseProgram(m_ProgramId);
	ActivateTexUnits();
	glUseProgram(0);
}

void Shader::Dispatch(const glm::uvec3& dimensions, GLenum barrier) const
{
	glUseProgram(m_ProgramId);
	glDispatchCompute(dimensions.x, dimensions.y, dimensions.z);
	glMemoryBarrier(barrier);
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

void Shader::CheckCompiled(GLuint shader) const
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

GLenum Shader::GetShaderType(const std::string& path) const
{
	const size_t lastSlashIdx = path.find_last_of('/');
	const std::string fileName = path.substr(lastSlashIdx + 1, path.length() - lastSlashIdx + 1);
	const size_t dotLocation = fileName.find_last_of('.');
	const std::string extension = fileName.substr(dotLocation + 1, fileName.length() - dotLocation + 1);
	if (extension == "vert") return GL_VERTEX_SHADER;
	else if (extension == "frag") return GL_FRAGMENT_SHADER;
	else if (extension == "tesc") return GL_TESS_CONTROL_SHADER;
	else if (extension == "tese") return GL_TESS_EVALUATION_SHADER;
	else if (extension == "comp") return GL_COMPUTE_SHADER;
	else if (extension == "geom") return GL_GEOMETRY_SHADER;
	return 0;
}

GLenum Shader::GetSamplerType(const std::string& typeStr) const
{
	if (typeStr == "sampler2D") return GL_TEXTURE_2D;
	else if (typeStr == "samplerCube") return GL_TEXTURE_CUBE_MAP;
	return 0;
}

void Shader::ParseTexUnits(Shader::ShaderInfo& shaderInfo)
{
	std::string line;
	std::istringstream sourceStream(shaderInfo.Source);
	std::string token;
	while (std::getline(sourceStream, line))
	{
		size_t found = line.find("sampler");
		if (found != std::string::npos)
		{
			std::istringstream lineStream(line);
			int tokenIndex = 0;
			TextureUnit textureUnit{};
			while (std::getline(lineStream, token, ' '))
			{
				if (tokenIndex == 1) // type
				{
					textureUnit.Type = GetSamplerType(token);
				}
				else if (tokenIndex == 2) // name
				{
					token = token.substr(0, token.find(';'));
					textureUnit.Name = token;
				}

				tokenIndex++;
			}
			textureUnit.Slot = m_TextureUnits.size();
			m_TextureUnits.push_back(textureUnit);
		}
	}
}
