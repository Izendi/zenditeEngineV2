#pragma once

#include "../ECS/Components.h"
#include "../utils.h"
#include "EntityScene.h"

class I_SceneFactory
{
protected:
	Coordinator& COORD;

	virtual void ProcessAssimpNode(aiNode* node, const aiScene* scene, EntityNode& entNode, unsigned int NumRenderables) = 0;
	virtual void ProcessMesh(aiMesh* mesh, const aiScene* scene, c_Renderable& c_Rd, c_Texture& c_tx) = 0;

	// Function to convert aiMatrix4x4 to glm::mat4
	glm::mat4 ConvertAssimpMMtoGlmMM(const aiMatrix4x4& assimpMM)
	{
		glm::mat4 glmMM;

		// Assimp matrices are row-major, but GLM matrices has column-major ordering. As such we need to adjust the inputs:
		glmMM[0][0] = assimpMM.a1; glmMM[1][0] = assimpMM.a2; glmMM[2][0] = assimpMM.a3; glmMM[3][0] = assimpMM.a4;
		glmMM[0][1] = assimpMM.b1; glmMM[1][1] = assimpMM.b2; glmMM[2][1] = assimpMM.b3; glmMM[3][1] = assimpMM.b4;
		glmMM[0][2] = assimpMM.c1; glmMM[1][2] = assimpMM.c2; glmMM[2][2] = assimpMM.c3; glmMM[3][2] = assimpMM.c4;
		glmMM[0][3] = assimpMM.d1; glmMM[1][3] = assimpMM.d2; glmMM[2][3] = assimpMM.d3; glmMM[3][3] = assimpMM.d4;

		return glmMM;
	}

private:


public:
	virtual EntityScene CreateEntityScene(std::string path, glm::mat4 worldModelMatrix, unsigned int NumRenderables) = 0;
	virtual void DestroyEntityScene(EntityScene& ES) = 0;

};