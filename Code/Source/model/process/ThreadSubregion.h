#pragma once

#include <QThread>
#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include <cmath>
#include <omp.h>
#include <QFileInfo>
#include <queue>

#include "Source/global.h"
#include "Source/controller/node/BlockWidget.h"

class BlockWidget;


class CounterTreeNode {
public:
	unsigned char type; //0: not included, 1: partially included, 2: included
	CounterTreeNode *child=NULL; // if type is 1, 8 children will be generated
};

class ThreadSubregion : public QThread
{
	Q_OBJECT

public:
	ThreadSubregion();
	ThreadSubregion(QObject *parent);
	~ThreadSubregion();
	void addJob(std::string id, BlockWidget *parent);

	bool isCounterTreeInit = false;
	std::vector<QVector3D> counterTreeRootBlockList;
	CounterTreeNode *counterTree=NULL;
	int counterTreeGenerate(); //return 0: set, 1: masking, 2: cut plane, 3: masking + cut plane
	void counterTreeCutPlaneOperation();
	void counterTreeMaskingOperation();

	void counterTreeSetOperation();
	void counterTreeNodeDeepCopy(CounterTreeNode *_from, CounterTreeNode *_to);
	long long voxelCount();
	void cellFiltering();
	int getBlockType(int level, int index, unsigned char *blockData,int blockBaseSize); //0: not included, 1: partially included, 2: included
	int getBaseLocation(int level, int p, int blockBaseSize);
	void counterTreeSubGenerate(int level, int index, unsigned char *blockData,CounterTreeNode *curNode,int blockBaseSize);
	void counterTreeCutting(int level, int index, CounterTreeNode *curNode, int blockBaseSize, int rootIndex);
	void counterTreeArrange(CounterTreeNode *curNode);
	void counterTreeNodeDeepDelete(CounterTreeNode *nodes);

	long long voxelCountSub(int level, int blockBaseSize, CounterTreeNode *curNode);
	int cellIncludeCheck(cell curCell);
	int pointIncludeCheck(CounterTreeNode *cur, int level, int index, int blockBaseSize,int x, int y, int z);
	void generateCellBoundary();
	void cellVolumeCount(CounterTreeNode *cur, int level, int index, int blockBaseSize,unsigned int *cellData, long long *res);
	void counterTreeToBinaryData(CounterTreeNode *cur, int level, int index, int blockBaseSize, unsigned char *res);
private:
	void run();
	void save(long long volume, int level);

	bool newJobFlag = false;
	bool waitFlag = true;
	int SubregionIndex = 0;
	std::string SubregionName;
	std::string SubregionPath;
	std::string SubregionID;

	float pos_x;
	float pos_y;
	float pos_z;
	float up_x;
	float up_y;
	float up_z;

	float precision_weight = 1;

	std::map<unsigned int, bool> include_map;
	std::map<unsigned int, bool> intersect_map;

	BlockWidget *mParent;

public slots:
	void kill_thread();

signals:
	void progress_update(float);
	void progress_log_update(QString);
	void subregion_information_update(QString);
	


};
