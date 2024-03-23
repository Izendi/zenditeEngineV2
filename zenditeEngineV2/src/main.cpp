#include "utils.h"
#include <GLFW/glfw3.h>
#include <chrono>

#include "vendor/stb_image/stb_image.h"
#include "assimp/Importer.hpp"

#include "Shader.h"
//#include "Texture2D.h"
#include "geometrySetup.h"
#include "menu.h"
#include "Camera.h"

#include <filesystem>

#include "Coordinator.h"
#include "ECS/Components.h"

namespace fs = std::filesystem;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void addDataToRenderable(c_Renderable& rc, float* vertCubePosData, float* vertCubeNormData, float* vertCubeTexCoordData, unsigned int* indices, size_t sizeofVertCubePosData, size_t sizeofIndices);
//void addDataToLightRenderable(c_LightRenderable& rc, float* vertCubePosData, unsigned int* indices, size_t sizeofVertCubePosData, size_t sizeofIndices);

void setUpBasicModelMatrix(glm::mat4& MM, glm::vec3 pos, glm::vec3 scale)
{	
	MM = glm::mat4(1.0f);
	MM = glm::translate(MM, pos);
	MM = glm::scale(MM, scale);
}

// camera
std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool trackMouseMovement = true;
double savedXpos, savedYpos;
double currentX, currentY;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//GUI Menu Toggles
bool toggle = true;
bool wireframe = false;
bool rotation = false;

int main(void)
{
	GLFWwindow* window;
	if (!glfwInit()) {
		return -1;
	}
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Zen", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	GLFWcursorposfun previousCursorPosCallback = nullptr;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	previousCursorPosCallback = glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//glfwSetCursorPosCallback(window, nullptr);
	//glfwSetCursorPosCallback(window, mouse_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "I'm am not GLEW_OK, I'm GLEW_SAD :(\n";
	}

	imGuiSetup(window);

	stbi_set_flip_vertically_on_load(true); //#### THIS NEEDS TO BE ACTIVE ### or else image texture will be upside down.
	//std::cout << "Current working directory: " << fs::current_path() << std::endl;
	glEnable(GL_DEPTH_TEST);

	std::cout << glGetString(GL_VERSION) << "\n";

	//Check the number of texture units we can have on the GPU
	GLint maxTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	std::cout << "Maximum texture units available: " << maxTextureUnits << std::endl;

	//Testing ECS: Start ECS - $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	Coordinator COORD("opengl", "opengl", camera); //std::string API_Type, std::string Render_Type, std::shared_ptr<Camera> camera
	COORD.RegisterComponents();
	COORD.RegisterSystems();
	COORD.SetUpSystemBitsets();

	std::cout << "\nRenderableSystem bitset: " << COORD.GetSystemBitset<RenderableSystem>() << std::endl;
	//std::cout << "\Rigid_CollisionDetectionSystem bitset: " << COORD.GetSystemBitset<Rigid_CollisionDetectionSystem>() << std::endl;

	std::shared_ptr<Shader> sh_basicWithTex = std::make_shared<Shader>("res/shaders/BasicShaders/vs_cubeWnormANDtex.glsl",
		"res/shaders/BasicShaders/fs_cubeWnormANDtex.glsl"); //#Shaders have not yet been abstracted into the API_Manger

	std::shared_ptr<Shader> sh_fboRender = std::make_shared<Shader>("",
		"");
		
	std::unique_ptr<I_SceneFactory> sceneFactory = std::make_unique<MinimalSceneFactory>(COORD);

	float verticalQuad[] = {
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f
	};

	float vertQuadVertNorms[] = {
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f
	};

	float vertQuadTexCoord[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	unsigned int vertQuadIndices[] = {
		0, 1, 2,
		2, 3, 0
	};

	//#TODO Need to pass data read in from the model loader to the ECS system for rendering.
	float vertCubePosData[] = {
		// Positions        
		// Front face
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,

		// Back face
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		//Right Face
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,

		 //LeftFace
		 -1.0f, -1.0f,  1.0f,
		 -1.0f, -1.0f, -1.0f,
		 -1.0f,  1.0f, -1.0f,
		 -1.0f,  1.0f,  1.0f,

		 //Top Face
		 -1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f, -1.0f,
		 -1.0f,  1.0f, -1.0f,

		 //Bottom Face
		 -1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f, -1.0f
	};

	float vertCubeNormData[] = {
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,

		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,

		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,

		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,

		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,

		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f
	};

	float vertCubeTexCoordData[] =
	{
		 0.0f, 0.0f,
		 1.0f, 0.0f,
		 1.0f, 1.0f,
		 0.0f, 1.0f,

		 0.0f, 0.0f,
		 1.0f, 0.0f,
		 1.0f, 1.0f,
		 0.0f, 1.0f,

		 0.0f, 0.0f,
		 1.0f, 0.0f,
		 1.0f, 1.0f,
		 0.0f, 1.0f,

		 0.0f, 0.0f,
		 1.0f, 0.0f,
		 1.0f, 1.0f,
		 0.0f, 1.0f,

		 0.0f, 0.0f,
		 1.0f, 0.0f,
		 1.0f, 1.0f,
		 0.0f, 1.0f,

		 0.0f, 0.0f,
		 1.0f, 0.0f,
		 1.0f, 1.0f,
		 0.0f, 1.0f
	};

	unsigned int indices[] =
	{
		//front face
		0, 1, 2,
		2, 3, 0,

		//back face
		4, 5, 6,
		6, 7, 4,

		//right face
		8, 9, 10,
		10, 11, 8,

		//left face
		12, 13, 14,
		14, 15, 12,

		//top face:
		16, 17, 18,
		18, 19, 16,

		//bottom face
		20, 21, 22,
		22, 23, 20

	};

	//size 72 values
	float AABBvertices[] = {
	-1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  // Edge 1 (x max and x min)
	 1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  // Edge 2 (y max and y min)
	 1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  // Edge 3 (z max and z min)
	-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  // Edge 4
	-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  // Edge 5
	 1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  // Edge 6
	 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  // Edge 7
	-1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  // Edge 8
	 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  // Edge 9
	-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  // Edge 10
	 1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  // Edge 11
	-1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f   // Edge 12
	};

	std::vector<Entity> entities;
	std::vector<Entity> allEntites;
	std::unordered_map<std::string, std::shared_ptr<EntityScene>> map_SceneNameToEntitiyScene;
	std::unordered_map<std::string, std::vector<Entity>> map_SceneEntites;
	
	entities.push_back(COORD.CreateEntity());
	entities.push_back(COORD.CreateEntity());
	entities.push_back(COORD.CreateEntity());
	entities.push_back(COORD.CreateEntity());
	entities.push_back(COORD.CreateEntity());

	for(int i = 0; i < entities.size(); ++i)
	{
		allEntites.push_back(entities[i]);
	}

	unsigned short int containerTexUnit = COORD.GenerateTexUnit("res/textures/container2.png", "png");		 // tx Unit = 0
	unsigned short int rockySurfaceTexUnit = COORD.GenerateTexUnit("res/textures/rockySurface.png", "png");	 // tx Unit = 1
	unsigned short int waterTexUnit = COORD.GenerateTexUnit("res/textures/water.jpg", "jpg");				 // tx Unit = 2
	unsigned short int grassTexUnit = COORD.GenerateTexUnit("res/textures/grass.png", "png");				 // tx Unit = 3
	unsigned short int lavaTexUnit = COORD.GenerateTexUnit("res/textures/lava.jpg", "jpg");					 // tx Unit = 4
	unsigned short int redWindowTexUnit = COORD.GenerateTexUnit("res/textures/redWindow.png", "png");		 // tx Unit = 5

	//unsigned short int heightMapTex = COORD.GenerateTexUnit("res/textures/heightmap.png", "PNG");

	c_Transform tr_0;
	c_Transform tr_1;
	c_Transform tr_2;
	c_Transform tr_3;
	c_Transform tr_4;

	//tr_0
	glm::mat4 mm_tr0 = glm::mat4(1.0f);
	glm::vec3 pos_tr0(0.0f, -2.3f, 0.0f);
	glm::vec3 scale_tr0(1.0f, 1.0f, 1.0f);
	mm_tr0 = glm::translate(mm_tr0, pos_tr0);
	mm_tr0 = glm::scale(mm_tr0, scale_tr0);
	tr_0.modelMat.push_back(mm_tr0);

	//tr_1
	glm::mat4 mm_tr1 = glm::mat4(1.0f);
	glm::vec3 pos_tr1(-5.0f, -3.5f, -5.0f);
	glm::vec3 scale_tr1(15.0f, 0.2f, 15.0f);
	mm_tr1 = glm::translate(mm_tr1, pos_tr1);
	mm_tr1 = glm::scale(mm_tr1, scale_tr1);
	tr_1.modelMat.push_back(mm_tr1);

	//tr_2
	glm::mat4 mm_tr2 = glm::mat4(1.0f);
	glm::vec3 pos_tr2(-1.2f, -2.3f, -6.0f);
	glm::vec3 scale_tr2(1.0f, 1.0f, 1.0f);
	mm_tr2 = glm::translate(mm_tr2, pos_tr2);
	mm_tr2 = glm::scale(mm_tr2, scale_tr2);
	tr_2.modelMat.push_back(mm_tr2);

	//tr_3
	glm::mat4 mm_tr3 = glm::mat4(1.0f);
	glm::vec3 pos_tr3(-2.5f, -2.3f, -0.48f);
	glm::vec3 scale_tr3(1.0f, 1.0f, 1.0f);
	mm_tr3 = glm::translate(mm_tr3, pos_tr3);
	mm_tr3 = glm::scale(mm_tr3, scale_tr3);
	tr_3.modelMat.push_back(mm_tr3);

	//tr_4
	glm::mat4 mm_tr4 = glm::mat4(1.0f);
	glm::vec3 pos_tr4(-4.5f, -2.3f, -0.48f);
	glm::vec3 scale_tr4(1.0f, 1.0f, 1.0f);
	mm_tr4 = glm::translate(mm_tr4, pos_tr4);
	mm_tr4 = glm::scale(mm_tr4, scale_tr4);
	tr_4.modelMat.push_back(mm_tr4);

	size_t sizeOfVertCubePosData = sizeof(vertCubePosData) / sizeof(float);
	size_t sizeOfIndices = sizeof(indices) / sizeof(unsigned int);

	size_t sizeOfVerticalQuad = sizeof(verticalQuad) / sizeof(float);
	size_t sizeOfVQIndices = sizeof(vertQuadIndices) / sizeof(unsigned int);

	c_Renderable rc_0;
	addDataToRenderable(rc_0, vertCubePosData, vertCubeNormData, vertCubeTexCoordData, indices, sizeOfVertCubePosData, sizeOfIndices);
	rc_0.outline = false;

	std::cout << "\nsize of vertCubePosData = " << sizeOfVertCubePosData << std::endl;
	std::cout << "\nsize of Indices         = " << sizeOfIndices << std::endl;

	c_Renderable rc_1;
	addDataToRenderable(rc_1, vertCubePosData, vertCubeNormData, vertCubeTexCoordData, indices, sizeOfVertCubePosData, sizeOfIndices);
	rc_1.outline = false;

	// verticalQuad
	// vertQuadTexCoord
	// vertQuadIndices

	//Grass Billboard renderable
	c_Renderable rc_3;
	addDataToRenderable(rc_3, verticalQuad, vertQuadVertNorms, vertQuadTexCoord, vertQuadIndices, sizeOfVerticalQuad, sizeOfVQIndices);
	rc_3.outline = false;

	//c_Renderable rc_grass;

	c_Texture tx_0;
	tx_0.texUnit = containerTexUnit;

	c_Texture tx_1;
	tx_1.texUnit = waterTexUnit;

	c_Texture tx_2;
	tx_2.texUnit = rockySurfaceTexUnit;

	c_Texture tx_3;
	tx_3.texUnit = grassTexUnit;

	c_Texture tx_4;
	tx_4.texUnit = waterTexUnit;

	c_Texture tx_5;
	tx_5.texUnit = lavaTexUnit;

	c_Texture tx_6;
	tx_6.texUnit = redWindowTexUnit;


	c_Modified md_0;
	md_0.isModifed = true;

	c_Modified md_1;
	md_1.isModifed = true;

	c_Modified md_2;
	md_2.isModifed = true;

	c_Modified md_3;
	md_3.isModifed = true;

	c_Modified md_4;
	md_4.isModifed = true;


	c_AABB aabb_0;
	aabb_0.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	aabb_0.vertices = AABBvertices;

	c_AABB aabb_2;
	aabb_2.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	aabb_2.vertices = AABBvertices;

	c_Wall wall_0;
	c_WallCollider wallCollider_2;

	c_EntityInfo ei_0;
	ei_0.name = "Wall Col Cube";

	c_EntityInfo ei_1;
	ei_1.name = "Floor";

	c_EntityInfo ei_2;
	ei_2.name = "Wall cube";

	c_EntityInfo ei_3;
	ei_3.name = "Grass Billboard 1";

	c_EntityInfo ei_4;
	ei_4.name = "red Window";

	//C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/models/OakTree/OakTree.obj

	/*
	glm::mat4 ES2_mm = glm::mat4(1.0f);
	glm::vec3 ES2_pos(-8.6f, -3.1f, -4.0f);
	glm::vec3 ES2_scale(0.3f, 0.3f, 0.3f);
	ES2_mm = glm::translate(ES2_mm, ES2_pos);
	ES2_mm = glm::scale(ES2_mm, ES2_scale);

	map_SceneNameToEntitiyScene["OakTree_1"] = sceneFactory->CreateEntityScene("res/models/OakTree/", "OakTree.obj", ES2_mm, sh_basicWithTex, 1);
	map_SceneEntites["OakTree_1"] = map_SceneNameToEntitiyScene["OakTree_1"]->GetSceneEntities();
	*/

	COORD.AddComponentToEntity<c_Transform>(entities[0], tr_1);
	COORD.AddComponentToEntity<c_Renderable>(entities[0], rc_1);
	COORD.AddComponentToEntity<c_Texture>(entities[0], tx_1);
	//COORD.AddComponentToEntity<c_AABB>(entities[1], aabb_0);
	//COORD.AddComponentToEntity<c_WallCollider>(entities[1], wallCollider_2);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[0], ei_1);
	COORD.AddComponentToEntity<c_Modified>(entities[0], md_1);
	COORD.SetUpRenderData(entities[0]);
	COORD.setShaderForEntity(entities[0], sh_basicWithTex);
	COORD.StoreShaderInEntityDataHandle(entities[0]);

	COORD.AddComponentToEntity<c_Transform>(entities[1], tr_2);
	COORD.AddComponentToEntity<c_Renderable>(entities[1], rc_0);
	COORD.AddComponentToEntity<c_Texture>(entities[1], tx_2);
	COORD.AddComponentToEntity<c_AABB>(entities[1], aabb_2);
	COORD.AddComponentToEntity<c_Wall>(entities[1], wall_0);
	COORD.AddComponentToEntity<c_Modified>(entities[1], md_2);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[1], ei_2);
	COORD.SetUpRenderData(entities[1]);
	COORD.setShaderForEntity(entities[1], sh_basicWithTex);
	COORD.StoreShaderInEntityDataHandle(entities[1]);

	COORD.AddComponentToEntity<c_Transform>(entities[2], tr_0);
	COORD.AddComponentToEntity<c_Renderable>(entities[2], rc_0);
	COORD.AddComponentToEntity<c_Texture>(entities[2], tx_0);
	COORD.AddComponentToEntity<c_AABB>(entities[2], aabb_0);
	COORD.AddComponentToEntity<c_WallCollider>(entities[2], wallCollider_2);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[2], ei_0);
	COORD.AddComponentToEntity<c_Modified>(entities[2], md_0);
	COORD.SetUpRenderData(entities[2]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
	COORD.setShaderForEntity(entities[2], sh_basicWithTex); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
	COORD.StoreShaderInEntityDataHandle(entities[2]);

	COORD.AddComponentToEntity<c_Transform>(entities[3], tr_3);
	COORD.AddComponentToEntity<c_Renderable>(entities[3], rc_3);
	COORD.AddComponentToEntity<c_Texture>(entities[3], tx_3);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[3], ei_3);
	COORD.AddComponentToEntity<c_Modified>(entities[3], md_3);
	COORD.SetUpRenderData(entities[3]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
	COORD.setShaderForEntity(entities[3], sh_basicWithTex); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
	COORD.StoreShaderInEntityDataHandle(entities[3]);

	COORD.AddComponentToEntity<c_Transform>(entities[4], tr_4);
	COORD.AddComponentToEntity<c_Renderable>(entities[4], rc_3);
	COORD.AddComponentToEntity<c_Texture>(entities[4], tx_6);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[4], ei_4);
	COORD.AddComponentToEntity<c_Modified>(entities[4], md_4);
	COORD.SetUpRenderData(entities[4]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
	COORD.setShaderForEntity(entities[4], sh_basicWithTex); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
	COORD.StoreShaderInEntityDataHandle(entities[4]);
		

	//std::cout << "\nc_AABB bitset position: " << static_cast<unsigned int>(COORD.GetComponentBitsetPos<c_AABB>());
	//std::cout << "\nentities[2] bitset: " << COORD.GetEntitySignature(entities[2]) << std::endl;

	std::cout << "\nImGui Version: " << IMGUI_VERSION << std::endl;
	std::cout << "\n Number of active entities: " << COORD.GetActiveEntities() << std::endl;

	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);

	std::cout << "GLFW Version: " << major << "." << minor << "." << revision << std::endl;

	//Fill AllEntities Vector:
	for(const auto& pair :  map_SceneEntites)
	{
		for(int i = 0; i < pair.second.size(); ++i)
		{
			allEntites.push_back(pair.second[i]);
		}
	}

	//bool wireframe = false;

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_CULL_FACE);

	//Framebuffer Code:
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	unsigned int color_buf_texture;
	unsigned int depthAndStencil_buf_texture;

	GLCALL(glGenTextures(1, &color_buf_texture));
	GLCALL(glBindTexture(GL_TEXTURE_2D, color_buf_texture));

	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600, 1000, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));

	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buf_texture, 0);

	GLCALL(glGenTextures(1, &depthAndStencil_buf_texture));
	GLCALL(glBindTexture(GL_TEXTURE_2D, depthAndStencil_buf_texture));

	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 1600, 1000, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL));

	GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAndStencil_buf_texture, 0));

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{

	}
	else
	{
		DEBUG_ASSERT(false, "Framebuffer was not complete :(");
	}



	//set framebuffer back to default:
	

	while (!glfwWindowShouldClose(window))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // <== #HERE

		//glDepthFunc(GL_ALWAYS);

		if(wireframe == true)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/* Render here */
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//glClearDepth(1.0f);

		//moveEntityBackAndFourth(COORD.GetComponentDataFromEntity<c_Transform>(entities[0]), deltaTime);

		COORD.runAllSystems(2.0f, allEntites); //#ECS_RENDERING

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);




		genMenu_2(allEntites,
			entities,
			map_SceneEntites,
			map_SceneNameToEntitiyScene,
			COORD,
			containerTexUnit,
			rockySurfaceTexUnit,
			waterTexUnit,
			grassTexUnit,
			lavaTexUnit,
			redWindowTexUnit
		);

		glfwPollEvents();

		processInput(window);

		glfwSwapBuffers(window);


	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	//std::cin.get();

	return 0;
}

void addDataToRenderable(c_Renderable& rc, float* vertCubePosData, float* vertCubeNormData, float* vertCubeTexCoordData, unsigned int* indices, size_t sizeofVertCubePosData, size_t sizeofIndices)
{
	for (size_t i = 0; i < sizeofVertCubePosData; i = i + 3)
	{
		Vertex vert;
		glm::vec3 pos;
		pos.x = vertCubePosData[i];
		pos.y = vertCubePosData[i + 1];
		pos.z = vertCubePosData[i + 2];

		glm::vec3 norm;
		norm.x = vertCubeNormData[i];
		norm.y = vertCubeNormData[i + 1];
		norm.z = vertCubeNormData[i + 2];

		glm::vec2 texCoord;
		texCoord.x = vertCubeTexCoordData[2 * (i / 3)];
		texCoord.y = vertCubeTexCoordData[2 * (i / 3) + 1];

		vert.Position = pos;
		vert.Normal = norm;
		vert.TexCoords = texCoord;

		rc.vertices.push_back(vert);
	}

	for (size_t i = 0; i < sizeofIndices; ++i)
	{
		rc.indices.push_back(indices[i]);
	}
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera->ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera->ProcessKeyboard(DOWN, deltaTime);


	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{

		//glfwGetCursorPos(window, &savedXpos, &savedYpos);
		savedXpos = currentX;
		savedYpos = currentY;
		trackMouseMovement = false;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		//glfwSetCursorPos(window, savedXpos, savedYpos);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		lastX = savedXpos;
		lastY = savedYpos;

		currentX = lastX;
		currentY = lastY;

		glfwSetCursorPos(window, currentX, currentY);

		trackMouseMovement = true;

	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		wireframe = true;
		
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		wireframe = false;

	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

	if (trackMouseMovement)
	{

		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		currentX = xpos;
		currentY = ypos;

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera->ProcessMouseMovement(xoffset, yoffset);
	}

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->ProcessMouseScroll(static_cast<float>(yoffset));
}