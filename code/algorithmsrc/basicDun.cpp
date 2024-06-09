#include "ezdungeon.h"
#include <iostream>
#include <vector>
#include <stdlib.h> 
#include <random>

const int HALL_ID = -2;
const int EMPTY_ID = -1;

// ---------------------------------------------------
// ------------- GENERATION --------------------------
// ---------------------------------------------------

struct roomOptions{
    int maxRoomWidth;
    int minRoomWidth;
    int maxRoomDepth;
    int minRoomDepth;
    float probability;
    int minHallLength;
    int maxHallLength;
};

struct tileInfo{
    int roomId = -1;  
    int hallId0 = -1;
    int hallId1 = -1;
};

void generateHallTiles(int x, int y, int dir, int length,std::vector<std::vector<tileInfo>> * dungeon,int room0,int room1){
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
        if(posY >= 0 && posX >= 0 && posX < dungeon->size() && posY < (*dungeon)[0].size() && (*dungeon)[posX][posY].roomId == EMPTY_ID){
            (*dungeon)[posX][posY].roomId = HALL_ID;
        }
        (*dungeon)[posX][posY].hallId0 = room0;
        (*dungeon)[posX][posY].hallId1 = room1;
    }
}


bool testHallTiles(int x, int y, int dir, int length,std::vector<std::vector<tileInfo>> * dungeon,int ignore){
    bool valid = true;

    for(int i = 0; i < length; i++){
        int posX = -1;
        int posY = -1;
        switch(dir){
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
        if(posY >= 0 && posX >= 0 && posX<dungeon->size()&&posY<(*dungeon)[0].size()){
            if((*dungeon)[posX][posY].roomId!=EMPTY_ID&&(*dungeon)[posX][posY].roomId!=ignore)
                valid = false;
        }
        else
            valid = false;
    }
    return valid;
}

void generateRoomTiles(int x, int y, int roomWidth, int roomDepth,std::vector<std::vector<tileInfo>> * dungeon, int value){
    int cornerX = x - roomWidth / 2;
    int cornerY = y - roomDepth / 2;
    for(int i = 0; i < roomWidth; i++)
    {
        for(int j = 0; j < roomDepth; j++)
        {
            if(cornerX + i < dungeon->size() && cornerY + j < (*dungeon)[0].size())
            {
                (*dungeon)[cornerX + i][cornerY + j].roomId = value;
            }
        }
    }
}

bool testRoomTiles(int x, int y,int roomWidth, int roomDepth,std::vector<std::vector<tileInfo>> * dungeon){
    int cornerX = x - roomWidth / 2;
    int cornerY = y - roomDepth / 2;
    for(int i = 0; i < roomWidth; i++){
        for(int j = 0; j < roomDepth; j++){
            if(!(cornerX + i < dungeon->size() && cornerY + j < (*dungeon)[0].size() && (*dungeon)[cornerX + i][cornerY + j].roomId == EMPTY_ID))
                return false;
        }
    }

    return true;
}

bool generateRoom(int x, int y,roomOptions opts,std::vector<std::vector<tileInfo>> * dungeon, int * value,int * endX,int * endY,int hallDir = -1,int hallLength = -1,int parent = -1){
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
        if(hallLength!=-1 && hallDir!=-1 && parent !=-1)
            generateHallTiles(x,y,(hallDir+2)%4,hallLength,dungeon,(*value),parent);
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
        if(rand()/(float)RAND_MAX<=opts.probability)
        {
            int expectedId = (*value);
            if(testHallTiles(x,y,0,length,dungeon,id))
                // if(generateRoom(x + length,y,opts,dungeon,value,endX,endY))
                //     generateHallTiles(x,y,0,length,dungeon,expectedId,id);
                generateRoom(x + length,y,opts,dungeon,value,endX,endY,0,length,id);
        }
            
        // Left
        if(rand()/(float)RAND_MAX<=opts.probability)
        {
            int expectedId = (*value);
            if(testHallTiles(x,y,2,length,dungeon,id))
                // if(generateRoom(x - length,y,opts,dungeon,value,endX,endY))
                //     generateHallTiles(x,y,2,length,dungeon,expectedId,id);
                generateRoom(x - length,y,opts,dungeon,value,endX,endY,2,length,id);

        }

        // Down
        if(rand()/(float)RAND_MAX<=opts.probability)
        {
            int expectedId = (*value);
            if(testHallTiles(x,y,1,length,dungeon,id))
                // if(generateRoom(x,y + length,opts,dungeon,value,endX,endY))
                //     generateHallTiles(x,y,1,length,dungeon,expectedId,id);
                generateRoom(x,y + length,opts,dungeon,value,endX,endY,1,length,id);
        }

        // Up
        if(rand()/(float)RAND_MAX<=opts.probability)
        {
            int expectedId = (*value);
            if(testHallTiles(x,y,3,length,dungeon,id))
                // if(generateRoom(x,y - length,opts,dungeon,value,endX,endY))
                //     generateHallTiles(x,y,3,length,dungeon,expectedId,id);
                generateRoom(x,y - length,opts,dungeon,value,endX,endY,3,length,id);

        }
        return true;
    }
    return false;
}

bool isSeparated(int origin,int other,int hall0,int hall1){
    if(origin == EMPTY_ID && other == EMPTY_ID) return false;
    if(origin == EMPTY_ID && other == HALL_ID) return true;
    if(other == EMPTY_ID && origin == HALL_ID) return true;
    if(other != HALL_ID && origin != HALL_ID){
        if((origin==hall0&&other==hall1)||(origin==hall1&&other==hall0)) return false;
        if(origin!=EMPTY_ID && other!=origin) return true;
        if(other!=EMPTY_ID && other!=origin) return true;
    }

    return false;
}

DungeonMatrix* vector2dungeon(std::vector<std::vector<tileInfo>> * dun){
    
    int sizeY = (*dun)[0].size();
    int sizeX = dun->size();
    std::cout << "width " << sizeX << " depth " << sizeY << "\n";

    DungeonMatrix *matrix = new DungeonMatrix(sizeX,1,sizeY);

    for(int i = 0; i < sizeX;i++)
    {
        for(int j = 0; j < sizeY;j++)
        {
            unsigned int tile = 0;
            int v = (*dun)[i][j].roomId;
            int h0 = (*dun)[i][j].hallId0;
            int h1 = (*dun)[i][j].hallId1;
            
            if(i>0)
                if(isSeparated(v,(*dun)[i-1][j].roomId,h0,h1))
                    tile |= DUN_PYZ_WALL;
                    
            if(j>0)
                if(isSeparated(v,(*dun)[i][j-1].roomId,h0,h1))
                    tile |= DUN_PXY_WALL;
                    
            if(v!=EMPTY_ID)
                tile |= DUN_PXZ_WALL;

            matrix->SetPos(i,0,j,tile);
        }
    }

    return matrix;
}

DungeonMatrix * generate(int width, int depth,roomOptions opts,int seed){
    srand (seed);
    std::vector<std::vector<tileInfo>> dungeon;

    dungeon.resize(width);

    for(int i = 0; i < width ; i++){
        dungeon[i].resize(depth);
        for(int j = 0; j < depth; j++){
            dungeon[i][j].roomId = EMPTY_ID;
        }
    }

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

    DungeonMatrix * dun = vector2dungeon(&dungeon);

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
    argHandler.AddArg("width",50,"dungeon's width");
    argHandler.AddArg("depth",50,"dungeon's depth");
    argHandler.AddArg("rmMaxDepth",10,"a room's maximun depth");
    argHandler.AddArg("rmMinDepth",5,"a room's minimun depth");
    argHandler.AddArg("rmMaxWidth",10,"a room's maximun width");
    argHandler.AddArg("rmMinWidth",5,"a room's minimun width");
    argHandler.AddArg("hallMin",10,"a hall's minimun length");
    argHandler.AddArg("hallMax",15,"a hall's maximun length");
    argHandler.AddArg("prob",0.5f,"sets {probability} value");

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
        std::cout << "A float number between 0 and 1 will be generated, ";
        std::cout << "if the number is less than {probability}, a room will spawn." << std::endl; 
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
        argHandler.GetFloatArg("prob"),
        argHandler.GetIntArg("hallMin"),
        argHandler.GetIntArg("hallMax")
    };

    DungeonMatrix * dun = generate(argHandler.GetIntArg("width"),argHandler.GetIntArg("depth"),opts,argHandler.GetIntArg("seed"));
    if(dun->Dun2File(argHandler.GetStringArg("o"))){
        std::cout << "archivo generado" << std::endl;
    }
    else{
        std::cout << "el archivo no se ha podido generar" << std::endl;
    }
}