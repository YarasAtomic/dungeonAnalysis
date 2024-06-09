#include "densityMap.h"
#include <iostream>

DensityMap::DensityMap(){}

DensityMap::DensityMap(int sizeX, int sizeY){
    Init(sizeX,sizeY);
}

void DensityMap::Init(int sizeX,int sizeY){
    max = 0;
    width = sizeX;
    height = sizeY;
    if(data!=nullptr) delete data;
    data = new std::vector<std::vector<unsigned int>>();
    (*data).resize(sizeX); // resize density map
    for(int i = 0; i < sizeX;i++)
        (*data)[i] = std::vector<unsigned int> (sizeY,0);// initialize density map
}

unsigned int DensityMap::GetMax() const{
    return max;
}

unsigned int DensityMap::Get(int x, int y) const{
    return (*data)[x][y];
}

void DensityMap::Set(int x, int y, unsigned v){
    // check if the max has changed
    if(v>max) max = v;
    else if((*data)[x][y] == max){
        max = 0;
        for(int i = 0; i < width;i++)
            for(int j = 0; j < height;j++)
                if((*data)[i][j]>max) max = (*data)[i][j];
    }
    // change value
    (*data)[x][y] = v;
}

void DensityMap::Add(int x, int y, int v){
    // change value
    (*data)[x][y] += v;
    // check if the max has changed
    if((*data)[x][y]>max) max = (*data)[x][y];
}

DensityMap::~DensityMap(){
    delete data;
}

unsigned char GetColorGradientByte(unsigned char c0, unsigned char c1, float value){
    return ((float)(c1-c0) * value) + c0;
}

Color GetColorGradient(Color c0, Color c1, float value){
    return {
        GetColorGradientByte(c0.r,c1.r,value),
        GetColorGradientByte(c0.g,c1.g,value),
        GetColorGradientByte(c0.b,c1.b,value),
        GetColorGradientByte(c0.a,c1.a,value)
    };
}

void DensityMap::Draw(Vector3 pos, Vector3 size, Color color,int maxValue){
    float densityStep = 1/(float)maxValue;
    if(data == nullptr) return;
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            float relDensity = (*data)[i][j]*densityStep;
            /*
            Vector3 tilePos = {
                (pos.x+(i+0.5f) * size.x),
                pos.y+(relDensity*10) * size.y,
                pos.z+(j+0.5f) * size.z};
            DrawPlane(tilePos,{size.x,size.z},GetColorGradient(color,WHITE,relDensity));*/
             Vector3 tilePos = {
                (pos.x+(i+0.5f) * size.x),
                pos.y+relDensity*5,
                pos.z+(j+0.5f) * size.z};
            DrawCube(tilePos,size.x,(relDensity*10) * size.y,size.x,GetColorGradient(color,WHITE,relDensity));
        }
    }
}

void DensityMap::DrawDivided(Vector3 pos, Vector3 size, Color color,int maxValue,const DensityMap * densityMap){
    if(width!=densityMap->width || height!=densityMap->height) return;
    float densityStep = 1/(float)maxValue;
    if(data == nullptr) return;
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            unsigned int otherDensity = densityMap->Get(i,j);
            float relDensity = otherDensity == 0 ? 0 : (*data)[i][j] / (float)otherDensity; 
            // float relDensity = (*data)[i][j]*densityStep;
            Vector3 tilePos = {
                (pos.x+(i+0.5f) * size.x),
                pos.y+relDensity*5,
                pos.z+(j+0.5f) * size.z};
            DrawCube(tilePos,size.x,(relDensity*10) * size.y,size.x,GetColorGradient(color,WHITE,relDensity));
        }
    }
}
