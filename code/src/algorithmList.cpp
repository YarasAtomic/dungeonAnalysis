#include "algorithmList.h"

void AlgorithmList::Show()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

    
    if (ImGui::Begin("Algorithm list", &open, ImGuiWindowFlags_NoScrollbar))
    {
        if(ImGui::Button("Reload")) Reload();

        ImGui::BeginListBox("##listbox::algorithms");
        for(int i = 0; i < algorithms.size();i++){
            if(ImGui::Button(algorithms[i].c_str())){
                selected = algorithms[i];
            }
        }

        ImGui::EndListBox();

    }
    ImGui::End();
    ImGui::PopStyleVar();
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
            // std::cout << entry.path().filename().string() << std::endl;
        }
    } else {
        std::cerr << "Directory does not exist or is not a directory." << std::endl;
    }

    hasToUpdate = true;
}

void AlgorithmList::UpdateAlgorithms(std::vector<AlgorithmConfig*> * algorithms){
    if(selected!=""){
        std::cout << "Selected " << selected << std::endl;
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