#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <list>

#include <stdio.h>
#include <stdlib.h>

#include <QString>
#include <QColor>
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QObject>
#include <QDialog>
#include <QtWidgets>


#include "Source/Utils.h"

class DataCellAnalytics
{
public:
	DataCellAnalytics();
	~DataCellAnalytics();
	void Init(std::string path);

	void CellFeatureCalculation(QString emDirPath, QString cellDirPath, QString savePath, int MaxCellN);
	float getEccentricity(std::vector<coord> points);
	void EssentricityCaculation(QString cellDirPath, QString savePath, int StartCellN, int EndCellN, int MaxCellN);

	void CellVolumeSurfaceSphericityCalculation(QString cellDirPath, QString savePath,int MaxCellN);
	QVector2D getVolumeSurface(std::vector<QVector3D> points); //(volume, surface)
	double getSurfaceUsingMarchingCube(bool ck[2][2][2]);


	void featurePrint(QString inputCoordPath, QString OutputPath);

	std::vector<cell> mCellList;
	long long MaxCellIndex;
	long long MaxCellCount;
	std::string AnalyticsResultPath;
	bool CellColorGPU_On = false;


private:

	std::ifstream mifs;
	void LoadCellInformation();
	void LoadFeature();
	
};

