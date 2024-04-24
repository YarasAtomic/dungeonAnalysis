#include <raylib.h>
#include <iostream>
#include "dungeonTab.h"
#include "algorithmList.h"
#include "dungeonView.h"
#include "dungeonStat.h"
#include "dungeonList.h"
#include "algorithmConfig.h"

// #include "rlImGui.h"

void playerModeUpdate(dungeonMatrix *,Vector3,Vector2&,Vector3&);

AlgorithmList algorithmList;
DungeonList dungeonList;

std::vector<DungeonTab*> dungeonTabs;

bool quit = false;
bool imGuiDemoOpen = false;

void DoMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
				quit = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("ImGui Demo", nullptr, &imGuiDemoOpen);
			// ImGui::MenuItem("Algorithm List", nullptr, &algorithmList.open);
            // ImGui::MenuItem("Dungeon List", nullptr, &dungeonList.open);

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void raylib()
{
    // Initialize the window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE|FLAG_MSAA_4X_HINT);
    int screenWidth = 1920;
	int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "raydungeon");
    SetTargetFPS(144); // Set framerate
    rlImGuiSetup(true);
	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Style setup
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding.x = 6;
    style.FramePadding.x = 6;

    // Default windows

    algorithmList.Setup();
    algorithmList.open = true;

    dungeonList.Setup();
    dungeonList.open = true;

    while ((!WindowShouldClose()||IsKeyDown(KEY_ESCAPE))&&!quit) 
    {
        algorithmList.Update();
        dungeonList.Update();

        algorithmList.UpdateDungeonTabs(&dungeonTabs);
        dungeonList.UpdateDungeonTabs(&dungeonTabs);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        rlImGuiBegin();

        DoMainMenu();

        // Updates -------
        auto it = dungeonTabs.begin();
        for (; it != dungeonTabs.end();){
            if((*it)->open){
                (*it)->Update();
                ++it;
            }else{
                (*it)->Shutdown();
                it = dungeonTabs.erase(it);
            }
        }

        // Demo -------

        if(imGuiDemoOpen)
            ImGui::ShowDemoWindow(&imGuiDemoOpen);

        // Show --------

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
        
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::Begin("##main",0, windowFlags);
        if(ImGui::BeginChild("Side Panel",ImVec2(200, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX)){
            algorithmList.ShowAsChild();
            dungeonList.ShowAsChild();
            ImGui::EndChild();
        }

        ImGui::SameLine();
        if(ImGui::BeginChild("Dungeon panel",ImVec2(0, 0), ImGuiChildFlags_Border)){
            if(ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None|ImGuiTabBarFlags_Reorderable)){
                for(int i = 0; i < dungeonTabs.size();i++){
                    
                    if (ImGui::BeginTabItem(dungeonTabs[i]->GetName().c_str(),&dungeonTabs[i]->open)){
                        dungeonTabs[i]->ShowAsChild();
                        ImGui::EndTabItem();
                    }
                }
                ImGui::EndTabBar();
            }
            ImGui::EndChild();
        }
        ImGui::End();

        rlImGuiEnd();

        EndDrawing();

    }
    std::cout << "CLOSING" <<std::endl;

    rlImGuiShutdown();


    for(int i = 0; i < dungeonTabs.size();i++){
        dungeonTabs[i]->Shutdown();
        delete dungeonTabs[i];
    }

    // Close
    CloseWindow();
}

int main()
{
    raylib();

    return 0;
}