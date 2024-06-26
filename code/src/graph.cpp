#include "graph.h"
#include "utils.h"
#include <cmath>
#include <set>
#include <queue>
#include <iostream>

int Node::CostDistance(DungeonMatrix * dungeon){
    Vector3 distance = {
        (float)dungeon->GetEnd().x-(float)state.agentPos.x,
        (float)dungeon->GetEnd().y-(float)state.agentPos.y,
        (float)dungeon->GetEnd().z-(float)state.agentPos.z};

    return (int)Vector3Length(distance); //we could use LengthSquared
}

void DungeonPath::Add(Vector3Int step){
    steps.push_back(step);
}

Vector3Int DungeonPath::GetPos(int step){
    return steps[step];
}

bool DungeonPath::Validate(bool allowDiagonal){
    for(int i = 0; i < steps.size()-1;i++){
        int xDiff = abs(steps[i].x-steps[i].x);
        int yDiff = abs(steps[i].y-steps[i].y);
        int zDiff = abs(steps[i].z-steps[i].z);
        if(
            (xDiff>1||yDiff>1||zDiff>1) ||
            (!allowDiagonal&&xDiff+yDiff+zDiff>1)
            )return false;
    }
    return true;
}

void DungeonPath::Draw(Vector3 pos, Color color,float size,bool drawLast){
    if(steps.size()>1){
        Vector3 currentStep;
        Vector3 nextStep = {(float)steps[0].x,(float)steps[0].y,(float)steps[0].z};
        for(int i = 0; i < steps.size()-1;i++){
            currentStep = nextStep;
            nextStep = {(float)steps[i+1].x,(float)steps[i+1].y,(float)steps[i+1].z};
            DrawLine3D(Vector3Sum(pos,currentStep),Vector3Sum(pos,nextStep),color);
            if(drawLast||(i < steps.size()-2))
                DrawSphere(Vector3Sum(pos,nextStep),size,color);
        }
    }
}

void DungeonPath::FromSequence(Vector3Int start,std::vector<Action> sequence){
    steps.clear();

    Vector3Int currentPos = start;
    steps.push_back(currentPos);
    
    for(int i = 0 ; i < sequence.size();i++){
        switch (sequence[i])
        {
        case Action::MOVE_FRONT:
            currentPos.z++;
            break;
        case Action::MOVE_BACK:
            currentPos.z--;
            break;
        case Action::MOVE_LEFT:
            currentPos.x--;
            break;
        case Action::MOVE_RIGHT:
            currentPos.x++;
            break;
        }
        steps.push_back(currentPos);
    }
}

int DungeonPath::GetLength(){
    return steps.size();
}

bool DunState::TakeAction(Action action,DungeonMatrix * dungeon, DunState & newState){
    newState = *this;
    switch(action){
    case Action::MOVE_FRONT:
        if(agentPos.z+1>=dungeon->GetSize().z) return false;
        if(dungeon->GetPos({
            (unsigned int)agentPos.x,
            (unsigned int)agentPos.y,
            (unsigned int)agentPos.z+1
        })&DUN_PXY_WALL) return false;
        newState.agentPos.z++;
        break;
    case Action::MOVE_BACK:
        if(agentPos.z<=0) return false;
        if(dungeon->GetPos({
            (unsigned int)agentPos.x,
            (unsigned int)agentPos.y,
            (unsigned int)agentPos.z
        })&DUN_PXY_WALL) return false;
        newState.agentPos.z--;
        break;
    case Action::MOVE_LEFT:
        if(agentPos.x<=0) return false;
        if(dungeon->GetPos({
            (unsigned int)agentPos.x,
            (unsigned int)agentPos.y,
            (unsigned int)agentPos.z
        })&DUN_PYZ_WALL) return false;
        newState.agentPos.x--;
        break;
    case Action::MOVE_RIGHT:
        if(agentPos.x+1>=dungeon->GetSize().x) return false;
        if(dungeon->GetPos({
            (unsigned int)agentPos.x+1,
            (unsigned int)agentPos.y,
            (unsigned int)agentPos.z
        })&DUN_PYZ_WALL) return false;
        newState.agentPos.x++;
        break;
    }
    return true;
}

Pathfinder::Pathfinder(DungeonMatrix * dungeon){
    this->dungeon = dungeon;
}

DungeonPath Pathfinder::AStar(Vector3Int start,Vector3Int end){
    DungeonPath path;
    std::set<DunState, ComparatorLess> exploredStates;
    std::priority_queue<Node,std::vector<Node>,std::less<Node>> openNodes;

    DunState initState;
    initState.agentPos = start;
    Node current;
    current.cost = 0;
    current.accumulatedCost = 0;
    current.state = initState;

    openNodes.push(current);

    bool found = false;

    const int actionCount = 4;
    Action actions[actionCount] = {Action::MOVE_BACK,Action::MOVE_FRONT,Action::MOVE_LEFT,Action::MOVE_RIGHT};

    const int rate = 1000;
    int timer = 0;
    int iteration = 0;

    while(!openNodes.empty()&&!found){
        timer ++;
        iteration++;
        if(timer>1000){
            timer = 0;
            
        }
        current = openNodes.top();

        if((current.state.agentPos.x==end.x)&&(current.state.agentPos.y==end.y)&&(current.state.agentPos.z==end.z)){
            found = true;
            path.FromSequence(start,current.sequence);
            path.valid = true;
            return path;
        }

        for(int i = 0; i < actionCount; i++){
            DunState newState;
            if(current.state.TakeAction(actions[i],dungeon,newState)){
                if(exploredStates.find(newState)==exploredStates.end()){
                    Node newNode;
                    newNode.sequence = current.sequence;
                    newNode.sequence.push_back(actions[i]);
                    newNode.state = newState;
                    newNode.cost = newNode.CostDistance(dungeon);
                    newNode.accumulatedCost = current.accumulatedCost+newNode.cost;
                    openNodes.push(newNode);
                    auto pair = exploredStates.insert(newState);                    
                }
            }
        }
        openNodes.pop();
    }
    return path;
}