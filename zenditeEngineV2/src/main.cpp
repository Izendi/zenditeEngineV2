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
void addDataToLightRenderable(c_LightRenderable& rc, float* vertCubePosData, unsigned int* indices, size_t sizeofVertCubePosData, size_t sizeofIndices);

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
	std::shared_ptr<Shader> sh_shadows = std::make_shared<Shader>("res/shaders/Shadows/vs_multiLightShadowNoSpecular.glsl",
		"res/shaders/Shadows/fs_multiLightShadowNoSpecular.glsl");
	std::shared_ptr<Shader> sh_LightSource = std::make_shared<Shader>("res/shaders/LightingShaders/vs_LightSource.glsl",
		"res/shaders/LightingShaders/fs_LightSource.glsl");
		
	std::unique_ptr<I_SceneFactory> sceneFactory = std::make_unique<MinimalSceneFactory>(COORD);

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
		 0.0f, 1.0f,
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
	entities.push_back(COORD.CreateEntity());
	entities.push_back(COORD.CreateEntity());

	//Walls:
	entities.push_back(COORD.CreateEntity());
	entities.push_back(COORD.CreateEntity());
	entities.push_back(COORD.CreateEntity());
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
	unsigned short int grassTexUnit = COORD.GenerateTexUnit("res/textures/grass.jpg", "jpg");				 // tx Unit = 3
	unsigned short int lavaTexUnit = COORD.GenerateTexUnit("res/textures/lava.jpg", "jpg");					 // tx Unit = 4
	unsigned short int brickWallTexUnit = COORD.GenerateTexUnit("res/textures/wall.jpg", "jpg");			 // tx Unit = 5

	//unsigned short int heightMapTex = COORD.GenerateTexUnit("res/textures/heightmap.png", "PNG");

	c_Transform tr_0;
	c_Transform tr_1;
	c_Transform tr_2;
	c_Transform tr_3;
	c_Transform tr_4;
	c_Transform tr_5;
	c_Transform tr_6;

	c_Transform tr_7;
	c_Transform tr_8;
	c_Transform tr_9;
	c_Transform tr_10;

	c_Transform tr_11;
	c_Transform tr_12;
	c_Transform tr_13;
	c_Transform tr_14;

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
	glm::vec3 pos_tr3(camera->getPosition());
	glm::vec3 scale_tr3(0.2f, 0.2f, 0.2f);
	mm_tr3 = glm::translate(mm_tr3, pos_tr3);
	mm_tr3 = glm::scale(mm_tr3, scale_tr3);
	tr_3.modelMat.push_back(mm_tr3);

	//tr_4
	glm::mat4 mm_tr4 = glm::mat4(1.0f);
	glm::vec3 pos_tr4(-2.1f, 0.5f, -3.05f);
	glm::vec3 scale_tr4(0.2f, 0.2f, 0.2f);
	mm_tr4 = glm::translate(mm_tr4, pos_tr4);
	mm_tr4 = glm::scale(mm_tr4, scale_tr4);
	tr_4.modelMat.push_back(mm_tr4);

	//tr_5
	glm::mat4 mm_tr5 = glm::mat4(1.0f);
	glm::vec3 pos_tr5(-9.35f, -2.4f, -2.85f);
	glm::vec3 scale_tr5(0.2f, 0.2f, 0.2f);
	mm_tr5 = glm::translate(mm_tr5, pos_tr5);
	mm_tr5 = glm::scale(mm_tr5, scale_tr5);
	tr_5.modelMat.push_back(mm_tr5);

	//tr_6
	glm::mat4 mm_tr6 = glm::mat4(1.0f);
	glm::vec3 pos_tr6(5.0f, 5.0f, -2.0f);
	glm::vec3 scale_tr6(0.2f, 0.2f, 0.2f);
	mm_tr6 = glm::translate(mm_tr6, pos_tr6);
	mm_tr6 = glm::scale(mm_tr6, scale_tr6);
	tr_6.modelMat.push_back(mm_tr6);

	//tr_7
	glm::mat4 mm_tr7;
	setUpBasicModelMatrix(mm_tr7, glm::vec3(-2.7f, -2.3f, 8.5f), glm::vec3(4.0f, 1.2f, 0.4f));
	tr_7.modelMat.push_back(mm_tr7);

	//tr_8
	glm::mat4 mm_tr8;
	setUpBasicModelMatrix(mm_tr8, glm::vec3(-10.6f, -2.3f, -8.3f), glm::vec3(4.0f, 1.2f, 0.4f));
	tr_8.modelMat.push_back(mm_tr8);

	//tr_9
	glm::mat4 mm_tr9;
	setUpBasicModelMatrix(mm_tr9, glm::vec3(-15.0f, -2.3f, 4.1f), glm::vec3(0.4f, 1.2f, 4.0f));
	tr_9.modelMat.push_back(mm_tr9);

	//tr_10
	glm::mat4 mm_tr10;
	setUpBasicModelMatrix(mm_tr10, glm::vec3(1.65f, -2.3f, 4.1f), glm::vec3(0.4f, 1.2f, 4.0f));
	tr_10.modelMat.push_back(mm_tr10);



	//tr_11
	glm::mat4 mm_tr11;
	setUpBasicModelMatrix(mm_tr11, glm::vec3(-10.7f, -2.3f, 8.5f), glm::vec3(4.0f, 1.2f, 0.4f));
	tr_11.modelMat.push_back(mm_tr11);

	//tr_12
	glm::mat4 mm_tr12;
	setUpBasicModelMatrix(mm_tr12, glm::vec3(-2.6f, -2.3f, -8.3f), glm::vec3(4.0f, 1.2f, 0.4f));
	tr_12.modelMat.push_back(mm_tr12);

	//tr_13
	glm::mat4 mm_tr13;
	setUpBasicModelMatrix(mm_tr13, glm::vec3(-15.0f, -2.3f, -3.9f), glm::vec3(0.4f, 1.2f, 4.0f));
	tr_13.modelMat.push_back(mm_tr13);

	//tr_14
	glm::mat4 mm_tr14;
	setUpBasicModelMatrix(mm_tr14, glm::vec3(1.65f, -2.3f, -3.9f), glm::vec3(0.4f, 1.2f, 4.0f));
	tr_14.modelMat.push_back(mm_tr14);


	size_t sizeOfVertCubePosData = sizeof(vertCubePosData) / sizeof(float);
	size_t sizeOfIndices = sizeof(indices) / sizeof(unsigned int);

	c_Renderable rc_0;
	addDataToRenderable(rc_0, vertCubePosData, vertCubeNormData, vertCubeTexCoordData, indices, sizeOfVertCubePosData, sizeOfIndices);

	std::cout << "\nsize of vertCubePosData = " << sizeOfVertCubePosData << std::endl;
	std::cout << "\nsize of Indices         = " << sizeOfIndices << std::endl;

	c_Renderable rc_3;
	addDataToRenderable(rc_3, vertCubePosData, vertCubeNormData, vertCubeTexCoordData, indices, sizeOfVertCubePosData, sizeOfIndices);


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
	tx_6.texUnit = brickWallTexUnit;


	//c_Texture tex_3;
	//tex_3.setTexCoordsVertexArray(oddShapedTexCoordData, sizeof(oddShapedTexCoordData));
	//tex_3.texUnit = heightMapTex;

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

	c_Modified md_5;
	md_5.isModifed = true;

	c_Modified md_6;
	md_6.isModifed = true;

	c_Modified md_7;
	md_7.isModifed = true;

	c_Modified md_8;
	md_8.isModifed = true;

	c_Modified md_9;
	md_9.isModifed = true;

	c_Modified md_10;
	md_10.isModifed = true;

	c_Modified md_11;
	md_11.isModifed = true;

	c_Modified md_12;
	md_12.isModifed = true;

	c_Modified md_13;
	md_13.isModifed = true;

	c_Modified md_14;
	md_14.isModifed = true;

	c_AABB aabb_0;
	aabb_0.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	aabb_0.vertices = AABBvertices;

	c_AABB aabb_2;
	aabb_2.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	aabb_2.vertices = AABBvertices;



	c_AABB aabb_7;
	aabb_7.scale = glm::vec3(4.0f, 1.2f, 0.4f);
	aabb_7.vertices = AABBvertices;
	aabb_7.active = true;

	c_AABB aabb_8;
	aabb_8.scale = glm::vec3(4.0f, 1.2f, 0.4f);
	aabb_8.vertices = AABBvertices;
	aabb_8.active = true;

	c_AABB aabb_9;
	aabb_9.scale = glm::vec3(0.4f, 1.2f, 4.0f);
	aabb_9.vertices = AABBvertices;
	aabb_9.active = true;

	c_AABB aabb_10;
	aabb_10.scale = glm::vec3(0.4f, 1.2f, 4.0f);
	aabb_10.vertices = AABBvertices;
	aabb_10.active = true;

	c_AABB aabb_11;
	aabb_11.scale = glm::vec3(4.0f, 1.2f, 0.4f);
	aabb_11.vertices = AABBvertices;
	aabb_11.active = true;

	c_AABB aabb_12;
	aabb_12.scale = glm::vec3(4.0f, 1.2f, 0.4f);
	aabb_12.vertices = AABBvertices;
	aabb_12.active = true;
	
	c_AABB aabb_13;
	aabb_13.scale = glm::vec3(0.4f, 1.2f, 4.0f);
	aabb_13.vertices = AABBvertices;
	aabb_13.active = true;

	c_AABB aabb_14;
	aabb_14.scale = glm::vec3(0.4f, 1.2f, 4.0f);
	aabb_14.vertices = AABBvertices;
	aabb_14.active = true;


	c_Wall wall_0;
	c_WallCollider wallCollider_2;

	c_SpotLightEmitter sle_3;
	sle_3.ambient = glm::vec3(0.0f);
	sle_3.diffuse = glm::vec3(0.8f);
	sle_3.specular = glm::vec3(1.0f);
	sle_3.direction = camera->Front;
	sle_3.cutOff = glm::cos(glm::radians(10.5f));
	sle_3.outerCutOff = glm::cos(glm::radians(15.0f));
	sle_3.constant = 0.3f;
	sle_3.linear = 0.02f;
	sle_3.quadratic = 0.0019f;
	sle_3.active = true;

	c_PointLightEmitter ple_4;
	ple_4.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	ple_4.diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
	ple_4.specular = glm::vec3(0.5f, 0.0f, 0.0f);
	ple_4.constant = 0.5f;
	ple_4.linear = 0.09f;
	ple_4.quadratic = 0.032f;
	ple_4.active = true;

	c_PointLightEmitter ple_5;
	ple_5.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	ple_5.diffuse = glm::vec3(0.0f, 0.0f, 1.0f);
	ple_5.specular = glm::vec3(0.0f, 0.0f, 0.5f);
	ple_5.constant = 0.2f;
	ple_5.linear = 0.09f;
	ple_5.quadratic = 0.032f;
	ple_5.active = true;

	c_DirLightEmitter dle_6;

	glm::vec3 tr_6_pos;
	tr_6_pos.x = tr_6.modelMat[0][3][0];
	tr_6_pos.y = tr_6.modelMat[0][3][1];
	tr_6_pos.z = tr_6.modelMat[0][3][2];

	dle_6.direction = -tr_6_pos;
	dle_6.ambient = glm::vec3(0.4f, 0.4f, 0.4f);
	dle_6.diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
	dle_6.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	dle_6.active = false;

	c_EntityInfo ei_0;
	ei_0.name = "Wall Col Cube";

	c_EntityInfo ei_1;
	ei_1.name = "Floor";

	c_EntityInfo ei_2;
	ei_2.name = "Wall cube";

	c_EntityInfo ei_3;
	ei_3.name = "Camera flash light";

	c_EntityInfo ei_4;
	ei_4.name = "Red point light";

	c_EntityInfo ei_5;
	ei_5.name = "Blue point light";

	c_EntityInfo ei_6;
	ei_6.name = "Directional light";

	c_EntityInfo ei_7;
	ei_7.name = "Wall_1";

	c_EntityInfo ei_8;
	ei_8.name = "Wall_2";

	c_EntityInfo ei_9;
	ei_9.name = "Wall_3";

	c_EntityInfo ei_10;
	ei_10.name = "Wall_4";

	c_EntityInfo ei_11;
	ei_11.name = "Wall_5";

	c_EntityInfo ei_12;
	ei_12.name = "Wall_6";

	c_EntityInfo ei_13;
	ei_13.name = "Wall_7";

	c_EntityInfo ei_14;
	ei_14.name = "Wall_8";

	glm::mat4 ES0_mm = glm::mat4(1.0f);
	glm::vec3 ES0_pos(-3.0f, -2.8f, 7.7f);
	glm::vec3 ES0_scale(0.3f, 0.3f, 0.3f);
	ES0_mm = glm::translate(ES0_mm, ES0_pos);
	float angleY = glm::radians(180.0f);
	float angleX = glm::radians(-13.0f);
	ES0_mm = glm::rotate(ES0_mm, angleY, glm::vec3(0, 1, 0));
	ES0_mm = glm::rotate(ES0_mm, angleX, glm::vec3(1, 0, 0));
	ES0_mm = glm::scale(ES0_mm, ES0_scale);

	map_SceneNameToEntitiyScene["Backpack_1"] = sceneFactory->CreateEntityScene("res/models/backpack/", "backpack.obj", ES0_mm, sh_shadows, 1);
	//std::shared_ptr<EntityScene> ES_0 = sceneFactory->CreateEntityScene("res/models/backpack/", "backpack.obj", ES0_mm, sh_shadows, 1);
	map_SceneEntites["Backpack_1"] = map_SceneNameToEntitiyScene["Backpack_1"]->GetSceneEntities();
	

	glm::mat4 ES1_mm = glm::mat4(1.0f);
	glm::vec3 ES1_pos(-6.0f, -2.3f, 3.0f);
	glm::vec3 ES1_scale(0.3f, 0.3f, 0.3f);
	ES1_mm = glm::translate(ES1_mm, ES1_pos);
	float angleYRadians = glm::radians(180.0f);
	ES1_mm = glm::rotate(ES1_mm, angleYRadians, glm::vec3(0, 1, 0)); //  Y
	ES1_mm = glm::scale(ES1_mm, ES1_scale);

	map_SceneNameToEntitiyScene["House_1"] = sceneFactory->CreateEntityScene("res/models/House/", "House.obj", ES1_mm, sh_shadows, 1);
	map_SceneEntites["House_1"] = map_SceneNameToEntitiyScene["House_1"]->GetSceneEntities();

	//C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/models/OakTree/OakTree.obj

	glm::mat4 ES2_mm = glm::mat4(1.0f);
	glm::vec3 ES2_pos(-8.6f, -3.1f, -4.0f);
	glm::vec3 ES2_scale(0.3f, 0.3f, 0.3f);
	ES2_mm = glm::translate(ES2_mm, ES2_pos);
	ES2_mm = glm::scale(ES2_mm, ES2_scale);

	map_SceneNameToEntitiyScene["OakTree_1"] = sceneFactory->CreateEntityScene("res/models/OakTree/", "OakTree.obj", ES2_mm, sh_shadows, 1);
	map_SceneEntites["OakTree_1"] = map_SceneNameToEntitiyScene["OakTree_1"]->GetSceneEntities();

	c_LightRenderable lr_3;
	c_LightRenderable lr_4;
	c_LightRenderable lr_5;
	c_LightRenderable lr_6;

	addDataToLightRenderable(lr_3, vertCubePosData, indices, sizeof(vertCubePosData) / sizeof(float), sizeof(indices) / sizeof(unsigned int));
	lr_3.active = false; //This light source is on the camera, so whole screen will be covered by a white box if this is set to true.

	addDataToLightRenderable(lr_4, vertCubePosData, indices, sizeof(vertCubePosData) / sizeof(float), sizeof(indices) / sizeof(unsigned int));
	lr_4.active = true;

	addDataToLightRenderable(lr_5, vertCubePosData, indices, sizeof(vertCubePosData) / sizeof(float), sizeof(indices) / sizeof(unsigned int));
	lr_5.active = true;

	addDataToLightRenderable(lr_6, vertCubePosData, indices, sizeof(vertCubePosData) / sizeof(float), sizeof(indices) / sizeof(unsigned int));
	lr_6.active = true;


	COORD.AddComponentToEntity<c_Transform>(entities[0], tr_0);
	COORD.AddComponentToEntity<c_Renderable>(entities[0], rc_0);
	COORD.AddComponentToEntity<c_Texture>(entities[0], tx_0);
	COORD.AddComponentToEntity<c_AABB>(entities[0], aabb_0);
	COORD.AddComponentToEntity<c_WallCollider>(entities[0], wallCollider_2);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[0], ei_0);
	COORD.AddComponentToEntity<c_Modified>(entities[0], md_0);
	COORD.SetUpRenderData(entities[0]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
	COORD.setShaderForEntity(entities[0], sh_shadows); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
	COORD.StoreShaderInEntityDataHandle(entities[0]);

	COORD.AddComponentToEntity<c_Transform>(entities[1], tr_1);
	COORD.AddComponentToEntity<c_Renderable>(entities[1], rc_0);
	COORD.AddComponentToEntity<c_Texture>(entities[1], tx_1);
	//COORD.AddComponentToEntity<c_AABB>(entities[1], aabb_0);
	//COORD.AddComponentToEntity<c_WallCollider>(entities[1], wallCollider_2);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[1], ei_1);
	COORD.AddComponentToEntity<c_Modified>(entities[1], md_1);
	COORD.SetUpRenderData(entities[1]);
	COORD.setShaderForEntity(entities[1], sh_shadows);
	COORD.StoreShaderInEntityDataHandle(entities[1]);

	COORD.AddComponentToEntity<c_Transform>(entities[2], tr_2);
	COORD.AddComponentToEntity<c_Renderable>(entities[2], rc_0);
	COORD.AddComponentToEntity<c_Texture>(entities[2], tx_2);
	COORD.AddComponentToEntity<c_AABB>(entities[2], aabb_2);
	COORD.AddComponentToEntity<c_Wall>(entities[2], wall_0);
	COORD.AddComponentToEntity<c_Modified>(entities[2], md_2);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[2], ei_2);
	COORD.SetUpRenderData(entities[2]);
	COORD.setShaderForEntity(entities[2], sh_shadows);
	COORD.StoreShaderInEntityDataHandle(entities[2]);

	COORD.AddComponentToEntity<c_Transform>(entities[3], tr_3);
	COORD.AddComponentToEntity<c_Modified>(entities[3], md_3);
	COORD.AddComponentToEntity<c_SpotLightEmitter>(entities[3], sle_3);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[3], ei_3);
	COORD.GenerateShadowMapForEntity(entities[3]);
	COORD.AddComponentToEntity<c_LightRenderable>(entities[3], lr_3);

	COORD.AddComponentToEntity<c_Transform>(entities[4], tr_4);
	COORD.AddComponentToEntity<c_Modified>(entities[4], md_4);
	COORD.AddComponentToEntity<c_PointLightEmitter>(entities[4], ple_4);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[4], ei_4);
	COORD.AddComponentToEntity<c_LightRenderable>(entities[4], lr_4);
	
	COORD.AddComponentToEntity<c_Transform>(entities[5], tr_5);
	COORD.AddComponentToEntity<c_Modified>(entities[5], md_5);
	COORD.AddComponentToEntity<c_PointLightEmitter>(entities[5], ple_5);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[5], ei_5);
	COORD.AddComponentToEntity<c_LightRenderable>(entities[5], lr_5);

	COORD.AddComponentToEntity<c_Transform>(entities[6], tr_6);
	COORD.AddComponentToEntity<c_Modified>(entities[6], md_6);
	COORD.AddComponentToEntity<c_DirLightEmitter>(entities[6], dle_6);
	COORD.AddComponentToEntity<c_LightRenderable>(entities[6], lr_6);
	COORD.GenerateShadowMapForEntity(entities[6]);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[6], ei_6);

	//e 7
	COORD.AddComponentToEntity<c_Transform>(entities[7], tr_7);
	COORD.AddComponentToEntity<c_Renderable>(entities[7], rc_0);
	COORD.AddComponentToEntity<c_AABB>(entities[7], aabb_7);
	COORD.AddComponentToEntity<c_Wall>(entities[7], wall_0);
	COORD.AddComponentToEntity<c_Texture>(entities[7], tx_6);
	COORD.AddComponentToEntity<c_Modified>(entities[7], md_7);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[7], ei_7);
	COORD.SetUpRenderData(entities[7]);
	COORD.setShaderForEntity(entities[7], sh_shadows);
	COORD.StoreShaderInEntityDataHandle(entities[7]);

	//e 8
	COORD.AddComponentToEntity<c_Transform>(entities[8], tr_8);
	COORD.AddComponentToEntity<c_Renderable>(entities[8], rc_0);
	COORD.AddComponentToEntity<c_AABB>(entities[8], aabb_8);
	COORD.AddComponentToEntity<c_Wall>(entities[8], wall_0);
	COORD.AddComponentToEntity<c_Texture>(entities[8], tx_6);
	COORD.AddComponentToEntity<c_Modified>(entities[8], md_8);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[8], ei_8);
	COORD.SetUpRenderData(entities[8]);
	COORD.setShaderForEntity(entities[8], sh_shadows);
	COORD.StoreShaderInEntityDataHandle(entities[8]);

	//e 9
	COORD.AddComponentToEntity<c_Transform>(entities[9], tr_9);
	COORD.AddComponentToEntity<c_Renderable>(entities[9], rc_0);
	COORD.AddComponentToEntity<c_AABB>(entities[9], aabb_9);
	COORD.AddComponentToEntity<c_Wall>(entities[9], wall_0);
	COORD.AddComponentToEntity<c_Texture>(entities[9], tx_6);
	COORD.AddComponentToEntity<c_Modified>(entities[9], md_9);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[9], ei_9);
	COORD.SetUpRenderData(entities[9]);
	COORD.setShaderForEntity(entities[9], sh_shadows);
	COORD.StoreShaderInEntityDataHandle(entities[9]);

	//e 10
	COORD.AddComponentToEntity<c_Transform>(entities[10], tr_10);
	COORD.AddComponentToEntity<c_Renderable>(entities[10], rc_0);
	COORD.AddComponentToEntity<c_AABB>(entities[10], aabb_10);
	COORD.AddComponentToEntity<c_Wall>(entities[10], wall_0);
	COORD.AddComponentToEntity<c_Texture>(entities[10], tx_6);
	COORD.AddComponentToEntity<c_Modified>(entities[10], md_10);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[10], ei_10);
	COORD.SetUpRenderData(entities[10]);
	COORD.setShaderForEntity(entities[10], sh_shadows);
	COORD.StoreShaderInEntityDataHandle(entities[10]);

	//e 11
	COORD.AddComponentToEntity<c_Transform>(entities[11], tr_11);
	COORD.AddComponentToEntity<c_Renderable>(entities[11], rc_0);
	COORD.AddComponentToEntity<c_AABB>(entities[11], aabb_11);
	COORD.AddComponentToEntity<c_Wall>(entities[11], wall_0);
	COORD.AddComponentToEntity<c_Texture>(entities[11], tx_6);
	COORD.AddComponentToEntity<c_Modified>(entities[11], md_11);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[11], ei_11);
	COORD.SetUpRenderData(entities[11]);
	COORD.setShaderForEntity(entities[11], sh_shadows);
	COORD.StoreShaderInEntityDataHandle(entities[11]);

	//e 12
	COORD.AddComponentToEntity<c_Transform>(entities[12], tr_12);
	COORD.AddComponentToEntity<c_Renderable>(entities[12], rc_0);
	COORD.AddComponentToEntity<c_AABB>(entities[12], aabb_12);
	COORD.AddComponentToEntity<c_Wall>(entities[12], wall_0);
	COORD.AddComponentToEntity<c_Texture>(entities[12], tx_6);
	COORD.AddComponentToEntity<c_Modified>(entities[12], md_12);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[12], ei_12);
	COORD.SetUpRenderData(entities[12]);
	COORD.setShaderForEntity(entities[12], sh_shadows);
	COORD.StoreShaderInEntityDataHandle(entities[12]);

	//e 13
	COORD.AddComponentToEntity<c_Transform>(entities[13], tr_13);
	COORD.AddComponentToEntity<c_Renderable>(entities[13], rc_0);
	COORD.AddComponentToEntity<c_AABB>(entities[13], aabb_13);
	COORD.AddComponentToEntity<c_Wall>(entities[13], wall_0);
	COORD.AddComponentToEntity<c_Texture>(entities[13], tx_6);
	COORD.AddComponentToEntity<c_Modified>(entities[13], md_13);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[13], ei_13);
	COORD.SetUpRenderData(entities[13]);
	COORD.setShaderForEntity(entities[13], sh_shadows);
	COORD.StoreShaderInEntityDataHandle(entities[13]);

	//e 14
	COORD.AddComponentToEntity<c_Transform>(entities[14], tr_14);
	COORD.AddComponentToEntity<c_Renderable>(entities[14], rc_0);
	COORD.AddComponentToEntity<c_AABB>(entities[14], aabb_14);
	COORD.AddComponentToEntity<c_Wall>(entities[14], wall_0);
	COORD.AddComponentToEntity<c_Texture>(entities[14], tx_6);
	COORD.AddComponentToEntity<c_Modified>(entities[14], md_14);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[14], ei_14);
	COORD.SetUpRenderData(entities[14]);
	COORD.setShaderForEntity(entities[14], sh_shadows);
	COORD.StoreShaderInEntityDataHandle(entities[14]);

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

	while (!glfwWindowShouldClose(window))
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

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

		//moveEntityBackAndFourth(COORD.GetComponentDataFromEntity<c_Transform>(entities[0]), deltaTime);

		c_Transform& flashLightMM = COORD.GetComponentDataFromEntity<c_Transform>(entities[3]);
		flashLightMM.modelMat[0][3][0] = camera->getPosition().x;
		flashLightMM.modelMat[0][3][1] = camera->getPosition().y - 0.5;
		flashLightMM.modelMat[0][3][2] = camera->getPosition().z;
		c_SpotLightEmitter& flashLightData = COORD.GetComponentDataFromEntity<c_SpotLightEmitter>(entities[3]);
		flashLightData.direction = camera->Front;

		COORD.runAllSystems(2.0f, allEntites); //#ECS_RENDERING

		genMenu_1(allEntites,
			entities,
			map_SceneEntites,
			map_SceneNameToEntitiyScene,
			COORD,
			containerTexUnit,
			rockySurfaceTexUnit,
			waterTexUnit,
			grassTexUnit,
			lavaTexUnit,
			brickWallTexUnit
		);

		//#Removed_1: 206 - 314

		if (wireframe) {
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//posData.pos.x = posData.pos.x + 0.2f;
			//texData.texUnit = tx_2.texUnit;
			//COORD.GetComponentDataFromEntity<c_Modified>(entities[0]).isModifed = true;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}


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

void addDataToLightRenderable(c_LightRenderable& rc, float* vertCubePosData, unsigned int* indices, size_t sizeofVertCubePosData, size_t sizeofIndices)
{
	for (size_t i = 0; i < sizeofVertCubePosData; i = i + 3)
	{
		LightweightVertex vert;
		glm::vec3 pos;
		pos.x = vertCubePosData[i];
		pos.y = vertCubePosData[i + 1];
		pos.z = vertCubePosData[i + 2];

		vert.Position = pos;

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