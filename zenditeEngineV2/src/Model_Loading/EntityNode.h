#pragma once

#include "../Coordinator.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../vendor/stb_image/stb_image.h"

class EntityNode
{
private:
	std::vector<Entity> m_vec_Entites;
	std::vector<EntityNode> m_vec_children;
	glm::mat4 m_localModelMat;

	void getAllEntites(std::vector<Entity>& entities);

public:
	EntityNode();

	void SetLocalModelMat(glm::mat4 modelMat);
	void SetAllTransformCompoennts(glm::mat4 ParentModelMat, Coordinator& COORD); //Recursive function to iterate over all nodes and set the transform component for each associated entity

	void AddEntity(Entity EID); //Entity must be created beforehand using COORD.CreateEntity();

	std::vector<Entity> GetAllEntitesDownward();

};