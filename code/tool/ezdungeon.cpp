#include "ezdungeon.h"
#include <fstream>
#include <iostream>

void ArgHandler::AddArg(std::string key,int defaultValue,std::string description){
    if(argEntries.find(key)==argEntries.end()){
        ArgEntry newArg;
        newArg.type = ArgType::INT;
        newArg.value = std::to_string(defaultValue);
        newArg.description = description;
        argEntries.insert(std::pair<std::string,ArgEntry> (key,newArg));
    }
}

void ArgHandler::AddStringArg(std::string key,std::string defaultValue,std::string description){
    if(argEntries.find(key)==argEntries.end()){
        ArgEntry newArg;
        newArg.type = ArgType::STRING;
        newArg.value = defaultValue;
        newArg.description = description;
        argEntries.insert(std::pair<std::string,ArgEntry> (key,newArg));
    }
}

void ArgHandler::AddArg(std::string key,float defaultValue,std::string description){
    if(argEntries.find(key)==argEntries.end()){
        ArgEntry newArg;
        newArg.type = ArgType::FLOAT;
        newArg.value = std::to_string(defaultValue);
        newArg.description = description;
        argEntries.insert(std::pair<std::string,ArgEntry> (key,newArg));
    }
}

void ArgHandler::AddArg(std::string key,bool defaultValue,std::string description){
    if(argEntries.find(key)==argEntries.end()){
        ArgEntry newArg;
        newArg.type = ArgType::BOOL;
        newArg.value = defaultValue ? "true" : "false";
        newArg.description = description;
        argEntries.insert(std::pair<std::string,ArgEntry> (key,newArg));
    }
}

void ArgHandler::AddIntArg(std::string key,std::string description){
    if(argEntries.find(key)==argEntries.end()){
        ArgEntry newArg;
        newArg.type = ArgType::STRING;
        newArg.value = "";
        newArg.description = description;
        argEntries.insert(std::pair<std::string,ArgEntry> (key,newArg));
    }
}

bool ArgHandler::ReadArgs(int args,char ** argv){
    int expectedValues = 0;
    ArgType expectedType;
    std::map<std::string, ArgHandler::ArgEntry>::iterator argData;
    bool validArgs = true;
    std::pair<std::string,ArgEntry> currentPair;
    for(int i = 1; i < args && validArgs;i++)
    {
        if(expectedValues>0){
            if(argData->second.type==ArgType::INT&&!isInteger(std::string(argv[i])))
                validArgs = false;

            argData->second.value = std::string(argv[i]);
            expectedValues--;
        }else{
            std::string key = std::string(argv[i]).substr(1);
            argData = argEntries.find(key);
            if(argData!=argEntries.end()&&argv[i][0]=='-'&&key!="help"){
                expectedType = argData->second.type;
                expectedValues = 1;
            }else{
                validArgs = false;
            }
        }
    }
    return validArgs && expectedValues == 0;
}

int ArgHandler::GetIntArg(std::string key){
    auto argData = argEntries.find(key);
    if(argData!=argEntries.end()){
        if(argData->second.type==ArgType::INT){
            return stoi(argData->second.value);
        }
    }
    return -1;
}

float ArgHandler::GetFloatArg(std::string key){
    auto argData = argEntries.find(key);
    if(argData!=argEntries.end()){
        if(argData->second.type==ArgType::FLOAT){
            return stof(argData->second.value);
        }
    }
    return -1;
}

std::string ArgHandler::GetStringArg(std::string key){
    auto argData = argEntries.find(key);
    if(argData!=argEntries.end()){
        return argData->second.value;
    }
    return "";
}

void ArgHandler::PrintHelp(std::string key){
    auto argData = argEntries.find(key);
    if(argData!=argEntries.end()){
        std::string stringType = "";
        switch(argData->second.type){
        case INT:
            stringType = "[int]";
            break;
        case STRING:
            stringType = "[str]";
            break;
        case FLOAT:
            stringType = "[flt]";
            break;
        case BOOL:
            stringType = "[bol]";
            break;
        }
    }
}

bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}

DungeonMatrix::DungeonMatrix(int sizeX,int sizeY,int sizeZ){
    data = new unsigned int[sizeX*sizeY*sizeZ];
    size_x = sizeX;
    size_y = sizeY;
    size_z = sizeZ;
}

DungeonMatrix::DungeonMatrix(dunVec3 size){
    DungeonMatrix(size.x,size.y,size.z);
}

DungeonMatrix::DungeonMatrix(const DungeonMatrix&other){
    size_x = other.size_x;
    size_y = other.size_y;
    size_z = other.size_z;
    start_x = other.start_x;
    start_y = other.start_y;
    start_z = other.start_z;
    end_x = other.end_x;
    end_y = other.end_y;
    end_z = other.end_z;

    data = new unsigned int(size_x*size_y*size_z);

    unsigned int dataSize = size_x * size_y * size_z;
    for(int i = 0; i < dataSize; i ++)
        data[i] = other.data[i];
}

DungeonMatrix::~DungeonMatrix(){
    delete[] data;
}

bool DungeonMatrix::File2Dun(std::string filename){
    std::ifstream file(filename);
    if(file.is_open()){
        std::string header;
        file >> header;
        if(header=="dungeonMatrix-1"){
            file >> size_x;
            file >> size_y;
            file >> size_z;
            start_x = 0;
            start_y = 0;
            start_z = 0;
            end_x = 0;
            end_y = 0;
            end_z = 0;
            char c;
            file.read((char*)&c,sizeof(char));
            unsigned int dataSize = size_x * size_y * size_z;
            delete[] data;
            data = new unsigned int[dataSize];
            for (int i = 0; i < dataSize; i++){
                unsigned int v;
                file.read((char*)(&v),sizeof(unsigned int));
                data[i]= v;
            }
            
            file.close();

            return true;
        } 
        else if(header=="dungeonMatrix-2"){
            file >> size_x;
            file >> size_y;
            file >> size_z;
            file >> start_x;
            file >> start_y;
            file >> start_z;
            file >> end_x;
            file >> end_y;
            file >> end_z;
            
            char c;
            file.read((char*)&c,sizeof(char));
            unsigned int dataSize = size_x * size_y * size_z;
            delete[] data;
            data = new unsigned int[dataSize];
            for (int i = 0; i < dataSize; i++){
                unsigned int v;
                file.read((char*)(&v),sizeof(unsigned int));
                data[i] = v;
            }

            file.close();
            
            return true;
        }
        file.close(); 
    }
    return false;
}

bool DungeonMatrix::Dun2File(std::string filename){
    std::ofstream file(filename,std::ofstream::trunc);
    if(file.is_open()){
        file << "dungeonMatrix-2\n";
        file << size_x << " ";
        file << size_y << " ";
        file << size_z << " ";

        file << start_x << " ";
        file << start_y << " ";
        file << start_z << " ";

        file << end_x << " ";
        file << end_y << " ";
        file << end_z << " ";

        unsigned int dataSize = size_x * size_y * size_z;

        for(int i = 0; i < dataSize; i++)
            file.write((char*)&(data[i]),sizeof(unsigned int));
        file.close();
        return true;
    }
    return false;
}

dunVec3 DungeonMatrix::GetStart(){
    dunVec3 out = {start_x,start_y,start_z};
    return out;
}

void DungeonMatrix::SetStart(dunVec3 pos){
    start_x = pos.x;
    start_y = pos.y;
    start_z = pos.z;
}

void DungeonMatrix::SetStart(unsigned int x,unsigned int y ,unsigned int z){
    SetStart({x,y,z});
}

unsigned int DungeonMatrix::Pos2Index(unsigned int x,unsigned int y,unsigned int z){
    return x + y * size_x + z * size_x * size_y;
}

dunVec3 DungeonMatrix::GetEnd(){
    dunVec3 out = {end_x,end_y,end_z};
    return out;
}

void DungeonMatrix::SetEnd(dunVec3 pos){
    end_x = pos.x;
    end_y = pos.y;
    end_z = pos.z;
}

void DungeonMatrix::SetEnd(unsigned int x,unsigned int y ,unsigned int z){
    SetEnd({x,y,z});
}

dunVec3 DungeonMatrix::GetSize(){
    dunVec3 out = {size_x,size_y,size_z};
    return out;
}
unsigned int DungeonMatrix::GetPos(dunVec3 pos){
    return data[Pos2Index(pos.x,pos.y,pos.z)];
}

void DungeonMatrix::SetPos(dunVec3 pos, unsigned int v){
    data[Pos2Index(pos.x,pos.y,pos.z)] = v;
}

void DungeonMatrix::AddPos(dunVec3 pos, unsigned int v){
    data[Pos2Index(pos.x,pos.y,pos.z)] |= v;
}

void DungeonMatrix::RemovePos(dunVec3 pos, unsigned int v){
    data[Pos2Index(pos.x,pos.y,pos.z)] &= ~v;
}

bool DungeonMatrix::CheckPos(dunVec3 pos, unsigned int v){
    return data[Pos2Index(pos.x,pos.y,pos.z)] & v;
}

void DungeonMatrix::SetPos(unsigned int x, unsigned int y, unsigned int z, unsigned int v){
    data[Pos2Index(x,y,z)] = v;
}

void DungeonMatrix::AddPos(unsigned int x, unsigned int y, unsigned int z, unsigned int v){
    data[Pos2Index(x,y,z)] |= v;
}

void DungeonMatrix::RemovePos(unsigned int x, unsigned int y, unsigned int z, unsigned int v){
    data[Pos2Index(x,y,z)] &= ~v;
}

bool DungeonMatrix::CheckPos(unsigned int x, unsigned int y, unsigned int z, unsigned int v){
    return data[Pos2Index(x,y,z)] & v;
}