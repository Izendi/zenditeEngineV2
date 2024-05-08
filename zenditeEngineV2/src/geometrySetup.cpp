#include "geometrySetup.h"
#include "Coordinator.h"
#include "ECS/Components.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <cstdlib>

#include "Model_Loading/MinimalSceneFactory.h"

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
)
{

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Here, you can start using ImGui to create interfaces

	if (ImGui::Begin("Entities", nullptr))
	{

		ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);

		static int selected = 0;
		{
			int i = 0;
			ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
			for (i = 0; i < nonSceneEntities.size(); ++i)
			{
				// FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
				char label[60];
				std::string name = COORD.GetComponentDataFromEntity<c_EntityInfo>(nonSceneEntities[i]).name;

				sprintf_s(label, " %s", name.c_str());
				if (ImGui::Selectable(label, selected == i))
					selected = i;
			}
			i = i - 1;
			for (const auto& pair : map_sceneNameToEntitiesVec)
			{
				if (ImGui::TreeNode(pair.first.c_str())) // The TreeNode function returns true if the node is open
				{
					if (ImGui::BeginChild("MyBoxedSection", ImVec2(200, 110), true)) {

						static glm::vec3 scenePos;
						static glm::vec3 sceneRot;
						static glm::vec3 sceneScl(1.0f);
						ImGui::InputFloat3("pos", &scenePos[0]);
						ImGui::InputFloat3("rot", &sceneRot[0]);
						ImGui::InputFloat3("scl", &sceneScl[0]);
						//ImGui::SameLine();
						if (ImGui::Button("Update All"))
						{
							glm::mat4 MM(1.0f);

							float angleXRadians = glm::radians(sceneRot.x);
							float angleYRadians = glm::radians(sceneRot.y);
							float angleZRadians = glm::radians(sceneRot.z);

							MM = glm::translate(MM, scenePos);
							MM = glm::rotate(MM, angleZRadians, glm::vec3(0, 0, 1)); //  Z
							MM = glm::rotate(MM, angleYRadians, glm::vec3(0, 1, 0)); //  Y
							MM = glm::rotate(MM, angleXRadians, glm::vec3(1, 0, 0)); //  X
							MM = glm::scale(MM, sceneScl);

							MM = glm::translate(MM, scenePos);

							map_SceneNameToEntitiyScene[pair.first]->SetSceneModelMat(MM, COORD);
						}

					}
					ImGui::EndChild();

					for (int j = 0; j < pair.second.size(); ++j)
					{
						++i;
						char label[60];
						std::string name = COORD.GetComponentDataFromEntity<c_EntityInfo>(pair.second[j]).name;

						sprintf_s(label, " %s", name.c_str());
						if (ImGui::Selectable(label, selected == i))
							selected = i;
					}


					ImGui::TreePop(); // Pop the parent node when done with it
				}
				else
				{
					i = i + pair.second.size() - 1;
				}

			}

			ImGui::EndChild();

		}
		ImGui::SameLine();

		// Right
		{
			float v_speed = 0.05f;
			//float v_min = 0.0f;
			//float v_max = 1.0f;
			const char* format = "%.03f";

			auto& modified = COORD.GetComponentDataFromEntity<c_Modified>(entities[selected]);
			auto& infoData = COORD.GetComponentDataFromEntity<c_EntityInfo>(entities[selected]);

			//auto& aabb = COORD.GetComponentDataFromEntity<c_AABB>(entities[selected]);

			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("%s", infoData.name.c_str());
			ImGui::Separator();
			if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Components"))
				{
					short int bitSetPos = COORD.GetComponentBitsetPos<c_Transform>();
					std::bitset<32> transBitset; // Create a bitset of size 32
					transBitset.set(bitSetPos);

					std::bitset<32> entitySig = COORD.GetEntitySignature(entities[selected]);

					if ((entitySig & transBitset) == transBitset) // If this entity has a texture component
					{
						ImGui::SeparatorText("Transform:");

						ImGui::Separator;

						auto& posData = COORD.GetComponentDataFromEntity<c_Transform>(entities[selected]);

						glm::mat4 modelMat = posData.modelMat[0];

						glm::vec3 pos;
						glm::vec3 scale;
						pos.x = modelMat[3][0];
						pos.y = modelMat[3][1];
						pos.z = modelMat[3][2];

						scale.x = glm::length(glm::vec3(modelMat[0]));
						scale.y = glm::length(glm::vec3(modelMat[1]));
						scale.z = glm::length(glm::vec3(modelMat[2]));

						modelMat[0] = modelMat[0] / scale.x;
						modelMat[1] = modelMat[1] / scale.y;
						modelMat[2] = modelMat[2] / scale.z;

						glm::vec3 angles;
						glm::quat q = glm::quat_cast(modelMat);
						angles = glm::eulerAngles(q);
						angles = glm::degrees(angles);

						if (ImGui::DragFloat3("Position XYZ", &pos[0], v_speed))  //Position
						{
							float angleXRadians = glm::radians(angles.x);
							float angleYRadians = glm::radians(angles.y);
							float angleZRadians = glm::radians(angles.z);

							glm::mat4 MM(1.0f);
							MM = glm::translate(MM, pos);
							MM = glm::rotate(MM, angleZRadians, glm::vec3(0, 0, 1)); //  Z
							MM = glm::rotate(MM, angleYRadians, glm::vec3(0, 1, 0)); //  Y
							MM = glm::rotate(MM, angleXRadians, glm::vec3(1, 0, 0)); //  X
							MM = glm::scale(MM, scale);

							posData.modelMat[0] = MM;
						}

						if (ImGui::DragFloat3("Rotation XYZ", &angles[0], v_speed))  //Rotation
						{
							float angleXRadians = glm::radians(angles.x);
							float angleYRadians = glm::radians(angles.y);
							float angleZRadians = glm::radians(angles.z);

							glm::mat4 MM(1.0f);
							MM = glm::translate(MM, pos);
							MM = glm::rotate(MM, angleZRadians, glm::vec3(0, 0, 1)); //  Z
							MM = glm::rotate(MM, angleYRadians, glm::vec3(0, 1, 0)); //  Y
							MM = glm::rotate(MM, angleXRadians, glm::vec3(1, 0, 0)); //  X
							MM = glm::scale(MM, scale);

							posData.modelMat[0] = MM;
						}

						if (ImGui::DragFloat3("Scale XYZ", &scale[0], v_speed))  //Scale
						{
							float angleXRadians = glm::radians(angles.x);
							float angleYRadians = glm::radians(angles.y);
							float angleZRadians = glm::radians(angles.z);

							glm::mat4 MM(1.0f);
							MM = glm::translate(MM, pos);
							MM = glm::rotate(MM, angleZRadians, glm::vec3(0, 0, 1)); //  Z
							MM = glm::rotate(MM, angleYRadians, glm::vec3(0, 1, 0)); //  Y
							MM = glm::rotate(MM, angleXRadians, glm::vec3(1, 0, 0)); //  X
							MM = glm::scale(MM, scale);

							posData.modelMat[0] = MM;
						}

					}
					else
					{
						ImGui::SeparatorText("Transform: NA");

						ImGui::Separator;
					}

					ImGui::NewLine();

					short int bitSetPos_AABB = COORD.GetComponentBitsetPos<c_AABB>();
					std::bitset<32> AABB_Bitset; // Create a bitset of size 32
					AABB_Bitset.set(bitSetPos_AABB);

					if ((entitySig & AABB_Bitset) == AABB_Bitset)
					{
						ImGui::SeparatorText("AABB:");

						ImGui::Separator;

						auto& AABB_Data = COORD.GetComponentDataFromEntity<c_AABB>(entities[selected]);

						bool& AABBactive = COORD.GetComponentDataFromEntity<c_AABB>(entities[selected]).active;

						if (ImGui::RadioButton("Visible", AABBactive == true)) {
							AABBactive = true;
						}
						ImGui::SameLine();
						if (ImGui::RadioButton("Invisible", AABBactive == false)) {
							AABBactive = false;
						}

						glm::vec3 AABB_scale;
						AABB_scale = AABB_Data.scale;

						if (ImGui::DragFloat3("Scale", &AABB_scale[0], v_speed))
						{

							AABB_Data.scale = AABB_scale;

						}

					}
					else
					{
						ImGui::SeparatorText("AABB: NA");

						ImGui::Separator;
					}

					ImGui::NewLine();


					short int bitSetPos2 = COORD.GetComponentBitsetPos<c_Texture>();
					std::bitset<32> texBitset; // Create a bitset of size 32
					texBitset.set(bitSetPos2);

					if ((entitySig & texBitset) == texBitset) // If this entity has a texture component
					{
						auto& texData = COORD.GetComponentDataFromEntity<c_Texture>(entities[selected]);
						ImGui::SeparatorText("Texture:");

						// Simple selection popup (if you want to show the current selection inside the Button itself,
						// you may want to build a string using the "###" operator to preserve a constant ID with a variable label)

						static int selected_tex = -1;
						const char* names[] = { "Wooden planks", "Rock" , "water", "grass", "lava", "wood brown" };
						static bool toggles[] = { true, false };

						if (ImGui::Button("Select texture"))
							ImGui::OpenPopup("my_select_popup");
						ImGui::SameLine();
						ImGui::TextUnformatted(selected_tex == -1 ? "<None>" : names[selected_tex]);
						if (ImGui::BeginPopup("my_select_popup"))
						{

							ImGui::SeparatorText("Default textures");
							for (int i = 0, j = 0; i < IM_ARRAYSIZE(names); i++)
							{
								if (ImGui::Selectable(names[i])) {
									selected_tex = i;
									switch (i) {
									case 0:
										texData.texUnit = containerTexUnit;
										modified.isModifed = true;
										break;

									case 1:
										texData.texUnit = rockySurfaceTexUnit;
										modified.isModifed = true;
										break;

									case 2:
										texData.texUnit = grassTexUnit;
										modified.isModifed = true;
										break;

									case 3:
										texData.texUnit = waterTexUnit;
										modified.isModifed = true;
										break;

									case 4:
										texData.texUnit = lavaTexUnit;
										modified.isModifed = true;
										break;

									case 5:
										texData.texUnit = brickWallTexUnit;
										modified.isModifed = true;
										break;

									default:
										break;
									}
								}

							}

							ImGui::EndPopup();
						}
					}



						ImGui::NewLine();

						

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("EID"))
				{
					ImGui::Text("EID: %d", entities[selected]);
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();


			}
			ImGui::EndChild();

			ImGui::EndGroup();
		}

	}
	ImGui::End();
	// Rendering


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


}

namespace util
{
	void setupSceneECS(Coordinator& COORD,
		std::vector<std::shared_ptr<Shader>>& shaders,
		std::vector<Entity>& entities,
		std::vector<Entity>& allEntites,
		std::vector<unsigned short int>& allTexUnits)
	{

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

		// -----
		entities.push_back(COORD.CreateEntity());
		entities.push_back(COORD.CreateEntity());
		entities.push_back(COORD.CreateEntity());
		entities.push_back(COORD.CreateEntity());
		entities.push_back(COORD.CreateEntity());

		for (int i = 0; i < entities.size(); ++i)
		{
			allEntites.push_back(entities[i]);
		}

		unsigned short int containerTexUnit = COORD.GenerateTexUnit("res/textures/container2.png", "png");		 // tx Unit = 0
		allTexUnits.push_back(containerTexUnit);
		unsigned short int rockySurfaceTexUnit = COORD.GenerateTexUnit("res/textures/rockySurface.png", "png");	 // tx Unit = 1
		allTexUnits.push_back(rockySurfaceTexUnit);
		unsigned short int waterTexUnit = COORD.GenerateTexUnit("res/textures/water.jpg", "jpg");				 // tx Unit = 2
		allTexUnits.push_back(waterTexUnit);
		unsigned short int grassTexUnit = COORD.GenerateTexUnit("res/textures/grass.png", "png");				 // tx Unit = 3
		allTexUnits.push_back(grassTexUnit);
		unsigned short int lavaTexUnit = COORD.GenerateTexUnit("res/textures/lava.jpg", "jpg");					 // tx Unit = 4
		allTexUnits.push_back(lavaTexUnit);
		unsigned short int redWindowTexUnit = COORD.GenerateTexUnit("res/textures/redWindow.png", "png");		 // tx Unit = 5
		allTexUnits.push_back(redWindowTexUnit);

		//Set up cube map tex unit:
		std::vector<std::string> cm_faces; //Contains the file path to the faces:
		cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/right.jpg");
		cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/left.jpg");
		cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/top.jpg");
		cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/bottom.jpg");
		cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/front.jpg");
		cm_faces.push_back("C:/Code/Chalmers/myGraphicsCode/zenditeEngineV2/zenditeEngineV2/res/textures/skybox/back.jpg");

		unsigned short int fbo_tex_attachment = COORD.GenerateTexUnit("res/textures/awesomeface.png", "png");
		allTexUnits.push_back(fbo_tex_attachment);

		//A cube map is just a texture, as such it is created using a texture ID handle:
		unsigned short int cubeMapTexUnit = COORD.GenerateTexUnit("res/textures/awesomeface.png", "png"); // even though I pass in awesome face here, the calls to glBindTexture will overide this.
		allTexUnits.push_back(cubeMapTexUnit);

		unsigned int cubeMapHandle;
		glGenTextures(1, &cubeMapHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapHandle);

		int width, height, nrChannels;
		unsigned char* cubeMapData;

		for (unsigned int i = 0; i < cm_faces.size(); i++)
		{
			unsigned char* data = stbi_load(cm_faces[i].c_str(), &width, &height, &nrChannels, 0);

			if (data)
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

		//verticalQuad
		//vertQuadTexCoord
		//vertQuadIndices

		//Grass Billboard renderable
		c_Renderable rc_3;
		addDataToRenderable(rc_3, verticalQuad, vertQuadVertNorms, vertQuadTexCoord, vertQuadIndices, sizeOfVerticalQuad, sizeOfVQIndices);
		rc_3.outline = false;

		c_Renderable rc_cubeEM;
		addDataToRenderable(rc_cubeEM, vertCubePosData, vertCubeNormData, vertCubeTexCoordData, indices, sizeOfVertCubePosData, sizeOfIndices);
		rc_cubeEM.emReflection = true;
		rc_cubeEM.outline = false;
		

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

		c_Texture tx_FBT;
		tx_FBT.texUnit = fbo_tex_attachment;

		c_Texture tx_EM;
		tx_EM.texUnit = cubeMapTexUnit;


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
		//aabb_0.vertices = new float[72];//#TOFIX_New_is_used_here_not_IDEAL
		aabb_0.active = true;
		for(int i = 0; i < 72; i++)
		{
			aabb_0.vertices[i] = AABBvertices[i];
		}

		c_AABB aabb_2;
		aabb_2.scale = glm::vec3(1.0f, 1.0f, 1.0f);
		//aabb_2.vertices = new float[72];
		for (int i = 0; i < 72; i++)
		{
			aabb_2.vertices[i] = AABBvertices[i];
		}
		//AABBvertices;
		aabb_2.active = true;
		

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
		COORD.setShaderForEntity(entities[0], shaders[0]);
		COORD.StoreShaderInEntityDataHandle(entities[0]);

		COORD.AddComponentToEntity<c_Transform>(entities[1], tr_2);
		COORD.AddComponentToEntity<c_Renderable>(entities[1], rc_0);
		COORD.AddComponentToEntity<c_Texture>(entities[1], tx_FBT);
		COORD.AddComponentToEntity<c_AABB>(entities[1], aabb_2);
		COORD.AddComponentToEntity<c_Wall>(entities[1], wall_0);
		COORD.AddComponentToEntity<c_Modified>(entities[1], md_2);
		COORD.AddComponentToEntity<c_EntityInfo>(entities[1], ei_2);
		COORD.SetUpRenderData(entities[1]);
		COORD.setShaderForEntity(entities[1], shaders[0]);
		COORD.StoreShaderInEntityDataHandle(entities[1]);

		COORD.AddComponentToEntity<c_Transform>(entities[2], tr_0);
		COORD.AddComponentToEntity<c_Renderable>(entities[2], rc_cubeEM);
		COORD.AddComponentToEntity<c_Texture>(entities[2], tx_EM);
		COORD.AddComponentToEntity<c_AABB>(entities[2], aabb_0);
		COORD.AddComponentToEntity<c_WallCollider>(entities[2], wallCollider_2);
		COORD.AddComponentToEntity<c_EntityInfo>(entities[2], ei_0);
		COORD.AddComponentToEntity<c_Modified>(entities[2], md_0);
		COORD.SetUpRenderData(entities[2]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
		COORD.setShaderForEntity(entities[2], shaders[1]); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
		COORD.StoreShaderInEntityDataHandle(entities[2]);

		COORD.AddComponentToEntity<c_Transform>(entities[3], tr_3);
		COORD.AddComponentToEntity<c_Renderable>(entities[3], rc_3);
		COORD.AddComponentToEntity<c_Texture>(entities[3], tx_3);
		COORD.AddComponentToEntity<c_EntityInfo>(entities[3], ei_3);
		COORD.AddComponentToEntity<c_Modified>(entities[3], md_3);
		COORD.SetUpRenderData(entities[3]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
		COORD.setShaderForEntity(entities[3], shaders[0]); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
		COORD.StoreShaderInEntityDataHandle(entities[3]);

		COORD.AddComponentToEntity<c_Transform>(entities[4], tr_4);
		COORD.AddComponentToEntity<c_Renderable>(entities[4], rc_3);
		COORD.AddComponentToEntity<c_Texture>(entities[4], tx_6);
		COORD.AddComponentToEntity<c_EntityInfo>(entities[4], ei_4);
		COORD.AddComponentToEntity<c_Modified>(entities[4], md_4);
		COORD.SetUpRenderData(entities[4]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
		COORD.setShaderForEntity(entities[4], shaders[0]); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
		COORD.StoreShaderInEntityDataHandle(entities[4]);
	}

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

void GenerateBasicSquareVAO(unsigned int& VAO_Square, unsigned int& VBO_Square, unsigned int& EBO_Square)
{
	//Object Data Setup:
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	GLCALL(glGenVertexArrays(1, &VAO_Square));
	GLCALL(glBindVertexArray(VAO_Square));

	GLCALL(glGenBuffers(1, &VBO_Square));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_Square));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCALL(glGenBuffers(1, &EBO_Square));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Square));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0));
	GLCALL(glEnableVertexAttribArray(0));

	GLCALL(glBindVertexArray(0)); //Unbind the VAO
}

void GenerateTexTriVAO(unsigned int& VAO_TexTri, unsigned int& VBO_TexTri, unsigned int& EBO_TexTri)
{	
	float vertices[] = {
		// positions        // texture coords
		 0.5f,  0.5f, 0.0f, 2.0f, 2.0f,   // top right
		 0.5f, -0.5f, 0.0f, 2.0f, -1.0f,   // bottom right
		-0.5f, -0.5f, 0.0f, -1.0f, -1.0f,   // bottom left
		-0.5f,  0.5f, 0.0f, -1.0f, 2.0f    // top left 
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	GLCALL(glGenVertexArrays(1, &VAO_TexTri));
	GLCALL(glBindVertexArray(VAO_TexTri));

	GLCALL(glGenBuffers(1, &VBO_TexTri));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_TexTri));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCALL(glGenBuffers(1, &EBO_TexTri));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TexTri));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0));
	GLCALL(glEnableVertexAttribArray(0));

	GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3*sizeof(float))));
	GLCALL(glEnableVertexAttribArray(1));

	GLCALL(glBindVertexArray(0)); //Unbind the VAO

}

void GenerateCubeNoEBO(unsigned int& VAO_Cube, unsigned int& VBO_Cube)
{
	float vertices[] = 
	{
		    // positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	GLCALL(glGenVertexArrays(1, &VAO_Cube));
	GLCALL(glBindVertexArray(VAO_Cube));

	GLCALL(glGenBuffers(1, &VBO_Cube));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0));
	GLCALL(glEnableVertexAttribArray(0));

	GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float))));
	GLCALL(glEnableVertexAttribArray(1));

	GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float))));
	GLCALL(glEnableVertexAttribArray(2));

	GLCALL(glBindVertexArray(0)); //Unbind the VAO
}

void GenerateCubeNoEBONoNormals(unsigned int& VAO_Cube, unsigned int& VBO_Cube)
{
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	GLCALL(glGenVertexArrays(1, &VAO_Cube));
	GLCALL(glBindVertexArray(VAO_Cube));

	GLCALL(glGenBuffers(1, &VBO_Cube));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0));
	GLCALL(glEnableVertexAttribArray(0));

	//GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float))));
	//GLCALL(glEnableVertexAttribArray(1));

	GLCALL(glBindVertexArray(0)); //Unbind the VAO
}

void GenerateHeightmapPlane(unsigned int& heightMapVAO,
	unsigned int& heightMapVBO,
	unsigned int& heightMapEBO,
	std::vector<Vert>& hmVerts,
	std::vector<unsigned int>& hmIndices,
	std::vector<Face>& hmFaces,
	int hmWidth,
	int hmHeight,
	int hmNrChannels,
	unsigned char* hmTexData)
{

	float yScale = 64.0f / 256.0f;
	float yShift = 16.0f;

	float x_texPoint = 1.0f / (hmWidth / 6);
	float y_texPoint = 1.0f / (hmHeight / 6);

	for (unsigned int z = 0; z < hmHeight; z++)
	{
		for (unsigned int x = 0; x < hmWidth; x++)
		{
			Vert vert;
			unsigned char* texel = hmTexData + (x + hmWidth * z) * hmNrChannels;
			unsigned char y = texel[0];

			glm::vec3 vec;
			vec.x = x;
			vec.y = (int)y * yScale - yShift;
			vec.z = z;

			vert.pos = vec;

			glm::vec2 texCoord;
			texCoord.x = x * x_texPoint;
			texCoord.y = z * y_texPoint;

			vert.texCord = texCoord;

			hmVerts.push_back(vert);
		}
	}

	//Fill Height map indices data and hm face data:
	for (unsigned int i = 0; i < (hmHeight - 1); i++)
	{
		for (unsigned int ii = 0; ii < (hmWidth - 1) * 6; ii = ii + 6)
		{
			unsigned int val = 0;
			Face face1;
			Face face2;

			val = i * hmWidth + ii / 6;
			hmIndices.push_back(val);
			face1.in_1 = val;

			val = (i + 1) * hmWidth + ii / 6;
			hmIndices.push_back(val);
			face1.in_2 = val;

			val = ((i + 1) * hmWidth + ii / 6) + 1;
			hmIndices.push_back(val);
			face1.in_3 = val;

			glm::vec3 AB = hmVerts[face1.in_2].pos - hmVerts[face1.in_1].pos;
			glm::vec3 AC = hmVerts[face1.in_3].pos - hmVerts[face1.in_1].pos;

			face1.faceNormal = glm::normalize(glm::cross(AB, AC));


			val = i * hmWidth + ii / 6;
			hmIndices.push_back(val);
			face2.in_1 = val;

			val = ((i + 1) * hmWidth + ii / 6) + 1;
			hmIndices.push_back(val);
			face2.in_2 = val;

			val = (i * hmWidth + ii / 6) + 1;
			hmIndices.push_back(val);
			face2.in_3 = val;

			AB = hmVerts[face2.in_2].pos - hmVerts[face2.in_1].pos;
			AC = hmVerts[face2.in_3].pos - hmVerts[face2.in_1].pos;

			face2.faceNormal = glm::normalize(glm::cross(AB, AC));

			hmFaces.push_back(face1);
			hmFaces.push_back(face2);
		}
	}

	//Iterate though all hmVerts and calculate the normal from the faces:

	//First row and last row are special cases, as such they shall be handled separately

	//First element (of first row - special case)
	std::vector<Face> fb;
	fb.push_back(hmFaces[0]);
	fb.push_back(hmFaces[1]);

	hmVerts[0].norm = calcVertNormal(fb);

	fb.clear();

	for (unsigned int i = 1; i < hmWidth - 1; i++)
	{
		fb.clear();

		fb.push_back(hmFaces[(i * 2) + 1]);
		fb.push_back(hmFaces[(i * 2) + 2]);
		fb.push_back(hmFaces[(i * 2) + 3]);

		hmVerts[i].norm = calcVertNormal(fb);
	}

	//last element of first row (special case)
	fb.clear();

	fb.push_back(hmFaces[hmWidth - 1]);

	hmVerts[hmWidth - 1].norm = calcVertNormal(fb);

	fb.clear();

	unsigned int facesPerRow = (hmWidth * 2) - 2;

	for (unsigned int i = 1; i < hmHeight - 1; i++)
	{
		//Handle first and last elements of each row here
		fb.clear();

		fb.push_back(hmFaces[(i - 1) * facesPerRow]);
		fb.push_back(hmFaces[(i)*facesPerRow]);
		fb.push_back(hmFaces[(i * facesPerRow) + 1]);

		hmVerts[i * hmWidth].norm = calcVertNormal(fb);

		fb.clear();

		for (unsigned int ii = 1; ii < hmWidth - 1; ii++)
		{
			fb.clear();

			fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow)]); //0
			fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow) + 1]); //1
			fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow) + 2]); //2

			fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 1]);
			fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 2]);
			fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 3]);

			hmVerts[(i * hmWidth) + ii].norm = calcVertNormal(fb);

		}

		fb.clear();

		fb.push_back(hmFaces[((i)*facesPerRow) - 1]);
		fb.push_back(hmFaces[((i)*facesPerRow) - 2]);
		fb.push_back(hmFaces[((i + 1) * facesPerRow) - 1]);

		hmVerts[((i + 1) * hmWidth) - 1].norm = calcVertNormal(fb);

		fb.clear();

	}

	//Last Row Elements:
	fb.clear();

	//First last town vertex:
	fb.push_back(hmFaces[facesPerRow * (hmHeight - 2)]);

	hmVerts[hmWidth * (hmHeight - 1)].norm = calcVertNormal(fb);

	fb.clear();

	//Each last row element
	unsigned int k = 0;
	for (unsigned int i = hmWidth * (hmHeight - 1) + 1; i < (hmWidth * (hmHeight)) - 2; i++)
	{
		fb.clear();

		fb.push_back(hmFaces[facesPerRow * (hmHeight - 2) + (k * 2)]);
		fb.push_back(hmFaces[facesPerRow * (hmHeight - 2) + (k * 2) + 1]);
		fb.push_back(hmFaces[facesPerRow * (hmHeight - 2) + (k * 2) + 2]);

		hmVerts[i].norm = calcVertNormal(fb);

		k++;
	}

	//last element of first row (special case)
	fb.clear();

	fb.push_back(hmFaces[facesPerRow * (hmHeight - 2) + ((k - 1) * 2) + 2]);
	fb.push_back(hmFaces[facesPerRow * (hmHeight - 2) + ((k - 1) * 2) + 3]); //#Check this one again!!!

	hmVerts[(hmWidth * (hmHeight)) - 2].norm = calcVertNormal(fb);

	fb.clear();


	GLCALL(glGenVertexArrays(1, &heightMapVAO));
	GLCALL(glGenBuffers(1, &heightMapVBO));
	GLCALL(glGenBuffers(1, &heightMapEBO));

	GLCALL(glBindVertexArray(heightMapVAO));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, heightMapVBO));

	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, heightMapVBO));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, hmVerts.size() * sizeof(Vert), &hmVerts[0], GL_STATIC_DRAW));

	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, heightMapEBO));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, hmIndices.size() * sizeof(unsigned int), &hmIndices[0], GL_STATIC_DRAW));

	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)0));
	GLCALL(glEnableVertexAttribArray(0));

	GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, norm)));
	GLCALL(glEnableVertexAttribArray(1));

	GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, texCord)));
	GLCALL(glEnableVertexAttribArray(2));
}

void bindVao(unsigned int VAO)
{
	GLCALL(glBindVertexArray(VAO));
}

void unbindVao()
{
	GLCALL(glBindVertexArray(0));
}