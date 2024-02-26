#pragma once
#include "I_SceneFactory.h"

class MinimalSceneFactory : public I_SceneFactory
{
	EntityScene CreateEntityScene(std::string path, glm::mat4 worldModelMatrix) override;
	void DestroyEntityScene(EntityScene& ES) override;
};