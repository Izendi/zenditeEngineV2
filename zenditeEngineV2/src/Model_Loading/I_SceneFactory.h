#pragma once

#include "../ECS/Components.h"
#include "../utils.h"
#include "EntityScene.h"

class I_SceneFactory
{
protected:
	Coordinator& COORD;

	void ProcessAssimpNode(aiNode* node, const aiScene* scene, EntityNode& entNode);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene, c_Renderable& c_Rd, c_Transform c_tr);

private:


public:
	virtual EntityScene CreateEntityScene(std::string path, glm::mat4 worldModelMatrix) = 0;
	virtual void DestroyEntityScene(EntityScene& ES) = 0;

};