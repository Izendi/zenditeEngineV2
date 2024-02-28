#include "EntityNode.h"
#include "../Coordinator.h"
#include "../ECS/Components.h"


EntityNode::EntityNode()
{

}

void EntityNode::SetLocalModelMat(glm::mat4 modelMat)
{
	m_localModelMat = modelMat;
}

void EntityNode::SetAllTransformCompoennts(glm::mat4 ParentModelMat, Coordinator& COORD)
{
	glm::mat4 UniversalModelMat = m_localModelMat * ParentModelMat; //This will create a model matrix that is the local model matrixc moved relative to the parents model matrix

	for(int i = 0; i < m_vec_Entites.size(); ++i)
	{
		c_Transform& transData = COORD.GetComponentDataFromEntity<c_Transform>(m_vec_Entites[i]);

		for (int j = 0; j < transData.modelMat.size(); ++j)
		{
			transData.modelMat[j] = UniversalModelMat;
		}
	}

	for(int i = 0; i < m_vec_children.size(); ++i)
	{
		m_vec_children[i]->SetAllTransformCompoennts(UniversalModelMat, COORD);
	}

}

void EntityNode::AddEntity(Entity EID)
{
	m_vec_Entites.push_back(EID);
}

void EntityNode::GetAllEntitesDownward(std::vector<Entity>& entities)
{

	for(int i = 0; i < m_vec_Entites.size(); ++i)
	{
		entities.push_back(m_vec_Entites[i]);
	}

	for(int i = 0; i < m_vec_children.size(); ++i)
	{
		m_vec_children[i]->GetAllEntitesDownward(entities);
	}

}

std::shared_ptr<EntityNode> EntityNode::CreateNewChild()
{

	m_vec_children.emplace_back(std::make_shared<EntityNode>());
	return m_vec_children.back();
}

Entity EntityNode::GetFirstEntity()
{
	DEBUG_ASSERT(!m_vec_Entites.empty(), "Attempting to retrive data from an empty vector in EntityNode.");

	return m_vec_Entites[0];
}
