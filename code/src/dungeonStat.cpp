#include "dungeonStat.h"
#include "dungeonView.h"
#include "graph.h"
#include <thread>
#include <iostream>

DungeonStat::DungeonStat(std::string path,std::string dunname,bool isGenerated,bool hasToGeneratePath){
    this->dunpath = path;
    this->dunname = dunname;
    this->isGenerated = isGenerated;
    this->hasToGeneratePath = hasToGeneratePath;
}

void DungeonStat::Import(){
    std::cout << "importing: " << dunpath << std::endl;
    bool dungeonImported = file2Dun(&currentDungeon,dunpath);

    if(dungeonImported&&currentDungeon!=nullptr)
    {
        std::cout << "file readed succesfully" << std::endl;
        std::cout << "Imported dungeon:" << std::endl;
        std::cout << "Size X: " << currentDungeon->size_x << std::endl;
        std::cout << "Size Y: " << currentDungeon->size_y << std::endl;
        std::cout << "Size Z: " << currentDungeon->size_z << std::endl;
    }
    if((view!=nullptr&&view->open)||view==nullptr){ // Create and open a new view if it doesnt exist, or update if its open
        hasToUpdateView = true;
    }
    statsGenerated = false;
    pathGenerated = false;
}

void DungeonStat::Setup(){}

void DungeonStat::Show(){
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));
    std::string windowName = dunname + " Stats";
    if (ImGui::Begin(windowName.c_str(), &open)){
        if(statsGenerated){

            ImGui::Text("Wall count:\t%i",wallCount);
            ImGui::Text("Floor count:\t%i",floorCount);
            ImGui::Text("Linear distance S/G:\t%f",linearDistanceStartGoal);
            if(isGenerated){
                ImGui::Text("Generation time:\t%fs",dungeonTime);
            }
            if(pathGenerated)
                ImGui::Text("Step count S/G:\t%i",dungeonPath.GetLength());
        }

        

        if(ImGui::Button("View")){
            hasToUpdateView = true;
        }
        ImGui::BeginDisabled(pathGenerated);
        
        if(ImGui::Button("Resolve")){
            hasToGeneratePath = true;
        }
        
        ImGui::EndDisabled();
        ImGui::BeginDisabled(!pathGenerated);
        ImGui::SameLine();
        ImGui::Checkbox("Show path",&showPath);
        ImGui::EndDisabled();
        ImGui::IsItemActive();
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void DungeonStat::Update(){
    if(!statsGenerated) GenerateStats();
    if(!pathGenerated&&hasToGeneratePath&&!generatingPath) {
        std::cout << "Generate path" << std::endl;
        generatingPath = true;
        std::jthread generatePathThread([this](){GeneratePath();});
        // new std::jthread(GeneratePath);
        // GeneratePath();
    }
}

void DungeonStat::Shutdown(){
    freeDungeonMatrix(&currentDungeon);
}

void DungeonStat::UpdateViews(std::vector<DungeonView*> & views){
    if(hasToUpdateView){
        if(view == nullptr){
            view = new DungeonView(dunname,this);
            views.push_back(view);
            view->Setup();
        }
        view->open = true;
    }
}

void DungeonStat::GenerateStats(){
    if(currentDungeon==nullptr) return;
    wallCount = 0;
    floorCount = 0;

    for(int i = 0; i < currentDungeon->size_x;i++){
        for(int j = 0; j < currentDungeon->size_y;j++){
            for(int k = 0; k < currentDungeon->size_z;k++){
                unsigned tile = currentDungeon->data[i][j][k];
                if(tile&DUN_PXY_WALL) wallCount++;
                if(tile&DUN_PYZ_WALL) wallCount++;
                if(tile&DUN_PXZ_WALL) floorCount++;
            }
        }
    }

    start = {(float)currentDungeon->start_x,(float)currentDungeon->start_y,(float)currentDungeon->start_z};
    end = {(float)currentDungeon->end_x,(float)currentDungeon->end_y,(float)currentDungeon->end_z};
    linearDistanceStartGoal = Vector3Length( Vector3Subtract(end,start));

    statsGenerated = true;
}

void DungeonStat::GeneratePath(){
    std::cout << "Generating path in new thread" << std::endl;
    Pathfinder finder(currentDungeon);

    dungeonPath = finder.AStar(
        {(int)currentDungeon->start_x,(int)currentDungeon->start_y,(int)currentDungeon->start_z},
        {(int)currentDungeon->end_x,(int)currentDungeon->end_y,(int)currentDungeon->end_z});

    hasToGeneratePath = false;
    pathGenerated = true;
    generatingPath = false;
}

Vector3 DungeonStat::GetEnd(){
    return end;
}

Vector3 DungeonStat::GetStart(){
    return start;
}

void DungeonStat::DrawPath(Vector3 pos,Color color,float size,bool drawLast){
    if(showPath&&pathGenerated) dungeonPath.Draw(pos,color,size,drawLast);
}

std::string DungeonStat::GetName(){
    return dunname;
}