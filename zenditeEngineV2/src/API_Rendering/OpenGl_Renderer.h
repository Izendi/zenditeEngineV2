#pragma once
#include "I_Renderer.h"

class ECSCoordinator;

class OpenGL_Renderer : public I_Renderer
{
public:
	OpenGL_Renderer(std::shared_ptr<Camera> cam);
	void Render(const R_DataHandle& DataHandle, ECSCoordinator& ECScoord, Entity EID, float deltaTime, float time, int clippingPlane, float& offset, float r, float g, float b, int renderPass, bool& castShadows, float& shadowIntensity, float& waterShadowIntensity) override;
	void RenderAABB(const R_DataHandle& DataHandle, 
		Shader& AABBShader,
		const c_AABB& AABB_Data, 
		const c_Transform& trans) override;
	

private:
	std::shared_ptr<Shader> m_sh_SingleColor;
	

};