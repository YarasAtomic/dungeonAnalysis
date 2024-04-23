#include "algorithmConfig.h"
#include "dungeonStat.h"
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>

AlgorithmConfig::AlgorithmConfig(std::string name,std::string dp,std::string outpath){
    dirpath = dp;
    filename = name;
    outdirpath = outpath;
    options = std::vector<HelpSection>();
}

void AlgorithmConfig::GetHelp(){
    std::cout << "Reading arg data" << std::endl;
    std::string command = (dirpath+"/"+ filename+ " " + "-help");
    std::string result = commandFifo(command,outdirpath+"/tempfifo");
    if(result== "") return;
    // std::cout << "Captured Output:\n" << result;
    GenerateConfig(result);
}

void AlgorithmConfig::Setup(){};

void AlgorithmConfig::ShowAsWindow(){
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));
    std::string windowName = filename + " Generation Settings";
    if (ImGui::Begin(windowName.c_str(), &open)){        
        Show();
    }

    ImGui::End();
}

void AlgorithmConfig::ShowAsChild(){
    std::string windowName = filename + " Generation Settings";
    if (ImGui::BeginChild(windowName.c_str(),ImVec2(0, 0), ImGuiChildFlags_None/* | ImGuiChildFlags_ResizeX*/,ImGuiWindowFlags_None))
    {
        Show();
    }
    ImGui::EndChild();
}

void AlgorithmConfig::Show(){
    if(configGenerated){
        for(int i = 0; i < options.size(); i++){
            const char * text = options[i].text.c_str();
            
            switch(options[i].type){
            case HelpSectionType::Title:
                ImGui::TextUnformatted(text);
                break;
            case HelpSectionType::Desc:
                ImGui::SameLine();
                HelpMarker(text);
                break;
            case HelpSectionType::Option:
                ImGui::TextUnformatted(text);
                ImGui::SameLine();
                std::stringstream ss;
                ss << "##" << options[i].text;
                // const char * label = std::string("##" + options[i].text).c_str();
                // const char * label = "##label";
                ImGui::SetNextItemWidth(-40.0);
                ImGui::InputText((ss.str().c_str()),options[i].value,IM_ARRAYSIZE(options[i].value));
                break;
            }
        }
    }
    ImGui::Checkbox("Resolve",&hasToGeneratePath);

    ImGui::BeginDisabled(runningAlgorithm);
    if(ImGui::Button("Run")&&pendingRuns==0&&!runningAlgorithm){
        // runAlgorithm = true;
        pendingRuns = 1;
        runModeBatch = false;
        std::cout << "Running " << filename << std::endl;
    }
    ImGui::SameLine();
    if(ImGui::Button("Run batch")&&pendingRuns==0&&!runningAlgorithm){
        // runAlgorithm = true;
        pendingRuns = batchRun;
        runModeBatch = true;
        hasToClearStats = true;
        std::cout << "Running " << filename << " in batch (" << batchRun << ")" << std::endl;
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-40.0);
    ImGui::InputInt("##runNumber",&batchRun,1,16);
    if(batchRun < 1) batchRun = 1;
    ImGui::EndDisabled();
    if(runningAlgorithm){
        
        if(runModeBatch){
            
            char buf[32];
            int done = batchRun-pendingRuns;
            sprintf(buf, "%d/%d", done, batchRun);
            // ImGui::TextUnformatted(buf);
            ImGui::ProgressBar(done/(float)(batchRun),ImVec2(0.f, 0.f), buf);
            ImGui::SameLine();
        }
        ImGui::Text("Running...");
    }
}

void AlgorithmConfig::ThreadRunAlgorithm(AlgorithmConfig & config){
    config.RunAlgorithm();
    config.hasToUpdateStats = true;
    config.runningAlgorithm = false;
}

void TestWait(){
    std::cout << "start wait" << std::endl;
    for(int i = 0; i < 30; i++){
        sleep(1);
        std::cout << "ping" << std::endl;
    }
    std::cout << "end wait" << std::endl;
}

void AlgorithmConfig::Update(){
    if(!open) return;
    if(!configGenerated) GetHelp();
    if(pendingRuns>0&&!runningAlgorithm){
        pendingRuns--;
        runningAlgorithm = true;

        // we cant pass a non-static method
        algorithmThread = new std::jthread(ThreadRunAlgorithm,std::ref(*this));
    }
};

void AlgorithmConfig::Shutdown(){
    if(runningAlgorithm){
        std::string killCmd = "pkill " + filename; //! This is dangerous for other processes, only works on linux
        std::system(killCmd.c_str());
    }
};

std::string AlgorithmConfig::GetName(){
    return filename;
}

void AlgorithmConfig::GenerateConfig(std::string help){
    std::string next;
    HelpSectionType lastType = HelpSectionType::None;
    HelpSectionType currentType = HelpSectionType::None;
    // For each character in the string
    for (std::string::const_iterator it = help.begin(); it != help.end(); it++) {
        if(currentType == HelpSectionType::None){
            if(*it == '\t'||*it == '-'){
                currentType = HelpSectionType::Option;
            }else if(lastType == HelpSectionType::Title){
                currentType = HelpSectionType::Desc;
            }else{
                currentType = HelpSectionType::Title;
            }
        }
        // If we've hit the terminal character
        if (*it == '\n') {
            // If we have some characters accumulated
            if (!next.empty()) {
                switch(currentType){
                case HelpSectionType::Title:
                    options.push_back({currentType,next});
                    break;
                case HelpSectionType::Desc:
                    options.push_back({currentType,next});
                    break;
                case HelpSectionType::Option:
                    std::string optionName = GetOptionName(next);
                    std::string optionDesc = GetOptionDesc(next);
                    if(optionName!="o"&&optionName!="help"){
                        options.push_back({currentType,optionName});
                        options.push_back({HelpSectionType::Desc,optionDesc});
                    }
                    break;
                }
                // Add them to the result vector
                // result.push_back(next);
                lastType = currentType;
                currentType = HelpSectionType::None;
                next.clear();
            }
        } else {
            // Accumulate the next character into the sequence
            next += *it;
        }
    }
    if (!next.empty()){
        switch(currentType){
        case HelpSectionType::Title:
            options.push_back({currentType,next});
            break;
        case HelpSectionType::Desc:
            options.push_back({currentType,next});
            break;
        case HelpSectionType::Option:
            std::string optionName = GetOptionName(next);
            std::string optionDesc = GetOptionDesc(next);
            if(optionName!="o"&&optionName!="help"){
                options.push_back({currentType,optionName});
                options.push_back({HelpSectionType::Desc,optionDesc});
            }

            break;
        }
    }
    configGenerated = true;
}

std::string AlgorithmConfig::GetOptionName(std::string string){
    std::string name = "";
    for (std::string::const_iterator it = string.begin(); it != string.end(); it++){
        if ((*it == ' '||*it == '\t') && name != ""){
            return name;
        }else if(*it != '-'&&*it != '\t'){
            name += *it;
        }
    }
    return name;
}

std::string AlgorithmConfig::GetOptionDesc(std::string string){
    std::string desc = "";
    bool readingDescription = false;
    bool readingOption = false;
    for (std::string::const_iterator it = string.begin(); it != string.end(); it++){
        if (readingDescription && *it != '\n'){
            desc += *it;
        }else if(*it == '-' && !readingDescription){
            readingOption = true;
        }else if((*it == ' '||*it == '\t') && readingOption){
            readingOption = false;
        }else if(!readingDescription && 
            !readingOption && 
            ((*it >= 'a' && *it <= 'z')||(*it >= 'A' && *it <= 'Z')))
        {
            readingDescription = true;
            desc += *it;
        }   
    }
    return desc;
}

void AlgorithmConfig::RunAlgorithm(){

    const auto start{std::chrono::steady_clock::now()};

    std::string cmdBin = (dirpath+"/"+ filename);
    std::string cmdOptions = "";

    for(int i = 0; i < options.size();i++){
        // if(options[i].type == HelpSectionType::Option)
        //     std::cout << "option " << options[i].text << " value: \"" << options[i].value << "\"" << " length " << CharArrayLength(options[i].value) << std::endl;
        if(options[i].type == HelpSectionType::Option && CharArrayLength(options[i].value)>0){
            cmdOptions += " -" + options[i].text + " " + options[i].value;
        }
    }

    cmdOptions += " -o " + outdirpath + "/temp.dun";

    std::string command = cmdBin + cmdOptions;
    std::cout << command << std::endl;

    std::string result = commandFifo(command,outdirpath+"/tempfifo");
    if(result== "") return;
    // std::cout << "Algorithm std Output:\n" << result;
    
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    dungeonTime = elapsed_seconds.count();
    std::cout << "execution Time " << dungeonTime << " seconds" << std::endl;
}

//! this is utterly useless and i should remove it, also it doesnt work
void AlgorithmConfig::UpdateStats(std::vector<DungeonStat*> & stats){
    if(hasToUpdateStats){
        if(statWindow==nullptr) stats.push_back(SetUpStats());
        if(hasToClearStats){
            hasToClearStats = false;
            statWindow->ClearBatchData();
        }
        statWindow->Import(runModeBatch);
        statWindow->hasToGeneratePath = hasToGeneratePath;
        statWindow->dungeonTime = dungeonTime;
        hasToUpdateStats = false;
        statWindow->open = true;
    }
}

void AlgorithmConfig::UpdateStats(DungeonStat * & stats){
    if(hasToUpdateStats){
        if(stats==nullptr) stats = SetUpStats();
        if(hasToClearStats){
            hasToClearStats = false;
            stats->ClearBatchData();
        }
        statWindow->Import(runModeBatch);
        statWindow->hasToGeneratePath = hasToGeneratePath;
        statWindow->dungeonTime = dungeonTime;
        hasToUpdateStats = false;
        statWindow->open = true;
    }
}


DungeonStat* AlgorithmConfig::SetUpStats(){
    if(statWindow == nullptr){
        statWindow = new DungeonStat(outdirpath+"/temp.dun",filename,this,hasToGeneratePath);
        statWindow->Setup();
    }
    return statWindow;
}

// void AlgorithmConfig::ImportDungeon(std::string file){
//     std::cout << "importing: " << file << std::endl;
//     bool dungeonImported = file2Dun(&currentDungeon,file);

//     if(dungeonImported&&currentDungeon!=nullptr)
//     {
//         std::cout << "file readed succesfully" << std::endl;
//         std::cout << "Imported dungeon:" << std::endl;
//         std::cout << "Size X: " << currentDungeon->size_x << std::endl;
//         std::cout << "Size Y: " << currentDungeon->size_y << std::endl;
//         std::cout << "Size Z: " << currentDungeon->size_z << std::endl;
//     }
// }