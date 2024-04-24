#include "algorithmConfig.h"
#include "dungeonStat.h"
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <regex>

AlgorithmConfig::AlgorithmConfig(std::string name,std::string dp,std::string outpath){
    dirpath = dp;
    filename = name;
    outdirpath = outpath;
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
        int cursorStart = ImGui::GetCursorPosX();
        for(int i = 0; i < optionsLines.size(); i++){
            const char * text = optionsLines[i].text.c_str();
            const char * desc = optionsLines[i].desc.c_str();
            
            switch(optionsLines[i].type){
            case HelpSectionType::Title:
                ImGui::TextUnformatted(text);
                if(optionsLines[i].desc.size()>0) {
                    ImGui::SameLine();
                    HelpMarker(desc);
                }
                break;
            case HelpSectionType::Option:
                if(optionsLines[i].text!="o"){
                    ImGui::TextUnformatted(text);
                    ImGui::SameLine();
                    int titleWidth = ImGui::GetCursorPosX() - cursorStart;
                    argMaxCursor = argMaxCursor < titleWidth ? titleWidth : argMaxCursor;
                    std::stringstream ss;
                    std::stringstream checkboxss;
                    ss << "##" << optionsLines[i].text;
                    checkboxss << "##" << optionsLines[i].text << "_use";
                    ImGui::SetCursorPosX(cursorStart + argMaxCursor);
                    switch(optionsLines[i].valueType){
                    case ValueType::Int:
                        ImGui::Checkbox((checkboxss.str().c_str()),&optionsLines[i].use);
                        ImGui::SameLine();
                        ImGui::BeginDisabled(!optionsLines[i].use);
                        ImGui::SetNextItemWidth(-40.0);
                        ImGui::InputInt((ss.str().c_str()),&optionsLines[i].intValue);
                        ImGui::EndDisabled();
                        break;
                    case ValueType::Str:
                        ImGui::InputText((ss.str().c_str()),optionsLines[i].textValue,IM_ARRAYSIZE(optionsLines[i].textValue));
                        break;
                    case ValueType::Flt:
                        ImGui::Checkbox((checkboxss.str().c_str()),&optionsLines[i].use);
                        ImGui::SameLine();
                        ImGui::BeginDisabled(!optionsLines[i].use);
                        ImGui::SetNextItemWidth(-40.0);
                        ImGui::InputFloat((ss.str().c_str()),&optionsLines[i].fltValue);
                        ImGui::EndDisabled();
                        break;
                    }
                    if(optionsLines[i].desc.size()>0) {
                        ImGui::SameLine();
                        HelpMarker(desc);
                    }
                }

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
    if(algorithmThread!=nullptr){
        delete algorithmThread;
    }
};

std::string AlgorithmConfig::GetName(){
    return filename;
}

void AlgorithmConfig::GenerateConfig(std::string help){
    std::regex argregex("\\t[\\w\\s]*");
    std::string delimiter = "\n";
    size_t pos = 0;
    std::string line;
    std::string wordregexp = "([\\w\\s':\\(\\)\\[\\]\\.\\,]*)";
    std::regex exparg("\\t\\-(\\w*)\\t\\[(int|flt|str)\\]\\t"+wordregexp);
    std::regex expargnodesc("\\t\\-(\\w*)\\t\\[(int|flt|str)\\]");
    std::regex exptitle(wordregexp+"\\t"+wordregexp);
    std::regex exptitlenodesc(wordregexp);
    std::smatch sm1;
    while ((pos = help.find(delimiter)) != std::string::npos) {
        line = help.substr(0, pos);
        help.erase(0, pos + delimiter.length());
        if(std::regex_match(line,exparg)){
            if(std::regex_search(line,sm1,exparg)){
                optionsLines.push_back({HelpSectionType::Option,StringToValue(sm1.str(2)),sm1.str(1),sm1.str(3)});
            }
        }else if(std::regex_match(line,expargnodesc)){
            if(std::regex_search(line,sm1,expargnodesc)){
                optionsLines.push_back({HelpSectionType::Option,StringToValue(sm1.str(2)),sm1.str(1),""});
            }
        } else if(std::regex_match(line,exptitle)){
            if(std::regex_search(line,sm1,exptitle)){
                optionsLines.push_back({HelpSectionType::Title,ValueType::Void,sm1.str(1),sm1.str(1)});
            }
        } else if(std::regex_match(line,exptitlenodesc)){
            if(std::regex_search(line,sm1,exptitlenodesc)){
                optionsLines.push_back({HelpSectionType::Title,ValueType::Void,sm1.str(1),""});
            }
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

    for(int i = 0; i < optionsLines.size();i++){
        if(optionsLines[i].type == HelpSectionType::Option){
            switch(optionsLines[i].valueType){
            case ValueType::Int:
                if(optionsLines[i].use == true)
                    cmdOptions += " -" + optionsLines[i].text + " " + std::to_string(optionsLines[i].intValue);
                break;
            case ValueType::Flt:
                if(optionsLines[i].use == true)
                    cmdOptions += " -" + optionsLines[i].text + " " + std::to_string(optionsLines[i].fltValue);
                break;
            case ValueType::Str:
                if(CharArrayLength(optionsLines[i].textValue)>0)
                    cmdOptions += " -" + optionsLines[i].text + " " + optionsLines[i].textValue;
                break;
            }
        }
    }

    cmdOptions += " -o " + outdirpath + "/temp.dun";

    std::string command = cmdBin + cmdOptions;
    if(!batchRun)
        std::cout << command << std::endl;

    std::string result = commandFifo(command,outdirpath+"/tempfifo");
    if(result== "") return;
    
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    dungeonTime = elapsed_seconds.count();
    if(!batchRun)
        std::cout << "execution Time " << dungeonTime << " seconds" << std::endl;
}

//! this is utterly useless and i should remove it, also it doesnt work
void AlgorithmConfig::UpdateStats(std::vector<DungeonStat*> & stats){
    if(hasToUpdateStats){
        if(algorithmThread!=nullptr){
            delete algorithmThread;
            algorithmThread = nullptr;
        }
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