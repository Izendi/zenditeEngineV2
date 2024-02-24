#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ECS/Components.h"

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

void genMenu_1(c_Transform& posData, c_Texture& texData, c_Modified& modified, short int containerTexUnit, unsigned short int rockySurfaceTexUnit, c_AABB& aabb)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Here, you can start using ImGui to create interfaces
	ImGui::Begin("Hello, world!");
	ImGui::Text("This is some useful text.");

	ImGui::Separator();

	if (ImGui::SliderFloat3("My Vec3 Slider", &(posData.pos[0])[0], -5.0f, 5.0f));
	{
		// The slider was used; myVec3 has been updated.
		// You can handle the change here if needed.
	}

	if (ImGui::SliderFloat3("AABB scale", &aabb.scale[0], -5.0f, 5.0f));
	{
		// The slider was used; myVec3 has been updated.
		// You can handle the change here if needed.
	}

	ImGui::NewLine();
	
	ImGui::Text("Choose a Texture:");
	if (ImGui::RadioButton("Wooden Box", selectedOption == 0)) 
	{
		selectedOption = 0;
		if (selectedOption != lastSelectedOption)
		{
			
			texData.texUnit = containerTexUnit;
			modified.isModifed = true;
			lastSelectedOption = selectedOption;
			std::cout << selectedOption << std::endl;
		}
	}
	if (ImGui::RadioButton("Rocky Surface", selectedOption == 1)) 
	{
		selectedOption = 1;
		if (selectedOption != lastSelectedOption)
		{
			
			texData.texUnit = rockySurfaceTexUnit;
			modified.isModifed = true;
			lastSelectedOption = selectedOption;
			std::cout << selectedOption << std::endl;
		}
	}

	ImGui::NewLine();
	ImGui::NewLine();

	//Radio Button Testing:
	/*
	if (ImGui::RadioButton("Label", &variable, value)) {
		// Action to take when this radio button is selected
	}
	*/

	ImGui::End();


	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



}

