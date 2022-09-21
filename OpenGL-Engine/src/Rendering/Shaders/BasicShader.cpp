#include "Rendering/Shaders/BasicShader.h"
#include <vector>

BasicShader::BasicShader(const char* vertexPath, const char* fragmentPath, const char* outputName)
{
	std::string vertexStr = ReadSource(vertexPath);
	std::string fragmentStr = ReadSource(fragmentPath);

	const char* vertexSrc = vertexStr.c_str();
	const char* fragmentSrc = fragmentStr.c_str();

	uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSrc, NULL);
	glCompileShader(vertexShader);
	GLint vertexShaderCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompiled);
	if (vertexShaderCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);

		for (uint32_t i = 0; i < maxLength; i++)
			printf("%c", errorLog[i]);

		glDeleteShader(vertexShader);
	}
	uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
	glCompileShader(fragmentShader);
	GLint fragmentShaderCompiled = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompiled);
	if (fragmentShaderCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]);

		for (uint32_t i = 0; i < maxLength; i++)
			printf("%c", errorLog[i]);

		glDeleteShader(fragmentShader);
	}

	m_ProgramId = glCreateProgram();
	glAttachShader(m_ProgramId, vertexShader);
	glAttachShader(m_ProgramId, fragmentShader);
	glLinkProgram(m_ProgramId);
	glBindFragDataLocation(m_ProgramId, 0, outputName);
	glUseProgram(m_ProgramId);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

BasicShader::~BasicShader()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);
}