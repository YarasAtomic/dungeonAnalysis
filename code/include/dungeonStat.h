#ifndef STAT_H
#define STAT_H

#include "ui.h"
#include "graph.h"
struct dungeonMatrix;

class DungeonView;

struct Stats{
    int wallCount = -1;
    int floorCount = -1;
    float generationTime = 0;
    float linearDistanceStartGoal = 0;
    Vector3 end = {0,0,0};
    Vector3 start = {0,0,0};
    int dungeonPathLength = 0;
    bool pathGenerated = false;
};

class DungeonStat : public DocumentWindow{
    std::string dunpath;
    std::string dunname;
    bool isGenerated = false;
    DungeonView * view = nullptr;
    
    bool batch = false;

    // stats
    bool statsGenerated = false;
    int wallCount = -1;
    int floorCount = -1;
    float linearDistanceStartGoal = 0;
    Vector3 end = {0,0,0};
    Vector3 start = {0,0,0};
    DungeonPath dungeonPath;
    bool pathGenerated = false;
    bool generatingPath = false;

    std::vector<Stats *> statsBatch;
    std::vector<std::vector<unsigned int>> * densityMap = nullptr;

    void GenerateStats();
    void GeneratePath();
    void SaveStats();
    void SetUpDensityMap();
protected:
    void Show() override;
public:
    dungeonMatrix * currentDungeon = nullptr;
    bool hasToUpdateView = false;
    bool hasToGeneratePath = false;
    bool hasToSave = false;
    double dungeonTime = 0;

    DungeonStat(std::string path,std::string file,bool isGenerated,bool hasToGeneratePath);
    void Import(bool batchImport);

    void Setup() override;
    void ShowAsWindow() override;
    void ShowAsChild() override;
    // void ShowAsTabItem() override;
    void Update() override;
    void Shutdown() override;

    void UpdateViews(std::vector<DungeonView*> &);
    void UpdateView(DungeonView * &);
    void ClearBatchData();

    Vector3 GetEnd();
    Vector3 GetStart();

    std::string GetName();

    void DrawPath(Vector3 pos,Color color,float size,bool drawLast);
    bool IsPathGenerated();

    void DrawDensityMap(Vector3 pos, Color color);
    bool IsBatch();
};

# endif