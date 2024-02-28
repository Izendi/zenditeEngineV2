#pragma once

//#include "../Coordinator.h"
#include "../ECS/ECSUtils.h"
#include "../utils.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../vendor/stb_image/stb_image.h"

class Coordinator;

class EntityNode
{
private:
	std::vector<Entity> m_vec_Entites;
	std::vector<std::shared_ptr<EntityNode>> m_vec_children;
	glm::mat4 m_localModelMat;

public:
	EntityNode();

	void SetLocalModelMat(glm::mat4 modelMat);
	void SetAllTransformCompoennts(glm::mat4 ParentModelMat, Coordinator& COORD); //Recursive function to iterate over all nodes and set the transform component for each associated entity

	void AddEntity(Entity EID); //Entity must be created beforehand using COORD.CreateEntity();

	void GetAllEntitesDownward(std::vector<Entity>& entities);

	std::shared_ptr<EntityNode> CreateNewChild();
	
	Entity GetFirstEntity();
};