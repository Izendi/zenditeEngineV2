#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct c_Transform
{
	//std::vector<glm::vec3> pos;
	//glm::vec3 prevPos;
	//glm::vec3 scale;

	glm::mat4 modelMat;
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
};

struct c_Texture
{
	void setTexCoordsVertexArray(float* verts, size_t size)
	{
		texCoords = new float[size]; //#EXCEPTION_THROWN_RANDOMLY_HERE

		for (int i = 0; i < size; ++i)
		{
			texCoords[i] = verts[i];
		}

		arraySize = size;
	}

	unsigned short int texUnit;
	float* texCoords;
	size_t arraySize;
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