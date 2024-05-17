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

bool isInteger(const std::string & s);

const unsigned int DUN_FULL_BLOCK = 1;
const unsigned int DUN_PXY_WALL = 2;
const unsigned int DUN_PXZ_WALL = 4;
const unsigned int DUN_PYZ_WALL = 8;
const unsigned int DUN_FULL_VAR = 16;
const unsigned int DUN_PXY_VAR = 32;
const unsigned int DUN_PXZ_VAR = 64;
const unsigned int DUN_PYZ_VAR = 128;

typedef struct dunVec3{
    unsigned int x;
    unsigned int y;
    unsigned int z;
} dunVec3;

class DungeonMatrix {
    unsigned int * data;
    unsigned int size_x = 0;
    unsigned int size_y = 0;
    unsigned int size_z = 0;
    unsigned int start_x = 0;
    unsigned int start_y = 0;
    unsigned int start_z = 0;
    unsigned int end_x = 0;
    unsigned int end_y = 0;
    unsigned int end_z = 0;

    unsigned int Pos2Index(unsigned int ,unsigned int ,unsigned int );
public:
    DungeonMatrix(std::string filename);
    DungeonMatrix(int,int,int);
    DungeonMatrix(dunVec3);
    DungeonMatrix(const DungeonMatrix&);
    ~DungeonMatrix();

    void SetStart(unsigned int,unsigned int,unsigned int);
    void SetStart(dunVec3);
    void SetEnd(unsigned int,unsigned int,unsigned int);
    void SetEnd(dunVec3);
    dunVec3 GetSize();
    dunVec3 GetStart();
    dunVec3 GetEnd();
    unsigned int GetPos(dunVec3);
    void SetPos(dunVec3,unsigned int);
    void AddPos(dunVec3,unsigned int);
    void RemovePos(dunVec3,unsigned int);
    bool CheckPos(dunVec3,unsigned int);
    void SetPos(unsigned int,unsigned int,unsigned int,unsigned int);
    void AddPos(unsigned int,unsigned int,unsigned int,unsigned int);
    void RemovePos(unsigned int,unsigned int,unsigned int,unsigned int);
    bool CheckPos(unsigned int,unsigned int,unsigned int,unsigned int);

    bool Dun2File(std::string filename);
    bool File2Dun(std::string filename);
};

#endif