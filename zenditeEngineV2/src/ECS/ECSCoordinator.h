#pragma once
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

class ECSCoordinator
{
private:
	std::shared_ptr<EntityManager> m_EntityManager;
	std::shared_ptr<ComponentManager> m_ComponentManager;
	std::shared_ptr<SystemManager> m_SystemManager;
	std::set<Entity>* m_PointLightEntities;
	std::set<Entity>* m_SpotLightEntities;

	std::set<Entity>* m_set_WallAABBEntities;
	std::set<Entity>* m_set_WallColliderAABBEntities;

public:
	ECSCoordinator()
	{
		m_EntityManager = std::make_shared<EntityManager>();
		m_ComponentManager = std::make_shared<ComponentManager>();
		m_SystemManager = std::make_shared<SystemManager>();
	}

	uint32_t GetActiveEntities() const
	{
		return m_EntityManager->GetActiveEntites();
	}

	std::set<Entity>* GetWallAABBEntitiesSetPtr()
	{
		return m_set_WallAABBEntities;
	}
	std::set<Entity>* GetWallCollidableAABBEntitiesSetPtr()
	{
		return m_set_WallColliderAABBEntities;
	}
	void SetWallAABBEntitiesSetPtr(std::set<Entity>* set_WallAABBEntities)
	{
		m_set_WallAABBEntities = set_WallAABBEntities;
	}
	void SetWallCollidableAABBEntitiesSetPtr(std::set<Entity>* set_WallColliderAABBEntities)
	{
		m_set_WallColliderAABBEntities = set_WallColliderAABBEntities;
	}

	std::set<Entity>* GetPointLightEntitiesPtr()
	{
		return m_PointLightEntities;
	}
	std::set<Entity>* GetSpotLightEntitiesPtr()
	{
		return m_SpotLightEntities;
	}
	void SetPointLightEntitiesPtr(std::set<Entity>* set)
	{
		m_PointLightEntities = set;
	}
	void SetSpotLightEntitiesPtr(std::set<Entity>* set)
	{
		m_SpotLightEntities = set;
	}


	//Entity Member Functions:
	Entity CreateEntity()
	{
		return m_EntityManager->CreateEntity();
	}

	void DestroyEntity(Entity EID)
	{
		m_EntityManager->DestroyEntity(EID);
		m_ComponentManager->EntityDestroyed(EID);
		m_SystemManager->EntityDestroyed(EID);
	}


	//Component Member Functions:
	template<typename T>
	void RegisterComponent()
	{
		m_ComponentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponentToEntity(Entity EID, T Component)
	{
		m_ComponentManager->AddComponentToEntity<T>(EID, Component);

		auto signature = m_EntityManager->GetEntitySignature(EID);
		signature.set(m_ComponentManager->GetComponentBitsetPos<T>(), true);
		m_EntityManager->SetEntitySignature(EID, signature);
		m_SystemManager->EntitySignatureChanged(EID, signature);

	}

	Signature GetEntitySignature(Entity EID)
	{
		return m_EntityManager->GetEntitySignature(EID);
	}

	template<typename T>
	void RemoveComponentFromEntity(Entity EID)
	{
		m_ComponentManager->RemoveComponentFromEntity<T>(EID);
		auto signature = m_EntityManager->GetEntitySignature(EID);
		signature.set(m_ComponentManager->GetComponentBitsetPos<T>(), false);
		m_EntityManager->SetEntitySignature(EID, signature);

		m_SystemManager->EntitySignatureChanged(EID, signature);
	}

	template<typename T>
	T& GetComponentDataFromEntity(Entity EID)
	{
		return m_ComponentManager->GetSpecificComponentData<T>(EID);
	}

	template<typename T>
	ComponentBitsetPos GetComponentBitsetPos()
	{
		return m_ComponentManager->GetComponentBitsetPos<T>();
	}


	//System Member Functions:
	template<typename T>
	std::shared_ptr<I_System> RegisterSystem()
	{
		return m_SystemManager->GenerateAndRegisterSystem<T>();
	}

	template<typename T>
	void SetSystemBitsetSignature(Signature sig)
	{
		m_SystemManager->SetSystemSignature<T>(sig);
	}

	template<typename T>
	Signature GetSystemBitsetSignature()
	{
		return m_SystemManager->GetSystemBitset<T>();
	}

	const std::shared_ptr<ComponentManager> GetComponentManager() const
	{
		return m_ComponentManager;
	}

	template<typename T>
	ComponentBitsetPos GetComponentBitsetPos() const
	{
		return m_ComponentManager->GetComponentBitsetPos<T>();
	}

};

