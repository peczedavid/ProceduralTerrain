#pragma once

//// ----------- CUBE GENEREATION START -----------
//glGenVertexArrays(1, &m_VaoCube);
//glGenBuffers(1, &m_VboCube);
//glGenBuffers(1, &m_EboCube);
//
//glBindVertexArray(m_VaoCube);
//glBindBuffer(GL_ARRAY_BUFFER, m_VboCube);
//
//float s = 0.5f;
//float vertices[24 * 5] = {
//	// FAR FACE
//	 s, -s, -s, 0, 0,   -s, -s, -s, 1, 0,   -s,  s, -s, 1, 1,    s,  s, -s, 0, 1,
//	 // CLOSE FACE															
//	 -s, -s,  s, 0, 0,    s, -s,  s, 1, 0,    s,  s,  s, 1, 1,   -s,  s,  s, 0, 1,
//	 // RIGHT FACE															
//	  s, -s,  s, 0, 0,    s, -s, -s, 1, 0,    s,  s, -s, 1, 1,    s,  s,  s, 0, 1,
//	  // LEFT FACE															
//	  -s, -s, -s, 0, 0,   -s, -s,  s, 1, 0,   -s,  s,  s, 1, 1,   -s,  s, -s, 0, 1,
//	  // UP FACE																
//	  -s,  s,  s, 0, 0,    s,  s,  s, 1, 0,    s,  s, -s, 1, 1,   -s,  s, -s, 0, 1,
//	  // DOWN FACE															
//	  -s, -s, -s, 0, 0,    s, -s, -s, 1, 0,    s, -s,  s, 1, 1,   -s, -s,  s, 0, 1,
//};
//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
//
//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboCube);
//uint32_t indices[36] = {
//	0,   1,  2,    0,  2,  3,
//	4,   5,  6,    4,  6,  7,
//	8,   9, 10,    8, 10, 11,
//	12, 13, 14,   12, 14, 15,
//	16, 17, 18,   16, 18, 19,
//	20, 21, 22,   20, 22, 23
//};
//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
//
//glEnableVertexAttribArray(0);
//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
//glEnableVertexAttribArray(1);
//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//
//glBindVertexArray(0);
//glBindBuffer(GL_ARRAY_BUFFER, 0);
//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//// ----------- CUBE GENEREATION END -----------