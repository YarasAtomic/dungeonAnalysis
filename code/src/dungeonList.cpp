#include "dungeonList.h"
#include "dungeonStat.h"
#include "dungeonTab.h"

void DungeonList::ShowAsWindow()
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

    
    if (ImGui::Begin("Dungeon list", &open, ImGuiWindowFlags_NoScrollbar))
    {
        Show();
    }
    ImGui::End();
}

void DungeonList::ShowAsChild()
{
    ImGui::TextUnformatted("Dungeon list");
    ImGui::Separator();
    ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 1), ImVec2(FLT_MAX, ImGui::GetTextLineHeightWithSpacing() * 10));
    if (ImGui::BeginChild("Dungeon list",ImVec2(-FLT_MIN, 0.0f), ImGuiChildFlags_None,ImGuiWindowFlags_None))
    {
        Show();
    }
    ImGui::EndChild();
}

void DungeonList::Show(){
    if(ImGui::Button("Reload")) Reload();
    ImGui::Spacing();
    ImGui::BeginChild("##child::dungeons", ImVec2(0, 0), ImGuiChildFlags_Border,ImGuiWindowFlags_None);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign,ImVec2(0,0.5));
    if (ImGui::BeginTable("##table::dungeons", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
    {
        ImGui::TableNextColumn();
        for(int i = 0; i < dungeons.size();i++){
            if(ImGui::Button(dungeons[i].c_str(), ImVec2(-FLT_MIN, 0.0f)))
                selected = dungeons[i];
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();
}

void DungeonList::Setup(){
    Reload();
}

void DungeonList::Update(){
}

void DungeonList::Shutdown(){
}

void DungeonList::Reload(){
    dungeons.clear();
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
        // Iterate over the files in the directory
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            dungeons.push_back(entry.path().filename().string());
            // std::cout << entry.path().filename().string() << std::endl;
        }
    } else {
        std::cerr << "Directory does not exist or is not a directory." << std::endl;
    }

    hasToUpdate = true;
}

void DungeonList::UpdateDungeons(std::vector<DungeonStat*> * dungeons){
    if(selected!=""){
        std::cout << "Selected " << selected << std::endl;
        int index = -1;
        for(int i = 0 ; i < dungeons->size();i++){
            if((*dungeons)[i]->GetName()==selected) index = i;
        }

        if(index == -1){
            dungeons->push_back(new DungeonStat(dir+"/"+selected,selected,false,false));
            (*dungeons)[dungeons->size()-1]->open = true;
            (*dungeons)[dungeons->size()-1]->Import(false);
        }else{
            (*dungeons)[index]->open = true;
        }
        selected = "";
    }
}

void DungeonList::UpdateDungeonTabs(std::vector<DungeonTab*> * dungeons){
    if(selected!=""){
        std::cout << "Selected " << selected << std::endl;
        int index = -1;
        for(int i = 0 ; i < dungeons->size();i++){
            if((*dungeons)[i]->GetDungeonName()==selected) index = i;
        }

        if(index == -1){
            dungeons->push_back(new DungeonTab());
            (*dungeons)[dungeons->size()-1]->open = true;
            (*dungeons)[dungeons->size()-1]->SetDungeon(dir+"/"+selected,selected);
        }else{
            (*dungeons)[index]->open = true;
        }
        selected = "";
    }
}