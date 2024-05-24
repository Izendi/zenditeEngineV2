#include "geometrySetup.h"
#include "Coordinator.h"
#include "ECS/Components.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <cstdlib>

#include "Model_Loading/MinimalSceneFactory.h"

float FbmNoise2D(float x, float y, unsigned int frequency, int octaves, float lacunarity, float persistence, float amplitude, float maxAmplitude)
{
	float total = 0.0f;
	//float amplitude = 1.0f;
	//float maxAmplitude = 0.0f; // Used for normalization
	float freq = (float)frequency;

	for (int i = 0; i < octaves; ++i) {
		total += stb_perlin_noise3(x * frequency, y * frequency, 0.0f, 0, 0, 0) * amplitude;
		frequency *= lacunarity;
		maxAmplitude += amplitude;
		amplitude *= persistence;
	}

	return total / maxAmplitude; // Normalize the result
}

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
	unsigned short int redWindowTexUnit,
	unsigned short int cloudNoiseTexUnit,
	unsigned int& SEED,
	unsigned int& frequency,
	bool& reload,
	int& octaves,
	float& lacunarity,
	float& persistence,
	float& amplitude,
	float& maxAmplitude,
	float& cloud_noiseFrequency,
	float& cloud_persistence,
	float& cloud_amplitude,
	float& discardThreshold,
	bool& pauseSun
)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Here, you can start using ImGui to create interfaces

	if (ImGui::Begin("Debug Menu", nullptr))
	{

		ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);

		static int selected = 0;
		{
			int i = 0;
			ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);

			float framerate = 0.0f;

			if (deltaTime > 0.0f)
			{
				framerate = 1.0f / deltaTime;
			}

			ImGui::Text("Framerate: %.1f FPS", framerate);
			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Checkbox("Reload", &reload))
			{
				// This block is executed when the checkbox state changes
				if (!reload)
				{
					//reload = true;
					std::cout << "Reload == False" << std::endl;
				}
				else
				{
					//reload = false;
					std::cout << "Reload == True" << std::endl;
				}
			}

			if (ImGui::Checkbox("Pause Sun", &pauseSun))
			{
				// This block is executed when the checkbox state changes
				if (!pauseSun)
				{
					std::cout << "pauseSun == False" << std::endl;
				}
				else
				{
					std::cout << "pauseSun == True" << std::endl;
				}
			}

			

			if (ImGui::InputScalar("Unsigned Int Input", ImGuiDataType_U32, &SEED))
			{
				// This block is executed when the input value changes
				//std::cout << "Seed" << SEED << std::endl;
			}
			if (ImGui::InputScalar("Frequency", ImGuiDataType_U32, &frequency))
			{
				// This block is executed when the input value changes
				std::cout << "Frequency = " << frequency << std::endl;
			}
			if (ImGui::InputScalar("octaves", ImGuiDataType_U32, &octaves))
			{

			}
			if (ImGui::InputFloat("lacunarity", &lacunarity))
			{
				
			}
			if (ImGui::InputFloat("persistence", &persistence))
			{
				// This block is executed when the input value changes
				
			}
			if (ImGui::InputFloat("amplitude", &amplitude))
			{
				// This block is executed when the input value changes
				
			}
			if (ImGui::InputFloat("maxAmplitude", &maxAmplitude))
			{
				// This block is executed when the input value changes
				
			}

			ImGui::NewLine();
			ImGui::Text("--- Cloud Parameters ---");

			if (ImGui::InputFloat("cloud_noiseFrequency", &cloud_noiseFrequency)) //#BUG_input_always_becomes_8_after_reload
			{
				// This block is executed when the input value changes
			}
			if (ImGui::InputFloat("cloud_persistence", &cloud_persistence))
			{
				// This block is executed when the input value changes
			}
			if (ImGui::InputFloat("cloud_amplitude", &cloud_amplitude))
			{
				// This block is executed when the input value changes
			}

			if (ImGui::InputFloat("discardThreshold", &discardThreshold))
			{
				// This block is executed when the input value changes
			}

			ImGui::NewLine();
			ImGui::Text(" --- Entities --- ");
			ImGui::Separator();

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
						const char* names[] = { "Height Field", "Rock" , "water", "grass Billboard", "lava", "Red Window", "Noise Clouds" };
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
										texData.texUnit = hfTexUnit;
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
										texData.texUnit = redWindowTexUnit;
										modified.isModifed = true;
										break;

									case 6:
										texData.texUnit = cloudNoiseTexUnit;
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

float scaleToRange(float value, float min, float max) {
	return (value - min) / (max - min);
}



namespace util
{
	void setupHeightField
	(
		c_Renderable& rc_hf,
		std::vector<unsigned short int>& allTexUnits, 
		unsigned int hfTexUnit, 
		unsigned int hfWidth, 
		unsigned int hfHeight, 
		unsigned int frequency,
		unsigned int SEED
	)
	{
		rc_hf.clear();

		std::vector<float> data(hfWidth * hfHeight * 4);
		std::vector<float> noiseData(hfWidth * hfHeight * 4);

		//std::cout << "\n ------------- Data Size = " << data.size() << "\n" << std::endl;

		std::random_device rd; // Non-deterministic random number generator
		std::mt19937 gen(rd()); // Mersenne Twister engine seeded with rd()

		// Define a uniform real distribution in the range [0.0, 255.0]
		std::uniform_real_distribution<> dis(0.0, 255.0);

		/*
		for(int i = 0; i < data.size(); i = i + 4)
		{
			float rnd = dis(gen);
			rnd = scaleToRange(rnd, 0.0f, 255.0f);

			data[i] = rnd;
			data[i+1] = rnd;
			data[i+2] = rnd;
			data[i+3] = 255.0f;
		}
		*/

		//GeneratePerlinNoise(data, hfWidth, hfHeight, SEED);

		for (int y = 0; y < hfWidth; ++y)
		{
			for (int x = 0; x < hfHeight; ++x)
			{
				int index = (y * hfWidth + x) * 4;

				float fx = static_cast<float>(x + SEED) / hfWidth;
				float fy = static_cast<float>(y + SEED) / hfHeight;

				//fx = fx * frequency;
				//fy = fy * frequency;

				//FbmNoise2D(fx, fy, frequency, 4, 2.0f, 0.5f, 1.0f, 0.0f);

				float pnoise = FbmNoise2D(fx, fy, frequency, 4, 2.0f, 0.5f, 1.0f, 0.0f);

				pnoise = (pnoise + 1.0f) / 2.0f;

				noiseData[index] = pnoise;
				noiseData[index + 1] = pnoise;
				noiseData[index + 2] = pnoise;
				noiseData[index + 3] = 1.0f;
			}
		}

		//Store perlin noise in OpenGL texture.
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, hfWidth, hfHeight, GL_RGBA, GL_FLOAT, data.data());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, hfWidth, hfHeight, GL_RGBA, GL_FLOAT, noiseData.data());

		allTexUnits.push_back(hfTexUnit);


		std::vector<Face> hmFaces;

		float x_texPoint = 1.0f / (hfWidth);// / 6);
		float y_texPoint = 1.0f / (hfHeight);// / 6);

		for (unsigned int z = 0; z < hfHeight; z++)
		{
			for (unsigned int x = 0; x < hfWidth; x++)
			{
				unsigned int index = (x + z * hfWidth) * 4;
				Vertex vert;

				float y = noiseData[index];

				//Scale by 5 (value is between 0 and 1)
				y = y * 10.0f;

				vert.Position.x = x;
				vert.Position.y = y;
				vert.Position.z = z;

				vert.TexCoords.x = x_texPoint * x;
				vert.TexCoords.y = y_texPoint * z;

				rc_hf.vertices.push_back(vert);
			}

		}

		//Fill Height map indices data and hm face data:
		for (unsigned int i = 0; i < (hfHeight - 1); i++)
		{
			for (unsigned int ii = 0; ii < (hfWidth - 1) * 6; ii = ii + 6)
			{

				unsigned int val = 0;
				Face face1;
				Face face2;

				val = i * hfWidth + ii / 6;
				rc_hf.indices.push_back(val);
				face1.in_1 = val;

				val = (i + 1) * hfWidth + ii / 6;
				rc_hf.indices.push_back(val);
				face1.in_2 = val;

				val = ((i + 1) * hfWidth + ii / 6) + 1;
				rc_hf.indices.push_back(val);
				face1.in_3 = val;

				glm::vec3 AB = rc_hf.vertices[face1.in_2].Position - rc_hf.vertices[face1.in_1].Position;
				glm::vec3 AC = rc_hf.vertices[face1.in_3].Position - rc_hf.vertices[face1.in_1].Position;

				face1.faceNormal = glm::normalize(glm::cross(AB, AC));


				val = i * hfWidth + ii / 6;
				rc_hf.indices.push_back(val);
				face2.in_1 = val;

				val = ((i + 1) * hfWidth + ii / 6) + 1;
				rc_hf.indices.push_back(val);
				face2.in_2 = val;

				val = (i * hfWidth + ii / 6) + 1;
				rc_hf.indices.push_back(val);
				face2.in_3 = val;

				AB = rc_hf.vertices[face2.in_2].Position - rc_hf.vertices[face2.in_1].Position;
				AC = rc_hf.vertices[face2.in_3].Position - rc_hf.vertices[face2.in_1].Position;

				face2.faceNormal = glm::normalize(glm::cross(AB, AC));

				hmFaces.push_back(face1);
				hmFaces.push_back(face2);

			}
		}

		//First element (of first row - special case)
		std::vector<Face> fb;
		fb.push_back(hmFaces[0]);
		fb.push_back(hmFaces[1]);

		rc_hf.vertices[0].Normal = calcVertNormal(fb);

		fb.clear();

		for (unsigned int i = 1; i < hfWidth - 1; i++)
		{
			fb.clear();

			fb.push_back(hmFaces[(i * 2) + 1]);
			fb.push_back(hmFaces[(i * 2) + 2]);
			fb.push_back(hmFaces[(i * 2) + 3]);

			rc_hf.vertices[i].Normal = calcVertNormal(fb);
		}

		//last element of first row (special case)
		fb.clear();

		fb.push_back(hmFaces[hfWidth - 1]);

		rc_hf.vertices[hfWidth - 1].Normal = calcVertNormal(fb);

		fb.clear();

		unsigned int facesPerRow = (hfWidth * 2) - 2;

		for (unsigned int i = 1; i < hfHeight - 1; i++)
		{
			//Handle first and last elements of each row here
			fb.clear();

			fb.push_back(hmFaces[(i - 1) * facesPerRow]);
			fb.push_back(hmFaces[(i)*facesPerRow]);
			fb.push_back(hmFaces[(i * facesPerRow) + 1]);

			rc_hf.vertices[i * hfWidth].Normal = calcVertNormal(fb);

			fb.clear();

			for (unsigned int ii = 1; ii < hfWidth - 1; ii++)
			{
				fb.clear();

				fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow)]); //0
				fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow) + 1]); //1
				fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow) + 2]); //2

				fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 1]);
				fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 2]);
				fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 3]);

				rc_hf.vertices[(i * hfWidth) + ii].Normal = calcVertNormal(fb);

			}

			fb.clear();

			fb.push_back(hmFaces[((i)*facesPerRow) - 1]);
			fb.push_back(hmFaces[((i)*facesPerRow) - 2]);
			fb.push_back(hmFaces[((i + 1) * facesPerRow) - 1]);

			rc_hf.vertices[((i + 1) * hfWidth) - 1].Normal = calcVertNormal(fb);

			fb.clear();

		}

		//Last Row Elements:
		fb.clear();

		//First last town vertex:
		fb.push_back(hmFaces[facesPerRow * (hfHeight - 2)]);

		rc_hf.vertices[hfWidth * (hfHeight - 1)].Normal = calcVertNormal(fb);

		fb.clear();

		//Each last row element
		unsigned int k = 0;
		for (unsigned int i = hfWidth * (hfHeight - 1) + 1; i < (hfWidth * (hfHeight)) - 2; i++)
		{
			fb.clear();

			fb.push_back(hmFaces[facesPerRow * (hfHeight - 2) + (k * 2)]);
			fb.push_back(hmFaces[facesPerRow * (hfHeight - 2) + (k * 2) + 1]);
			fb.push_back(hmFaces[facesPerRow * (hfHeight - 2) + (k * 2) + 2]);

			rc_hf.vertices[i].Normal = calcVertNormal(fb);

			k++;
		}

		//last element of first row (special case)
		fb.clear();

		fb.push_back(hmFaces[facesPerRow * (hfHeight - 2) + ((k - 1) * 2) + 2]);
		fb.push_back(hmFaces[facesPerRow * (hfHeight - 2) + ((k - 1) * 2) + 3]); //#Check this one again!!!

		rc_hf.vertices[(hfWidth * (hfHeight)) - 2].Normal = calcVertNormal(fb);

		fb.clear();

	}

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
	)
	{
		rc_hf.clear();
		//auto& texData = COORD.GetComponentDataFromEntity<c_Texture>(hfEntity);
		auto& modified = COORD.GetComponentDataFromEntity<c_Modified>(hfEntity);
		
		modified.isModifed = true;
		glActiveTexture(GL_TEXTURE0 + hf_originalTexUnit);
		glBindTexture(GL_TEXTURE_2D, heightFieldTex);

		std::vector<float> data(hfWidth * hfHeight * 4);
		std::vector<float> noiseData(hfWidth * hfHeight * 4);

		//std::cout << "\n ------------- Data Size = " << data.size() << "\n" << std::endl;

		std::random_device rd; // Non-deterministic random number generator
		std::mt19937 gen(rd()); // Mersenne Twister engine seeded with rd()

		// Define a uniform real distribution in the range [0.0, 255.0]
		std::uniform_real_distribution<> dis(0.0, 255.0);

		for (int y = 0; y < hfWidth; ++y)
		{
			for (int x = 0; x < hfHeight; ++x)
			{
				int index = (y * hfWidth + x) * 4;

				float fx = static_cast<float>(x + SEED) / hfWidth;
				float fy = static_cast<float>(y + SEED) / hfHeight;

				//fx = fx * frequency;
				//fy = fy * frequency;

				//FbmNoise2D(fx, fy, frequency, 4, 2.0f, 0.5f, 1.0f, 0.0f);

				//Normal
				float pnoise = FbmNoise2D(fx, fy, frequency, octaves, lacunarity, persistence, amplitude, maxAmplitude);

				//Shell Texturing test version:
				//float pnoise = FbmNoise2D(fx, fy, 0, octaves, lacunarity, persistence, amplitude, maxAmplitude);

				pnoise = (pnoise + 1.0f) / 2.0f;

				noiseData[index] = pnoise;
				noiseData[index + 1] = pnoise;
				noiseData[index + 2] = pnoise;
				noiseData[index + 3] = 1.0f;
			}
		}

		//Store perlin noise in OpenGL texture.
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, hfWidth, hfHeight, GL_RGBA, GL_FLOAT, data.data());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, hfWidth, hfHeight, GL_RGBA, GL_FLOAT, noiseData.data());


		std::vector<Face> hmFaces;

		float x_texPoint = 1.0f / (hfWidth);// / 6);
		float y_texPoint = 1.0f / (hfHeight);// / 6);

		for (unsigned int z = 0; z < hfHeight; z++)
		{
			for (unsigned int x = 0; x < hfWidth; x++)
			{
				unsigned int index = (x + z * hfWidth) * 4;
				Vertex vert;

				float y = noiseData[index];

				//Scale by 5 (value is between 0 and 1)
				y = y * 30.0f;

				vert.Position.x = x;
				vert.Position.y = y;
				vert.Position.z = z;

				vert.TexCoords.x = x_texPoint * x;
				vert.TexCoords.y = y_texPoint * z;

				rc_hf.vertices.push_back(vert);
			}

		}

		//Fill Height map indices data and hm face data:
		for (unsigned int i = 0; i < (hfHeight - 1); i++)
		{
			for (unsigned int ii = 0; ii < (hfWidth - 1) * 6; ii = ii + 6)
			{

				unsigned int val = 0;
				Face face1;
				Face face2;

				val = i * hfWidth + ii / 6;
				rc_hf.indices.push_back(val);
				face1.in_1 = val;

				val = (i + 1) * hfWidth + ii / 6;
				rc_hf.indices.push_back(val);
				face1.in_2 = val;

				val = ((i + 1) * hfWidth + ii / 6) + 1;
				rc_hf.indices.push_back(val);
				face1.in_3 = val;

				glm::vec3 AB = rc_hf.vertices[face1.in_2].Position - rc_hf.vertices[face1.in_1].Position;
				glm::vec3 AC = rc_hf.vertices[face1.in_3].Position - rc_hf.vertices[face1.in_1].Position;

				face1.faceNormal = glm::normalize(glm::cross(AB, AC));


				val = i * hfWidth + ii / 6;
				rc_hf.indices.push_back(val);
				face2.in_1 = val;

				val = ((i + 1) * hfWidth + ii / 6) + 1;
				rc_hf.indices.push_back(val);
				face2.in_2 = val;

				val = (i * hfWidth + ii / 6) + 1;
				rc_hf.indices.push_back(val);
				face2.in_3 = val;

				AB = rc_hf.vertices[face2.in_2].Position - rc_hf.vertices[face2.in_1].Position;
				AC = rc_hf.vertices[face2.in_3].Position - rc_hf.vertices[face2.in_1].Position;

				face2.faceNormal = glm::normalize(glm::cross(AB, AC));

				hmFaces.push_back(face1);
				hmFaces.push_back(face2);

			}
		}

		//First element (of first row - special case)
		std::vector<Face> fb;
		fb.push_back(hmFaces[0]);
		fb.push_back(hmFaces[1]);

		rc_hf.vertices[0].Normal = calcVertNormal(fb);

		fb.clear();

		for (unsigned int i = 1; i < hfWidth - 1; i++)
		{
			fb.clear();

			fb.push_back(hmFaces[(i * 2) + 1]);
			fb.push_back(hmFaces[(i * 2) + 2]);
			fb.push_back(hmFaces[(i * 2) + 3]);

			rc_hf.vertices[i].Normal = calcVertNormal(fb);
		}

		//last element of first row (special case)
		fb.clear();

		fb.push_back(hmFaces[hfWidth - 1]);

		rc_hf.vertices[hfWidth - 1].Normal = calcVertNormal(fb);

		fb.clear();

		unsigned int facesPerRow = (hfWidth * 2) - 2;

		for (unsigned int i = 1; i < hfHeight - 1; i++)
		{
			//Handle first and last elements of each row here
			fb.clear();

			fb.push_back(hmFaces[(i - 1) * facesPerRow]);
			fb.push_back(hmFaces[(i)*facesPerRow]);
			fb.push_back(hmFaces[(i * facesPerRow) + 1]);

			rc_hf.vertices[i * hfWidth].Normal = calcVertNormal(fb);

			fb.clear();

			for (unsigned int ii = 1; ii < hfWidth - 1; ii++)
			{
				fb.clear();

				fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow)]); //0
				fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow) + 1]); //1
				fb.push_back(hmFaces[((ii - 1) * 2) + ((i - 1) * facesPerRow) + 2]); //2

				fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 1]);
				fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 2]);
				fb.push_back(hmFaces[((ii - 1) * 2) + ((i)*facesPerRow) + 3]);

				rc_hf.vertices[(i * hfWidth) + ii].Normal = calcVertNormal(fb);

			}

			fb.clear();

			fb.push_back(hmFaces[((i)*facesPerRow) - 1]);
			fb.push_back(hmFaces[((i)*facesPerRow) - 2]);
			fb.push_back(hmFaces[((i + 1) * facesPerRow) - 1]);

			rc_hf.vertices[((i + 1) * hfWidth) - 1].Normal = calcVertNormal(fb);

			fb.clear();

		}

		//Last Row Elements:
		fb.clear();

		//First last town vertex:
		fb.push_back(hmFaces[facesPerRow * (hfHeight - 2)]);

		rc_hf.vertices[hfWidth * (hfHeight - 1)].Normal = calcVertNormal(fb);

		fb.clear();

		//Each last row element
		unsigned int k = 0;
		for (unsigned int i = hfWidth * (hfHeight - 1) + 1; i < (hfWidth * (hfHeight)) - 2; i++)
		{
			fb.clear();

			fb.push_back(hmFaces[facesPerRow * (hfHeight - 2) + (k * 2)]);
			fb.push_back(hmFaces[facesPerRow * (hfHeight - 2) + (k * 2) + 1]);
			fb.push_back(hmFaces[facesPerRow * (hfHeight - 2) + (k * 2) + 2]);

			rc_hf.vertices[i].Normal = calcVertNormal(fb);

			k++;
		}

		//last element of first row (special case)
		fb.clear();

		fb.push_back(hmFaces[facesPerRow * (hfHeight - 2) + ((k - 1) * 2) + 2]);
		fb.push_back(hmFaces[facesPerRow * (hfHeight - 2) + ((k - 1) * 2) + 3]); //#Check this one again!!!

		rc_hf.vertices[(hfWidth * (hfHeight)) - 2].Normal = calcVertNormal(fb);

		/*
		float tallestheight = -100.0f;
		float smallestHeight = 100.0f;

		
		for(int i = 0; i < rc_hf.vertices.size(); ++i)
		{
			if(rc_hf.vertices[i].Position.y > tallestheight)
			{
				tallestheight = rc_hf.vertices[i].Position.y;
			}

			if(rc_hf.vertices[i].Position.y < smallestHeight)
			{
				smallestHeight = rc_hf.vertices[i].Position.y;
			}
			
		}
		

		std::cout << "tallest height = " << tallestheight << std::endl;
		std::cout << "smallest height = " << smallestHeight << std::endl;

		*/
		fb.clear();


	}

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
		)
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
		entities.push_back(COORD.CreateEntity());
		entities.push_back(COORD.CreateEntity());

		std::cout << "\n - entities.size() = " << entities.size() << std::endl;

		for (int i = 0; i < entities.size(); ++i)
		{
			allEntites.push_back(entities[i]);
		}

		unsigned short int hfTexUnit = COORD.GenerateTexUnit("res/textures/container2.png", "png");		 // tx Unit = 0
		//allTexUnits.push_back(hfTexUnit); //Done in setupHeightField function

		glGenTextures(1, &heightFieldTex);
		glBindTexture(GL_TEXTURE_2D, heightFieldTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hfWidth, hfHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// -- HERE --
		c_Renderable rc_hf; //#HERE

		/*
		setupHeightField(
			rc_hf,
			allTexUnits,
			hfTexUnit,
			hfWidth,
			hfHeight,
			frequency,
			SEED
		);
		*/

		setupHeightField(
			rc_hf,
			allTexUnits,
			hfTexUnit,
			hfWidth,
			hfHeight,
			frequency,
			SEED
		);

		//resetHF(COORD, rc_hf, entities[5], heightFieldTex, hfTexUnit, hfWidth, hfHeight, SEED);
		
		unsigned short int rockySurfaceTexUnit = COORD.GenerateTexUnit("res/textures/rockySurface.png", "png");	 // tx Unit = 1
		allTexUnits.push_back(rockySurfaceTexUnit);
		unsigned short int waterTexUnit = COORD.GenerateTexUnit("res/textures/water.jpg", "jpg");				 // tx Unit = 2
		allTexUnits.push_back(waterTexUnit);
		unsigned short int grassTexUnit = COORD.GenerateTexUnit("res/textures/grass.png", "png");				 // tx Unit = 3
		allTexUnits.push_back(grassTexUnit);
		unsigned short int lavaTexUnit = COORD.GenerateTexUnit("res/textures/lava.jpg", "jpg");					 // tx Unit = 4
		allTexUnits.push_back(lavaTexUnit);
		unsigned short int redWindowTexUnit = COORD.GenerateTexUnit("res/textures/grassColor3.png", "png");		 // tx Unit = 5
		allTexUnits.push_back(redWindowTexUnit);
		unsigned short int cloudNoiseTextureUnit = COORD.GenerateTexUnit("res/textures/awesomeface.png", "png"); // tx Unit = 6
		allTexUnits.push_back(cloudNoiseTextureUnit);


		c_Transform tr_0;
		c_Transform tr_1;
		c_Transform tr_2;
		c_Transform tr_3;
		c_Transform tr_4;
		c_Transform tr_hf;
		c_Transform tr_sun;

		//tr_sun
		glm::mat4 mm_tr_sun = glm::mat4(1.0f);
		glm::vec3 pos_tr_sun(0.0f, 6.0f, 0.0f);
		glm::vec3 scale_tr_sun(0.25f, 0.25f, 0.25f);
		mm_tr_sun = glm::translate(mm_tr_sun, pos_tr_sun);
		mm_tr_sun = glm::scale(mm_tr_sun, scale_tr_sun);
		tr_sun.modelMat.push_back(mm_tr_sun);

		//tr_0
		glm::mat4 mm_tr0 = glm::mat4(1.0f);
		glm::vec3 pos_tr0(0.0f, -2.2f, 0.0f);
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
		glm::vec3 pos_tr2(-1.2f, -2.2f, -6.0f);
		glm::vec3 scale_tr2(1.0f, 1.0f, 1.0f);
		mm_tr2 = glm::translate(mm_tr2, pos_tr2);
		mm_tr2 = glm::scale(mm_tr2, scale_tr2);
		tr_2.modelMat.push_back(mm_tr2);

		//tr_3
		glm::mat4 mm_tr3 = glm::mat4(1.0f);
		glm::vec3 pos_tr3(-2.5f, -2.2f, -0.48f);
		glm::vec3 scale_tr3(1.0f, 1.0f, 1.0f);
		mm_tr3 = glm::translate(mm_tr3, pos_tr3);
		mm_tr3 = glm::scale(mm_tr3, scale_tr3);
		tr_3.modelMat.push_back(mm_tr3);

		//tr_4
		glm::mat4 mm_tr4 = glm::mat4(1.0f);
		glm::vec3 pos_tr4(-4.5f, -2.2f, -0.48f);
		glm::vec3 scale_tr4(1.0f, 1.0f, 1.0f);
		mm_tr4 = glm::translate(mm_tr4, pos_tr4);
		mm_tr4 = glm::scale(mm_tr4, scale_tr4);
		tr_4.modelMat.push_back(mm_tr4);

		//tr_hf //#Check_if_Correct
		glm::mat4 ShiftUpOneMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.02f, 0.0f));

		//16 layers at present

		glm::mat4 mm_tr_hf = glm::mat4(1.0f);
		glm::vec3 pos_tr_hf(0.0f, -7.2f, 0.0f);
		glm::vec3 scale_tr_hf(0.2f, 0.2f, 0.2f);
		mm_tr_hf = glm::translate(mm_tr_hf, pos_tr_hf);
		mm_tr_hf = glm::scale(mm_tr_hf, scale_tr_hf);
		tr_hf.modelMat.push_back(mm_tr_hf);

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));   

		// halfway:

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		mm_tr_hf = ShiftUpOneMatrix * mm_tr_hf;
		tr_hf.modelMat.push_back((mm_tr_hf));

		size_t sizeOfVertCubePosData = sizeof(vertCubePosData) / sizeof(float);
		size_t sizeOfIndices = sizeof(indices) / sizeof(unsigned int);

		size_t sizeOfVerticalQuad = sizeof(verticalQuad) / sizeof(float);
		size_t sizeOfVQIndices = sizeof(vertQuadIndices) / sizeof(unsigned int);

		c_Renderable rc_sun;
		rc_sun.isLightEmitter = true;
		addDataToRenderable(rc_sun, vertCubePosData, vertCubeNormData, vertCubeTexCoordData, indices, sizeOfVertCubePosData, sizeOfIndices);
		rc_sun.outline = false;

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

		//We need to generate a VBO based on the HF data here:

		//c_Renderable rc_grass;

		c_Texture tx_0;
		tx_0.texUnit = hfTexUnit;

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

		c_Modified md_sun;
		md_sun.isModifed = true;

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

		c_Modified md_hf;
		md_hf.isModifed = true;


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

		c_EntityInfo ei_sun;
		ei_sun.name = "Sun Object";

		c_EntityInfo ei_1;
		ei_1.name = "Floor";

		c_EntityInfo ei_2;
		ei_2.name = "Wall cube";

		c_EntityInfo ei_3;
		ei_3.name = "Grass Billboard 1";

		//c_EntityInfo ei_4;
		//ei_4.name = "red Window";

		c_EntityInfo ei_hf;
		ei_hf.name = "Height Field";

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

		COORD.AddComponentToEntity<c_Transform>(entities[1], tr_sun);
		COORD.AddComponentToEntity<c_Renderable>(entities[1], rc_sun);
		//COORD.AddComponentToEntity<c_Texture>(entities[1], tx_1);
		//COORD.AddComponentToEntity<c_AABB>(entities[1], aabb_0);
		//COORD.AddComponentToEntity<c_WallCollider>(entities[1], wallCollider_2);
		COORD.AddComponentToEntity<c_EntityInfo>(entities[1], ei_sun);
		COORD.AddComponentToEntity<c_Modified>(entities[1], md_sun);
		COORD.SetUpRenderData(entities[1]);
		COORD.setShaderForEntity(entities[1], shaders[5]);
		COORD.StoreShaderInEntityDataHandle(entities[1]);

		COORD.AddComponentToEntity<c_Transform>(entities[2], tr_2);
		COORD.AddComponentToEntity<c_Renderable>(entities[2], rc_0);
		COORD.AddComponentToEntity<c_Texture>(entities[2], tx_2);
		COORD.AddComponentToEntity<c_AABB>(entities[2], aabb_2);
		COORD.AddComponentToEntity<c_Wall>(entities[2], wall_0);
		COORD.AddComponentToEntity<c_Modified>(entities[2], md_2);
		COORD.AddComponentToEntity<c_EntityInfo>(entities[2], ei_2);
		COORD.SetUpRenderData(entities[2]);
		COORD.setShaderForEntity(entities[2], shaders[0]);
		COORD.StoreShaderInEntityDataHandle(entities[2]);

		COORD.AddComponentToEntity<c_Transform>(entities[3], tr_0);
		COORD.AddComponentToEntity<c_Renderable>(entities[3], rc_1);
		COORD.AddComponentToEntity<c_Texture>(entities[3], tx_3);
		COORD.AddComponentToEntity<c_AABB>(entities[3], aabb_0);
		COORD.AddComponentToEntity<c_WallCollider>(entities[3], wallCollider_2);
		COORD.AddComponentToEntity<c_EntityInfo>(entities[3], ei_0);
		COORD.AddComponentToEntity<c_Modified>(entities[3], md_0);
		COORD.SetUpRenderData(entities[3]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
		COORD.setShaderForEntity(entities[3], shaders[0]); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
		COORD.StoreShaderInEntityDataHandle(entities[3]);

		COORD.AddComponentToEntity<c_Transform>(entities[4], tr_3);
		COORD.AddComponentToEntity<c_Renderable>(entities[4], rc_3);
		COORD.AddComponentToEntity<c_Texture>(entities[4], tx_3);
		COORD.AddComponentToEntity<c_EntityInfo>(entities[4], ei_3);
		COORD.AddComponentToEntity<c_Modified>(entities[4], md_3);
		COORD.SetUpRenderData(entities[4]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
		COORD.setShaderForEntity(entities[4], shaders[0]); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
		COORD.StoreShaderInEntityDataHandle(entities[4]);

		skydome.CreateSkydome(16, 16, 8, glm::vec3(-3.35f, 2.25f, 2.95f), glm::vec3(0.8f, 0.45f, 0.8f));
		//skydome.setSkydomeTransform(glm::vec3(1.0f, 10.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

		COORD.AddComponentToEntity<c_Transform>(entities[5], skydome.GetComponent_Transform());
		COORD.AddComponentToEntity<c_Renderable>(entities[5], skydome.GetComponent_Renderable());
		COORD.AddComponentToEntity<c_Texture>(entities[5], tx_3/*skydome.GetComponent_Texture()*/);
		COORD.AddComponentToEntity<c_EntityInfo>(entities[5], skydome.GetComponent_EntityInfo());
		COORD.AddComponentToEntity<c_Modified>(entities[5], skydome.GetComponent_Modified());
		COORD.SetUpRenderData(entities[5]); //#NOTE: SetUpRenderData and setShaderForEntity will do nothing if the entity does no have a c_RenderableComponent
		COORD.setShaderForEntity(entities[5], shaders[0]); //#C_NOTE: Will need to set the map but not the DH, that needs to be done separatly by the renderer.
		COORD.StoreShaderInEntityDataHandle(entities[5]);

		COORD.AddComponentToEntity<c_Transform>(entities[6], tr_hf);
		COORD.AddComponentToEntity<c_Renderable>(entities[6], rc_hf);
		COORD.AddComponentToEntity<c_Texture>(entities[6], tx_0);
		//COORD.AddComponentToEntity<c_AABB>(entities[1], aabb_0);
		//COORD.AddComponentToEntity<c_WallCollider>(entities[1], wallCollider_2);
		COORD.AddComponentToEntity<c_EntityInfo>(entities[6], ei_hf);
		COORD.AddComponentToEntity<c_Modified>(entities[6], md_hf);
		COORD.SetUpRenderData(entities[6]);
		COORD.setShaderForEntity(entities[6], shaders[3]);
		COORD.StoreShaderInEntityDataHandle(entities[6]);

		//Loaded Models:
		glm::mat4 ES0_mm = glm::mat4(1.0f);
		glm::vec3 ES0_pos(-3.0f, -2.8f, 7.7f);
		glm::vec3 ES0_scale(0.3f, 0.3f, 0.3f);
		ES0_mm = glm::translate(ES0_mm, ES0_pos);
		float angleY = glm::radians(180.0f);
		float angleX = glm::radians(-13.0f);
		ES0_mm = glm::rotate(ES0_mm, angleY, glm::vec3(0, 1, 0));
		ES0_mm = glm::rotate(ES0_mm, angleX, glm::vec3(1, 0, 0));
		ES0_mm = glm::scale(ES0_mm, ES0_scale);

		map_SceneNameToEntitiyScene["Backpack_1"] = sceneFactory->CreateEntityScene("res/models/backpack/", "backpack.obj", ES0_mm, shaders[0], 1);
		//std::shared_ptr<EntityScene> ES_0 = sceneFactory->CreateEntityScene("res/models/backpack/", "backpack.obj", ES0_mm, sh_shadows, 1);
		map_SceneEntites["Backpack_1"] = map_SceneNameToEntitiyScene["Backpack_1"]->GetSceneEntities();
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

std::vector<glm::vec2> generateUniformVectors(int num_vectors) {
	std::vector<glm::vec2> vectors;
	float angle_step = 2 * M_PI / num_vectors;  // radians
	for (int i = 0; i < num_vectors; ++i)
	{
		float angle = i * angle_step;
		vectors.emplace_back(cos(angle), sin(angle));
	}
	return vectors;
}

//Here we are using pure random gradients, usually we want to use a set of uniform gradients that we RANDOMLY sample from.
glm::vec2 randomGradient(int ix, int iy, unsigned seed)
{
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;
	unsigned a = ix + seed;
	unsigned b = iy + seed;

	a *= 3284157443;

	b ^= a << s | a >> w - s;
	b *= 1911520717;

	a ^= b << s | b >> w - s;
	a *= 2048419325;
	float random = a * (3.14159265 / ~(~0u >> 1));

	glm::vec2 v;
	v.x = sin(random);
	v.y = cos(random);

	return v;
}

glm::vec2 randomGradient_2(int ix, int iy, const std::vector<glm::vec2>& vectors, unsigned seed)
{
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;
	unsigned a = ix + seed;
	unsigned b = iy + seed;

	a *= 3284157443;

	b ^= a << s | a >> w - s;
	b *= 1911520717;

	a ^= b << s | b >> w - s;
	a *= 2048419325;

	a = a % 8;

	return vectors[a];
}

float interpolate(float a0, float a1, float w)
{
	return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float dotGridGradient(int ix, int iy, float x, float y, const std::vector<glm::vec2> vectors, unsigned seed)
{
	glm::vec2 gradient = randomGradient(ix, iy, seed);
	//glm::vec2 gradient = randomGradient_2(ix, iy, vectors, seed);

	float dx = x - (float)ix;
	float dy = y - (float)iy;

	//Compute the dot product
	return (dx * gradient.x + dy * gradient.y);
}

float perlin(float x, float y, const std::vector<glm::vec2> vectors, unsigned seed)
{
	int x0 = (int)x;
	int y0 = (int)y;

	int x1 = x0 + 1;
	int y1 = y0 + 1;

	//Compute interpolation weights:
	float sx = x - (float)x0;
	float sy = y - (float)y0;

	//compute an interpolate the two top corners:
	float n0 = dotGridGradient(x0, y0, x, y, vectors, seed);
	float n1 = dotGridGradient(x1, y0, x, y, vectors, seed);
	float ix0 = interpolate(n0, n1, sx);

	//compute and interpolate the bottom 2 corners:
	n0 = dotGridGradient(x0, y1, x, y, vectors, seed);
	n1 = dotGridGradient(x1, y1, x, y, vectors, seed);
	float ix1 = interpolate(n0, n1, sx);

	//interpolate between the two previous values:
	float value = interpolate(ix0, ix1, sy);

	return value;
}

void GeneratePerlinNoise(std::vector<float>& data, int width, int height, unsigned int SEED)
{
	const unsigned seed = SEED;
	const int GRID_SIZE = 50;

	std::vector<glm::vec2> vectors = generateUniformVectors(8);

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int index = (y * width + x) * 4;

			float val = 0;

			float freq = 1;
			float amp = 1;

			for (int i = 0; i < 12; i++)
			{
				val += perlin(x * freq / GRID_SIZE, y * freq / GRID_SIZE, vectors, seed) * amp;

				freq *= 2;
				amp /= 2;
			}

			//contrast
			val *= 1.2;

			//clamping:
			//val = std::min(std::max(val, 1.0f), 1.0f);
			if (val > 1.0f)
			{
				val = 1.0f;
			}
			else if (val < -1.0f)
			{
				val = -1.0f;
			}

			int color = (int)(((val + 1.0f) * 0.5f) * 255);
			float finalColor = color;
			finalColor = scaleToRange(color, 0.0f, 255.0f);

			data[index] = finalColor;
			data[index + 1] = finalColor;
			data[index + 2] = finalColor;

			data[index + 3] = 1.0f;

			int xy = x * y;

			if (xy > height * width - 1)
			{
				std::cout << "What is going on?" << std::endl;
			}

		}
	}
}