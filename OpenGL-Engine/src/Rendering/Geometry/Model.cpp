#include "pch.h"
#include "Rendering/Geometry/Model.h"

//std::vector<std::string>& split(std::string& s, const std::string& delim)
//{
//	size_t pos = 0;
//	std::vector<std::string> elements;
//	while ((pos = s.find(delim)) != std::string::npos) {
//		elements.push_back(s.substr(0, pos));
//		s.erase(0, pos + delim.length());
//	}
//	return elements;
//}

Model::Model(const std::string& path)
{
	std::ifstream in(path, std::ios::in);
	if (!in)
		ERROR("Cannot open model file: {0}", path);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texture;
	std::vector<int> faceIndex, textureIndex;

	std::vector<glm::vec3> meshVertices;
	std::vector<glm::vec2> texCoord;

	std::string line;
	while (std::getline(in, line))
	{
		//check v for vertices
		if (line.substr(0, 2) == "v ") {
			std::istringstream v(line.substr(2));
			glm::vec3 vert;
			double x, y, z;
			v >> x; v >> y; v >> z;
			vert = glm::vec3(x, y, z);
			vertices.push_back(vert);
		}
		//check for texture co-ordinate
		else if (line.substr(0, 2) == "vt") {

			std::istringstream v(line.substr(3));
			glm::vec2 tex;
			int U, V;
			v >> U; v >> V;
			tex = glm::vec2(U, V);
			texture.push_back(tex);

		}
		//check for faces
		else if (line.substr(0, 2) == "f ") {
			int a, b, c; //to store mesh index
			int A, B, C; //to store texture index
			//std::istringstream v;
		  //v.str(line.substr(2));
			const char* chh = line.c_str();
			sscanf(chh, "f %i/%i %i/%i %i/%i", &a, &A, &b, &B, &c, &C); //here it read the line start with f and store the corresponding values in the variables

			//v>>a;v>>b;v>>c;
			a--; b--; c--;
			A--; B--; C--;
			//std::cout<<a<<b<<c<<A<<B<<C;
			faceIndex.push_back(a); textureIndex.push_back(A);
			faceIndex.push_back(b); textureIndex.push_back(B);
			faceIndex.push_back(c); textureIndex.push_back(C);
		}

	}
	//the mesh data is finally calculated here
	for (unsigned int i = 0; i < faceIndex.size(); i++)
	{
		glm::vec3 meshData;
		glm::vec2 texData;
		meshData = glm::vec3(vertices[faceIndex[i]].x, vertices[faceIndex[i]].y, vertices[faceIndex[i]].z);
		texData = glm::vec2(texture[textureIndex[i]].x, texture[textureIndex[i]].y);
		meshVertices.push_back(meshData);
		texCoord.push_back(texData);
	}


	//std::ifstream infile(path);
	//std::string line;
	//std::vector<glm::vec3> vertices;
	//std::vector<glm::vec2> texcoords;
	//std::vector<glm::vec3> normals;
	//std::vector<uint32_t> indices;
	//while (std::getline(infile, line))
	//{
	//	if (line.rfind("v ", 0) == 0)
	//	{
	//		auto& elements = split(line, "");
	//		for (const auto& str : elements)
	//			std::cout << str << " ";
	//		std::cout << "\n";
	//		//TRACE("Vertex");
	//	}
	//	else if (line.rfind("vt ", 0) == 0) TRACE("TexCoord");
	//	else if (line.rfind("vn ", 0) == 0) TRACE("Normal");
	//	else if (line.rfind("f ", 0) == 0) TRACE("Face");
	//}

	//glGenVertexArrays(1, &m_Vao);
	//glGenBuffers(1, &m_Vbo);
	//glGenBuffers(1, &m_Ebo);

	//glBindVertexArray(m_Vao);
	//glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	//constexpr float s = 0.5f;
	//constexpr float vertices[24 * 8] = {
	//	// FAR FACE
	//	 s, -s, -s,  0,  0, -1, 0, 0,   -s, -s, -s,  0,  0, -1, 1, 0,   -s,  s, -s,  0,  0, -1, 1, 1,    s,  s, -s,  0,  0, -1, 0, 1,
	//	// CLOSE FACE															
	//	-s, -s,  s,  0,  0,  1, 0, 0,    s, -s,  s,  0,  0,  1, 1, 0,    s,  s,  s,  0,  0,  1, 1, 1,   -s,  s,  s,  0,  0,  1, 0, 1,
	//	// RIGHT FACE															
	//	 s, -s,  s,  1,  0,  0, 0, 0,    s, -s, -s,  1,  0,  0, 1, 0,    s,  s, -s,  1,  0,  0, 1, 1,    s,  s,  s,  1,  0,  0, 0, 1,
	//	// LEFT FACE															
	//	-s, -s, -s, -1,  0,  0, 0, 0,   -s, -s,  s, -1,  0,  0, 1, 0,   -s,  s,  s, -1,  0,  0, 1, 1,   -s,  s, -s, -1,  0,  0, 0, 1,
	//	// UP FACE																
	//	-s,  s,  s,  0,  1,  0, 0, 0,    s,  s,  s,  0,  1,  0, 1, 0,    s,  s, -s,  0,  1,  0, 1, 1,   -s,  s, -s,  0,  1,  0, 0, 1,
	//	// DOWN FACE															
	//	-s, -s, -s,  0, -1,  0, 0, 0,    s, -s, -s,  0, -1,  0, 1, 0,    s, -s,  s,  0, -1,  0, 1, 1,   -s, -s,  s,  0, -1,  0, 0, 1,
	//};
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	//constexpr uint32_t indices[36] = {
	//	 2,  1,  0,    3,  2,  0,
	//	 6,  5,  4,    7,  6,  4,
	//	10,  9,  8,   11, 10,  8,
	//	14, 13, 12,   15, 14, 12,
	//	18, 17, 16,   19, 18, 16,
	//	22, 21, 20,   23, 22, 20
	//};
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Model::~Model()
{
}

void Model::Draw(Ref<Shader> shader)
{
	//glBindVertexArray(m_Vao);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
