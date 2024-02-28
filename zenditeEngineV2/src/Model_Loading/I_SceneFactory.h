#pragma once

#include "../utils.h"
//#include "../ECS/Components.h"
#include "EntityScene.h"

class Coordinator;
struct c_Renderable;
struct c_Texture;

class I_SceneFactory
{
protected:
	Coordinator& COORD;

	virtual void ProcessAssimpNode(std::string dir, aiNode* node, const aiScene* scene, EntityNode& entNode, unsigned int NumRenderables) = 0;
	virtual void ProcessMesh(std::string dir, aiMesh* mesh, const aiScene* scene, c_Renderable& c_Rd, c_Texture& c_tx) = 0;

	// Function to convert aiMatrix4x4 to glm::mat4
	glm::mat4 ConvertAssimpMMtoGlmMM(const aiMatrix4x4& assimpMM);

public:
	I_SceneFactory(Coordinator& Coord);

	virtual EntityScene CreateEntityScene(std::string dir, std::string objFile, glm::mat4 worldModelMatrix, std::shared_ptr<Shader> shader, unsigned int NumRenderables) = 0;
	virtual void DestroyEntityScene(EntityScene& ES) = 0;

};