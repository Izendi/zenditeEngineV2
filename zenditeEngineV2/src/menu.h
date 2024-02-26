#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Coordinator.h"
#include "ECS/Components.h"

#include <cstdlib>


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

void genMenu_1(std::vector<Entity>& entities, Coordinator& COORD, short int containerTexUnit, unsigned short int rockySurfaceTexUnit)
{


    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Here, you can start using ImGui to create interfaces




    const char* names[] = { "cube", "cuboid" };

    if (ImGui::Begin("Entities", nullptr))
    {
        
        ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);

        static int selected = 0;
        {
            ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
            for (int i = 0; i < entities.size(); i++)
            {
                // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
                char label[50];
                std::string name = COORD.GetComponentDataFromEntity<c_EntityInfo>(entities[i]).name;

                sprintf_s(label, "Cube: %s", name.c_str());
                if (ImGui::Selectable(label, selected == i))
                    selected = i;
            }
            ImGui::EndChild();


        }
        ImGui::SameLine();

        // Right
        {
            auto& posData = COORD.GetComponentDataFromEntity<c_Transform>(entities[selected]);
            auto& texData = COORD.GetComponentDataFromEntity<c_Texture>(entities[selected]);
            auto& modified = COORD.GetComponentDataFromEntity<c_Modified>(entities[selected]);

            auto& infoData = COORD.GetComponentDataFromEntity<c_EntityInfo>(entities[selected]);
            auto& aabb = COORD.GetComponentDataFromEntity<c_AABB>(entities[selected]);

            ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            ImGui::Text("Cube: %d", selected); 
            ImGui::Separator();
            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Description"))
                {
                    short int bitSetPos = COORD.GetComponentBitsetPos<c_RenderableComponent>();
                    std::bitset<32> renderableBitset; // Create a bitset of size 32
                    renderableBitset.set(bitSetPos);

                    std::bitset<32> entitySig = COORD.GetEntitySignature(entities[selected]);

                    if ((entitySig & renderableBitset) == renderableBitset) // If this entity has a texture component
                    {
                        // Entity is renderable
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 0.8f), "Entity is renderable");
                       
                        
                    }
                    else
                    {
                        //entity is not renderable
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Entity is not renderable");
                    }
                    

                    ImGui::SeparatorText("Transform:");

                    ImGui::Separator;


                    if (ImGui::InputFloat3("Position XYZ", & posData.pos[0]))  //Position
                    {
                      
                        // The slider was used; myVec3 has been updated.
                        // You can handle the change here if needed.
                    }

                    if (ImGui::InputFloat3("Scale XYZ", &posData.scale[0]));  //Scale
                    {
                        // The slider was used; myVec3 has been updated.
                        // You can handle the change here if needed.
                    }

                    ImGui::SeparatorText("Collider box:");

                    if (ImGui::SliderFloat3("AABB scale", &aabb.scale[0], -5.0f, 5.0f));
                    {
                        // The slider was used; myVec3 has been updated.
                        // You can handle the change here if needed.
                    }

                    ImGui::EndTabItem();

                    ImGui::NewLine();

                    // We were trying to add custom texture here via openLocalRepository() but it didn't work

                    ImGui::SeparatorText("Texture:");

                    // Simple selection popup (if you want to show the current selection inside the Button itself,
                    // you may want to build a string using the "###" operator to preserve a constant ID with a variable label)

                    static int selected_fish = -1;
                    const char* names[] = { "Wooden planks", "Rock" };
                    static bool toggles[] = { true, false };

                    if (ImGui::Button("Select texture"))
                        ImGui::OpenPopup("my_select_popup");
                    ImGui::SameLine();
                    ImGui::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);
                    if (ImGui::BeginPopup("my_select_popup"))
                    {

                        ImGui::SeparatorText("Default textures");
                        for (int i = 0, j = 0; i < IM_ARRAYSIZE(names); i++)
                        {
                            if (ImGui::Selectable(names[i])) {
                                selected_fish = i;
                                switch (i) {
                                case 0:
                                    texData.texUnit = containerTexUnit;
                                    modified.isModifed = true;
                                    break;

                                case 1:
                                    texData.texUnit = rockySurfaceTexUnit;
                                    modified.isModifed = true;
                                    break;

                                default:
                                    break;
                                }
                            }

                            std::cout << selectedOption << std::endl;

                        }

                        ImGui::EndPopup();
                    }
                }
                if (ImGui::BeginTabItem("Details"))
                {
                    ImGui::Text("ID: 0123456789");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndChild();
            if (ImGui::Button("Add")) {}
            ImGui::SameLine();
            if (ImGui::Button("Delete")) {}
            ImGui::EndGroup();
        }
        ImGui::End();
    }
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