#pragma once
#include "utils.h"
#include "ECS/ECSUtils.h"

class Coordinator;
class c_Renderable;
class EntityScene;

namespace util
{
	void setupSceneECS(Coordinator& COORD,
		std::vector<std::shared_ptr<Shader>>& shaders,
		std::vector<Entity>& entities,
		std::vector<Entity>& allEntites,
		std::vector<unsigned short int>& allTexUnits);
}

void GenerateBasicSquareVAO(unsigned int &VAO_Square, unsigned int &VBO_Square, unsigned int &EBO_Square);

void GenerateTexTriVAO(unsigned int &VAO_TexTri, unsigned int &VBO_TexTri, unsigned int& EBO_TexTri);

void GenerateCubeNoEBO(unsigned int& VAO_Cube, unsigned int& VBO_Cube);

void GenerateCubeNoEBONoNormals(unsigned int& VAO_Cube, unsigned int& VBO_Cube);

void GenerateHeightmapPlane(unsigned int& heightMapVAO, 
	unsigned int& heightMapVBO, 
	unsigned int& heightMapEBO, 
	std::vector<Vert>& hmVerts,
	std::vector<unsigned int>& hmIndices,
	std::vector<Face>& hmFaces,
	int hmWidth,
	int hmHeight,
	int hmNrChannels,
	unsigned char* hmTexData
	);

void bindVao(unsigned int VAO);

void unbindVao();

void addDataToRenderable(c_Renderable& rc, float* vertCubePosData, float* vertCubeNormData, float* vertCubeTexCoordData, unsigned int* indices, size_t sizeofVertCubePosData, size_t sizeofIndices);

void genMenu_1(std::vector<Entity>& entities,
	std::vector<Entity>& nonSceneEntities,
	std::unordered_map<std::string, std::vector<Entity>> map_sceneNameToEntitiesVec,
	std::unordered_map<std::string, std::shared_ptr<EntityScene>>& map_SceneNameToEntitiyScene,
	Coordinator& COORD,
	short int containerTexUnit,
	unsigned short int rockySurfaceTexUnit,
	unsigned short int grassTexUnit,
	unsigned short int waterTexUnit,
	unsigned short int lavaTexUnit,
	unsigned short int brickWallTexUnit
);