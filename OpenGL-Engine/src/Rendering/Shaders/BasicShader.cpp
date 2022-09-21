#include "Rendering/Shaders/BasicShader.h"
#include <vector>

BasicShader::BasicShader(const char* vertexPath, const char* fragmentPath, const char* outputName)
	: m_VertexPath(vertexPath), m_FragmentPath(fragmentPath), m_OutputName(outputName)
{
	Compile();
	glUseProgram(m_ProgramId);
}

BasicShader::~BasicShader()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);
}

void BasicShader::Compile()
{
	if (m_ProgramId > 0)
		glDeleteProgram(m_ProgramId);

	m_VertexSrc = ReadSource(m_VertexPath.c_str());
	m_FragmentSrc = ReadSource(m_FragmentPath.c_str());

	const char* vertexSrc = m_VertexSrc.c_str();
	const char* fragmentSrc = m_FragmentSrc.c_str();

	m_ProgramId = glCreateProgram();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSrc, NULL);
	glCompileShader(vertexShader);
	CheckCompile(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
	glCompileShader(fragmentShader);
	CheckCompile(fragmentShader);

	glAttachShader(m_ProgramId, vertexShader);
	glAttachShader(m_ProgramId, fragmentShader);
	glLinkProgram(m_ProgramId);

	glBindFragDataLocation(m_ProgramId, 0, m_OutputName.c_str());

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	m_UniformCache.clear();
}
