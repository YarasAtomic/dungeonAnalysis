#include <raylib.h>
#include <iostream>
#include "algorithmList.h"
#include "dungeonView.h"
#include "dungeonStat.h"
#include "dungeonList.h"

// #include "rlImGui.h"

void playerModeUpdate(dungeonMatrix *,Vector3,Vector2&,Vector3&);

AlgorithmList algorithmList;
DungeonList dungeonList;
std::vector<AlgorithmConfig*> algorithConfig;
std::vector<DungeonView*> dungeonView;
std::vector<DungeonStat*> dungeonStat;

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
			ImGui::MenuItem("Algorithm List", nullptr, &algorithmList.open);
            ImGui::MenuItem("Dungeon List", nullptr, &dungeonList.open);

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

    // Style setup
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding.x = 6;
    style.FramePadding.x = 6;

    // Default windows

    algorithmList.Setup();
    algorithmList.open = true;

    dungeonList.Setup();
    dungeonList.open = true;

    while (!WindowShouldClose()||IsKeyDown(KEY_ESCAPE)||quit) 
    {
        algorithmList.Update();
        dungeonList.Update();

        for(int i = 0; i < algorithConfig.size();i++){
            algorithConfig[i]->Update();
            algorithConfig[i]->UpdateStats(dungeonStat);
        }
        for(int i = 0; i < dungeonStat.size();i++){
            dungeonStat[i]->Update();
            dungeonStat[i]->UpdateViews(dungeonView);
        }
        for(int i = 0; i < dungeonView.size();i++){
            dungeonView[i]->Update();
        }

        algorithmList.UpdateAlgorithms(&algorithConfig);
        dungeonList.UpdateDungeons(&dungeonStat);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        rlImGuiBegin();

        DoMainMenu();

        if(imGuiDemoOpen)
            ImGui::ShowDemoWindow(&imGuiDemoOpen);

        if(algorithmList.open)
            algorithmList.ShowAsWindow();
            
        if(dungeonList.open)
            dungeonList.ShowAsWindow();

        for(int i = 0; i < algorithConfig.size();i++){
            if(algorithConfig[i]->open) algorithConfig[i]->ShowAsWindow();
        }

        for(int i = 0; i < dungeonStat.size();i++){
            if(dungeonStat[i]->open) dungeonStat[i]->ShowAsWindow();
        }

        for(int i = 0; i < dungeonView.size();i++){
            if(dungeonView[i]->open) dungeonView[i]->ShowAsWindow();
        }

        rlImGuiEnd();

        EndDrawing();

    }
    std::cout << "CLOSING" <<std::endl;

    rlImGuiShutdown();
    for(int i = 0; i < dungeonView.size();i++){
        dungeonView[i]->Shutdown();
    }
    for(int i = 0; i < algorithConfig.size();i++){
        algorithConfig[i]->Shutdown();
    }

    // Close
    CloseWindow();
}

int main()
{
    raylib();

    return 0;
}