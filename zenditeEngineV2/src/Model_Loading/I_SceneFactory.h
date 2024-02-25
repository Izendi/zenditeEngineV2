#pragma once

#include "../ECS/ECSCoordinator.h"
#include "../utils.h"
#include "EntityScene.h"

class I_SceneFactory
{
protected:
	ECSCoordinator& ECScoord;

	void ProcessAssimpNode();

private:


public:
	virtual EntityScene CreateEntityScene(std::string path, glm::mat4 worldModelMatrix) = 0;
	virtual void DestroyEntityScene(EntityScene& ES) = 0;


};