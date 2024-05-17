#include "ezdungeon.h"
#include <iostream>
#include <vector>
#include <stdlib.h> 
#include <random>

const int HALL_ID = 9999;

// ---------------------------------------------------
// ------------- GENERATION --------------------------
// ---------------------------------------------------

struct roomOptions{
    int maxRoomWidth;
    int minRoomWidth;
    int maxRoomDepth;
    int minRoomDepth;
    int chanceTotal;
    int probability;
    int minHallLength;
    int maxHallLength;
};

void printDungeon(std::vector<std::vector<int>> * dungeon){
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
        // std::cout << std::endl;
    } 
}

void generateHallTiles(int x, int y, int dir, int length,std::vector<std::vector<int>> * dungeon){
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

bool testHallTiles(int x, int y, int dir, int length,std::vector<std::vector<int>> * dungeon,int ignore){
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

void generateRoomTiles(int x, int y, int roomWidth, int roomDepth,std::vector<std::vector<int>> * dungeon, int value){
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

bool testRoomTiles(int x, int y,int roomWidth, int roomDepth,std::vector<std::vector<int>> * dungeon){
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

bool generateRoom(int x, int y,roomOptions opts,std::vector<std::vector<int>> * dungeon, int * value,int * endX,int * endY){
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

bool isSeparated(int origin,int other){
    if(origin < 0 && other == HALL_ID) return true;
    if(other < 0 && origin == HALL_ID) return true;
    if(other != HALL_ID && origin != HALL_ID)
    {
        if(origin>=0 && other!=origin) return true;
        if(other>=0 && other!=origin) return true;
    }

    return false;
}

DungeonMatrix* vector2dungeon(std::vector<std::vector<int>> * dun){
    
    int sizeY = (*dun)[0].size();
    int sizeX = dun->size();
    std::cout << "width " << sizeX << " depth " << sizeY << "\n";

    DungeonMatrix *matrix = new DungeonMatrix(sizeX,1,sizeY);

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

            matrix->SetPos(i,0,j,tile);
        }
    }

    return matrix;
}

DungeonMatrix * generate(int width, int depth,roomOptions opts,int seed){
    srand (seed);
    std::vector<std::vector<int>> dungeon;

    dungeon.resize(width);

    for(int i = 0; i < width ; i++){
        dungeon[i].resize(depth);
        for(int j = 0; j < depth; j++){
            dungeon[i][j] = -1;
        }
    }

    std::cout << "1\n";

    int defaultEndX = -1;
    int defaultEndY = -1;

    int * endX = &defaultEndX;
    int * endY = &defaultEndY;
    if(width > opts.minRoomWidth && depth > opts.minRoomDepth){
        int originX = (rand() % (width - opts.minRoomWidth)) + opts.minRoomWidth / 2;
        int originY = (rand() % (depth - opts.minRoomDepth)) + opts.minRoomDepth / 2;

        int roomNumber = 0;

        generateRoom(width/2,depth/2,opts,&dungeon,&roomNumber,endX,endY);
    }
    std::cout << "2\n";

    // printDungeon(&dungeon);
    DungeonMatrix * dun = vector2dungeon(&dungeon);
    std::cout << "3\n";

    dun->SetStart(width/2,0,depth/2);
    dun->SetEnd(*endX,0,*endY);

    return dun;
}

// ----------------------------------------------------

int main(int args,char ** argv){
    bool defaultArgs = true;
    ArgHandler argHandler;
    std::random_device rd;
    int seed = rd();
    argHandler.AddArg("width",64,"dungeon's width");
    argHandler.AddArg("depth",64,"dungeon's depth");
    argHandler.AddArg("rmMaxDepth",10,"a room's maximun depth");
    argHandler.AddArg("rmMinDepth",5,"a room's minimun depth");
    argHandler.AddArg("rmMaxWidth",10,"a room's maximun width");
    argHandler.AddArg("rmMinWidth",5,"a room's minimun width");
    argHandler.AddArg("hallMin",10,"a hall's minimun length");
    argHandler.AddArg("hallMax",15,"a hall's maximun length");
    argHandler.AddArg("chance",4,"sets {totalChance} value");
    argHandler.AddArg("prob",3,"sets {probability} value");

    argHandler.AddStringArg("o","a.dun","output file name");

    argHandler.AddArg("seed",seed,"dungeon's generation seed");

    if(!argHandler.ReadArgs(args,argv)){
        std::cout << "Dungeon's dimensions:" << std::endl;
        argHandler.PrintHelp("width");
        argHandler.PrintHelp("depth");
        argHandler.PrintHelp("rmMaxDepth");
        argHandler.PrintHelp("rmMinDepth");
        argHandler.PrintHelp("rmMaxWidth");
        argHandler.PrintHelp("rmMinWidth");
        argHandler.PrintHelp("hallMin");
        argHandler.PrintHelp("hallMax");
        std::cout << "Room chance:\t";
        std::cout << "Each time a room spawns, it will try to spawn one more room at each side. ";
        std::cout << "A number between 0 and {totalChance} will be generated, ";
        std::cout << "if the number is less than {probability}, a room will spawn." << std::endl; 
        argHandler.PrintHelp("chance");
        argHandler.PrintHelp("prob");
        std::cout << "General" << std::endl;
        argHandler.PrintHelp("seed");
        argHandler.PrintHelp("o");
        return 0;
    }

    roomOptions opts = {
        argHandler.GetIntArg("rmMaxWidth"),
        argHandler.GetIntArg("rmMinWidth"),
        argHandler.GetIntArg("rmMaxDepth"),
        argHandler.GetIntArg("rmMinDepth"),
        argHandler.GetIntArg("chance"),
        argHandler.GetIntArg("prob"),
        argHandler.GetIntArg("hallMin"),
        argHandler.GetIntArg("hallMax")
    };

    DungeonMatrix * dun = generate(argHandler.GetIntArg("width"),argHandler.GetIntArg("depth"),opts,argHandler.GetIntArg("seed"));
    std::cout << "4\n";
    if(dun->Dun2File(argHandler.GetStringArg("o"))){
        std::cout << "archivo generado" << std::endl;
    }
    else{
        std::cout << "el archivo no se ha podido generar" << std::endl;
    }
    
}