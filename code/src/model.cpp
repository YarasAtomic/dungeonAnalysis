#include <raylib.h>

#include "model.h"
#include "renderer.h"

void generateVertex(int x,int y,int z,std::vector<int>* indices, std::vector<float> *vertices, int *** vertexMatrix,unsigned int * nIndices)
{
    if(vertexMatrix[x][y][z] == -1)
    {
        vertexMatrix[x][y][z] = *nIndices;

        indices->push_back(*nIndices);

        vertices->push_back(x);
        vertices->push_back(y);
        vertices->push_back(z);

        (*nIndices)++;
    }
    else
    {
        indices->push_back(vertexMatrix[x][y][z]);
    }
}

void generateVertex(int x, int y, int z,std::vector<int> *indices, std::vector<float> *vertices)
{
    vertices->push_back(x);
    vertices->push_back(y);
    vertices->push_back(z);

    indices->push_back(indices->size());
}

void calculateFlatNormals(std::vector<float> * vertices,std::vector<int> * indices, std::vector<float> * normals)
{
    for(int i = 0; i < indices->size();i+=3)
    {
        Vector3 v0 = {(*vertices)[i*3],(*vertices)[i*3+1],(*vertices)[i*3+2]};
        Vector3 v1 = {(*vertices)[(i+1)*3],(*vertices)[(i+1)*3+1],(*vertices)[(i+1)*3+2]};
        Vector3 v2 = {(*vertices)[(i+2)*3],(*vertices)[(i+2)*3+1],(*vertices)[(i+2)*3+2]};

        Vector3 edge0 = {v1.x-v0.x,v1.y-v0.y,v1.z-v0.z};
        Vector3 edge1 = {v2.x-v0.x,v2.y-v0.y,v2.z-v0.z};

        Vector3 n = {
            edge0.y*edge1.z - edge0.z*edge1.y,
            edge0.z*edge1.x - edge0.x*edge1.z,
            edge0.x*edge1.y - edge0.y*edge1.x
        };

        normals->push_back(n.x);
        normals->push_back(n.y);
        normals->push_back(n.z);

        normals->push_back(n.x);
        normals->push_back(n.y);
        normals->push_back(n.z);

        normals->push_back(n.x);
        normals->push_back(n.y);
        normals->push_back(n.z);
    }
}

Mesh meshFromDungeon(DungeonMatrix * dgn)
{
    int vm_size_x = dgn->GetSize().x + 1;
    int vm_size_y = dgn->GetSize().y + 1;
    int vm_size_z = dgn->GetSize().z + 1;

    Mesh mesh = {0};

    unsigned int nIndices = 0;

    std::vector<int> indices;
    std::vector<float> vertices;
    std::vector<float> normals;

    for(unsigned int i = 0 ; i < dgn->GetSize().x; i++)
    {
        for(unsigned int j = 0 ; j < dgn->GetSize().y; j++)
        {
            for(unsigned int k = 0 ; k < dgn->GetSize().z; k++)
            {
                if(dgn->GetPos({i,j,k}) & DUN_PXY_WALL)
                {
                    // Front face
                    generateVertex(i,j,k,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j+1,k,&indices,&vertices);

                    generateVertex(i+1,j+1,k,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j+1,k,&indices,&vertices);

                    //--------------------------
                    // Back face
                    generateVertex(i,j+1,k,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j,k,&indices,&vertices);

                    generateVertex(i,j+1,k,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i+1,j+1,k,&indices,&vertices);

                    mesh.triangleCount+=4;
                }
                if(dgn->GetPos({i,j,k}) & DUN_PXZ_WALL)
                {
                    // Top face
                    generateVertex(i,j,k,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);

                    generateVertex(i+1,j,k+1,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);

                    //--------------------------
                    // Botton face
                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j,k,&indices,&vertices);

                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i+1,j,k+1,&indices,&vertices);

                    mesh.triangleCount+=4;
                }
                if(dgn->GetPos({i,j,k}) & DUN_PYZ_WALL)
                {
                    // Front face
                    generateVertex(i,j,k,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i,j+1,k,&indices,&vertices);

                    generateVertex(i,j+1,k+1,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i,j+1,k,&indices,&vertices);
 
                    //--------------------------
                    // Back face
                    generateVertex(i,j+1,k,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i,j,k,&indices,&vertices);

                    generateVertex(i,j+1,k,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i,j+1,k+1,&indices,&vertices);

                    mesh.triangleCount+=4;
                }
            }
        }
    }

    // Allocate memory to the mesh
    mesh.indices = (unsigned short *)malloc(indices.size() * sizeof(unsigned short));
    mesh.vertices = (float *)malloc(vertices.size() * sizeof(float));
    mesh.normals = (float *)malloc(vertices.size() * sizeof(float));
    //! hay que liberar esto en algun momento

    calculateFlatNormals(&vertices,&indices,&normals);

    for(int i = 0; i < indices.size(); i++){
        mesh.indices[i] = indices[i];
        // ! dado que unsigned short solo llega a 65535, en modelos muy grandes, los indices que apunta > 65535, empezarán a apuntar al principio
        // ! se podria hacer un sistema de chunks 8x8x8 o 10x10x10 o 12x12x12 o 16x4x16
    }
        

    for(int i = 0; i < vertices.size(); i++)
        mesh.vertices[i] = vertices[i];

    for(int i = 0; i < normals.size(); i++)
        mesh.normals[i] = normals[i];

    mesh.vertexCount = vertices.size() / 3;

    UploadMesh(&mesh,true);

    return mesh;
}

std::vector<Model> modelVectorFromDungeon(DungeonMatrix * dgn,Vector3Int chunkSize){
    Vector3Int matrixSize;
    matrixSize.x = dgn->GetSize().x % chunkSize.x != 0 ? dgn->GetSize().x / chunkSize.x + 1 : dgn->GetSize().x / chunkSize.x;
    matrixSize.y = dgn->GetSize().y % chunkSize.y != 0 ? dgn->GetSize().y / chunkSize.y + 1 : dgn->GetSize().y / chunkSize.y;
    matrixSize.z = dgn->GetSize().z % chunkSize.z != 0 ? dgn->GetSize().z / chunkSize.z + 1 : dgn->GetSize().x / chunkSize.z;

    std::vector<Model> models;

    for(int i = 0; i < matrixSize.x; i++){
        for(int j = 0; j < matrixSize.y; j++){
            for(int k = 0; k < matrixSize.z; k++){
                models.push_back(LoadModelFromMesh(meshFromDungeonSector(dgn,chunkSize,{i*chunkSize.x,j*chunkSize.y,k*chunkSize.z})));
            }
        }
    }

    return models;

}

Mesh meshFromDungeonSector(DungeonMatrix * dgn, Vector3Int chunkSize,Vector3Int chunkOrigin)
{
    int vm_size_x = dgn->GetSize().x + 1;
    int vm_size_y = dgn->GetSize().y + 1;
    int vm_size_z = dgn->GetSize().z + 1;

    Mesh mesh = {0};

    unsigned int nIndices = 0;

    std::vector<int> indices;
    std::vector<float> vertices;
    std::vector<float> normals;

    Vector3Int chunkEnd = {chunkOrigin.x + chunkSize.x,chunkOrigin.y + chunkSize.y,chunkOrigin.z + chunkSize.z};

    for(unsigned int i = chunkOrigin.x ; i < dgn->GetSize().x && i < chunkEnd.x; i++){
        for(unsigned int j = chunkOrigin.y ; j < dgn->GetSize().y && j < chunkEnd.y; j++){
            for(unsigned int k = chunkOrigin.z ; k < dgn->GetSize().z && k < chunkEnd.z; k++){
                if(dgn->GetPos({i,j,k}) & DUN_PXY_WALL){
                    // Front face
                    generateVertex(i,j,k,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j+1,k,&indices,&vertices);

                    generateVertex(i+1,j+1,k,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j+1,k,&indices,&vertices);

                    //--------------------------
                    // Back face
                    generateVertex(i,j+1,k,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j,k,&indices,&vertices);

                    generateVertex(i,j+1,k,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i+1,j+1,k,&indices,&vertices);

                    mesh.triangleCount+=4;
                }
                if(dgn->GetPos({i,j,k}) & DUN_PXZ_WALL){
                    // Top face
                    generateVertex(i,j,k,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);

                    generateVertex(i+1,j,k+1,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);

                    //--------------------------
                    // Botton face
                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i,j,k,&indices,&vertices);

                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i+1,j,k,&indices,&vertices);
                    generateVertex(i+1,j,k+1,&indices,&vertices);

                    mesh.triangleCount+=4;
                }
                if(dgn->GetPos({i,j,k}) & DUN_PYZ_WALL){
                    // Front face
                    generateVertex(i,j,k,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i,j+1,k,&indices,&vertices);

                    generateVertex(i,j+1,k+1,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i,j+1,k,&indices,&vertices);
 
                    //--------------------------
                    // Back face
                    generateVertex(i,j+1,k,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i,j,k,&indices,&vertices);

                    generateVertex(i,j+1,k,&indices,&vertices);
                    generateVertex(i,j,k+1,&indices,&vertices);
                    generateVertex(i,j+1,k+1,&indices,&vertices);

                    mesh.triangleCount+=4;
                }
            }
        }
    }

    // Allocate memory to the mesh
    mesh.indices = (unsigned short *)malloc(indices.size() * sizeof(unsigned short));
    mesh.vertices = (float *)malloc(vertices.size() * sizeof(float));
    mesh.normals = (float *)malloc(vertices.size() * sizeof(float));
    //! hay que liberar esto en algun momento

    calculateFlatNormals(&vertices,&indices,&normals);

    for(int i = 0; i < indices.size(); i++){
        mesh.indices[i] = indices[i];
        // ! dado que unsigned short solo llega a 65535, en modelos muy grandes, los indices que apunta > 65535, empezarán a apuntar al principio
        // ! se podria hacer un sistema de chunks 8x8x8 o 10x10x10 o 12x12x12 o 16x4x16
    }
        

    for(int i = 0; i < vertices.size(); i++)
        mesh.vertices[i] = vertices[i];

    for(int i = 0; i < normals.size(); i++)
        mesh.normals[i] = normals[i];

    mesh.vertexCount = vertices.size() / 3;

    UploadMesh(&mesh,true);

    return mesh;
}