#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include <QString>
#include <QDebug>
#include <list>


#include "Source/Utils.h"
class LayerCell
{
public:
	LayerCell();
	LayerCell(std::string cell_path);

	~LayerCell();
	
	std::string CellPath;
	std::string CellHeaderPath;
	std::string CellInfoPath;

	std::list<label_layer> BlockList;
	std::vector<cell> mCellList;
	int* mCellIndex;

	long long MaxCellIndex;
	long long MaxCellCount;
	bool CellColorGPU_On = false;
	float Opacity = 0.5f;


	label_layer initializeBlock(block_info info);
	int checkBlockIndex(int x, int y, int z, int level, int axis);
	int LoadBlockBySerialIndex(label_layer tempblock);
	void removeBlock();

	std::string DataType;
	std::string DataPathXY;
	std::string DataPathYZ;
	std::string DataPathZX;

	int featureStartIndex = 0;
	int featureEndIndex = 0;

private:

	int DataSizeX = 0;
	int DataSizeY = 0;
	int DataSizeZ = 0;
	int DataBlockSize = 0;
	int DataLevel = 0;
	

	

	void headerReader();
	void loadCellInformation();
};

