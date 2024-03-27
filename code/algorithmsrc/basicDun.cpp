#include "ezdungeon.h"
#include <iostream>
#include <vector>
#include <stdlib.h> 

const int HALL_ID = 9999;

void help()
{
    std::cout << "Dungeon's dimensions:" << std::endl;
    std::cout << "\t-width\tdungeon's width" << std::endl;
    std::cout << "\t-depth\tdungeon's depth" << std::endl;
    std::cout << "\t-rmMinDepth\ta room's minimun depth" << std::endl;
    std::cout << "\t-rmMaxDepth\ta room's maximun depth" << std::endl;
    std::cout << "\t-rmMinWidth\ta room's minimun width" << std::endl;
    std::cout << "\t-rmMaxWidth\ta room's maximun width" << std::endl;
    std::cout << "\t-hallMin\ta hall's minimun length" << std::endl;
    std::cout << "\t-hallMax\ta hall's maximun length" << std::endl;
    std::cout << "Room chance:" << std::endl;
    std::cout << "Each time a room spawns, it will try to spawn one more room at each side. ";
    std::cout << "A number between 0 and {totalChance} will be generated, ";
    std::cout << "if the number is less than {probability}, a room will spawn." << std::endl; 
    std::cout << "\t-chance\tsets {totalChance} value" << std::endl;
    std::cout << "\t-prob\tsets {probability} value" << std::endl;
    std::cout << "General" << std::endl;
    std::cout << "\t-o\toutput" << std::endl;
    std::cout << "\t-seed\tgeneration seed" << std::endl;
    std::cout << "\t-help --help\tshow this prompt" << std::endl;
}

// ---------------------------------------------------
// ------------- GENERATION --------------------------
// ---------------------------------------------------

struct roomOptions
{
    int maxRoomWidth;
    int minRoomWidth;
    int maxRoomDepth;
    int minRoomDepth;
    int chanceTotal;
    int probability;
    int minHallLength;
    int maxHallLength;
};

void printDungeon(std::vector<std::vector<int>> * dungeon)
{
    std::cout << "\t";
    for(int i = 0; i < dungeon->size(); i+=2)
    {
        if(i<=9)
            std::cout << i << "   ";
        else
            std::cout << i << "  ";
    }
    std::cout << std::endl << "\t  ";

    for(int i = 1; i < dungeon->size(); i+=2)
    {
        if(i<=9)
            std::cout << i << "   ";
        else
            std::cout << i << "  ";
    }
    std::cout << std::endl;

    for(int j = 0; j < (*dungeon)[0].size(); j++)
    {
        std::cout << j << "\t";
        for(int i = 0; i < dungeon->size(); i++)
        {
            int v = (*dungeon)[i][j];
            if(v >= 0 && v <= 9)
                std::cout << v << " ";
            else if(v > 9 && v <= ('z'-'a' + 10))
                std::cout << (char)(('a')+v-10) << " ";
            else if(v > ('z'-'a' + 10) && v <= ('z'-'a')*2 + 10)
                std::cout << (char)(('A')+v-10-('z'-'a')) << " ";
            else if(v < 0)
                std::cout << "· ";
            else if(v == HALL_ID)
                std::cout << "# ";
            else 
                std::cout << "? ";
        }
        std::cout << std::endl;
    } 
}

void generateHallTiles(int x, int y, int dir, int length,std::vector<std::vector<int>> * dungeon)
{
    int posX = 0;
    int posY = 0;
    for(int i = 0; i < length; i++)
    {
        switch(dir)
        {
        case 0:
            posY = y;
            posX = x+i;
            break;
        case 1:
            posY = y+i;
            posX = x;
            break;
        case 2:
            posY = y;
            posX = x-i;
            break;
        case 3:
            posY = y-i;
            posX = x;
            break;
        }
        if(posY >= 0 && posX >= 0 && posX < dungeon->size() && posY < (*dungeon)[0].size() && (*dungeon)[posX][posY] < 0)
            (*dungeon)[posX][posY] = HALL_ID;
    }
}

bool testHallTiles(int x, int y, int dir, int length,std::vector<std::vector<int>> * dungeon,int ignore)
{
    bool valid = true;

    for(int i = 0; i < length; i++)
    {
        int posX = -1;
        int posY = -1;
        switch(dir)
        {
        case 0:
            posX = x+i;
            posY = y;
            break;
        case 1:
            posX = x;
            posY = y+i;
            break;
        case 2:
            posX = x-i;
            posY = y;
            break;
        case 3:
            posX = x;
            posY = y-i;
            break;
        }
        if(posY >= 0 && posX >= 0 && posX<dungeon->size()&&posY<(*dungeon)[0].size())
        {
            if((*dungeon)[posX][posY]>=0&&(*dungeon)[posX][posY]!=ignore)
                valid = false;
        }
        else
            valid = false;
    }
    return valid;
}

void generateRoomTiles(int x, int y, int roomWidth, int roomDepth,std::vector<std::vector<int>> * dungeon, int value)
{
    int cornerX = x - roomWidth / 2;
    int cornerY = y - roomDepth / 2;
    for(int i = 0; i < roomWidth; i++)
    {
        for(int j = 0; j < roomDepth; j++)
        {
            if(cornerX + i < dungeon->size() && cornerY + j < (*dungeon)[0].size())
            {
                (*dungeon)[cornerX + i][cornerY + j] = value;
            }
        }
    }
}

bool testRoomTiles(int x, int y,int roomWidth, int roomDepth,std::vector<std::vector<int>> * dungeon)
{
    int cornerX = x - roomWidth / 2;
    int cornerY = y - roomDepth / 2;
    bool valid = true;
    for(int i = 0; i < roomWidth; i++)
    {
        for(int j = 0; j < roomDepth; j++)
        {
            if(!(cornerX + i < dungeon->size() && cornerY + j < (*dungeon)[0].size() && (*dungeon)[cornerX + i][cornerY + j] < 0))
                valid = false;
        }
    }

    return valid;
}

bool generateRoom(int x, int y,roomOptions opts,std::vector<std::vector<int>> * dungeon, int * value,int * endX,int * endY)
{
    int roomWidth = 0;
    int roomDepth = 0;
    int id = (*value);

    if(opts.maxRoomWidth == opts.minRoomWidth)
        roomWidth = opts.maxRoomWidth;
    else
        roomWidth = (rand() % (opts.maxRoomWidth - opts.minRoomWidth)) + opts.minRoomWidth;
    
    if(opts.maxRoomDepth == opts.minRoomDepth)
        roomDepth = opts.maxRoomDepth;
    else
        roomDepth = (rand() % (opts.maxRoomDepth - opts.minRoomDepth)) + opts.minRoomDepth;

    if(testRoomTiles(x,y,roomWidth,roomDepth,dungeon))
    {
        generateRoomTiles(x,y,roomWidth,roomDepth,dungeon,(*value));

        (*endX) = x;
        (*endY) = y;

        (*value)++;
        int length = 0;

        if(opts.maxHallLength==opts.minHallLength)
            length = opts.maxHallLength;
        else
            length = rand()%(opts.maxHallLength - opts.minHallLength) + opts.minHallLength;

        // Right
        if(rand()%opts.chanceTotal<opts.probability)
        {
            if(testHallTiles(x,y,0,length,dungeon,id))
                if(generateRoom(x + length,y,opts,dungeon,value,endX,endY))
                    generateHallTiles(x,y,0,length,dungeon);
        }
            
        // Left
        if(rand()%opts.chanceTotal<opts.probability)
        {
            if(testHallTiles(x,y,2,length,dungeon,id))
                if(generateRoom(x - length,y,opts,dungeon,value,endX,endY))
                    generateHallTiles(x,y,2,length,dungeon);
        }

        // Down
        if(rand()%opts.chanceTotal<opts.probability)
        {
            if(testHallTiles(x,y,1,length,dungeon,id))
                if(generateRoom(x,y + length,opts,dungeon,value,endX,endY))
                    generateHallTiles(x,y,1,length,dungeon);
        }

        // Up
        if(rand()%opts.chanceTotal<opts.probability)
        {
            if(testHallTiles(x,y,3,length,dungeon,id))
                if(generateRoom(x,y - length,opts,dungeon,value,endX,endY))
                    generateHallTiles(x,y,3,length,dungeon);
        }
        return true;
    }
    return false;
}

bool isSeparated(int origin,int other)
{
    if(origin < 0 && other == HALL_ID) return true;
    if(other < 0 && origin == HALL_ID) return true;
    if(other != HALL_ID && origin != HALL_ID)
    {
        if(origin>=0 && other!=origin) return true;
        if(other>=0 && other!=origin) return true;
    }

    return false;
}

dungeonMatrix* vector2dungeon(std::vector<std::vector<int>> * dun)
{
    dungeonMatrix *matrix;
    int sizeX = (*dun)[0].size();
    int sizeY = dun->size();
    allocDungeonMatrix(&matrix,sizeX,1,sizeY);

    for(int i = 0; i < sizeX;i++)
    {
        for(int j = 0; j < sizeY;j++)
        {
            unsigned int tile = 0;
            int v = (*dun)[i][j];
            
            if(i>0)
                if(isSeparated(v,(*dun)[i-1][j]))
                    tile |= DUN_PYZ_WALL;
                    
            if(j>0)
                if(isSeparated(v,(*dun)[i][j-1]))
                    tile |= DUN_PXY_WALL;
                    
            if(v>=0)
                tile |= DUN_PXZ_WALL;

            

            matrix->data[i][0][j] = tile;
        }
    }

    return matrix;
}

dungeonMatrix * generate(int width, int depth,roomOptions opts,int seed)
{
    srand (seed);

    std::vector<std::vector<int>> dungeon;

    dungeon.resize(width);

    for(int i = 0; i < width ; i++)
    {
        dungeon[i].resize(depth);
        for(int j = 0; j < depth; j++)
        {
            dungeon[i][j] = -1;
        }
    }

    int defaultEndX = -1;
    int defaultEndY = -1;

    int * endX = &defaultEndX;
    int * endY = &defaultEndY;

    if(width > opts.minRoomWidth && depth > opts.minRoomDepth)
    {
        int originX = (rand() % (width - opts.minRoomWidth)) + opts.minRoomWidth / 2;
        int originY = (rand() % (depth - opts.minRoomDepth)) + opts.minRoomDepth / 2;

        int roomNumber = 0;

        generateRoom(width/2,depth/2,opts,&dungeon,&roomNumber,endX,endY);
    }

    printDungeon(&dungeon);

    dungeonMatrix * dun = vector2dungeon(&dungeon);

    dun->start_x = width/2;
    dun->start_y = 0;
    dun->start_z = depth/2;

    dun->end_x = (*endX);
    dun->end_y = 0;
    dun->end_z = (*endY);

    return dun;
}

// ----------------------------------------------------

int main(int args,char ** argv)
{
    bool defaultArgs = true;
    std::map<std::string,std::string> argMap = getArgMap(args,argv,&defaultArgs,&help);
    int width = 64;
    int depth = 64;
    roomOptions opts;
    opts.maxRoomDepth = 10;
    opts.maxRoomWidth = 10;
    opts.minRoomDepth = 5;
    opts.minRoomWidth = 5;
    opts.probability = 3;
    opts.chanceTotal = 4;
    opts.minHallLength = 10;
    opts.maxHallLength = 15;

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

    auto findMinDepth = argMap.find("rmMinDepth");
    if(findMinDepth != argMap.end())
    {
        if(isInteger(findMinDepth->second))
            opts.minRoomDepth = stoi(findMinDepth->second);
    }

    auto findMaxDepth = argMap.find("rmMaxDepth");
    if(findMaxDepth != argMap.end())
    {
        if(isInteger(findMaxDepth->second))
            opts.maxRoomDepth = stoi(findMaxDepth->second);
    }

    auto findMinWidth = argMap.find("rmMinWidth");
    if(findMinWidth != argMap.end())
    {
        if(isInteger(findMinWidth->second))
            opts.minRoomWidth = stoi(findMinWidth->second);
    }

    auto findMaxWidth = argMap.find("rmMaxWidth");
    if(findMaxWidth != argMap.end())
    {
        if(isInteger(findMaxWidth->second))
            opts.maxRoomWidth = stoi(findMaxWidth->second);
    }

    auto findhallMax = argMap.find("hallMax");
    if(findhallMax != argMap.end())
    {
        if(isInteger(findhallMax->second))
            opts.maxHallLength = stoi(findhallMax->second);
    }

    auto findhallMin = argMap.find("hallMin");
    if(findhallMin != argMap.end())
    {
        if(isInteger(findhallMin->second))
            opts.minHallLength = stoi(findhallMin->second);
    }

    auto findChance = argMap.find("chance");
    if(findChance != argMap.end())
    {
        if(isInteger(findChance->second))
            opts.chanceTotal = stoi(findChance->second);
    }

    auto findProbability = argMap.find("prob");
    if(findProbability != argMap.end())
    {
        if(isInteger(findProbability->second))
            opts.probability = stoi(findProbability->second);
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
        std::cout << "MAX ROOM WIDTH: " << opts.maxRoomWidth << std::endl;
        std::cout << "MIN ROOM WIDTH: " << opts.minRoomWidth << std::endl;
        std::cout << "MAX ROOM DEPTH: " << opts.maxRoomDepth << std::endl;
        std::cout << "MIN ROOM DEPTH: " << opts.minRoomDepth << std::endl;  
        std::cout << "MAX HALL LENGTH: " << opts.maxHallLength << std::endl;
        std::cout << "MIN HALL LENGTH: " << opts.minHallLength << std::endl;
        std::cout << "TOTAL CHANCE: " << opts.chanceTotal << std::endl;
        std::cout << "PROBABILITY: " << opts.probability << std::endl;
        std::cout << "----------------" << std::endl;
        std::cout << "SEED:" << seed << std::endl;

        dungeonMatrix * dun = generate(width,depth,opts,seed);

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