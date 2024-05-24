#pragma once
#include "utils.h"
#include "ECS/ECSUtils.h"
#include <random>

#include "stb_perlin/stb_perlin.h"

#include "helper/Skydome.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Coordinator;
class c_Renderable;
class EntityScene;
class I_SceneFactory;

namespace util
{
	void resetHF
	(
		unsigned short hf_originalTexUnit,
		Coordinator& COORD,
		c_Renderable& rc_hf,
		Entity& hfEntity,
		unsigned int& heightFieldTex,
		unsigned int hfWidth,
		unsigned int hfHeight,
		unsigned int frequency,
		unsigned int SEED,
		int& octaves,
		float& lacunarity,
		float& persistence,
		float& amplitude,
		float& maxAmplitude
	);

	void setupSceneECS(Coordinator& COORD,
		std::vector<std::shared_ptr<Shader>>& shaders,
		std::vector<Entity>& entities,
		std::vector<Entity>& allEntites,
		std::vector<unsigned short int>& allTexUnits,
		std::unordered_map<std::string, std::shared_ptr<EntityScene>>& map_SceneNameToEntitiyScene,
		std::unordered_map<std::string, std::vector<Entity>>& map_SceneEntites,
		std::shared_ptr<I_SceneFactory> sceneFactory,
		unsigned int& hfHeight,
		unsigned int& hfWidth,
		unsigned int& heightFieldTex,
		unsigned int& frequency,
		unsigned int SEED,
		int& octaves, 
		float& lacunarity, 
		float& persistence, 
		float& amplitude, 
		float& maxAmplitude,
		Skydome& skydome
	);
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

void genMenu_1(
	float deltaTime,
	std::vector<Entity>& entities,
	std::vector<Entity>& nonSceneEntities,
	std::unordered_map<std::string, std::vector<Entity>> map_sceneNameToEntitiesVec,
	std::unordered_map<std::string, std::shared_ptr<EntityScene>>& map_SceneNameToEntitiyScene,
	Coordinator& COORD,
	short int hfTexUnit,
	unsigned short int rockySurfaceTexUnit,
	unsigned short int grassTexUnit,
	unsigned short int waterTexUnit,
	unsigned short int lavaTexUnit,
	unsigned short int brickWallTexUnit,
	unsigned short int cloudNoiseTexUnit,
	unsigned int& SEED,
	unsigned int& frequency,
	bool& reload,
	int &octaves,
	float& lacunarity,
	float& persistence,
	float& amplitude,
	float& maxAmplitude,
	float& cloud_noiseFrequency,
	float& cloud_persistence,
	float& cloud_amplitude,
	float& discardThreshold,
	bool& pauseSun
);

std::vector<glm::vec2> generateUniformVectors(int num_vectors);

//Here we are using pure random gradients, usually we want to use a set of uniform gradients that we RANDOMLY sample from.
glm::vec2 randomGradient(int ix, int iy, unsigned seed);

glm::vec2 randomGradient_2(int ix, int iy, const std::vector<glm::vec2>& vectors, unsigned seed);

float interpolate(float a0, float a1, float w);

float dotGridGradient(int ix, int iy, float x, float y, const std::vector<glm::vec2> vectors, unsigned seed);

float perlin(float x, float y, const std::vector<glm::vec2> vectors, unsigned seed);

void GeneratePerlinNoise(std::vector<float>& data, int width, int height, unsigned int SEED);
