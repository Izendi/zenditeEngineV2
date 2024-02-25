#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Helper Data Types:

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};



//COMPONENTS:

struct c_Transform
{
	//std::vector<glm::vec3> pos;
	//glm::vec3 prevPos;
	//glm::vec3 scale;

	glm::mat4 prevModelMat;
	std::vector<glm::mat4> modelMat;
};

struct c_AABB
{
	glm::vec3 scale;
	float *vertices; //Data to go in the VBO that stores the vertices of the AABB
	bool isColliding;
	bool isWallColliding;
};

struct c_Wall
{

};

struct c_WallCollider
{

};

struct c_RenderableComponent
{
	void setPosVertexArray(float* verts, size_t size)
	{
		posVertices = new float[size]; //#Memory_Leak_Potential

		for (int i = 0; i < size; ++i)
		{
			posVertices[i] = verts[i];
		}

		posArraySize = size;
	}

	void setSurfaceNormalVertexArray(float* verts, size_t size)
	{
		surfaceNormalVertices = new float[size]; //#Memory_Leak_Potential

		for (int i = 0; i < size; ++i)
		{
			surfaceNormalVertices[i] = verts[i];
		}

		snArraySize = size;
	}

	float* posVertices;
	float* surfaceNormalVertices; //#HERE
	size_t posArraySize;
	size_t snArraySize;

	std::vector<unsigned int> indices;
};

struct c_Renderable
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

struct c_Texture
{
	unsigned short int texUnit;
	std::string type;
};

struct c_SpotLightEmitter
{

};

struct c_PointLightEmitter
{

};

struct c_LightResponderMesh
{

};



struct c_Modified
{
	bool isModifed;
};