#include "dungeonStat.h"
#include "dungeonView.h"
#include "graph.h"
#include "nfd.h"
#include <thread>
#include <iostream>
#include <fstream>

DungeonStat::DungeonStat(std::string path,std::string dunname,bool isGenerated,bool hasToGeneratePath){
    this->dunpath = path;
    this->dunname = dunname;
    this->isGenerated = isGenerated;
    this->hasToGeneratePath = hasToGeneratePath;
}

void DungeonStat::SaveStats(){
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_SaveDialog( "csv", "~", &outPath );
    if(result!=NFD_OKAY) return;
    std::cout << std::string(outPath) << std::endl;

    // file pointer 
    std::ofstream fout; 
  
    // opens an existing csv file or creates a new file. 
    fout.open(outPath,std::ios::trunc | std::ios::out); 

    for(int i = 0; i < statsBatch.size(); i++){
        fout 
            << statsBatch[i]->floorCount << "," 
            << statsBatch[i]->wallCount << "," 
            << statsBatch[i]->linearDistanceStartGoal << "," 
            << statsBatch[i]->dungeonPathLength << "," << std::endl;
    }

    fout.close();
  
}



void DungeonStat::Import(bool batchImport){
    batch = batchImport;
    if(!batch)
        std::cout << "importing: " << dunpath << std::endl;
    if(currentDungeon!=nullptr)
        freeDungeonMatrix(&currentDungeon);
    bool dungeonImported = file2Dun(&currentDungeon,dunpath);

    if(dungeonImported&&currentDungeon!=nullptr)
    {
        if(!batch){
            std::cout << "file readed succesfully" << std::endl;
            std::cout << "Imported dungeon:" << std::endl;
            std::cout << "Size X: " << currentDungeon->size_x << std::endl;
            std::cout << "Size Y: " << currentDungeon->size_y << std::endl;
            std::cout << "Size Z: " << currentDungeon->size_z << std::endl;
        }

        if(batchImport && hasToInitDensityMap){
            SetUpDensityMaps();
            hasToInitDensityMap = false;
        }
    }
    if(((view!=nullptr&&view->open)||view==nullptr)/*&&!batchImport*/){ // Create and open a new view if it doesnt exist, or update if its open
        hasToUpdateView = true;
    }
    statsGenerated = false;
    pathGenerated = false;
}

void DungeonStat::Setup(){}

void DungeonStat::ShowAsWindow(){
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));
    std::string windowName = dunname + " Stats";
    if (ImGui::Begin(windowName.c_str(), &open)){
        Show();
    }

    ImGui::End();
}

void DungeonStat::ShowAsChild()
{
    std::string windowName = dunname + " Stats";
    if (ImGui::BeginChild(windowName.c_str(),ImVec2(0, 0), ImGuiChildFlags_None,ImGuiWindowFlags_None)){
        Show();
    }
    ImGui::EndChild();
}

void DungeonStat::Show(){
    if(statsGenerated){
        if(batch){
            if(ImGui::Button("Export CSV")) hasToSave = true;
            int baseColumns = 5;
            ImGui::BeginTable("statsTable",baseColumns,ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable);
            ImGui::TableSetupColumn("Wall count");
            ImGui::TableSetupColumn("Floor count");
            ImGui::TableSetupColumn("Distance S/G");
            ImGui::TableSetupColumn("Generation time (seconds)");
            ImGui::TableSetupColumn("Steps S/G");
            ImGui::TableHeadersRow();
            
            for(int i = 0; i < statsBatch.size();i++){
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%i",statsBatch[i]->wallCount);
                ImGui::TableNextColumn();
                ImGui::Text("%i",statsBatch[i]->floorCount);
                ImGui::TableNextColumn();
                ImGui::Text("%f",statsBatch[i]->linearDistanceStartGoal);
                ImGui::TableNextColumn();
                ImGui::Text("%f",statsBatch[i]->generationTime);
                ImGui::TableNextColumn();
                if(statsBatch[i]->pathGenerated){
                    ImGui::Text("%i",statsBatch[i]->dungeonPathLength);
                }else{
                    ImGui::Text("???");
                }
            }
            ImGui::EndTable();
        }else{
            ImGui::Text("Wall count:\t%i",wallCount);
            ImGui::Text("Floor count:\t%i",floorCount);
            ImGui::Text("Linear distance S/G:\t%f",linearDistanceStartGoal);
            if(isGenerated){
                ImGui::Text("Generation time:\t%fs",dungeonTime);
            }
            if(pathGenerated)
                ImGui::Text("Step count S/G:\t%i",dungeonPath.GetLength());
        }

        ImGui::BeginDisabled(pathGenerated);
        
        if(ImGui::Button("Resolve")){
            hasToGeneratePath = true;
        }
        ImGui::EndDisabled();
    }
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
    if(hasToSave) {
        hasToSave = false;
        SaveStats();
    }
}

void DungeonStat::Shutdown(){
    freeDungeonMatrix(&currentDungeon);
    ClearBatchData();
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

void DungeonStat::UpdateView(DungeonView* & view){
    if(hasToUpdateView){
        if(this->view == nullptr){
            this->view = new DungeonView(dunname,this);
            view = this->view;
            view->Setup();
        }
        view->open = true;
    }
}

void DungeonStat::SetUpDensityMaps(){
    densityMap.Init(currentDungeon->size_x,currentDungeon->size_z);
    pathDensityMap.Init(currentDungeon->size_x,currentDungeon->size_z);
    startDensityMap.Init(currentDungeon->size_x,currentDungeon->size_z);
    goalDensityMap.Init(currentDungeon->size_x,currentDungeon->size_z);
}

void DungeonStat::GenerateStats(){
    if(currentDungeon==nullptr) return;
    wallCount = 0;
    floorCount = 0;

    // calculate density map, calculate wall and floor count
    for(int i = 0; i < currentDungeon->size_x;i++){
        for(int j = 0; j < currentDungeon->size_y;j++){
            for(int k = 0; k < currentDungeon->size_z;k++){
                unsigned tile = currentDungeon->data[i][j][k];
                if(tile&DUN_PXY_WALL) wallCount++;
                if(tile&DUN_PYZ_WALL) wallCount++;
                if(tile&DUN_PXZ_WALL) {
                    floorCount++;
                    if(batch) densityMap.Add(i,k,1);
                }
            }
        }
    }

    // get linear distance and end/goal
    start = {(float)currentDungeon->start_x,(float)currentDungeon->start_y,(float)currentDungeon->start_z};
    end = {(float)currentDungeon->end_x,(float)currentDungeon->end_y,(float)currentDungeon->end_z};
    linearDistanceStartGoal = Vector3Length( Vector3Subtract(end,start));

    if(batch){
        Stats * newStats = new Stats;
        newStats->wallCount = wallCount;
        newStats->floorCount = floorCount;
        newStats->linearDistanceStartGoal = linearDistanceStartGoal;
        newStats->start = start;
        newStats->end = end;
        newStats->generationTime = dungeonTime;
        statsBatch.push_back(newStats);
        startDensityMap.Add(start.x,start.z,1);
        goalDensityMap.Add(end.x,end.z,1);
    }

    statsGenerated = true;
}

bool DungeonStat::IsBatch(){
    return batch;
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

    if(batch){
        int length = dungeonPath.valid ? dungeonPath.GetLength() : -1;
        statsBatch.back()->dungeonPathLength = length;
        statsBatch.back()->pathGenerated = pathGenerated && length >= 0;
        for(int i = 0 ; i < dungeonPath.GetLength(); i++){
            Vector3Int pos = dungeonPath.GetPos(i);
            pathDensityMap.Add(pos.x,pos.z,1);
        }
    }
}

Vector3 DungeonStat::GetEnd(){
    return end;
}

Vector3 DungeonStat::GetStart(){
    return start;
}

void DungeonStat::DrawPath(Vector3 pos,Color color,float size,bool drawLast){
    if(pathGenerated) dungeonPath.Draw(pos,color,size,drawLast);
}

bool DungeonStat::IsPathGenerated(){
    return pathGenerated;
}

std::string DungeonStat::GetName(){
    return dunname;
}

void DungeonStat::ClearBatchData(){
    hasToInitDensityMap = true;
    int s = statsBatch.size();
    for(int i = 0; i < statsBatch.size(); i++){
        delete statsBatch[i];
    }
    statsBatch.clear();
}

void DungeonStat::DrawDensityMap(Vector3 pos, Vector3 size, Color color){
    densityMap.Draw(pos,size,color,statsBatch.size());
}

void DungeonStat::DrawStartDensityMap(Vector3 pos, Vector3 size, Color color){
    // startDensityMap.DrawDivided(pos,size,color,statsBatch.size(),&densityMap);
    startDensityMap.Draw(pos,size,color,statsBatch.size());
}

void DungeonStat::DrawGoalDensityMap(Vector3 pos, Vector3 size, Color color){
    goalDensityMap.DrawDivided(pos,size,color,statsBatch.size(),&densityMap);
    // goalDensityMap.Draw(pos,size,color,statsBatch.size());
}

void DungeonStat::DrawPathDensityMap(Vector3 pos, Vector3 size, Color color){
    pathDensityMap.DrawDivided(pos,size,color,statsBatch.size(),&densityMap);
    // pathDensityMap.Draw(pos,size,color,statsBatch.size());
}