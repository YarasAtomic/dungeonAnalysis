#include "ezdungeon.h"
#include <fstream>
#include <iostream>
#include <vector>

void genSimple(DungeonMatrix * fileMatrix,std::string outputFile){
	std::ofstream file(outputFile,std::ofstream::trunc);
	file << "mapFile" << "\n";
	file << fileMatrix->size_x << "\n";
	file << fileMatrix->size_z << "\n";
	file << fileMatrix->start_x << "\n";
	file << fileMatrix->start_z << "\n";
	file << fileMatrix->end_x << "\n";
	file << fileMatrix->end_z << "\n";
	for(int i = 0; i < fileMatrix->size_x;i++)
	{
		for(int j = 0; j < fileMatrix->size_z;j++)
		{
			if((fileMatrix->data[i][0][j] & DUN_PXZ_WALL) == DUN_PXZ_WALL)
				file << 0 << "\t";
			else
				file << -1 << "\t";
		}
		file << "\n";
	}
	file.close();
}

void genBig(DungeonMatrix * fileMatrix,std::string outputFile){
	std::ofstream file(outputFile,std::ofstream::trunc);
	file << "mapFile" << "\n";
	int sizeX = fileMatrix->size_x*2;
	int sizeZ = fileMatrix->size_z*2;
	int endX = fileMatrix->end_x*2+1;
	int endZ = fileMatrix->end_z*2+1;
	int startX = fileMatrix->start_x*2+1;
	int startZ = fileMatrix->start_z*2+1;

	file << sizeX << "\n";
	file << sizeZ << "\n";
	file << endX << "\n";
	file << endZ << "\n";
	file << startX << "\n";
	file << startZ << "\n";

	std::vector<std::vector<int>> newMatrix;
	newMatrix.resize(sizeX);
	for(int i = 0; i < sizeX;i++)
		newMatrix[i] = std::vector<int>(sizeZ, 0);

	for(int i = 0; i < fileMatrix->size_x;i++)
	{
		for(int j = 0; j < fileMatrix->size_z;j++)
		{
			bool xy = (fileMatrix->data[i][0][j] & DUN_PXY_WALL) == DUN_PXY_WALL;
			bool yz = (fileMatrix->data[i][0][j] & DUN_PYZ_WALL) == DUN_PYZ_WALL;
			if(xy){
				newMatrix[i*2+1][j*2] = -1;
			}
			if(yz){
				newMatrix[i*2][j*2+1] = -1;
			}
			newMatrix[i*2][j*2] = -1; // aqui haria falta un if tambien
		}
	}
	for(int i = 0; i < sizeX; i++)
	{
		for(int j = 0; j < sizeZ; j++)
		{
			file << newMatrix[i][j] << "\t";
		}
		file << "\n";
	}
	file.close();
}

int main(int args, char ** argv)
{
	if(args>=2){
		std::string inputFile = argv[1];
		std::string outputFile = "map.txt";
		if(args>=3) outputFile = argv[2];
		DungeonMatrix * fileMatrix;
		file2Dun(&fileMatrix,"../test.dun");
		// genSimple(fileMatrix,outputFile);
		genBig(fileMatrix,outputFile);
		return 0;
	}
	std::cout << "format: dbaMaker inputFile (outputFile)" << std::endl;
	return -1;
}