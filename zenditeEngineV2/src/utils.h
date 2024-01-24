#pragma once
#include <GL/glew.h>

#include <iostream>
#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Basic Error Checking
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCALL(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

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

struct Material
{
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float shininess; //AKA: specularStrength
};

struct PointLight
{
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct Light
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct DirLight
{
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
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