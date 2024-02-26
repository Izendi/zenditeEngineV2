#pragma once

#include "EntityNode.h"

class Coordinator;

class EntityScene
{
private:
	glm::mat4 m_EntitySceneModelMatrix;
	std::vector<Entity> m_vec_SceneEntities;
	EntityNode m_RootNode;

public:
	EntityScene(EntityNode EN, glm::mat4 sceneMM);

	void SetSceneModelMat(glm::mat4 ModelMat);
	void SetScenePos(glm::vec3 pos); //Sets the transform components of the model matrix (will make it easier to work with in certain situations)
	void SetShaderForAllSceneEntities(Coordinator& COORD, std::shared_ptr<Shader> shaderPtr); //Loop through m_vec_SceneEntities and assing this shader.

	std::vector<Entity> GetSceneEntities() const;
	Entity GetRootNodeRootEntity(); //The first entity in the root node.


};