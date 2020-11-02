#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <list>

#include <stdio.h>
#include <stdlib.h>

#include <QString>
#include <QColor>
#include <QDebug>


#include "Source/Utils.h"

class DataFeature
{
public:
	enum {
		TypeConstant = 0,
		TypeLinear = 1,
		TypeSquare = 2,
		TypeCubic = 3
	};
	DataFeature();
	DataFeature(std::string name, std::string path, int type);
	~DataFeature();

	void setFeatureValue(unsigned int index, double value);
	std::string FeaturePath;
	std::string FeatureName;
	int FeatureType;
	std::map<unsigned int, double> FeatureMap;
		
};

