#include "Rendering/Shaders/TessellationShader.h"
#include<fstream>
#include <vector>

std::string ReadFile(const char* fileName) {
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

TessellationShader::TessellationShader(const char* vertexPath,
	const char* tessControlPath, const char* tessEvalPath,
	const char* fragmentPath, const char* outputName)
{
	std::string vertexStr = ReadFile(vertexPath);
	std::string tessControlStr = ReadFile(tessControlPath);
	std::string tessEvalStr = ReadFile(tessEvalPath);
	std::string fragmentStr = ReadFile(fragmentPath);

	const char* vertexSrc = vertexStr.c_str();
	const char* tessControlSrc = tessControlStr.c_str();
	const char* tessEvalSrc = tessEvalStr.c_str();
	const char* fragmentSrc = fragmentStr.c_str();

	uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSrc, NULL);
	glCompileShader(vertexShader);

	uint32_t tessControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tessControlShader, 1, &tessControlSrc, NULL);
	glCompileShader(tessControlShader);
	GLint tessControlCompiled = 0;
	glGetShaderiv(tessControlShader, GL_COMPILE_STATUS, &tessControlCompiled);
	if (tessControlCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(tessControlShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(tessControlShader, maxLength, &maxLength, &errorLog[0]);

		for (uint32_t i = 0; i < maxLength; i++)
			printf("%c", errorLog[i]);

		glDeleteShader(tessControlShader);
	}

	uint32_t tessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tessEvalShader, 1, &tessEvalSrc, NULL);
	glCompileShader(tessEvalShader);
	GLint tessEvalShaderCompiled = 0;
	glGetShaderiv(tessEvalShader, GL_COMPILE_STATUS, &tessEvalShaderCompiled);
	if (tessEvalShaderCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(tessEvalShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(tessEvalShader, maxLength, &maxLength, &errorLog[0]);

		for (uint32_t i = 0; i < maxLength; i++)
			printf("%c", errorLog[i]);

		glDeleteShader(tessEvalShader);
	}

	uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
	glCompileShader(fragmentShader);

	m_ProgramId = glCreateProgram();
	glAttachShader(m_ProgramId, vertexShader);
	glAttachShader(m_ProgramId, fragmentShader);
	glAttachShader(m_ProgramId, tessControlShader);
	glAttachShader(m_ProgramId, tessEvalShader);
	glLinkProgram(m_ProgramId);
	GLint isLinked = 0;
	glGetProgramiv(m_ProgramId, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_ProgramId, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_ProgramId, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(m_ProgramId);
		// Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(tessControlShader);
		glDeleteShader(tessEvalShader);
		glDeleteShader(fragmentShader);

		// Use the infoLog as you see fit.
			for (uint32_t i = 0; i < maxLength; i++)
				printf("%c", infoLog[i]);
		// In this simple program, we'll just leave
		return;
	}
	glBindFragDataLocation(m_ProgramId, 0, outputName);
	glUseProgram(m_ProgramId);

	glDeleteShader(vertexShader);
	glDeleteShader(tessControlShader);
	glDeleteShader(tessEvalShader);
	glDeleteShader(fragmentShader);
}

TessellationShader::~TessellationShader()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);
}