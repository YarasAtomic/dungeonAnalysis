#ifndef STAT_H
#define STAT_H

#include "ui.h"
#include "graph.h"
struct dungeonMatrix;

class DungeonView;

class DungeonStat : public DocumentWindow{
    std::string dunpath;
    std::string dunname;
    bool isGenerated = false;
    DungeonView * view = nullptr;

    bool showPath = true;
    

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

    void GenerateStats();
    void GeneratePath();
protected:
    void Show() override;
public:
    dungeonMatrix * currentDungeon;
    bool hasToUpdateView = false;
    bool hasToGeneratePath = false;
    double dungeonTime = 0;

    DungeonStat(std::string path,std::string file,bool isGenerated,bool hasToGeneratePath);
    void Import();

    void Setup() override;
    void ShowAsWindow() override;
    void Update() override;
    void Shutdown() override;

    void UpdateViews(std::vector<DungeonView*> &);

    Vector3 GetEnd();
    Vector3 GetStart();

    std::string GetName();

    void DrawPath(Vector3 pos,Color color,float size,bool drawLast);
};

# endif