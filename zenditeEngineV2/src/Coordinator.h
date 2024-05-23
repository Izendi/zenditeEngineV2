#pragma once
#include "ECS/ECSCoordinator.h"
#include "ECS/Systems/RenderableSystem.h"
#include "ECS/Systems/RenderAABBSystem.h"
#include "ECS/Systems/CollisionDetectionAABBSystem.h"
#include "ECS/Systems/PositionTrackerSystem.h"

#include "ECS/Systems/SetUpWallAABBSystem.h"
#include "ECS/Systems/SetUpWallColliderAABBSystem.h"
#include "ECS/Systems/WallCollisionHandlingSystem.h"

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
	std::shared_ptr<CollisionDetectionAABBSystem> m_CollisionDetectionAABBSystem;
	std::shared_ptr<RenderAABBSystem> m_RenderAABBSystem;
	std::shared_ptr<SetUpWallAABBSystem> m_SetUpWallAABBSystem;
	std::shared_ptr<SetUpWallColliderAABBSystem> m_SetUpWallColliderAABBSystem;
	std::shared_ptr<WallCollisionHandlingSystem> m_WallCollisionHandlingSystem;
	std::shared_ptr<PositionTrackerSystem> m_PositionTrackerSystem;


	
public:
	Coordinator(std::string API_Type, std::string Render_Type, std::shared_ptr<Camera> camera);

	void RegisterComponents();

	void RegisterSystems();

	void SetUpSystemBitsets();

	uint32_t GetActiveEntities() const;

	template<typename T>
	Signature GetSystemBitset()
	{
		return m_ECSCoord->GetSystemBitsetSignature<T>();
	}

	Signature GetEntitySignature(Entity EID);

	void SetUpRenderData(Entity EID);

	Entity CreateEntity();

	void runAllSystems(float deltaTime, float time, std::vector<Entity>& entities);
	
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

	template<typename T>
	ComponentBitsetPos GetComponentBitsetPos() const
	{
		return m_ECSCoord->GetComponentBitsetPos<T>();
	}

	unsigned short int GenerateTexUnit(std::string texFilePath, std::string fileType);

	void GenerateShadowMapForEntity(Entity EID);
};