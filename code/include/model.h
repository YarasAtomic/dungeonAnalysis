#ifndef MODEL_H
#define MODEL_H

#include <raylib.h>
#include <vector>
#include "ezdungeon.h"

struct dungeonMatrix;

Mesh meshFromDungeon(dungeonMatrix * dgn);

Mesh meshFromDungeonSector(dungeonMatrix * dgn, Vector3Int chunkSize, Vector3Int dungeonOrigin);

void generateVertex(int x,int y,int z,std::vector<int>* indices, std::vector<float> *vertices, int *** vertexMatrix,unsigned int * nIndices);

std::vector<Model> modelVectorFromDungeon(dungeonMatrix * dgn , Vector3Int chunkSize);

#endif