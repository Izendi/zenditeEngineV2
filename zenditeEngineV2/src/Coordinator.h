#pragma once
#include "ECS/ECSCoordinator.h"
#include "ECS/Systems/RenderableSystem.h"
#include "ECS/Systems/RenderAABBSystem.h"
#include "API_Rendering/OpenGL_Manager.h"

class Camera;
class I_Renderer;

class Coordinator
{
private:
	std::shared_ptr<ECSCoordinator> m_ECSCoord;
	std::shared_ptr<I_Subject> m_Subject;

	std::shared_ptr<I_API_Manager> m_APImanager;
	std::shared_ptr<I_Renderer> m_Renderer;

	//Systems:
	std::shared_ptr<RenderableSystem> m_RenderableSystem;
	std::shared_ptr<RenderAABBSystem> m_RenderAABBSystem;

	
public:
	Coordinator(std::string API_Type, std::string Render_Type, std::shared_ptr<Camera> camera);

	void RegisterComponents();

	void RegisterSystems();

	void SetUpSystemBitsets();

	template<typename T>
	Signature GetSystemBitset()
	{
		return m_ECSCoord->GetSystemBitsetSignature<T>();
	}

	void SetUpRenderData(Entity EID);

	Entity CreateEntity();

	void runAllSystems(float deltaTime, std::vector<Entity>* entities);
	
	void setShaderForEntity(Entity EID, std::shared_ptr<Shader> shader);

	void StoreShaderInEntityDataHandle(Entity EID);

	template<typename T>
	T& GetComponentDataFromEntity(Entity EID)
	{
		return m_ECSCoord->GetComponentDataFromEntity<T>(EID);
	}

	template<typename T>
	void AddComponentToEntity(Entity EID, T comp)
	{
		m_ECSCoord->AddComponentToEntity<T>(EID, comp);
	}

	template<typename T>
	void RemoveComponentFromEntity(Entity EID)
	{
		m_ECSCoord->RemoveComponentFromEntity<T>(EID);
	}

	std::shared_ptr<ComponentManager> GetComponentManager()
	{
		return m_ECSCoord->GetComponentManager();
	}

	unsigned short int GenerateTexUnit(std::string texFilePath, std::string fileType);

};