#pragma once
#include <Qstring>
#include <QObject>
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <QVector3D>
#include <cmath>

//#include "Source/global.h"

#include "Source/model/LayerBackground.h"
#include "Source/model/LayerCell.h"
#include "Source/model/LayerSubregion.h"

#include "Source/model/DataFeature.h"
#include "Source/Utils.h"


class DataProject : public QObject
{
	Q_OBJECT
public:
	bool ProjectStatus = false;
	QString ProjectName = "";
	QString ProjectPath = "";

	QString SubregionPath = "";
	QString FeaturePath = "";

	
	bool LayerBackgroundStatus = false;
	LayerBackground *mLayerBack;

	bool LayerCellStatus = false;
	LayerCell *mLayerCell[100];
	int curLayerCell = -1;
	int maxLayerCell = 0;
	QColor SelectedColor;
	QColor UnSelectedColor;

	QStringList UnitList;

	std::vector<DataFeature> mFeature;
	
	std::vector<LayerSubregion> mSubregion;
	//std::list<int> mActivatedSubregionIndex;


	double ResolutionXnm = 112.8;
	double ResolutionYnm = 112.8;
	double ResolutionZnm = 120;


	double ResolutionX = 112.8;
	double ResolutionY = 112.8;
	double ResolutionZ = 120;
	int ResolutionUnitX = 0;
	int ResolutionUnitY = 0;
	int ResolutionUnitZ = 0;

	int DataSizeX = 0;
	int DataSizeY = 0;
	int DataSizeZ = 0;
	int DataBlockSize = 0;
	int DataLevel = 0;


	float ViewPos_X = 0;
	float ViewPos_Y = 0;
	float ViewPos_Z = 123;
	float ViewZoomLevel = 2.0;

	int VisualizeMethod_Index = 0;
	float CellOpacity = 0.5;

	void ProjectMake(QString aProjName, QString aProjPath, QString aLayerBGPath, QString aLayerLBPath, QString aSubregionsPath, QString aFeaturesPath);
	void ProjectOpen(QString aProjPath);
	void ProjectSave(QString aProjPath);

	void NewProject();

	void AddFeature(QString aPath, int aType);
	void removeFeature(int index);
	int AddSubregion(QString aPath);
	void removeSubregion(int index);


	QVector3D planeNormal;
	QVector3D planePoint;

	DataProject();
	~DataProject();

	int getSerialIndex(int x, int y, int z, int lv);
	void ProjectSizeLoad();

	std::ifstream mIFS;

private:


signals:
	void project_on();
	void analytics_on();


};

