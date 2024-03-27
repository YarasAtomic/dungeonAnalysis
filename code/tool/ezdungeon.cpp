#include "ezdungeon.h"
#include <fstream>
#include <iostream>

std::map<std::string,std::string> getArgMap(int args,char ** argv,bool * defaultArgs, void (*help)(void))
{
    bool readingValues = false;
    bool validArgs = true;
    std::pair<std::string,std::string> currentPair;
    std::map<std::string,std::string> out;
    for(int i = 1; i < args && validArgs;i++)
    {
        if(!readingValues)
        {
            if(argv[i][0]!='-')
            {
                validArgs = false;
            }
            else
            {
                currentPair.first = "";
                int j = 1;
                while(argv[i][j]!='\0')
                {
                    currentPair.first+=argv[i][j];
                    
                    j++;
                }
                readingValues = true;
                if(currentPair.first == "help" || currentPair.first == "-help")
                {
                    help();
                    readingValues = false;
                    validArgs = false;
                }
            }
        }
        else
        {
            currentPair.second = std::string(argv[i]);
            out.insert(currentPair);
            readingValues = false;
        }
        
    }
    (*defaultArgs) = args == 1 ? true : false;
    if (!validArgs) return std::map<std::string,std::string>();
    return out;
}

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
        newArg.type = ArgType::STRING;
        newArg.value = std::to_string(defaultValue);
        newArg.description = description;
        argEntries.insert(std::pair<std::string,ArgEntry> (key,newArg));
    }
}

void ArgHandler::AddArg(std::string key,bool defaultValue,std::string description){
    if(argEntries.find(key)==argEntries.end()){
        ArgEntry newArg;
        newArg.type = ArgType::STRING;
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
        std::cout << "\t-"<<key<<"\t" << stringType <<"\t"<<argData->second.description << std::endl;
    }
}

bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}

void allocDungeonMatrix(dungeonMatrix **dungeon,unsigned int sizeX,unsigned int sizeY, unsigned int sizeZ)
{
    (*dungeon)= static_cast<struct dungeonMatrix*>(malloc(sizeof(struct dungeonMatrix)));

    unsigned int *** data = (unsigned int ***)malloc(sizeX * sizeof(unsigned int **));
    for(int i = 0 ; i < sizeX; i++)
    {
        data[i] = (unsigned int **)malloc(sizeY * sizeof(unsigned int *));
        for(int j = 0 ; j < sizeY; j++)
            data[i][j] = (unsigned int *)malloc(sizeZ * sizeof(unsigned int));
    }

    (*dungeon)->data = data;
    (*dungeon)->size_x = sizeX;
    (*dungeon)->size_y = sizeY;
    (*dungeon)->size_z = sizeZ;
}

void freeDungeonMatrix(dungeonMatrix ** matrix)
{
    if (*matrix) {
        for (unsigned int i = 0; i < (*matrix)->size_x; i++) {
            for (unsigned int j = 0; j < (*matrix)->size_y; j++) {
                delete[] (*matrix)->data[i][j];
            }
            delete[] (*matrix)->data[i];
        }
        delete[] (*matrix)->data;
        delete (*matrix);
    }
}

bool dun2File(dungeonMatrix * dun,std::string filename)
{
    int sizeX = dun->size_x;
    int sizeY = dun->size_y;
    int sizeZ = dun->size_z;

    std::ofstream file(filename,std::ofstream::trunc);
    if(file.is_open())
    {
        file << "dungeonMatrix-2\n";
        file << sizeX << " ";
        file << sizeY << " ";
        file << sizeZ << " ";

        file << dun->start_x << " ";
        file << dun->start_y << " ";
        file << dun->start_z << " ";

        file << dun->end_x << " ";
        file << dun->end_y << " ";
        file << dun->end_z << " ";

        for (int i = 0; i < sizeX; i++)
        {
            for(int j = 0; j < sizeY; j++)
            {
                for(int k = 0; k < sizeZ; k++)
                {
                    file.write((char*)&(dun->data[i][j][k]),sizeof(unsigned int));
                }
            }
        }
        // file.write(reinterpret_cast<char*>(dun->data), sizeof(unsigned int) * sizeX*sizeY*sizeZ);
        // file.write((char*)(dun->data),sizeof(unsigned int)*sizeX*sizeY*sizeZ);
        file.close();
        return true;
    }
    return false;
}

bool file2Dun(dungeonMatrix ** dun, std::string filename)
{
    std::ifstream file(filename);
    if(file.is_open())
    {
        std::string header;
        int sizeX;
        int sizeY;
        int sizeZ;
        int startX = -1;
        int startY = -1;
        int startZ = -1;
        int endX = -1;
        int endY = -1;
        int endZ = -1;
        file >> header;

        // std::cout << "header: " << header << std::endl;

        if(header=="dungeonMatrix-1")
        {
            file >> sizeX;
            file >> sizeY;
            file >> sizeZ;
            char c;
            file.read((char*)&c,sizeof(char));

            allocDungeonMatrix(dun,sizeX,sizeY,sizeZ);

            for (int i = 0; i < sizeX; i++)
            {
                for(int j = 0; j < sizeY; j++)
                {
                    for(int k = 0; k < sizeZ; k++)
                    {
                        unsigned int v;
                        file.read((char*)(&v),sizeof(unsigned int));
                        (*dun)->data[i][j][k] = v;
                    }
                }
            }
            
            file.close();

            return true;
        } 
        else if(header=="dungeonMatrix-2")
        {
            file >> sizeX;
            file >> sizeY;
            file >> sizeZ;
            file >> startX;
            file >> startY;
            file >> startZ;
            file >> endX;
            file >> endY;
            file >> endZ;
            char c;
            file.read((char*)&c,sizeof(char));

            allocDungeonMatrix(dun,sizeX,sizeY,sizeZ);

            for (int i = 0; i < sizeX; i++)
            {
                for(int j = 0; j < sizeY; j++)
                {
                    for(int k = 0; k < sizeZ; k++)
                    {
                        unsigned int v;
                        file.read((char*)(&v),sizeof(unsigned int));
                        (*dun)->data[i][j][k] = v;
                    }
                }
            }

            (*dun)->start_x = startX;
            (*dun)->start_y = startY;
            (*dun)->start_z = startZ;

            (*dun)->end_x = endX;
            (*dun)->end_y = endY;
            (*dun)->end_z = endZ;
            
            file.close();

            return true;
        }
        file.close(); 
    }
    return false;
}