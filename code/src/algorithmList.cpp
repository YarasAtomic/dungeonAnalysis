#include "algorithmList.h"
#include "algorithmConfig.h"
#include "dungeonTab.h"

void AlgorithmList::ShowAsWindow()
{
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

    
    if (ImGui::Begin("Algorithm list", &open, ImGuiWindowFlags_NoScrollbar))
    {
        Show();
    }
    ImGui::End();
}

void AlgorithmList::ShowAsChild()
{
    ImGui::TextUnformatted("Algorithm list");
    ImGui::Separator();
    ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 1), ImVec2(FLT_MAX, ImGui::GetTextLineHeightWithSpacing() * 10));
    if (ImGui::BeginChild("Algorithm list",ImVec2(-FLT_MIN, 0.0f), ImGuiChildFlags_None,ImGuiWindowFlags_None))
    {
        Show();
    }
    ImGui::EndChild();
}

void AlgorithmList::Show(){
    if(ImGui::Button("Reload")) Reload();
    ImGui::Spacing();
    ImGui::BeginChild("##child::algorithms", ImVec2(0, 0), ImGuiChildFlags_Border,ImGuiWindowFlags_None);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign,ImVec2(0,0.5));
    if (ImGui::BeginTable("##table::algorithms", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
    {
        ImGui::TableNextColumn();
        for(int i = 0; i < algorithms.size();i++){
            if(ImGui::Button(algorithms[i].c_str(), ImVec2(-FLT_MIN, 0.0f))){
                selected = algorithms[i];
            }
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();
}

void AlgorithmList::Setup(){
    Reload();
}

void AlgorithmList::Update(){
}

void AlgorithmList::Shutdown(){
}

void AlgorithmList::Reload(){
    algorithms.clear();
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
        // Iterate over the files in the directory
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            algorithms.push_back(entry.path().filename().string());
        }
    } else {
        std::cerr << "Directory does not exist or is not a directory." << std::endl;
    }

    hasToUpdate = true;
}

void AlgorithmList::UpdateAlgorithms(std::vector<AlgorithmConfig*> * algorithms){
    if(selected!=""){
        int index = -1;
        for(int i = 0 ; i < algorithms->size();i++){
            if((*algorithms)[i]->GetName()==selected) index = i;
        }

        if(index == -1){
            algorithms->push_back(new AlgorithmConfig(selected,dir,outdir));
            (*algorithms)[algorithms->size()-1]->open = true;
        }else{
            (*algorithms)[index]->open = true;
        }
        selected = "";
    }
}

void AlgorithmList::UpdateDungeonTabs(std::vector<DungeonTab*> * algorithms){
    if(selected!=""){
        int index = -1;
        for(int i = 0 ; i < algorithms->size();i++){
            if((*algorithms)[i]->GetAlgorithmName()==selected) index = i;
        }

        if(index == -1){
            algorithms->push_back(new DungeonTab());
            (*algorithms)[algorithms->size()-1]->SetConfig(selected,dir,outdir);
            (*algorithms)[algorithms->size()-1]->open = true;
        }else{
            (*algorithms)[index]->open = true;
        }
        selected = "";
    }
}