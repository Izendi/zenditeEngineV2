#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Coordinator.h"
#include "ECS/Components.h"

#include <cstdlib>

#include "Model_Loading/MinimalSceneFactory.h"

int selectedOption = 0; // You can assign default selection here, 0 for Option A, 1 for Option B, etc.
int lastSelectedOption = selectedOption;

void imGuiSetup(GLFWwindow* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130"); // Use GLSL version 130

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
}

/*

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

					for(int j = 0; j < pair.second.size(); ++j)
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
					

					
					short int bitSetPos3 = COORD.GetComponentBitsetPos<c_LightRenderable>();
					std::bitset<32> lr_Bitset; // Create a bitset of size 32
					lr_Bitset.set(bitSetPos3);

					short int bitSetPos4 = COORD.GetComponentBitsetPos<c_SpotLightEmitter>();
					std::bitset<32> spotl_Bitset; // Create a bitset of size 32
					spotl_Bitset.set(bitSetPos4);
					short int bitSetPos5 = COORD.GetComponentBitsetPos<c_PointLightEmitter>();
					std::bitset<32> pointl_Bitset; // Create a bitset of size 32
					pointl_Bitset.set(bitSetPos5);
					short int bitSetPos6 = COORD.GetComponentBitsetPos<c_DirLightEmitter>();
					std::bitset<32> dirl_Bitset; // Create a bitset of size 32
					dirl_Bitset.set(bitSetPos6);

					
					if ((entitySig & lr_Bitset) == lr_Bitset) 
					{
						float v_speed = 0.01f; 
						float v_min = 0.0f; 
						float v_max = 1.0f; 
						const char* format = "%.03f";

						bool active = COORD.GetComponentDataFromEntity<c_LightRenderable>(entities[selected]).active;

						static std::string toggle;

						if (active == true)
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.9f, 0.3f, 1.0f)); // Green for enabled
							toggle = "enabled";
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.3f, 0.3f, 1.0f)); // Red for disabled
							toggle = "disabled";
						}

						if(ImGui::Button(toggle.c_str()))
						{
							active = !active;
							COORD.GetComponentDataFromEntity<c_LightRenderable>(entities[selected]).active = active;
							
							if ((entitySig & spotl_Bitset) == spotl_Bitset)
							{
								(COORD.GetComponentDataFromEntity<c_SpotLightEmitter>(entities[selected])).active = active;
							}
							if((entitySig & pointl_Bitset) == pointl_Bitset)
							{
								(COORD.GetComponentDataFromEntity<c_PointLightEmitter>(entities[selected])).active = active;
							}
							if((entitySig & dirl_Bitset) == dirl_Bitset)
							{
								COORD.GetComponentDataFromEntity<c_DirLightEmitter>(entities[selected]).active = active;
							}
						
						}
						ImGui::PopStyleColor();

						ImGui::NewLine();

						ImGui::SeparatorText("Light Settings");

						ImGui::NewLine();

						glm::vec3* ambient_Data = nullptr;
						glm::vec3* diffuse_Data = nullptr;
						glm::vec3* specular_Data = nullptr;

						float* constant = nullptr;
						float* linear = nullptr;
						float* quadratic = nullptr;

						if ((entitySig & spotl_Bitset) == spotl_Bitset)
						{
							ambient_Data = &COORD.GetComponentDataFromEntity<c_SpotLightEmitter>(entities[selected]).ambient;
							diffuse_Data = &COORD.GetComponentDataFromEntity<c_SpotLightEmitter>(entities[selected]).diffuse;
							specular_Data = &COORD.GetComponentDataFromEntity<c_SpotLightEmitter>(entities[selected]).specular;
							
							constant = &COORD.GetComponentDataFromEntity<c_SpotLightEmitter>(entities[selected]).constant;
							linear = &COORD.GetComponentDataFromEntity<c_SpotLightEmitter>(entities[selected]).linear;
							quadratic = &COORD.GetComponentDataFromEntity<c_SpotLightEmitter>(entities[selected]).quadratic;
						}
						if ((entitySig & pointl_Bitset) == pointl_Bitset)
						{
							ambient_Data = &COORD.GetComponentDataFromEntity<c_PointLightEmitter>(entities[selected]).ambient;
							diffuse_Data = &COORD.GetComponentDataFromEntity<c_PointLightEmitter>(entities[selected]).diffuse;
							specular_Data = &COORD.GetComponentDataFromEntity<c_PointLightEmitter>(entities[selected]).specular;
						
							constant = &COORD.GetComponentDataFromEntity<c_PointLightEmitter>(entities[selected]).constant;
							linear = &COORD.GetComponentDataFromEntity<c_PointLightEmitter>(entities[selected]).linear;
							quadratic = &COORD.GetComponentDataFromEntity<c_PointLightEmitter>(entities[selected]).quadratic;
						}
						if ((entitySig & dirl_Bitset) == dirl_Bitset)
						{
							ambient_Data = &COORD.GetComponentDataFromEntity<c_DirLightEmitter>(entities[selected]).ambient;
							diffuse_Data = &COORD.GetComponentDataFromEntity<c_DirLightEmitter>(entities[selected]).diffuse;
							specular_Data = &COORD.GetComponentDataFromEntity<c_DirLightEmitter>(entities[selected]).specular;
						}

						ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Intensity");
						ImGui::Separator();

						glm::vec3 amVec3 = *ambient_Data;
						glm::vec3 spVec3 = *specular_Data;
						glm::vec3 dfVec3 = *diffuse_Data;

						if (ImGui::DragFloat3("Ambient", &amVec3[0], v_speed, v_min, v_max, format))
						{
							ambient_Data->x = amVec3.x;
							ambient_Data->y = amVec3.y;
							ambient_Data->z = amVec3.z;
						}
						if (ImGui::DragFloat3("diffuse", &dfVec3[0], v_speed, v_min, v_max, format))
						{
							diffuse_Data->x = dfVec3.x;
							diffuse_Data->y = dfVec3.y;
							diffuse_Data->z = dfVec3.z;
						}
						if (ImGui::DragFloat3("specular", &spVec3[0], v_speed, v_min, v_max, format))
						{
							specular_Data->x = spVec3.x;
							specular_Data->y = spVec3.y;
							specular_Data->z = spVec3.z;
						}

						ImGui::NewLine();
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Color");
						ImGui::Separator();

						
						if ((entitySig & dirl_Bitset) != dirl_Bitset)
						{
							float con = *constant;
							float lin = *linear;
							float quad = *quadratic;

							if (ImGui::DragFloat("constant", &con, v_speed))
							{
								*constant = con;
							}
							if (ImGui::DragFloat("linear", &lin, v_speed))
							{
								*linear = lin;
							}
							if (ImGui::DragFloat("quadratic", &quad, v_speed))
							{
								*quadratic = quad;
							}
						}

					}

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

*/

void genMenu_2(std::vector<Entity>& entities,
	std::vector<Entity>& nonSceneEntities,
	std::unordered_map<std::string, std::vector<Entity>> map_sceneNameToEntitiesVec,
	std::unordered_map<std::string, std::shared_ptr<EntityScene>>& map_SceneNameToEntitiyScene,
	Coordinator& COORD
)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Here, you can start using ImGui to create interfaces

	if (ImGui::Begin("Entities", nullptr))
	{

		ImGui::Text("Testing");

		

		

	}
	ImGui::End();
	// Rendering


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void openLocalRepository(const std::string& repositoryPath)
{

#ifdef _WIN32
	std::string command = "explorer \"" + repositoryPath + "\"";

#endif
	system(command.c_str());

}