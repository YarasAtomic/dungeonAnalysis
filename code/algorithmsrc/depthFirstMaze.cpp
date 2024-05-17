#include "ezdungeon.h"
#include <iostream>
#include <vector>
#include <stdlib.h> 
#include <set>
#include <random>
#ifdef MAZE_ANIM
#include <thread>
#include <chrono>
#endif

/**========================================================================
 * ?                                ABOUT
 * @author         :  Guillermo Medialdea Burgos
 * @email          :  
 * @repo           :  
 * @createdOn      :  24/04/24
 * @description    :  C++ implementation of Depth First Maze algorithm
 *========================================================================**/

// ---------------------------------------------------
// ------------- GENERATION --------------------------
// ---------------------------------------------------

struct dunTile{
    bool passed = false;
    bool fullyExplored = false;
    bool connectionUp = false;
    bool connectionLeft = false;
};

std::set<std::pair<int,int>>::iterator getRandomTile(std::set<std::pair<int,int>> tiles){ //O(n)
    std::set<std::pair<int,int>>::iterator it = std::begin(tiles);
    std::advance(it,rand() % tiles.size());
    return it;
}

std::pair<int,int> getNextTile(std::vector<std::vector<dunTile>>* dun,int x,int y){
    std::vector<std::pair<int,int>> availableDir;
    if(x>0 && !(*dun)[x-1][y].passed)
        availableDir.push_back(std::pair(x-1,y));
    if(x<dun->size()-1 && !(*dun)[x+1][y].passed)
        availableDir.push_back(std::pair(x+1,y));
    if(y>0 && !(*dun)[x][y-1].passed)
        availableDir.push_back(std::pair(x,y-1));
    if(y<(*dun)[0].size()-1 && !(*dun)[x][y+1].passed)
        availableDir.push_back(std::pair(x,y+1));

    if(availableDir.size()==0) return std::pair<int,int>(-1,-1);

    return availableDir[rand()%availableDir.size()];
}

std::pair<int,int> getConnectedTile(std::vector<std::vector<dunTile>>* dun,int x,int y){
    std::vector<std::pair<int,int>> availableDir;
    if(x>0 && (*dun)[x][y].connectionLeft && !(*dun)[x-1][y].fullyExplored)
        availableDir.push_back(std::pair(x-1,y));
    if(x<dun->size()-1 && (*dun)[x+1][y].connectionLeft && !(*dun)[x+1][y].fullyExplored)
        availableDir.push_back(std::pair(x+1,y));
    if(y>0 && (*dun)[x][y].connectionUp && !(*dun)[x][y-1].fullyExplored)
        availableDir.push_back(std::pair(x,y-1));
    if(y<(*dun)[0].size()-1 && (*dun)[x][y+1].connectionUp && !(*dun)[x][y+1].fullyExplored)
        availableDir.push_back(std::pair(x,y+1));

    if(availableDir.size()==0) return std::pair<int,int>(-1,-1);

    return availableDir[rand()%availableDir.size()];
}
#ifdef MAZE_ANIM
std::string drawTilePassed(std::vector<std::vector<dunTile>>* dun,int x,int y){
    dunTile tile = (*dun)[x][y];
    bool up = tile.connectionUp;
    bool down = (y<(*dun)[0].size()-1 && (*dun)[x][y+1].connectionUp);
    bool left = tile.connectionLeft;
    bool right = (x<dun->size()-1 && (*dun)[x+1][y].connectionLeft);
    if(up&&down&&left&&right) return "╋";
    if(up&&down&&left&&!right) return "┫";
    if(up&&down&&!left&&right) return "┣";
    if(up&&down&&!left&&!right) return "┃";
    if(up&&!down&&left&&right) return "┻";
    if(up&&!down&&left&&!right) return "┛";
    if(up&&!down&&!left&&right) return "┗";
    if(up&&!down&&!left&&!right) return "╹";
    if(!up&&down&&left&&right) return "┳";
    if(!up&&down&&left&&!right) return "┓";
    if(!up&&down&&!left&&right) return "┏";
    if(!up&&down&&!left&&!right) return "╻";
    if(!up&&!down&&left&&right) return "━";
    if(!up&&!down&&left&&!right) return "╸";
    if(!up&&!down&&!left&&right) return "╺";
    if(!up&&!down&&!left&&!right) return "·";
    return " ";
}

std::string drawTile(std::vector<std::vector<dunTile>>* dun,int x,int y){
    dunTile tile = (*dun)[x][y];
    bool up = tile.connectionUp;
    bool down = (y<(*dun)[0].size()-1 && (*dun)[x][y+1].connectionUp);
    bool left = tile.connectionLeft;
    bool right = (x<dun->size()-1 && (*dun)[x+1][y].connectionLeft);
    if(up&&down&&left&&right) return "┼";
    if(up&&down&&left&&!right) return "┤";
    if(up&&down&&!left&&right) return "├";
    if(up&&down&&!left&&!right) return "│";
    if(up&&!down&&left&&right) return "┴";
    if(up&&!down&&left&&!right) return "┘";
    if(up&&!down&&!left&&right) return "└";
    if(up&&!down&&!left&&!right) return "╵";
    if(!up&&down&&left&&right) return "┬";
    if(!up&&down&&left&&!right) return "┐";
    if(!up&&down&&!left&&right) return "┌";
    if(!up&&down&&!left&&!right) return "╷";
    if(!up&&!down&&left&&right) return "─";
    if(!up&&!down&&left&&!right) return "╴";
    if(!up&&!down&&!left&&right) return "╶";
    if(!up&&!down&&!left&&!right) return "·";
    return " ";
}

void drawClean(std::vector<std::vector<dunTile>>* dun){
    for(int j = 0; j < (*dun)[0].size();j++){
        std::cout<<"\e[A";
    }
}

void drawState(std::vector<std::vector<dunTile>>* dun){
    for(int j = 0; j < (*dun)[0].size();j++){
        for(int i = 0; i < dun->size();i++){
            std::string out = " ";
            if((*dun)[i][j].passed) out = drawTile(dun,i,j);
            if((*dun)[i][j].fullyExplored) out = drawTilePassed(dun,i,j);
            std::cout << out;
        }
        std::wcout << "\n";
    }
}
#endif

DungeonMatrix * Vector2Dun(std::vector<std::vector<dunTile>>* dun){
    DungeonMatrix * newDungeon = new DungeonMatrix(dun->size(),1, (*dun)[0].size());
    for(int i = 0; i < dun->size();i++){
        for(int j = 0; j < (*dun)[0].size();j++){
            unsigned int tile = DUN_PXZ_WALL;
            if(!(*dun)[i][j].connectionLeft) tile |= DUN_PYZ_WALL;
            if(!(*dun)[i][j].connectionUp) tile |= DUN_PXY_WALL;
            newDungeon->SetPos(i,0,j,tile);
        }
    }
    return newDungeon;
}

DungeonMatrix * generate(int width, int depth,int seed){
    srand (seed);
    int start_x = 0;
    int start_y = 0;
    int current_x;
    int current_y;

    std::vector<std::vector<dunTile>> dungeon;

    dungeon.resize(width);

    for(int i = 0; i < width ; i++){
        dungeon[i].resize(depth);
        for(int j = 0; j < depth; j++){
            dungeon[i][j].passed = false;
        }
    }

    current_x = start_x = rand() % width;
    current_y = start_y = rand() % depth;

    bool finished = false;
    std::cout << "width " << width << "\n";
    std::cout << "depth " << depth << "\n";

    bool firstIter = true;
    while (!finished){
        dungeon[current_x][current_y].passed = true;
        std::pair<int,int> next = getNextTile(&dungeon,current_x,current_y);
        if(next==std::pair<int,int>(-1,-1)){
            dungeon[current_x][current_y].fullyExplored = true;
            next = getConnectedTile(&dungeon,current_x,current_y);
            if(next==std::pair<int,int>(-1,-1)) 
                finished = true;
        }else{
            if(next.first==current_x+1&&next.second==current_y){
                dungeon[current_x+1][current_y].connectionLeft = true;
            }else if(next.first==current_x-1&&next.second==current_y){
                dungeon[current_x][current_y].connectionLeft = true;
            }else if(next.first==current_x&&next.second==current_y+1){
                dungeon[current_x][current_y+1].connectionUp = true;
            }else if(next.first==current_x&&next.second==current_y-1){
                dungeon[current_x][current_y].connectionUp = true;
            }
        }
        current_x = next.first;
        current_y = next.second;
        #ifdef MAZE_ANIM
        if(!firstIter) drawClean(&dungeon);
        drawState(&dungeon);
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
        firstIter = false;
        #endif
    }
    DungeonMatrix * result = Vector2Dun(&dungeon);
    result->SetStart(start_x,0,start_y);
    dunVec3 end = result->GetStart();
    while(end.x==result->GetStart().x && end.z==result->GetStart().z){
        end.x = rand() % width;
        end.z = rand() % depth;
    }
    result->SetEnd(end);
    return result;
}
// ----------------------------------------------------

int main(int args,char ** argv)
{
    std::random_device rd;
    int seed = rd();

    ArgHandler argHandler;
    argHandler.AddArg("width",10,"dungeon's width");
    argHandler.AddArg("depth",10,"dungeon's depth");

    argHandler.AddStringArg("o","a.dun","output file name");

    argHandler.AddArg("seed",seed,"dungeon's generation seed");

    if(!argHandler.ReadArgs(args,argv)){
        std::cout << "Dungeon's dimensions:" << std::endl;
        argHandler.PrintHelp("width");
        argHandler.PrintHelp("depth");
        std::cout << "General:" << std::endl;
        argHandler.PrintHelp("seed");
        argHandler.PrintHelp("o");
        return 0;
    }

    DungeonMatrix * dun = generate(
        argHandler.GetIntArg("width"),
        argHandler.GetIntArg("depth"),
        argHandler.GetIntArg("seed"));

    if(dun->Dun2File(argHandler.GetStringArg("o"))){
        std::cout << "archivo generado" << std::endl;
    }
    else{
        std::cout << "el archivo no se ha podido generar" << std::endl;
    }
    
}