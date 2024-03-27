#ifndef GRAPH_H
#define GRAPH_H

#include "ezdungeon.h"
#include "raylib.h"
#include <vector>

enum Action{
    MOVE_FRONT, MOVE_BACK, MOVE_LEFT, MOVE_RIGHT
};

class DungeonPath{
    std::vector<Vector3Int> steps;
public:
    void FromSequence(Vector3Int start,std::vector<Action> sequence);
    void Add(Vector3Int step);
    Vector3Int GetPos(int step);
    bool Validate(bool allowDiagonal=false);
    void Draw(Vector3 pos, Color color,float size,bool drawLast=true);
    int GetLength();
};

struct DunState{
    Vector3Int agentPos;
    bool TakeAction(Action action, dungeonMatrix * dungeon, DunState & newState);
    // inline bool operator < (const DunState & other) const {
    //     return 
    //         (this->agentPos.x > other.agentPos.x)||
    //         (this->agentPos.y > other.agentPos.y)||
    //         (this->agentPos.z > other.agentPos.z);
    // };
};

class ComparatorLess{
public:
    bool operator()(const DunState & a, const DunState & b) const {
        // return (
        //     a.agentPos.x > b.agentPos.x ||
        //     a.agentPos.y > b.agentPos.y ||
        //     a.agentPos.z > b.agentPos.z
        // );
        return std::tie(a.agentPos.x, a.agentPos.y, a.agentPos.z) < std::tie(b.agentPos.x, b.agentPos.y, b.agentPos.z);
    }
};

struct Node{
    std::vector<Action> sequence;
    int cost = 0;
    int accumulatedCost = 0;
    DunState state;
    inline bool operator < (const Node & other) const {return this->accumulatedCost > other.accumulatedCost;};
    int CostDistance(dungeonMatrix * dungeon);
};

class Pathfinder{
    dungeonMatrix * dungeon;
public:
    Pathfinder(dungeonMatrix * dungeon);
    DungeonPath AStar(Vector3Int start,Vector3Int end);
};

#endif 