#pragma once

#include "../ECS/Components.h"
//#include "../utils.h"
#include "../ECS/ECSUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Skydome
{
private:
	c_Transform trans;
	c_Renderable renderable;
	c_Texture texUnit;
	c_EntityInfo info;
	c_Modified mod;

	float m_Radius; 
	float m_HeightScale; 
	glm::vec3 m_worldOrigin;

	unsigned m_nVertices;

public:
	Skydome();

	void DestroySkydome();

	void CreateSkydome(
		unsigned nLats, 
		unsigned nlongs, 
		float fRadius, 
		glm::vec3 worldPosition,
		glm::vec3 scale);

	void generate3DNoiseTexture(unsigned short texUnitToAssign, int x, int y, int z);

	void setSkydomeTransform(glm::vec3 worldPos, glm::vec3 scale);

	void setSkydomeTexUnit(unsigned short skydomeTexUnit);

	void SetComponent_c_Transform(c_Transform transform);
	void SetComponent_c_Renderable(c_Renderable renderableComponent);
	void SetComponent_c_Texture(c_Texture texture);
	void SetComponent_c_EntityInfo(c_EntityInfo information);
	void SetComponent_c_Modified(bool modified);

	c_Transform& GetComponent_Transform();
	c_Renderable& GetComponent_Renderable();
	c_Texture& GetComponent_Texture();
	c_EntityInfo& GetComponent_EntityInfo();
	c_Modified& GetComponent_Modified();



};