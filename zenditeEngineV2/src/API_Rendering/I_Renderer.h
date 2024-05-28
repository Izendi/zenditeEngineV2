#pragma once
#include "../Camera.h"
#include "../utils.h"
#include "../ECS/ECSUtils.h"
#include "../Shader.h"

class ECSCoordinator;
class c_Transform;
class c_AABB;

class I_Renderer
{
protected:
	std::shared_ptr<Camera> cam;
	//std::unique_ptr<Shader> m_lightSourceShader;

public:
	I_Renderer(std::shared_ptr<Camera> camera)
	{
		cam = camera;
	}

	virtual void Render(const R_DataHandle& DataHandle, ECSCoordinator& ECScoord, Entity EID, float deltaTime, float time, int clippingPlane, float& offset) = 0;
	virtual void RenderAABB(const R_DataHandle& DataHandle, 
		Shader& AABBShader,
		const c_AABB& AABB_Data,
		const c_Transform& trans) = 0;

	
	
};