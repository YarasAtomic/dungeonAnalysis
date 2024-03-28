#ifndef ALGORITHMCONFIG_H
#define ALGORITHMCONFIG_H

#include <thread>
#include "ui.h"
struct dungeonMatrix;

class DungeonStat;

class AlgorithmConfig : public DocumentWindow{
    enum HelpSectionType{Title,Desc,Option,None};
    
    struct HelpSection{
        HelpSectionType type;
        std::string text;
        char value[128] = "";
    };

    std::string dirpath;
    std::string filename;
    std::string outdirpath;
    std::vector<HelpSection> options;
    bool configGenerated = false;
    bool runAlgorithm = false;
    bool runningAlgorithm = false;
    DungeonStat* statWindow = nullptr;
    std::jthread * algorithmThread;
   
    void GetHelp();
    void RunAlgorithm();
    void GenerateConfig(std::string help);
    std::string GetOptionName(std::string string);
    std::string GetOptionDesc(std::string string);

    bool hasToGeneratePath = false;

    double dungeonTime = 0;
protected:
    void Show() override;
public:
    bool hasToUpdateStats = false;
    dungeonMatrix * currentDungeon;

    AlgorithmConfig(std::string name,std::string path,std::string out);
    void Setup() override;
    void ShowAsWindow() override;
    void Update() override;
    void Shutdown() override;

    void UpdateStats(std::vector<DungeonStat*> &);

    std::string GetName();

    static void ThreadRunAlgorithm(AlgorithmConfig & config);
};

#endif