#include "utils.h"
#include <GLFW/glfw3.h>
#include <chrono>

#include "vendor/stb_image/stb_image.h"
#include "assimp/Importer.hpp"

#include "Shader.h"
#include "Texture2D.h"
#include "geometrySetup.h"
#include "menu.h"
#include "Camera.h"

#include "Helper/Model.h"

#include <filesystem>

#include "Coordinator.h"
#include "ECS/Components.h"

//ECS implementation ver 3.0
const unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;

namespace fs = std::filesystem;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void moveEntityBackAndFourth(c_Transform& entTrans, float DT);

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

//Coordinator:

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


	//#TODO Need to pass data read in from the model loader to the ECS system for rendering.
	float vertexDataValues[] = {
		// positions          
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,

		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		-0.5f,  0.5f, -0.5f
	};

	float surfaceNormalValues[] =
	{
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,

		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,

		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,

		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,

		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,

		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f
	};

	float textureCoords[] =
	{
		 0.0f,  0.0f,
		 1.0f,  0.0f,
		 1.0f,  1.0f,
		 1.0f,  1.0f,
		 0.0f,  1.0f,
		 0.0f,  0.0f,
		 0.0f,  0.0f,
		 1.0f,  0.0f,
		 1.0f,  1.0f,
		 1.0f,  1.0f,
		 0.0f,  1.0f,
		 0.0f,  0.0f,
		 1.0f,  0.0f,
		 1.0f,  1.0f,
		 0.0f,  1.0f,
		 0.0f,  1.0f,
		 0.0f,  0.0f,
		 1.0f,  0.0f,
		 1.0f,  0.0f,
		 1.0f,  1.0f,
		 0.0f,  1.0f,
		 0.0f,  1.0f,
		 0.0f,  0.0f,
		 1.0f,  0.0f,
		 0.0f,  1.0f,
		 1.0f,  1.0f,
		 1.0f,  0.0f,
		 1.0f,  0.0f,
		 0.0f,  0.0f,
		 0.0f,  1.0f,
		 0.0f,  1.0f,
		 1.0f,  1.0f,
		 1.0f,  0.0f,
		 1.0f,  0.0f,
		 0.0f,  0.0f,
		 0.0f,  1.0f
	};

	float oddShapeVertexData[] = {
		// positions          // normals           // texture coords
		-0.5f, -1.5f, -0.5f,
		 0.5f, -1.5f, -0.5f,
		 0.5f,  1.5f, -0.5f,
		 0.5f,  1.5f, -0.5f,
		-0.5f,  1.5f, -0.5f,
		-0.5f, -1.5f, -0.5f,

		-0.5f, -1.5f,  0.5f,
		 0.5f, -1.5f,  0.5f,
		 0.5f,  1.5f,  0.5f,
		 0.5f,  1.5f,  0.5f,
		-0.5f,  1.5f,  0.5f,
		-0.5f, -1.5f,  0.5f,

		-0.5f,  1.5f,  0.5f,
		-0.5f,  1.5f, -0.5f,
		-0.5f, -1.5f, -0.5f,
		-0.5f, -1.5f, -0.5f,
		-0.5f, -1.5f,  0.5f,
		-0.5f,  1.5f,  0.5f,

		 0.5f,  1.5f,  0.5f,
		 0.5f,  1.5f, -0.5f,
		 0.5f, -1.5f, -0.5f,
		 0.5f, -1.5f, -0.5f,
		 0.5f, -1.5f,  0.5f,
		 0.5f,  1.5f,  0.5f,

		-0.5f, -1.5f, -0.5f,
		 0.5f, -1.5f, -0.5f,
		 0.5f, -1.5f,  0.5f,
		 0.5f, -1.5f,  0.5f,
		-0.5f, -1.5f,  0.5f,
		-0.5f, -1.5f, -0.5f,

		-0.5f,  1.5f, -0.5f,
		 0.5f,  1.5f, -0.5f,
		 0.5f,  1.5f,  0.5f,
		 0.5f,  1.5f,  0.5f,
		-0.5f,  1.5f,  0.5f,
		-0.5f,  1.5f, -0.5f
	};

	float oddShapedVDataNormals[] =
	{
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,

		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,

		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,

		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,

		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,

		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f
	};

	float oddShapedTexCoordData[] =
	{
		0.0f,  0.0f,
		1.0f,  0.0f,
		1.0f,  1.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  0.0f,

		0.0f,  0.0f,
		1.0f,  0.0f,
		1.0f,  1.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  0.0f,

		1.0f,  0.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  0.0f,
		1.0f,  0.0f,

		1.0f,  0.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  0.0f,
		1.0f,  0.0f,

		0.0f,  1.0f,
		1.0f,  1.0f,
		1.0f,  0.0f,
		1.0f,  0.0f,
		0.0f,  0.0f,
		0.0f,  1.0f,

		0.0f,  1.0f,
		1.0f,  1.0f,
		1.0f,  0.0f,
		1.0f,  0.0f,
		0.0f,  0.0f,
		0.0f,  1.0f
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

	entities.push_back(COORD.CreateEntity());
	entities.push_back(COORD.CreateEntity());
	entities.push_back(COORD.CreateEntity());
	entities.push_back(COORD.CreateEntity());

	unsigned short int containerTexUnit = COORD.GenerateTexUnit("res/textures/container2.png", "PNG");
	unsigned short int rockySurfaceTexUnit = COORD.GenerateTexUnit("res/textures/rockySurface.png", "PNG");
	unsigned short int waterTexUnit = COORD.GenerateTexUnit("res/textures/water.jpg", "JPG");
	unsigned short int grassTexUnit = COORD.GenerateTexUnit("res/textures/grass.jpg", "JPG");
	unsigned short int lavaTexUnit = COORD.GenerateTexUnit("res/textures/lava.jpg", "JPG");



	//unsigned short int heightMapTex = COORD.GenerateTexUnit("res/textures/heightmap.png", "PNG");



	c_Transform tr_0;
	c_Transform tr_1;
	c_Transform tr_2;
	c_Transform tr_3;
	tr_0.pos = glm::vec3(0.0f, 0.0f, 0.0f);
	tr_0.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	tr_1.pos = glm::vec3(-2.0f, 0.0f, 3.0f);
	tr_1.scale = glm::vec3(0.5f, 0.5f, 0.5f);
	tr_2.pos = glm::vec3(-0.2f, 0.0f, -4.5f);
	tr_2.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	tr_3.pos = glm::vec3(0.0f, 0.0f, 0.0f);
	tr_3.scale = glm::vec3(1.0f, 1.0f, 1.0f);

	c_RenderableComponent rc_0;
	rc_0.setPosVertexArray(vertexDataValues, sizeof(vertexDataValues));
	rc_0.setSurfaceNormalVertexArray(surfaceNormalValues, sizeof(surfaceNormalValues));
	//rc_0.vertices = vertexDataValues;

	c_RenderableComponent rc_1;
	rc_1.setPosVertexArray(oddShapeVertexData, sizeof(oddShapeVertexData));
	rc_0.setSurfaceNormalVertexArray(oddShapedVDataNormals, sizeof(oddShapedVDataNormals));

	c_RenderableComponent rc_3;
	rc_3.setPosVertexArray(vertexDataValues, sizeof(vertexDataValues));
	rc_0.setSurfaceNormalVertexArray(surfaceNormalValues, sizeof(surfaceNormalValues));

	c_Texture tx_0;
	tx_0.setTexCoordsVertexArray(textureCoords, sizeof(textureCoords));
	tx_0.texUnit = containerTexUnit;

	c_Texture tx_1;
	tx_1.setTexCoordsVertexArray(oddShapedTexCoordData, sizeof(oddShapedTexCoordData));
	tx_1.texUnit = containerTexUnit;

	c_Texture tx_2;
	tx_2.setTexCoordsVertexArray(oddShapedTexCoordData, sizeof(oddShapedTexCoordData));
	tx_2.texUnit = rockySurfaceTexUnit;


	c_Texture tx_3;
	tx_3.setTexCoordsVertexArray(oddShapedTexCoordData, sizeof(oddShapedTexCoordData));
	tx_3.texUnit = rockySurfaceTexUnit;

	c_Texture tx_4;
	tx_4.setTexCoordsVertexArray(oddShapedTexCoordData, sizeof(oddShapedTexCoordData));
	tx_4.texUnit = grassTexUnit;

	c_Texture tx_5;
	tx_5.setTexCoordsVertexArray(oddShapedTexCoordData, sizeof(oddShapedTexCoordData));
	tx_5.texUnit = grassTexUnit;

	c_Texture tx_6;
	tx_6.setTexCoordsVertexArray(oddShapedTexCoordData, sizeof(oddShapedTexCoordData));
	tx_6.texUnit = waterTexUnit;

	c_Texture tx_7;
	tx_7.setTexCoordsVertexArray(oddShapedTexCoordData, sizeof(oddShapedTexCoordData));
	tx_7.texUnit = waterTexUnit;

	c_Texture tx_8;
	tx_8.setTexCoordsVertexArray(oddShapedTexCoordData, sizeof(oddShapedTexCoordData));
	tx_8.texUnit = lavaTexUnit;

	c_Texture tx_9;
	tx_9.setTexCoordsVertexArray(oddShapedTexCoordData, sizeof(oddShapedTexCoordData));
	tx_9.texUnit = lavaTexUnit;






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

	c_AABB aabb_0;
	aabb_0.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	aabb_0.vertices = AABBvertices;


	c_AABB aabb_2;
	aabb_2.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	aabb_2.vertices = AABBvertices;

	c_Wall wall_0;
	c_WallCollider wallCollider_2;


	c_AABB aabb_3;
	c_Wall wall_3;


	c_SpotLightEmitter spl_3;

	c_EntityInfo ei_0;
	ei_0.name = "Cube 1";

	c_EntityInfo ei_1;
	ei_1.name = "Cube 2";

	c_EntityInfo ei_2;
	ei_2.name = "Object 1";

	c_EntityInfo ei_3;
	ei_3.name = "Object 2";


	COORD.AddComponentToEntity<c_Transform>(entities[0], tr_0);
	COORD.AddComponentToEntity<c_RenderableComponent>(entities[0], rc_0);
	COORD.AddComponentToEntity<c_Texture>(entities[0], tx_0);
	COORD.AddComponentToEntity<c_AABB>(entities[0], aabb_0);
	COORD.AddComponentToEntity<c_WallCollider>(entities[0], wallCollider_2);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[0], ei_0);
	COORD.AddComponentToEntity<c_Modified>(entities[0], md_0);
	COORD.SetUpRenderData(entities[0]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
	COORD.setShaderForEntity(entities[0], sh_basicWithTex); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
	COORD.StoreShaderInEntityDataHandle(entities[0]);

	COORD.AddComponentToEntity<c_Transform>(entities[1], tr_1);
	COORD.AddComponentToEntity<c_RenderableComponent>(entities[1], rc_1);
	COORD.AddComponentToEntity<c_Texture>(entities[1], tx_1);
	COORD.AddComponentToEntity<c_Modified>(entities[1], md_1);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[1], ei_1);
	COORD.SetUpRenderData(entities[1]);
	COORD.setShaderForEntity(entities[1], sh_basicWithTex);
	COORD.StoreShaderInEntityDataHandle(entities[1]);

	COORD.AddComponentToEntity<c_Transform>(entities[2], tr_2);
	COORD.AddComponentToEntity<c_RenderableComponent>(entities[2], rc_0);
	COORD.AddComponentToEntity<c_Texture>(entities[2], tx_2);
	COORD.AddComponentToEntity<c_AABB>(entities[2], aabb_2);
	COORD.AddComponentToEntity<c_Wall>(entities[2], wall_0);
	COORD.AddComponentToEntity<c_Modified>(entities[2], md_2);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[2], ei_2);
	COORD.SetUpRenderData(entities[2]);
	COORD.setShaderForEntity(entities[2], sh_basicWithTex);
	COORD.StoreShaderInEntityDataHandle(entities[2]);

	COORD.AddComponentToEntity<c_Transform>(entities[3], tr_3);
	COORD.AddComponentToEntity<c_RenderableComponent>(entities[3], rc_3);
	COORD.AddComponentToEntity<c_Texture>(entities[3], tx_3);
	COORD.AddComponentToEntity<c_Modified>(entities[3], md_3);
	COORD.AddComponentToEntity<c_EntityInfo>(entities[3], ei_3);
	COORD.SetUpRenderData(entities[3]);
	COORD.setShaderForEntity(entities[3], sh_basicWithTex);
	COORD.StoreShaderInEntityDataHandle(entities[3]);

	//std::cout << "\nc_AABB bitset position: " << static_cast<unsigned int>(COORD.GetComponentBitsetPos<c_AABB>());
	//std::cout << "\nentities[2] bitset: " << COORD.GetEntitySignature(entities[2]) << std::endl;




	std::cout << "\nImGui Version: " << IMGUI_VERSION << std::endl;

	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);

	std::cout << "GLFW Version: " << major << "." << minor << "." << revision << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/* Render here */
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		moveEntityBackAndFourth(COORD.GetComponentDataFromEntity<c_Transform>(entities[0]), deltaTime);

		COORD.runAllSystems(2.0f, &entities); //#ECS_RENDERING

		genMenu_1(entities,
			COORD,
			containerTexUnit,
			rockySurfaceTexUnit,
			waterTexUnit,
			grassTexUnit,
			lavaTexUnit

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

void moveEntityBackAndFourth(c_Transform& entTrans, float DT)
{
	if (entTrans.pos.z > (-4.0f))
	{
		entTrans.pos.z = entTrans.pos.z + (DT * (-0.4f));
	}
	else
	{
		entTrans.pos.z = entTrans.pos.z + (DT * 0.4f);
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