#include "geometrySetup.h"

void GenerateBasicSquareVAO(unsigned int& VAO_Square, unsigned int& VBO_Square, unsigned int& EBO_Square)
{
	//Object Data Setup:
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	GLCALL(glGenVertexArrays(1, &VAO_Square));
	GLCALL(glBindVertexArray(VAO_Square));

	GLCALL(glGenBuffers(1, &VBO_Square));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_Square));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCALL(glGenBuffers(1, &EBO_Square));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Square));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));
	GLCALL(glEnableVertexAttribArray(0));

	GLCALL(glBindVertexArray(0)); //Unbind the VAO
}

void GenerateTexTriVAO(unsigned int& VAO_TexTri, unsigned int& VBO_TexTri, unsigned int& EBO_TexTri)
{	
	float vertices[] = {
		// positions        // texture coords
		 0.5f,  0.5f, 0.0f, 2.0f, 2.0f,   // top right
		 0.5f, -0.5f, 0.0f, 2.0f, -1.0f,   // bottom right
		-0.5f, -0.5f, 0.0f, -1.0f, -1.0f,   // bottom left
		-0.5f,  0.5f, 0.0f, -1.0f, 2.0f    // top left 
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	GLCALL(glGenVertexArrays(1, &VAO_TexTri));
	GLCALL(glBindVertexArray(VAO_TexTri));

	GLCALL(glGenBuffers(1, &VBO_TexTri));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_TexTri));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCALL(glGenBuffers(1, &EBO_TexTri));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TexTri));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0));
	GLCALL(glEnableVertexAttribArray(0));

	GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3*sizeof(float))));
	GLCALL(glEnableVertexAttribArray(1));

	GLCALL(glBindVertexArray(0)); //Unbind the VAO

}

void GenerateCubeNoEBO(unsigned int& VAO_Cube, unsigned int& VBO_Cube)
{
	float vertices[] = {
		    // positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	GLCALL(glGenVertexArrays(1, &VAO_Cube));
	GLCALL(glBindVertexArray(VAO_Cube));

	GLCALL(glGenBuffers(1, &VBO_Cube));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0));
	GLCALL(glEnableVertexAttribArray(0));

	GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float))));
	GLCALL(glEnableVertexAttribArray(1));

	GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float))));
	GLCALL(glEnableVertexAttribArray(2));

	GLCALL(glBindVertexArray(0)); //Unbind the VAO
}

void GenerateCubeNoEBONoNormals(unsigned int& VAO_Cube, unsigned int& VBO_Cube)
{
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	GLCALL(glGenVertexArrays(1, &VAO_Cube));
	GLCALL(glBindVertexArray(VAO_Cube));

	GLCALL(glGenBuffers(1, &VBO_Cube));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0));
	GLCALL(glEnableVertexAttribArray(0));

	//GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float))));
	//GLCALL(glEnableVertexAttribArray(1));

	GLCALL(glBindVertexArray(0)); //Unbind the VAO
}

void GenerateHeightmapPlane(unsigned int& heightMapVAO,
	unsigned int& heightMapVBO,
	unsigned int& heightMapEBO,
	std::vector<Vert>& hmVerts,
	std::vector<unsigned int>& hmIndices,
	std::vector<Face>& hmFaces,
	int hmWidth,
	int hmHeight,
	int hmNrChannels,
	unsigned char* hmTexData)
{

	float yScale = 64.0f / 256.0f;
	float yShift = 16.0f;

	float x_texPoint = 1.0f / (hmWidth / 6);
	float y_texPoint = 1.0f / (hmHeight / 6);

	for (unsigned int z = 0; z < hmHeight; z++)
	{
		for (unsigned int x = 0; x < hmWidth; x++)
		{
			Vert vert;
			unsigned char* texel = hmTexData + (x + hmWidth * z) * hmNrChannels;
			unsigned char y = texel[0];

			glm::vec3 vec;
			vec.x = x;
			vec.y = (int)y * yScale - yShift;
			vec.z = z;

			vert.pos = vec;

			glm::vec2 texCoord;
			texCoord.x = x * x_texPoint;
			texCoord.y = z * y_texPoint;

			vert.texCord = texCoord;

			hmVerts.push_back(vert);
		}
	}

	//Fill Height map indices data and hm face data:
	for (unsigned int i = 0; i < (hmHeight - 1); i++)
	{
		for (unsigned int ii = 0; ii < (hmWidth - 1) * 6; ii = ii + 6)
		{
			unsigned int val = 0;
			Face face1;
			Face face2;

			val = i * hmWidth + ii / 6;
			hmIndices.push_back(val);
			face1.in_1 = val;

			val = (i + 1) * hmWidth + ii / 6;
			hmIndices.push_back(val);
			face1.in_2 = val;

			val = ((i + 1) * hmWidth + ii / 6) + 1;
			hmIndices.push_back(val);
			face1.in_3 = val;

			glm::vec3 AB = hmVerts[face1.in_2].pos - hmVerts[face1.in_1].pos;
			glm::vec3 AC = hmVerts[face1.in_3].pos - hmVerts[face1.in_1].pos;

			face1.faceNormal = glm::normalize(glm::cross(AB, AC));


			val = i * hmWidth + ii / 6;
			hmIndices.push_back(val);
			face2.in_1 = val;

			val = ((i + 1) * hmWidth + ii / 6) + 1;
			hmIndices.push_back(val);
			face2.in_2 = val;

			val = (i * hmWidth + ii / 6) + 1;
			hmIndices.push_back(val);
			face2.in_3 = val;

			AB = hmVerts[face2.in_2].pos - hmVerts[face2.in_1].pos;
			AC = hmVerts[face2.in_3].pos - hmVerts[face2.in_1].pos;

			face2.faceNormal = glm::normalize(glm::cross(AB, AC));

			hmFaces.push_back(face1);
			hmFaces.push_back(face2);
		}
	}

	//Iterate though all hmVerts and calculate the normal from the faces:

	//First row and last row are special cases, as such they shall be handled separately

	//First element (of first row - special case)
	std::vector<Face> fb;
	fb.push_back(hmFaces[0]);
	fb.push_back(hmFaces[1]);

	hmVerts[0].norm = calcVertNormal(fb);

	fb.clear();

	for (unsigned int i = 1; i < hmWidth - 1; i++)
	{
		fb.clear();

		fb.push_back(hmFaces[(i * 2) + 1]);
		fb.push_back(hmFaces[(i * 2) + 2]);
		fb.push_back(hmFaces[(i * 2) + 3]);

		hmVerts[i].norm = calcVertNormal(fb);
	}

	//last element of first row (special case)
	fb.clear();

	fb.push_back(hmFaces[hmWidth - 1]);

	hmVerts[hmWidth - 1].norm = calcVertNormal(fb);

	fb.clear();

	unsigned int facesPerRow = (hmWidth * 2) - 2;

	for (unsigned int i = 1; i < hmHeight - 1; i++)
	{
		//Handle first and last elements of each row here
		fb.clear();

		fb.push_back(hmFaces[(i - 1) * facesPerRow]);
		fb.push_back(hmFaces[(i)*facesPerRow]);
		fb.push_back(hmFaces[(i * facesPerRow) + 1]);

		hmVerts[i * hmWidth].norm = calcVertNormal(fb);

		fb.clear();

		for (unsigned int ii = 1; ii < hmWidth - 1; ii++)
		{
			fb.clear();

			fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow)]); //0
			fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow) + 1]); //1
			fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow) + 2]); //2

			fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 1]);
			fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 2]);
			fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 3]);

			hmVerts[(i * hmWidth) + ii].norm = calcVertNormal(fb);

		}

		fb.clear();

		fb.push_back(hmFaces[((i)*facesPerRow) - 1]);
		fb.push_back(hmFaces[((i)*facesPerRow) - 2]);
		fb.push_back(hmFaces[((i + 1) * facesPerRow) - 1]);

		hmVerts[((i + 1) * hmWidth) - 1].norm = calcVertNormal(fb);

		fb.clear();

	}

	//Last Row Elements:
	fb.clear();

	//First last town vertex:
	fb.push_back(hmFaces[facesPerRow * (hmHeight - 2)]);

	hmVerts[hmWidth * (hmHeight - 1)].norm = calcVertNormal(fb);

	fb.clear();

	//Each last row element
	unsigned int k = 0;
	for (unsigned int i = hmWidth * (hmHeight - 1) + 1; i < (hmWidth * (hmHeight)) - 2; i++)
	{
		fb.clear();

		fb.push_back(hmFaces[facesPerRow * (hmHeight - 2) + (k * 2)]);
		fb.push_back(hmFaces[facesPerRow * (hmHeight - 2) + (k * 2) + 1]);
		fb.push_back(hmFaces[facesPerRow * (hmHeight - 2) + (k * 2) + 2]);

		hmVerts[i].norm = calcVertNormal(fb);

		k++;
	}

	//last element of first row (special case)
	fb.clear();

	fb.push_back(hmFaces[facesPerRow * (hmHeight - 2) + ((k - 1) * 2) + 2]);
	fb.push_back(hmFaces[facesPerRow * (hmHeight - 2) + ((k - 1) * 2) + 3]); //#Check this one again!!!

	hmVerts[(hmWidth * (hmHeight)) - 2].norm = calcVertNormal(fb);

	fb.clear();


	GLCALL(glGenVertexArrays(1, &heightMapVAO));
	GLCALL(glGenBuffers(1, &heightMapVBO));
	GLCALL(glGenBuffers(1, &heightMapEBO));

	GLCALL(glBindVertexArray(heightMapVAO));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, heightMapVBO));

	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, heightMapVBO));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, hmVerts.size() * sizeof(Vert), &hmVerts[0], GL_STATIC_DRAW));

	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, heightMapEBO));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, hmIndices.size() * sizeof(unsigned int), &hmIndices[0], GL_STATIC_DRAW));

	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)0));
	GLCALL(glEnableVertexAttribArray(0));

	GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, norm)));
	GLCALL(glEnableVertexAttribArray(1));

	GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, texCord)));
	GLCALL(glEnableVertexAttribArray(2));
}

void bindVao(unsigned int VAO)
{
	GLCALL(glBindVertexArray(VAO));
}

void unbindVao()
{
	GLCALL(glBindVertexArray(0));
}