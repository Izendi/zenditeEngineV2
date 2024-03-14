#pragma once

#include "../ECSCoordinator.h"
#include "../I_System.h"
#include "../../EventManager/I_Subject.h"
#include "../../API_Rendering/I_Renderer.h"
#include "../../API_Rendering/I_API_Manager.h"
#include "../../ECS/Components.h"

class RenderableSystem : public I_System, public I_Subject
{
private:
	//std::shared_ptr<ECSCoordinator> ECScoord;
	


public:
	RenderableSystem()
	{
		//#Temp_Simple_Rendering_System
		
	}

	void NotifyObservers(std::shared_ptr<ECSCoordinator> ECS_Coord, int* i) override
	{
		for (size_t i = 0; i < observerList.size(); ++i) //Iterate through every item in observerList
		{
			//#To_Complete_4
		}
	}

	void Render(std::shared_ptr<I_Renderer> renderer, std::shared_ptr<I_API_Manager> apiManager, std::shared_ptr<ECSCoordinator> ECScoord)
	{	
		for (auto const& EID : m_EntitySet)
		{
			//If entities "modified" component is true, then call
			if (ECScoord->GetComponentDataFromEntity<c_Modified>(EID).isModifed)
			{
				apiManager->SetupRenderData(EID, ECScoord);
				apiManager->SetShaderForDataHandle(EID);

			}
		}
		
		for (auto const& EID : m_EntitySet)
		{
			renderer->Render(apiManager->GetEntityDataHandle(EID), *ECScoord, EID);
		}
	}

	

private:

};