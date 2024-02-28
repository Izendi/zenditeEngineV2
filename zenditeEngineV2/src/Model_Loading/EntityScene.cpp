#include "EntityScene.h"
#include "../Coordinator.h"

EntityScene::EntityScene(EntityNode EN, glm::mat4 sceneMM)
{
	m_RootNode = EN;
	m_RootNode.GetAllEntitesDownward(m_vec_SceneEntities);
}

void EntityScene::SetSceneModelMat(glm::mat4 ModelMat, Coordinator& COORD)
{
	m_EntitySceneModelMatrix = ModelMat;
	m_RootNode.SetAllTransformCompoennts(m_EntitySceneModelMatrix, COORD);
}

void EntityScene::SetScenePos(glm::vec3 pos)
{
	m_EntitySceneModelMatrix[3][0] = pos.x;
	m_EntitySceneModelMatrix[3][1] = pos.y;
	m_EntitySceneModelMatrix[3][2] = pos.z;
}

void EntityScene::SetShaderForAllSceneEntities(Coordinator& COORD, std::shared_ptr<Shader> shaderPtr)
{
	for(int i = 0; i < m_vec_SceneEntities.size(); ++i)
	{
		COORD.setShaderForEntity(m_vec_SceneEntities[i], shaderPtr);
		COORD.StoreShaderInEntityDataHandle(m_vec_SceneEntities[i]);
	}
}

void EntityScene::SetSceneEntities()
{

}

std::vector<Entity> EntityScene::GetSceneEntities() const
{
	return m_vec_SceneEntities;
}

Entity EntityScene::GetRootNodeRootEntity()
{
	return m_vec_SceneEntities[0];
}
