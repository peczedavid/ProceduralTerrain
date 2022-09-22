#include "Rendering/Shaders/TessellationShader.h"
#include <vector>

TessellationShader::TessellationShader(const char* vertexPath,
	const char* tessControlPath, const char* tessEvalPath,
	const char* fragmentPath, const char* outputName)
	: m_VertexPath(vertexPath), m_TessControlPath(tessControlPath),
	  m_TessEvalPath(tessEvalPath), m_FragmentPath(fragmentPath),
	  m_OutputName(outputName)
{
	Compile();
	glUseProgram(m_ProgramId);
}

TessellationShader::~TessellationShader()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);
}

void TessellationShader::Compile()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);

	m_VertexSrc = ReadSource(m_VertexPath.c_str());
	m_TessControlSrc = ReadSource(m_TessControlPath.c_str());
	m_TessEvalSrc = ReadSource(m_TessEvalPath.c_str());
	m_FragmentSrc = ReadSource(m_FragmentPath.c_str());

	const char* vertexSrc = m_VertexSrc.c_str();
	const char* tessControlSrc = m_TessControlSrc.c_str();
	const char* tessEvalSrc = m_TessEvalSrc.c_str();
	const char* fragmentSrc = m_FragmentSrc.c_str();
	
	m_ProgramId = glCreateProgram();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSrc, NULL);
	glCompileShader(vertexShader);
	CheckCompiled(vertexShader);

	GLuint tessControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tessControlShader, 1, &tessControlSrc, NULL);
	glCompileShader(tessControlShader);
	CheckCompiled(tessControlShader);

	GLuint tessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tessEvalShader, 1, &tessEvalSrc, NULL);
	glCompileShader(tessEvalShader);
	CheckCompiled(tessEvalShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
	glCompileShader(fragmentShader);
	CheckCompiled(fragmentShader);

	glAttachShader(m_ProgramId, vertexShader);
	glAttachShader(m_ProgramId, tessControlShader);
	glAttachShader(m_ProgramId, tessEvalShader);
	glAttachShader(m_ProgramId, fragmentShader);
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

	glBindFragDataLocation(m_ProgramId, 0, m_OutputName.c_str());

	glDeleteShader(vertexShader);
	glDeleteShader(tessControlShader);
	glDeleteShader(tessEvalShader);
	glDeleteShader(fragmentShader);

	m_UniformCache.clear();
}
