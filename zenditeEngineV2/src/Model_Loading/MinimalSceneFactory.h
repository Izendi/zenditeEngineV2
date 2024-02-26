#pragma once
#include "I_SceneFactory.h"

class MinimalSceneFactory : public I_SceneFactory
{

protected:
	void ProcessAssimpNode(aiNode* node, const aiScene* scene, EntityNode& entNode, unsigned int NumRenderables) override;


	void ProcessMesh(aiMesh* mesh, const aiScene* scene, c_Renderable& c_Rd, c_Texture& c_tx) override;


public:

	EntityScene CreateEntityScene(std::string path, glm::mat4 worldModelMatrix, unsigned int NumRenderables) override;
	void DestroyEntityScene(EntityScene& ES) override;
};