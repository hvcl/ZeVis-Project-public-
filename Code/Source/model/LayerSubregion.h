#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <list>

#include <stdio.h>
#include <stdlib.h>
#include <QString>
#include <QColor>
#include <QDebug>
#include <QTime>
#include <QCryptographicHash>

#include "Source/Utils.h"

class QCheckBox;

class LayerSubregion
{
public:
	LayerSubregion();
	LayerSubregion(std::string name, std::string path);
	~LayerSubregion();

	int SubregionIndex;
	std::string SubregionID;


	std::string SubregionName;
	std::string SubregionPath;
	std::string SubregionHeaderPath;
	std::string TouchTablePath;
	std::string IntersectTablePath;

	QColor SubregionColor = QColor(23,55,88);
	float SubregionOpacity = 0.0f;

	QPushButton *SubregionColorBtn;

	
	std::list<subregion_layer> BlockList;

	subregion_layer initializeBlock(block_info info);
	
	
	int checkBlockIndex(int idx_x, int idx_y, int idx_z, int level, int axis);
	int LoadBlockBySerialIndex(subregion_layer tempblock);
	void removeBlock();



	int DataSizeX = 0;
	int DataSizeY = 0;
	int DataSizeZ = 0;
	int DataBlockSize = 0;
	int DataLevel = 0;

	std::string DataType;
	std::string DataPathXY;
	std::string DataPathYZ;
	std::string DataPathZX;


private:


	void readIndex();
	void headerReader();
};

