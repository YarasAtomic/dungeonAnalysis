#include "ezdungeon.h"
#include <iostream>

void help()
{
    std::cout << "-option print the value inserted" << std::endl;
}

int main(int args,char ** argv)
{
    bool defaultArgs;
    std::map<std::string,std::string> argMap = getArgMap(args,argv,&defaultArgs,&help);
    auto find =  argMap.find("option");
    if(find != argMap.end())
    {
        std::string value = find->second;
        std::cout << "Value: " << value << std::endl;
    }
    else
    {
        std::cout << "Option not found" << std::endl;
    }
}