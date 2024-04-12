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

//Test comment

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
	
	float QuadPos[] = {
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f
	};

	float QuadNorm[] = {
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f
	};

	float QuadTexCoord[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	unsigned int QuadIndices[] = {
		0, 1, 2,
		2, 3, 0
	};
	
	float ScreenQuadVertData[] = {
		//Pos				  //Tex Coords
		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,  0.0f, 1.0f,

		-1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 1.0f
	};
	
	Coordinator COORD("opengl", "opengl", camera); //std::string API_Type, std::string Render_Type, std::shared_ptr<Camera> camera
	COORD.RegisterComponents();
	COORD.RegisterSystems();
	COORD.SetUpSystemBitsets();

	std::cout << "\nRenderableSystem bitset: " << COORD.GetSystemBitset<RenderableSystem>() << std::endl;
	//std::cout << "\Rigid_CollisionDetectionSystem bitset: " << COORD.GetSystemBitset<Rigid_CollisionDetectionSystem>() << std::endl;

	std::shared_ptr<Shader> sh_fboShader = std::make_shared<Shader>("res/shaders/fbo/vs_BasicFbo.glsl",
		"res/shaders/fbo/fs_BasicFbo.glsl");

	std::shared_ptr<Shader> sh_fboKernalEffect = std::make_shared<Shader>("res/shaders/vs_KernalEffect.glsl",
		"res/shaders/fbo/fs_KernalEffect.glsl");

	std::shared_ptr<Shader> sh_CubeMap = std::make_shared<Shader>("res/shaders/cubeMaps/vs_basicCM.glsl",
		"res/shaders/cubeMaps/fs_basicCM.glsl");

	std::shared_ptr<Shader> sh_basicWithTex = std::make_shared<Shader>("res/shaders/BasicShaders/vs_cubeWnormANDtex.glsl",
		"res/shaders/BasicShaders/fs_cubeWnormANDtex.glsl"); //#Shaders have not yet been abstracted into the API_Manger
		
	std::unique_ptr<I_SceneFactory> sceneFactory = std::make_unique<MinimalSceneFactory>(COORD);

	std::vector<std::shared_ptr<Shader>> shaders;
	shaders.push_back(sh_basicWithTex);

	std::vector<Entity> entities;
	std::vector<Entity> allEntites;
	std::vector<unsigned short int> allTexUnits;
	std::unordered_map<std::string, std::shared_ptr<EntityScene>> map_SceneNameToEntitiyScene;
	std::unordered_map<std::string, std::vector<Entity>> map_SceneEntites;

	util::setupSceneECS(
		COORD,
		shaders,
		entities,
		allEntites,
		allTexUnits
	);

		
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

	//Cube Map Start - - - - - - - - - - - - - - - - - - - - - - -

	std::vector<std::string> cm_faces; //Contains the file path to the faces:
	cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/right.jpg");
	cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/left.jpg");
	cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/top.jpg");
	cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/bottom.jpg");
	cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/front.jpg");
	cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/back.jpg");

	//A cube map is just a texture, as such it is created using a texture ID handle:
	unsigned short int cubeMapTexUnit = COORD.GenerateTexUnit("res/textures/awesomeface.png", "png"); // even though I pass in awesome face here, the calls to glBindTexture will overide this.
	allTexUnits.push_back(cubeMapTexUnit);

	unsigned int cubeMapHandle;
	glGenTextures(1, &cubeMapHandle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapHandle);
	
	int width, height, nrChannels;
	unsigned char* cubeMapData;

	for(unsigned int i = 0; i < cm_faces.size(); i++)
	{
		unsigned char* data = stbi_load(cm_faces[i].c_str(), &width, &height, &nrChannels, 0);
		
		if(data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cube map texture failed to load :( " << std::endl;
		}
	}

	//set the texture parameters (which specify how a texture should be sampled):
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	//Cube Map End - - - - - - - - - - - - - - - - - - - - - - - -

	//bool wireframe = false;

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//Frame buffer Code:
	
	unsigned short int fbo_tex_attachment = COORD.GenerateTexUnit("res/textures/awesomeface.png", "png");
	
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//Creating framebuffer Texture:
	unsigned int texture;
	glGenTextures(1, &texture);
	
	glBindTexture(GL_TEXTURE_2D, texture); //#HERE_Binding_this_casuses_problems_with_rendering (Fixed, it was overriding last created tex unit texture)

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);


	// create a renderbuffer object
	unsigned int rbo; //#RBO_Created_Here

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);

	//Attach the rbo to the framebuffer:
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "\nFramebuffer Complete :) \n" << std::endl;
	}
	else
	{
		std::cout << "\nFramebuffer Incomplete :( \n" << std::endl;
	}
	

	//End Frame buffer Code ---

	//set default frame buffer back to active:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int screenQuadVAO;
	unsigned int screenQuadVBO;
	
	glGenVertexArrays(1, &screenQuadVAO);
	glBindVertexArray(screenQuadVAO);
	glGenBuffers(1, &screenQuadVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenQuadVertData), ScreenQuadVertData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // <== #HERE
		glEnable(GL_DEPTH_TEST);

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

		COORD.runAllSystems(2.0f, allEntites); //#ECS_RENDERING

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//sh_fboShader->bindProgram();

		sh_fboKernalEffect->bindProgram();
		sh_fboKernalEffect->setUniformTextureUnit("screenTexture", fbo_tex_attachment);

		GLCALL(glBindVertexArray(screenQuadVAO));
		glDisable(GL_DEPTH_TEST);

		glDrawArrays(GL_TRIANGLES, 0, 6);


		genMenu_1(allEntites,
			entities,
			map_SceneEntites,
			map_SceneNameToEntitiyScene,
			COORD,
			allTexUnits[0],
			allTexUnits[1],
			allTexUnits[2],
			allTexUnits[3],
			allTexUnits[4],
			allTexUnits[5]);

		glfwPollEvents();

		processInput(window);

		glfwSwapBuffers(window);


	}

	glDeleteFramebuffers(1, &fbo);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	//std::cin.get();

	return 0;
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