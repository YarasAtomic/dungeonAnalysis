#include "ezdungeon.h"
#include <iostream>
#include <vector>
#include <stdlib.h> 
#include <set>

/**========================================================================
 * ?                                ABOUT
 * @author         :  Guillermo Medialdea Burgos
 * @email          :  
 * @repo           :  
 * @createdOn      :  20/11/23
 * @description    :  C++ implementation of Wilson's Maze Algorithm
 *========================================================================**/

// El laberinto generado, es conexo, no tiene ciclos. Por esa razón, el laberinto se puede resolver girando siempre a la derecha.
// Topologicamente hablando, este laberinto es un circulo

void help()
{
    std::cout << "Dungeon's dimensions:" << std::endl;
    std::cout << "\t-width\tdungeon's width" << std::endl;
    std::cout << "\t-depth\tdungeon's depth" << std::endl;
    std::cout << "General" << std::endl;
    std::cout << "\t-o\toutput" << std::endl;
    std::cout << "\t-seed\tgeneration seed" << std::endl;
    std::cout << "\t-help --help\tshow this prompt" << std::endl;
}

// ---------------------------------------------------
// ------------- GENERATION --------------------------
// ---------------------------------------------------

struct dunTile
{
    bool used = false;
    short walkDir = -1;
};

dungeonMatrix* vector2dungeon(std::vector<std::vector<dunTile>> * dun)
{
    dungeonMatrix *matrix;
    int sizeY = (*dun)[0].size();
    int sizeX = dun->size();
    allocDungeonMatrix(&matrix,sizeX,1,sizeY);

    for(int i = 0; i < sizeX;i++){
        for(int j = 0; j < sizeY;j++){
            matrix->data[i][0][j] = (DUN_PYZ_WALL|DUN_PXZ_WALL|DUN_PXY_WALL);
        }
    }

    for(int i = 0; i < sizeX;i++){
        for(int j = 0; j < sizeY;j++){
            switch ((*dun)[i][j].walkDir)
            {
            case 0:
                std::cout << "YZ " << matrix->data[i][0][j] ;
                matrix->data[i][0][j] &= ~DUN_PYZ_WALL;
                std::cout << " " << matrix->data[i][0][j] << std::endl;
                break;
            case 1:
                matrix->data[i+1][0][j] &= ~DUN_PYZ_WALL;
                break;
            case 2:
                matrix->data[i][0][j] &= ~DUN_PXY_WALL;
                break;
            case 3:
                matrix->data[i][0][j+1] &= ~DUN_PXY_WALL;
                break;
            default:
                break;
            }
        }
    }
    return matrix;
}

std::set<std::pair<int,int>>::iterator getRandomTile(std::set<std::pair<int,int>> tiles){ //O(n)
    std::set<std::pair<int,int>>::iterator it = std::begin(tiles);
    std::advance(it,rand() % tiles.size());
    return it;
}

int nextTile(std::pair<int,int> *pos,std::vector<std::vector<dunTile>> * dunTile){ //O(1)
    std::vector<int> availableDir;
    // Dir
    // 0 - left
    // 1 - right
    // 2 - up
    // 3 - down
    if(pos->first>0) availableDir.push_back(0); 
    if(pos->first<(*dunTile).size()-1) availableDir.push_back(1);
    if(pos->second>0) availableDir.push_back(2);
    if(pos->second<(*dunTile)[0].size()-1) availableDir.push_back(3);
    if(availableDir.empty()) return -1;
    int dir = availableDir[rand() % availableDir.size()];
    switch(dir){
    case 0:
        pos->first--;
        break;
    case 1:
        pos->first++;
        break;
    case 2:
        pos->second--;
        break;
    case 3:
        pos->second++;
        break;
    }
    return dir;
}

dungeonMatrix * generate(int width, int depth,int seed) //O(n²log(n))
{
    srand (seed);
    int end_x = 0;
    int end_y = 0;
    int start_x = 0;
    int start_y = 0;

    std::vector<std::vector<dunTile>> dungeon;
    std::set<std::pair<int,int>> availableTiles;

    dungeon.resize(width);

    for(int i = 0; i < width ; i++) //O(n log(n))
    {
        dungeon[i].resize(depth);
        for(int j = 0; j < depth; j++)
        {
            dungeon[i][j].used = false;
            availableTiles.insert(std::pair<int,int>(i,j)); //O(log(n))
        }
    }
    std::set<std::pair<int,int>>::iterator initTile = getRandomTile(availableTiles);
    dungeon[initTile->first][initTile->second].used = true;

    start_x = initTile->first;
    start_y = initTile->second;

    //? availableTiles.erase(initTile); por que esto no funciona?
    availableTiles.erase(std::pair<int,int>(initTile->first,initTile->second));
    while(!availableTiles.empty()){ //O(n²log(n))
        // Select branch origin
        auto origin = getRandomTile(availableTiles);//O(n) n siendo tiles restantes
        //? aqui pasa lo mismo
        availableTiles.erase(std::pair<int,int>(origin->first,origin->second));//O(log(n))
        // Generate the branch
        std::pair<int,int> nextPos(origin->first,origin->second);
        while((nextPos.first>=0)&&(nextPos.second>=0)&&(dungeon[nextPos.first][nextPos.second].used == false)){ //O(n) resulta en una rama de cierta longitud
            std::pair<int,int> previousPos = nextPos;
            int dir = nextTile(&nextPos,&dungeon);
            dungeon[previousPos.first][previousPos.second].walkDir = dir;
        }
        
        // Clean the branch
        std::pair<int,int> currentTile(origin->first,origin->second);
        while(dungeon[currentTile.first][currentTile.second].used == false){//O(n log(n)) siendo n la longitud de la rama 
            availableTiles.erase(currentTile);//O(log(n))
            dungeon[currentTile.first][currentTile.second].used = true;
            switch (dungeon[currentTile.first][currentTile.second].walkDir){
            case 0:
                currentTile.first--;
                break;
            case 1:
                currentTile.first++;
                break;
            case 2:
                currentTile.second--;
                break;
            case 3:
                currentTile.second++;
                break;
            }
        }
        if(availableTiles.empty()){
            end_x = currentTile.first;
            end_y = currentTile.second;
        }
    }


    dungeonMatrix * dun = vector2dungeon(&dungeon);

    dun->start_x = start_x;
    dun->start_y = 0;
    dun->start_z = start_y;

    dun->end_x = end_x;
    dun->end_y = 0;
    dun->end_z = end_y;

    return dun;
}

// ----------------------------------------------------

int main(int args,char ** argv)
{
    bool defaultArgs = true;
    std::map<std::string,std::string> argMap = getArgMap(args,argv,&defaultArgs,&help);
    int width = 4;
    int depth = 4;

    srand(time(NULL));
    int seed = rand();
    std::string output = "a.dun";

    auto findWidth = argMap.find("width");
    if(findWidth != argMap.end())
    {
        if(isInteger(findWidth->second))
            width = stoi(findWidth->second);
    }

    auto findDepth = argMap.find("depth");
    if(findDepth != argMap.end())
    {
        if(isInteger(findDepth->second))
            depth = stoi(findDepth->second);
    }

    auto findSeed = argMap.find("seed");
    if(findSeed != argMap.end())
    {
        if(isInteger(findSeed->second))
            seed = stoi(findSeed->second);
    }

    auto findOutput = argMap.find("o");
    if(findOutput != argMap.end())
    {
        output = findOutput->second;
    }

    if(!argMap.empty()||defaultArgs)
    {
        std::cout << "WIDTH: " << width << std::endl;
        std::cout << "DEPTH: " << depth << std::endl;
        std::cout << "----------------" << std::endl;
        std::cout << "SEED:" << seed << std::endl;

        dungeonMatrix * dun = generate(width,depth,seed);

        if(dun2File(dun,output))
        {
            std::cout << "archivo generado" << std::endl;
        }
        else
        {
            std::cout << "el archivo no se ha podido generar" << std::endl;
        }
    }
}