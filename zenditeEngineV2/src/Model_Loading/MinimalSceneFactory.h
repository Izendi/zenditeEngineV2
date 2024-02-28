#pragma once
#include "I_SceneFactory.h"

class MinimalSceneFactory : public I_SceneFactory
{

protected:
	void ProcessAssimpNode(std::string dir, aiNode* node, const aiScene* scene, EntityNode& entNode, unsigned int NumRenderables) override;


	void ProcessMesh(std::string dir, aiMesh* mesh, const aiScene* scene, c_Renderable& c_Rd, c_Texture& c_tx) override;


public:

	MinimalSceneFactory(Coordinator& coorinator);

	EntityScene CreateEntityScene(std::string dir, std::string objFile, glm::mat4 worldModelMatrix, std::shared_ptr<Shader> shader, unsigned int NumRenderables) override;
	void DestroyEntityScene(EntityScene& ES) override;
};