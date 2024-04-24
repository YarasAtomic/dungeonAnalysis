#include "dungeonStat.h"
#include "dungeonView.h"
#include "algorithmConfig.h"
#include "dungeonTab.h"

DungeonTab::DungeonTab(){}

void DungeonTab::Setup(){
    if(stats!=nullptr) stats->Setup();
    if(view!=nullptr) view->Setup();
    if(config!=nullptr) config->Setup();
}

void DungeonTab::Update(){
    if(stats!=nullptr) {
        stats->open = open;
        stats->Update();
        stats->UpdateView(view);
    }
    if(view!=nullptr) {
        view->open = open;
        view->Update();
    }
    if(config!=nullptr) {
        config->open = open;
        config->Update();
        config->UpdateStats(stats);
    }
}

void DungeonTab::Show(){
    if(view!=nullptr) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(0,0)); //Show a fake line so the user know how to resize
        view->ShowAsChild();
        ImGui::PopStyleVar();
        ImGui::Separator();
    }
    ImGui::BeginTable("statsAndConfigTable",2,/*ImGuiTableFlags_SizingFixedFit|*/  ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable);
    ImGui::TableNextRow();
    // ImGui::TextUnformatted("column 1");
    // ImGui::TableNextColumn();
    // ImGui::TextUnformatted("column 2");
    if(config!=nullptr) {
        ImGui::TableNextColumn();
        config->ShowAsChild();
    }
    if(stats!=nullptr) {
        ImGui::TableNextColumn();//ImGui::SameLine(-100);
        stats->ShowAsChild();
    }
    ImGui::EndTable();
    
}

void DungeonTab::ShowAsChild(){
    ImGui::TextUnformatted(GetName().c_str());
    ImGui::Separator();
    if (ImGui::BeginChild("Dungeon tab",ImVec2(0.0f, 0.0f), ImGuiChildFlags_None,ImGuiWindowFlags_None)){
        Show();
    }
    ImGui::EndChild();
}

void DungeonTab::ShowAsWindow(){}

void DungeonTab::Shutdown(){
    if(stats!=nullptr) {
        stats->Shutdown();
        delete stats;
    }
    if(view!=nullptr) {
        view->Shutdown();
        delete view;
    }
    if(config!=nullptr) {
        config->Shutdown();
        delete config;
    }
}

std::string DungeonTab::GetAlgorithmName(){
    return config!=nullptr ? config->GetName() : "";
}

std::string DungeonTab::GetDungeonName(){
    return stats!=nullptr ? stats->GetName() : "";
}

void DungeonTab::SetConfig(std::string name,std::string path,std::string out){
    config = new AlgorithmConfig(name,path,out);
    stats = config->SetUpStats();
    stats->hasToUpdateView = true;
    stats->UpdateView(view);
}
void DungeonTab::SetDungeon(std::string path,std::string file){
    stats = new DungeonStat(path,file,false,false);
    stats->Import(false);
}
std::string DungeonTab::GetName(){
    if(config!=nullptr) return config->GetName();
    if(stats!=nullptr) return stats->GetName();
    return "dungeon";
}