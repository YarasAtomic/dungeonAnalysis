#ifndef EZ_H
#define EZ_H

#include <map>
#include <set>
#include <string>

struct Vector3Int{
    int x;
    int y;
    int z;
};

class ArgHandler{
    enum ArgType{
        STRING, INT, FLOAT, BOOL
    };
    struct ArgEntry{
        std::string description;
        std::string value;
        ArgType type;
    };
    std::map<std::string,ArgEntry> argEntries;
public:
    void AddArg(std::string key,int defaultValue,std::string description);
    void AddStringArg(std::string key,std::string defaultValue,std::string description);
    void AddArg(std::string key,float defaultValue,std::string description);
    void AddArg(std::string key,bool defaultValue,std::string description);

    void AddIntArg(std::string key,std::string description);

    bool ReadArgs(int args, char ** argv);

    std::string GetStringArg(std::string key);
    int GetIntArg(std::string key);
    float GetFloatArg(std::string key);
    bool GetBoolArg(std::string key);

    void PrintHelp(std::string);
};

std::map<std::string,std::string> getArgMap(int args,char ** argv,bool * defaultArgs,void (*help)(void));

// void help();

bool isInteger(const std::string & s);

const unsigned int DUN_FULL_BLOCK = 1;
const unsigned int DUN_PXY_WALL = 2;
const unsigned int DUN_PXZ_WALL = 4;
const unsigned int DUN_PYZ_WALL = 8;
const unsigned int DUN_FULL_VAR = 16;
const unsigned int DUN_PXY_VAR = 32;
const unsigned int DUN_PXZ_VAR = 64;
const unsigned int DUN_PYZ_VAR = 128;

struct dungeonMatrix {
    unsigned int *** data;
    unsigned int size_x = 0;
    unsigned int size_y = 0;
    unsigned int size_z = 0;
    unsigned int start_x = -1;
    unsigned int start_y = -1;
    unsigned int start_z = -1;
    unsigned int end_x = -1;
    unsigned int end_y = -1;
    unsigned int end_z = -1;
};

void allocDungeonMatrix(dungeonMatrix **dungeon,unsigned int sizeX,unsigned int sizeY, unsigned int sizeZ);

void freeDungeonMatrix(dungeonMatrix ** matrix);

bool dun2File(dungeonMatrix * dun, std::string filename);

bool file2Dun(dungeonMatrix ** dun, std::string filename);

#endif