#include "I_SceneFactory.h"
#include "../Coordinator.h"


I_SceneFactory::I_SceneFactory(Coordinator& coordinator) : COORD(coordinator)
{
	
}

glm::mat4 I_SceneFactory::ConvertAssimpMMtoGlmMM(const aiMatrix4x4& assimpMM)
{
	glm::mat4 glmMM;

	// Assimp matrices are row-major, but GLM matrices has column-major ordering. As such we need to adjust the inputs:
	glmMM[0][0] = assimpMM.a1; glmMM[1][0] = assimpMM.a2; glmMM[2][0] = assimpMM.a3; glmMM[3][0] = assimpMM.a4;
	glmMM[0][1] = assimpMM.b1; glmMM[1][1] = assimpMM.b2; glmMM[2][1] = assimpMM.b3; glmMM[3][1] = assimpMM.b4;
	glmMM[0][2] = assimpMM.c1; glmMM[1][2] = assimpMM.c2; glmMM[2][2] = assimpMM.c3; glmMM[3][2] = assimpMM.c4;
	glmMM[0][3] = assimpMM.d1; glmMM[1][3] = assimpMM.d2; glmMM[2][3] = assimpMM.d3; glmMM[3][3] = assimpMM.d4;

	return glmMM;
}