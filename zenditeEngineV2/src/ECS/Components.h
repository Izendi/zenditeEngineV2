#pragma once
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Helper Data Types:

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	Vertex()
	{

	}

	Vertex(glm::vec3 pos,
		glm::vec3 norm,
		glm::vec2 txCoord,
		glm::vec3 tang,
		glm::vec3 btan)
		:
		Position(pos),
		Normal(norm),
		TexCoords(txCoord),
		Tangent(tang),
		Bitangent(btan)
	{ }

	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct LightweightVertex
{
	glm::vec3 Position;
};


//COMPONENTS:

struct c_Transform
{
	//std::vector<glm::vec3> pos;
	//glm::vec3 prevPos;
	//glm::vec3 scale;

	glm::mat4 prevModelMat;
	std::vector<glm::mat4> modelMat;

	void clear()
	{
		prevModelMat = glm::mat4(1.0f);

		for(int i = 0; i < modelMat.size(); ++i)
		{
			modelMat[i] = glm::mat4(1.0f);
		}
		
	}
};

struct c_AABB
{
	glm::vec3 scale;
	//std::shared_ptr<float[]> vertices;
	float vertices[72]; //Data to go in the VBO that stores the vertices of the AABB
	bool isColliding;
	bool isWallColliding;

	bool active = true;
};

struct c_Wall
{

};

struct c_WallCollider
{

};

struct c_Renderable
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	bool outline = false;
	bool blending = false;
	bool active = true;
	bool isActive = true;
	
	bool isLightEmitter = false;

	bool emReflection = false; //#If_active_you_must_set_the_appropriate_shader_for_the_Entity
	
	void clear()
	{
		vertices.clear();
		indices.clear();
		outline = false;
		blending = false;
		active = true;
		emReflection = false;
	}
};

struct c_Texture
{
	unsigned short int texUnit; //If emReflection this should be a cube map tex unit.
	bool is3Dtex = false;
	//std::string type;
};

struct c_EntityInfo
{
	std::string name;

	
};

struct c_Modified
{
	bool isModifed;
};