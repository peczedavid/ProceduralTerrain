#include "pch.h"
#include "Rendering/Geometry/Model.h"

Model::Model(const std::string& path)
{
	std::ifstream infile(path);
	std::string line;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec3> normals;
	while (std::getline(infile, line))
	{
		if (line.rfind("v ", 0) == 0)
		{
			std::istringstream v(line.substr(2));
			glm::vec3 vertex;
			float x, y, z;
			v >> x; v >> y; v >> z;
			vertex = glm::vec3(x, y, z);
			m_Vertices.push_back(vertex);
			//TRACE("Vertex {0} {1} {2}", x, y, z);
		}
		else if (line.rfind("vt ", 0) == 0)
		{
			std::istringstream v(line.substr(2));
			glm::vec2 uv;
			float x, y, z;
			v >> x; v >> y; v >> z;
			uv = glm::vec2(x, y);
			texcoords.push_back(uv);
			//TRACE("TexCoord {0} {1}", x, y);
		}
		else if (line.rfind("vn ", 0) == 0)
		{
			std::istringstream v(line.substr(2));
			glm::vec3 normal;
			float x, y, z;
			v >> x; v >> y; v >> z;
			normal = glm::vec3(x, y, z);
			normals.push_back(normal);
			//TRACE("Normal {0} {1} {2}", x, y, z);
		}
		else if (line.rfind("f ", 0) == 0)
		{
			if (m_FirstFace)
			{
				m_TexCoords.resize(m_Vertices.size());
				m_Normals.resize(m_Vertices.size());
				m_FirstFace = false;
			}

			const char* chh = line.c_str();
			int v0, v1, v2;
			int t0, t1, t2;
			int n0, n1, n2;
			const int ret = sscanf(chh, "f %i/%i/%i %i/%i/%i %i/%i/%i",
				&v0, &t0, &n0,
				&v1, &t1, &n1,
				&v2, &t2, &n2);

			int vtxId = v0 - 1;
			m_Indices.push_back(vtxId);
			glm::vec2 tex = texcoords[t0 - 1];
			tex.y = 1 - tex.y;
			m_TexCoords[vtxId] = tex;
			glm::vec3 normal = normals[n0 - 1];
			m_Normals[vtxId] = normal;

			vtxId = v1 - 1;
			m_Indices.push_back(vtxId);
			tex = texcoords[t1 - 1];
			tex.y = 1 - tex.y;
			m_TexCoords[vtxId] = tex;
			normal = normals[n1 - 1];
			m_Normals[vtxId] = normal;

			vtxId = v2 - 1;
			m_Indices.push_back(vtxId);
			tex = texcoords[t2 - 1];
			tex.y = 1 - tex.y;
			m_TexCoords[vtxId] = tex;
			normal = normals[n2 - 1];
			m_Normals[vtxId] = normal;

			//TRACE("Face {0} {1} {2} {3} {4} {5} {6} {7} {8}",
			//	v0, t0, n0,
			//	v1, t1, n1,
			//	v2, t2, n2);
		}
	}

	m_VertexBuffer.resize(m_Vertices.size() * 8);
	for (size_t i = 0; i < m_Vertices.size(); i++)
	{
		const size_t vboIdx = 8 * i;
		m_VertexBuffer[vboIdx + 0] = m_Vertices[i].x;
		m_VertexBuffer[vboIdx + 1] = m_Vertices[i].y;
		m_VertexBuffer[vboIdx + 2] = m_Vertices[i].z;

		m_VertexBuffer[vboIdx + 3] = m_Normals[i].x;
		m_VertexBuffer[vboIdx + 4] = m_Normals[i].y;
		m_VertexBuffer[vboIdx + 5] = m_Normals[i].z;
		
		m_VertexBuffer[vboIdx + 6] = m_TexCoords[i].x;
		m_VertexBuffer[vboIdx + 7] = m_TexCoords[i].y;
	}

	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ebo);

	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_VertexBuffer.size(), &m_VertexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Model::~Model()
{
}

void Model::Draw()
{
	glBindVertexArray(m_Vao);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
}
