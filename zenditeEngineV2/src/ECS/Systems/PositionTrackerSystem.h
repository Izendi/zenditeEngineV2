#pragma once

#include "../ECSCoordinator.h"
#include "../I_System.h"
#include "../../EventManager/I_Subject.h"
#include "../../API_Rendering/I_Renderer.h"
#include "../../API_Rendering/I_API_Manager.h"
#include "../../ECS/Components.h"

#include "../../Shader.h"

class PositionTrackerSystem : public I_System, public I_Subject
{
private:
	Shader m_AABBShader;

public:
	//res/shaders/AABB/vs_BasicAABB.glsl //res/shaders/AABB/fs_BasicAABB.glsl
	PositionTrackerSystem() : m_AABBShader("res/shaders/AABB/vs_BasicAABB.glsl", "res/shaders/AABB/fs_BasicAABB.glsl")
	{

	}

	void NotifyObservers(std::shared_ptr<ECSCoordinator> ECS_Coord, int* i) override
	{
		for (size_t i = 0; i < observerList.size(); ++i) //Iterate through every item in observerList
		{
			//#To_Complete_3
		}
	}

	void UpdatePrePosData(std::shared_ptr<ECSCoordinator> ECScoord)
	{
		for (auto const& EID : m_EntitySet)
		{
			//If entities "modified" component is true, then call
			c_Transform& transData = ECScoord->GetComponentDataFromEntity<c_Transform>(EID);
			transData.prevModelMat = transData.modelMat[0];
			
		}
	}

private:

};