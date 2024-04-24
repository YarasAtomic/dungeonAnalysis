#ifndef ALGORITHMCONFIG_H
#define ALGORITHMCONFIG_H

#include <thread>
#include "ui.h"
struct dungeonMatrix;

class DungeonStat;

class AlgorithmConfig : public DocumentWindow{
    enum HelpSectionType{Title,Option,None};
    enum ValueType{Int,Flt,Str,Void};

    struct HelpLine{
        HelpSectionType type;
        ValueType valueType;
        std::string text;
        std::string desc;
        char textValue[128] = "";
        int intValue = 0;
        float fltValue = 0.0;
        bool use = false;
    };

    std::string dirpath;
    std::string filename;
    std::string outdirpath;
    std::vector<HelpLine> optionsLines;
    bool configGenerated = false;

    inline ValueType StringToValue(std::string str){
        if(str=="str")return ValueType::Str;
        if(str=="int")return ValueType::Int;
        if(str=="flt")return ValueType::Flt;
        return ValueType::Void;
    }
    
    int pendingRuns = 0;
    bool runningAlgorithm = false;
    DungeonStat* statWindow = nullptr;
    std::jthread * algorithmThread = nullptr;
   
    void GetHelp();
    void RunAlgorithm();
    void GenerateConfig(std::string help);
    
    std::string GetOptionName(std::string string);
    std::string GetOptionDesc(std::string string);

    int batchRun = 10;
    bool runModeBatch = false;
    bool hasToClearStats = false;
    bool hasToGeneratePath = false;

    double dungeonTime = 0;

    // Style
    int argMaxCursor = 0;

protected:
    void Show() override;
public:
    bool hasToUpdateStats = false;
    dungeonMatrix * currentDungeon;

    AlgorithmConfig(std::string name,std::string path,std::string out);
    void Setup() override;
    void ShowAsWindow() override;
    void ShowAsChild() override;
    // void ShowAsTabItem() override;
    void Update() override;
    void Shutdown() override;

    void UpdateStats(std::vector<DungeonStat*> &);
    void UpdateStats(DungeonStat * &);
    DungeonStat* SetUpStats();

    std::string GetName();

    static void ThreadRunAlgorithm(AlgorithmConfig & config);
};

#endif