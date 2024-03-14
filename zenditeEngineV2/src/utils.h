#pragma once
#include <GL/glew.h>

#include <iostream>
#include <memory>
#include <vector>
#include <bitset>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader;
class TextureData;

//This a utility file.
//Not a class file. Only add useful functions that can be used generally throughout the code.

//Basic Error Checking
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCALL(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1000;

#define SHADOWMAP_WIDTH 1024
#define SHADOWMAP_HEIGHT 1024

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ") \nFunction: "
			<< function << "\nFile: "
			<< file << "\nLine: "
			<< line << "\n"
			<< std::endl;

		return false;
	}

	return true;
}

struct R_DataHandle
{
	unsigned VAO;
	unsigned VBO;
	unsigned EBO;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<TextureData> texture; //(No need to contain a textureData object, only the tex unit is needed as it does not change)
	uint32_t texUnit;

	//std::bitset<32> signature;

	std::string name;

	//EXTRA:
	unsigned AABB_VAO;
	unsigned AABB_posVBO;

};

struct transform
{
	glm::vec3 pos;
	glm::vec3 rotation;
	glm::vec3 scale;

};

struct Vert
{
	//Position
	glm::vec3 pos;

	glm::vec3 norm;

	//TexCoords
	glm::vec2 texCord;

};

struct Face
{
	unsigned int in_1;
	unsigned int in_2;
	unsigned int in_3;

	glm::vec3 faceNormal;
};



inline glm::vec3 calcVertNormal(std::vector<Face> faces)
{
	glm::vec3 vertNormal(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		vertNormal = vertNormal + faces[0].faceNormal;
	}

	vertNormal = glm::normalize(vertNormal);

	return vertNormal;

}