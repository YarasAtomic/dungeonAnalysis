#include <vector>
#include "raylib.h"

class DensityMap{
    std::vector<std::vector<unsigned int>> * data = nullptr;
    int width = 0;
    int height = 0;
    unsigned int max = 0;
public:
    DensityMap(int sizeX,int sizeY);
    DensityMap();
    void Init(int sizeX,int sizeY);
    
    unsigned int Get(int x, int y) const;
    unsigned int GetMax() const;
    void Set(int x, int y, unsigned v);
    ~DensityMap();
    void Add(int x,int y, int v);

    void Draw(Vector3 pos, Vector3 size, Color color,int maxValue);
    void DrawDivided(Vector3 pos, Vector3 size, Color color,int maxValue,const DensityMap * densityMap);
};