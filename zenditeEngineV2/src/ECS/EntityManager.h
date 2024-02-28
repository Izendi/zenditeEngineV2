#pragma once
#include "ECSutils.h"

class EntityManager
{
private:
	std::queue<Entity> m_availableEntities = {};
	std::array<Signature, MAX_ENTITIES> m_entitySignatures; //index corresponds to the entity ID
	uint32_t m_activeEntities;

public:
	EntityManager()
	{
		//Fill the m_availableEntities queue with all available entity IDs
		for(Entity EID = 0; EID < MAX_ENTITIES; ++EID)
		{
			m_availableEntities.push(EID);
		}

		m_activeEntities = 0;
	}

	Entity CreateEntity()
	{
		DEBUG_ASSERT(m_activeEntities < MAX_ENTITIES, "Max number of entities already created");

		Entity EID = m_availableEntities.front();
		m_availableEntities.pop();
		++m_activeEntities;

		return EID;
	}

	void DestroyEntity(Entity EID)
	{
		DEBUG_ASSERT(EID < MAX_ENTITIES, "Entity out of range. (needs to be smaller than MAX_ENTITIES)");

		m_entitySignatures[EID].reset();

		m_availableEntities.push(EID);
		--m_activeEntities;
	}

	void SetEntitySignature(Entity EID, Signature sig)
	{
		DEBUG_ASSERT(EID < MAX_ENTITIES, "Entity out of range. (needs to be smaller than MAX_ENTITIES)");

		m_entitySignatures[EID] = sig;
	}

	Signature GetEntitySignature(Entity EID) const
	{
		DEBUG_ASSERT(EID < MAX_ENTITIES, "Entity out of range. (needs to be smaller than MAX_ENTITIES)");

		return m_entitySignatures[EID];
	}

	uint32_t GetActiveEntites() const
	{
		return m_activeEntities;
	}

};