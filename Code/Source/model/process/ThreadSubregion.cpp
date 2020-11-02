#include "ThreadSubregion.h"

void printArray(const float* a, const unsigned int n) {
	QString s = "(";
	unsigned int ii;
	for (ii = 0; ii < n - 1; ++ii)
		s.append(QString::number(a[ii])).append(", ");
	s.append(QString::number(a[ii])).append(")");

	qDebug() << s;
}


ThreadSubregion::ThreadSubregion() {
	//generateCellBoundary();

}
ThreadSubregion::ThreadSubregion(QObject *parent)
	: QThread(parent)
{
	connect(parent, SIGNAL(thread_kill()), this, SLOT(kill_thread()));
}

ThreadSubregion::~ThreadSubregion()
{
}

void ThreadSubregion::kill_thread() {
	this->terminate();
	qDebug() << "in thread kill_thread";
}

void ThreadSubregion::addJob(std::string id, BlockWidget *parent) {
	SubregionID = id;
	mParent = parent;
	if (mGlobals.CurrentProject->DataLevel == 5) {
		precision_weight = 2;
	}

	if (id != "-1") {
		std::vector<LayerSubregion>::iterator iter;
		for (iter = mGlobals.CurrentProject->mSubregion.begin(); iter != mGlobals.CurrentProject->mSubregion.end(); ++iter) {
			if (SubregionID.compare(iter->SubregionID) == 0) {
				SubregionName = iter->SubregionName;
				if (mGlobals.CurrentProject->DataLevel == 5)SubregionPath = iter->SubregionPath + "for_masking/XY/";
				else SubregionPath = iter->SubregionPath + "XY/";
				break;
			}
		}
	}
	else {
		pos_x = (float)parent->plane_pos_x / precision_weight;
		pos_y = (float)parent->plane_pos_y / precision_weight;
		pos_z = (float)parent->plane_pos_z / precision_weight;

		up_x = (float)parent->plane_up_x;
		up_y = (float)parent->plane_up_y;
		up_z = (float)parent->plane_up_z;

	}


	include_map.clear();
	intersect_map.clear();
	newJobFlag = true;
	waitFlag = false;
}


void ThreadSubregion::save(long long volume, int level) {
	mParent->SubregionVolume = volume;

	//mParent->TouchIndex.clear();
	//mParent->IntersectIndex.clear();
	//mParent->CompleteIndex.clear();

	////mParent->SubregionPrecision = mGlobals.CurrentProject->DataLevel - level + 1;
	//std::map<unsigned int, bool>::iterator mapmParent = include_map.begin();
	//for (mapmParent = include_map.begin(); mapmParent != include_map.end(); ++mapmParent) {
	//	mParent->TouchIndex[mapmParent->first] = true;
	//	mParent->CompleteIndex[mapmParent->first] = true;
	//}
	//for (mapmParent = intersect_map.begin(); mapmParent != intersect_map.end(); ++mapmParent) {
	//	mParent->IntersectIndex[mapmParent->first] = true;
	//	mParent->CompleteIndex.erase(mapmParent->first);
	//}
}


void ThreadSubregion::run()
{
	emit progress_log_update("Ready");

	while (true) {
	
		while (waitFlag) {
			sleep(1);
		}
				
		if (newJobFlag) {
			newJobFlag = false;
			continue;
		}

		std::string str = "Calculating " + SubregionID + " ";
		//mGlobals.mainWindow->statusBar()->showMessage(QString::fromStdString(str));
		qDebug() << QString::fromStdString(str);

		qDebug() << "initialization";
		emit progress_log_update("Initialization... ");

		int operationType = 0; //0: set 1: masking 2: cut 3:masking+cut
		operationType = counterTreeGenerate();

		if (operationType == -1) {
			emit progress_log_update("Ready");

			waitFlag = true;
			continue;
		}


		qDebug() << "Operation";


		if (operationType == 1 || operationType == 3) {
			emit progress_log_update("Subregion filtering... ");

			if (isCounterTreeInit) {
				emit progress_update(0);
				progress_log_update("Initial subregion already was chosen");
				waitFlag = true;
				continue;
			}
			counterTreeMaskingOperation();
		}

		if (operationType == 2 || operationType == 3) {
			emit progress_log_update("Cut plane operation... ");

			qDebug() << "cut plane";
			if (!isCounterTreeInit) {
				emit progress_update(0);
				progress_log_update("Subregion filtering should be done firstly");
				waitFlag = true;
				continue;
			}
			counterTreeCutPlaneOperation();
		}

		qDebug() << "voxel counting";
		emit progress_log_update("Subregion volume calculation... ");

		double volume = voxelCount();
		//double volume = getVolume(0);// *multi_value;
		qDebug() << volume;

		emit progress_log_update("Cell filtering... ");

		cellFiltering();

		if (newJobFlag) {
			newJobFlag = false;
			continue;
		}
		else {
			save(volume, 0);
			emit subregion_information_update(QString::fromStdString(SubregionID));
		}
		emit progress_log_update("Finish");
		mParent->IsSubregionGenerated = true;
		waitFlag = true;
	}
}


int ThreadSubregion::counterTreeGenerate() {

	emit progress_update(0);

	if (mParent->m_block->mBlockFlags == QNEBlock::SetBlock) {
		//ToDo

		emit progress_update(100);

		return 0;
	}
	else if(mParent->m_block->mBlockFlags == QNEBlock::SubregionBlock){
		if (isCounterTreeInit) {
			isCounterTreeInit = false;

			for (int i = 0; i < counterTreeRootBlockList.size(); i++) {
				counterTreeNodeDeepDelete(&counterTree[i]);
			}
			delete[]counterTree;
			counterTree = NULL;

			counterTreeRootBlockList.clear();

		}
		int initType = 0; //0: not defined, 1: previous definded
		QNEBlock *cur = mParent->m_block;
		while (true) {
			qDebug() << cur->mBlockFlags;
			QNEConnection *conn=NULL;

			foreach(QNEPort *port, cur->ports()) {
				if (port->portAlign() == QNEPort::Input) {
					if (port->connections().size() == 0)break;
					conn = port->connections()[0];
					break;
				}
			}
			if (conn == NULL)return -1;
			QNEBlock *prev;
			if (conn->port1()->isOutput()) {
				prev = conn->port1()->m_block;
			}
			else prev= conn->port2()->m_block;

			if (prev->mBlockFlags == QNEBlock::SubregionBlock || prev->mBlockFlags == QNEBlock::CutPlaneBlock) {
				if (prev->mBlock->mThread->isCounterTreeInit) {
					initType = 1;
					cur = prev;
					break;
				}
			}
			if (prev->mBlockFlags == QNEBlock::OriginBlock) {
				break;
			}
			cur = prev;
		}


		if (initType == 0) { //not definded
			

		}
		else { //previously definded
			counterTree = new CounterTreeNode[cur->mBlock->mThread->counterTreeRootBlockList.size()];

			for (int i = 0; i < cur->mBlock->mThread->counterTreeRootBlockList.size(); i++) {
				counterTreeRootBlockList.push_back(cur->mBlock->mThread->counterTreeRootBlockList[i]);
				counterTreeNodeDeepCopy(&cur->mBlock->mThread->counterTree[i], &counterTree[i]);
			}
			isCounterTreeInit = true;
		}
		emit progress_update(100);
		return 1;
	}
	else if (mParent->m_block->mBlockFlags == QNEBlock::CutPlaneBlock) {
		if (isCounterTreeInit) {
			isCounterTreeInit = false;

			for (int i = 0; i < counterTreeRootBlockList.size(); i++) {
				counterTreeNodeDeepDelete(&counterTree[i]);
			}
			delete[]counterTree;
			counterTree = NULL;

			counterTreeRootBlockList.clear();

		}
		int initType = 0; //0: not defined, 1: previous definded
		QNEBlock *cur = mParent->m_block;
		while (true) {
			qDebug() << cur->mBlockFlags;
			QNEConnection *conn=NULL;

			foreach(QNEPort *port, cur->ports()) {
				if (port->portAlign() == QNEPort::Input) {
					if (port->connections().size() == 0)break;
					conn = port->connections()[0];
					break;
				}
			}
			if (conn == NULL)return -1;

			QNEBlock *prev;
			if (conn->port1()->isOutput()) {
				prev = conn->port1()->m_block;
			}
			else prev = conn->port2()->m_block;

			if (prev->mBlockFlags == QNEBlock::SubregionBlock || QNEBlock::CutPlaneBlock) {
				if (prev->mBlock->mThread->isCounterTreeInit) {
					initType = 1;
					cur = prev;
					break;
				}
			}
			if (prev->mBlockFlags == QNEBlock::OriginBlock) {
				break;
			}
			cur = prev;
		}


		if (initType == 0) { //not definded


		}
		else { //previously definded
			counterTree = new CounterTreeNode[cur->mBlock->mThread->counterTreeRootBlockList.size()];

			for (int i = 0; i < cur->mBlock->mThread->counterTreeRootBlockList.size(); i++) {
				counterTreeRootBlockList.push_back(cur->mBlock->mThread->counterTreeRootBlockList[i]);
				counterTreeNodeDeepCopy(&cur->mBlock->mThread->counterTree[i], &counterTree[i]);
			}
			isCounterTreeInit = true;
		}
		emit progress_update(100);
		return 2;
	}
}

void ThreadSubregion::counterTreeCutPlaneOperation() {

	emit progress_update(0);
	int blockN = counterTreeRootBlockList.size();
	for (int i = 0; i < blockN; i++) {
		emit progress_update(float(i) / blockN * 100);
		counterTreeCutting(0, 0, &counterTree[i], mGlobals.CurrentProject->DataBlockSize,i);
		counterTreeArrange(&counterTree[i]);
	}
}
void ThreadSubregion::counterTreeArrange(CounterTreeNode *curNode) {
	if (curNode->type == 1) {
		int sum = 0;
		for (int i = 0; i < 8; i++) {
			counterTreeArrange(&curNode->child[i]);
			sum += curNode->child[i].type;
		}
		if (sum == 0) {
			curNode->type = 0;
			delete[]curNode->child;
			curNode->child = NULL;
		}
		else if (sum == 16) {
			curNode->type = 2;
			delete[]curNode->child;
			curNode->child = NULL;
		}
	}
}
void ThreadSubregion::counterTreeNodeDeepDelete(CounterTreeNode *node) {

	for (int i = 0; i < 8; i++) {
		if (node->type == 1) {
			counterTreeNodeDeepDelete(&node->child[i]);
		}
	}
	delete[]node->child;
	node->child = NULL;
}
void ThreadSubregion::counterTreeCutting(int level,int index,CounterTreeNode * curNode, int blockBaseSize,int rootIndex) {
	//qDebug() << curNode->type;
	//qDebug() << level;
	//qDebug() << index;
	if (curNode->type == 2) {
		int baseLocation = getBaseLocation(level, index, blockBaseSize);
		//qDebug() <<baseLocation;
		int baseX = counterTreeRootBlockList[rootIndex].x() * blockBaseSize + baseLocation % blockBaseSize;
		int baseY = counterTreeRootBlockList[rootIndex].y() * blockBaseSize + baseLocation / blockBaseSize % blockBaseSize;
		int baseZ = counterTreeRootBlockList[rootIndex].z() * blockBaseSize + baseLocation / (blockBaseSize * blockBaseSize);
		int blockSize = blockBaseSize / pow(2, level);
		//qDebug() << blockSize;


		int type = -1;//0: not included, 1: partially included, 2: included
		//test 8 vertex of block
		
		int sum = 0;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					int x, y, z;
					x = baseX+(blockSize-1)*i;
					y = baseY+(blockSize-1)*j;
					z = baseZ+(blockSize-1)*k;
					//qDebug() << up_x * (x - pos_x) + up_y * (y - pos_y) + up_z * (z - pos_z);
					if (up_x * (x - pos_x) + up_y * (y - pos_y) + up_z * (z - pos_z) >= 0) {
						sum++;
					}
				}
			}
		}
		if (sum == 0)type = 0;
		else if (sum == 8)type = 2;
		else type = 1;
		curNode->type = type;
		//qDebug() << type;
		//qDebug() << "00000000000000000000000";
		if (type == 1) {
			//qDebug() << blockSize;
			int s = blockSize * blockSize*blockSize;
			unsigned char *blockData = new unsigned char[s];
			int x, y, z;
			int blockSize2= blockSize * blockSize;
			for (int i = 0; i < s; i++) {
				z = baseZ + i / blockSize2;
				y = baseY + i / blockSize % blockSize;
				x = baseX + i % blockSize;
				blockData[i] = 0;
				if (up_x * (x - pos_x) + up_y * (y - pos_y) + up_z * (z - pos_z) >= 0) {
					blockData[i] = 1;
				}
			}
			counterTreeSubGenerate(0, 0, blockData, curNode, blockSize);

			delete[]blockData;
		}

	}
	else if (curNode->type == 1) {
		for (int i = 0; i < 8; i++) {
			counterTreeCutting(level + 1, index * 8 + i, &curNode->child[i], blockBaseSize,rootIndex);
		}
	}
}

void ThreadSubregion::counterTreeMaskingOperation() {
	Utils mUtil;
	QDir directory(QString::fromStdString(SubregionPath));
	QStringList target;
	target << "*_0.block";
	QStringList blocks = directory.entryList(target, QDir::Files);
	long long size = mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize;
	unsigned char *subregion_block = new unsigned char[size];

	int loopsize = blocks.size();
	float progress_value = 0;
	//qDebug() << loopsize;
	counterTree = new CounterTreeNode[loopsize];

	for (int i = 0; i < loopsize; i++) {
		QString filename = blocks[i];
			
		emit progress_update(float(i)/loopsize*100);
		std::string path = SubregionPath + filename.toStdString();
		vector<std::string> str_vec = mUtil.Split(filename.toStdString().c_str(), "_");

		counterTreeRootBlockList.push_back(QVector3D(stoi(str_vec[2]), stoi(str_vec[1]), stoi(str_vec[0])));//x,y,z

		FILE *subregion_blck_rfp = fopen(path.c_str(), "rb");
		fread(subregion_block, size, 1, subregion_blck_rfp);

		//qDebug() << counterTreeRootBlockList[counterTreeRootBlockList.size()-1];

		counterTreeSubGenerate(0, 0, subregion_block, &counterTree[i], mGlobals.CurrentProject->DataBlockSize);


		fclose(subregion_blck_rfp);
	}
	delete[]subregion_block;
	isCounterTreeInit = true;
}
void ThreadSubregion::generateCellBoundary() {

	qDebug() << "start cell boundary calculation";

	Utils mUtil;
	QDir directory(QString::fromStdString("F:/Brain/XY/"));
	QStringList target;
	target << "*_" + QString::fromStdString(to_string(0)) + ".block";
	QStringList blocks = directory.entryList(target, QDir::Files);
	long long size = mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize;
	unsigned int *cell_block = new unsigned int[size];


	cell *cells = new cell[mGlobals.CurrentProject->mLayerCell[mParent->m_block->labelIndex]->MaxCellCount];
	long long *volumes = new long long[mGlobals.CurrentProject->mLayerCell[mParent->m_block->labelIndex]->MaxCellCount+1]();
	for (int i = 0; i < mGlobals.CurrentProject->mLayerCell[mParent->m_block->labelIndex]->MaxCellCount; ++i) {
		cells[i].index = mGlobals.CurrentProject->mLayerCell[mParent->m_block->labelIndex]->mCellList[i].index;
		cells[i].maxbox.x = 0;
		cells[i].maxbox.y = 0;
		cells[i].maxbox.z = 0;

		cells[i].minbox.x = 1000000;
		cells[i].minbox.y = 1000000;
		cells[i].minbox.z = 1000000;
	}
	int cnt = 0;
	int loopsize = blocks.size();
	foreach(QString filename, blocks) {
		qDebug() << (float)cnt++ / loopsize;

		std::string cell_path = mGlobals.CurrentProject->mLayerCell[mParent->m_block->labelIndex]->DataPathXY + filename.toStdString();
		vector<std::string> str_vec = mUtil.Split(filename.toStdString().c_str(), "_");

		int baseX=(float)stoi(str_vec[2]) * (float)mGlobals.CurrentProject->DataBlockSize;
		int baseY= (float)stoi(str_vec[1]) * (float)mGlobals.CurrentProject->DataBlockSize;
		int baseZ= (float)stoi(str_vec[0]) * (float)mGlobals.CurrentProject->DataBlockSize;

		if (QFileInfo::exists(QString::fromStdString(cell_path)) && QFileInfo(QString::fromStdString(cell_path)).isFile()) {
			FILE *cell_blck_rfp = fopen(cell_path.c_str(), "rb");
			fread(cell_block, size * sizeof(unsigned int), 1, cell_blck_rfp);
			for (int z = 0; z < mGlobals.CurrentProject->DataBlockSize; z++) {
				for (int y = 0; y < mGlobals.CurrentProject->DataBlockSize; y++) {
					for (int x = 0; x < mGlobals.CurrentProject->DataBlockSize; x++) {
						int cellN = cell_block[z*mGlobals.CurrentProject->DataBlockSize*mGlobals.CurrentProject->DataBlockSize +
							y * mGlobals.CurrentProject->DataBlockSize + x];
						if (cellN != 0) {
							volumes[cellN]++;
							/*int curX = baseX + x;
							int curY = baseY + y;
							int curZ = baseZ + z;
							cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].maxbox.x =
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].maxbox.x > curX ?
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].maxbox.x : curX;
							cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].maxbox.y =
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].maxbox.y > curY ?
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].maxbox.y : curY;
							cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].maxbox.z =
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].maxbox.z > curZ ?
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].maxbox.z : curZ;

							cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].minbox.x =
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].minbox.x < curX ?
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].minbox.x : curX;
							cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].minbox.y =
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].minbox.y < curY ?
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].minbox.y : curY;
							cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].minbox.z =
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].minbox.z < curZ ?
								cells[mGlobals.CurrentProject->mLayerCell->mCellIndex[cellN]].minbox.z : curZ;*/
						}
					}
				}
			}

			fclose(cell_blck_rfp);
		}
	}

	delete[] cell_block;


	//FILE *f = fopen("cellBoundary.txt", "w");

	//for (int i = 0; i < mGlobals.CurrentProject->mLayerCell->MaxCellCount; ++i) {
	//	fprintf(f, "%d %d %d %d %d %d %d\n", cells[i].index, cells[i].minbox.x,  cells[i].minbox.y, cells[i].minbox.z, 
	//		cells[i].maxbox.x,  cells[i].maxbox.y, cells[i].maxbox.z);
	//}
	//fclose(f);
	FILE *f = fopen("volume.txt", "w");

	for (int i = 1; i <= mGlobals.CurrentProject->mLayerCell[mParent->m_block->labelIndex]->MaxCellCount; ++i) {
		fprintf(f, "%d %d\n", i,volumes[i]);
	}
	fclose(f);
	delete[]cells;
	delete[]volumes;

}
void ThreadSubregion::counterTreeSubGenerate(int level, int index, unsigned char *blockData, CounterTreeNode *curNode, int blockBaseSize) {
	curNode->type = getBlockType(level, index, blockData, blockBaseSize);
	if (curNode->type == 0 || curNode->type == 2)return;

	curNode->child = new CounterTreeNode[8];
	for (int i = 0; i < 8; i++) {
		counterTreeSubGenerate(level + 1, index * 8 + i, blockData, &curNode->child[i], blockBaseSize);
	}
}

void ThreadSubregion::counterTreeSetOperation() {

}
void ThreadSubregion::counterTreeNodeDeepCopy(CounterTreeNode *_from, CounterTreeNode *_to){
	_to->type = _from->type;
	if (_to->type == 0 || _to->type == 2)return;

	_to->child = new CounterTreeNode[8];
	for (int i = 0; i < 8; i++) {
		counterTreeNodeDeepCopy(&_from->child[i],&_to->child[i]);
	}
}

long long ThreadSubregion::voxelCount() {
	emit progress_update(0);

	int nodeN = counterTreeRootBlockList.size();
	//qDebug() << nodeN;
	long long result = 0;
	for (int i = 0; i < nodeN; i++) {
		emit progress_update(float(i)/nodeN*100);

		result += voxelCountSub(0, mGlobals.CurrentProject->DataBlockSize, &counterTree[i]) * precision_weight * precision_weight *precision_weight;
		//qDebug() << result;
	}
	emit progress_update(100);

	return result;
}

long long ThreadSubregion::voxelCountSub(int level, int blockBaseSize, CounterTreeNode *curNode) {
	//qDebug() << "---------------------";
	//qDebug() << curNode->type;
	//qDebug() << blockBaseSize;
	//qDebug() << blockBaseSize / pow(2, level);

	if (curNode->type == 2) {
		long long n = blockBaseSize / pow(2, level);
		//qDebug() << n;
		return n*n*n;
	}
	else if (curNode->type == 1) {
		long long n = 0;
		for (int i = 0; i < 8; i++) {
			n += voxelCountSub(level + 1, blockBaseSize, &curNode->child[i]);
		}
		//qDebug() << n;

		return n;
	}
	else {
		return 0;
	}

}
//void ThreadSubregion::cellFiltering() {
//	emit progress_update(0);
//
//	mParent->TouchIndex.clear();
//	mParent->IntersectIndex.clear();
//	mParent->CompleteIndex.clear();
//
//	long long size = mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize;
//	unsigned int *cell_block = new unsigned int[size]();
//	unsigned char *subregion_block = new unsigned char[size]();
//
//	for (int i = 0; i < counterTreeRootBlockList.size(); i++) {
//		emit progress_update(float(i) / counterTreeRootBlockList.size() * 100);
//		std::string cell_path = mGlobals.CurrentProject->mLayerCell->DataPathXY + 
//			to_string(int(counterTreeRootBlockList[i].z()))+"_"+ 
//			to_string(int(counterTreeRootBlockList[i].y()))+"_"+
//			to_string(int(counterTreeRootBlockList[i].x()))+"_0.block";
//
//		int baseX = counterTreeRootBlockList[i].x() * mGlobals.CurrentProject->DataBlockSize;
//		int baseY = counterTreeRootBlockList[i].y() * mGlobals.CurrentProject->DataBlockSize;
//		int baseZ = counterTreeRootBlockList[i].z() * mGlobals.CurrentProject->DataBlockSize;
//
//		if (QFileInfo::exists(QString::fromStdString(cell_path)) && QFileInfo(QString::fromStdString(cell_path)).isFile()) {
//			FILE *cell_blck_rfp = fopen(cell_path.c_str(), "rb");
//			fread(cell_block, size * sizeof(unsigned int), 1, cell_blck_rfp);
//			fclose(cell_blck_rfp);
//			memset(subregion_block, 0, size);
//			counterTreeToBinaryData(&counterTree[i], 0, 0, mGlobals.CurrentProject->DataBlockSize, subregion_block);
//
//			int *table = cuda_parallel_cell_type_decision(subregion_block, cell_block, mGlobals.CurrentProject->DataBlockSize, mGlobals.CurrentProject->mLayerCell->MaxCellCount+1);
//
//
//			std::list<unsigned int>::iterator iter = mParent->CellIndexListInput.begin();
//			for (; iter != mParent->CellIndexListInput.end(); ++iter) {
//				int curCell = *iter;
//				int type = table[curCell];
//				if (type == 1) {
//					mParent->TouchIndex[curCell] = true;
//					mParent->IntersectIndex[curCell] = true;
//				}
//				else if (type == 2) {
//					mParent->TouchIndex[curCell] = true;
//					mParent->CompleteIndex[curCell] = true;
//				}
//			}
//		}
//	}
//	delete[] cell_block;
//	delete[]subregion_block;
//
//	//Utils mUtil;
//	//QDir directory(QString::fromStdString(SubregionPath));
//	//QStringList target;
//	//target << "*_" + QString::fromStdString(to_string(0)) + ".block";
//	//QStringList blocks = directory.entryList(target, QDir::Files);
//	//long long size = mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize;
//	//unsigned int *cell_block = new unsigned int[size];
//
//
//	//int loopsize = blocks.size();
//	//int i = 0;
//	//foreach(QString filename, blocks) {
//	//	emit progress_update(float(i)/loopsize*100);
//
//	//	std::string cell_path = mGlobals.CurrentProject->mLayerCell->DataPathXY + filename.toStdString();
//	//	vector<std::string> str_vec = mUtil.Split(filename.toStdString().c_str(), "_");
//
//	//	int baseX = (float)stoi(str_vec[2]) * (float)mGlobals.CurrentProject->DataBlockSize;
//	//	int baseY = (float)stoi(str_vec[1]) * (float)mGlobals.CurrentProject->DataBlockSize;
//	//	int baseZ = (float)stoi(str_vec[0]) * (float)mGlobals.CurrentProject->DataBlockSize;
//
//	//	if (QFileInfo::exists(QString::fromStdString(cell_path)) && QFileInfo(QString::fromStdString(cell_path)).isFile()) {
//	//		FILE *cell_blck_rfp = fopen(cell_path.c_str(), "rb");
//	//		fread(cell_block, size * sizeof(unsigned int), 1, cell_blck_rfp);
//	//		cellVolumeCount(&counterTree[i], 0, 0, mGlobals.CurrentProject->DataBlockSize, cell_block, cellVolume);
//	//		fclose(cell_blck_rfp);
//	//	}
//	//	i++;
//	//}
//
//	//delete[] cell_block;
//
//
//
//	emit progress_update(100);
//
//}

void ThreadSubregion::cellFiltering() {
	emit progress_update(0);

	mParent->TouchIndex.clear();
	mParent->IntersectIndex.clear();
	mParent->CompleteIndex.clear();

	int cellN = mParent->CellIndexListInput.size();
	int i = 0;

	std::list<unsigned int>::iterator iter = mParent->CellIndexListInput.begin();
	for (; iter != mParent->CellIndexListInput.end(); ++iter) {
		emit progress_update(float(i)/cellN*100);

		cell curCell = mGlobals.CurrentProject->mLayerCell[mParent->m_block->labelIndex]->mCellList[mGlobals.CurrentProject->mLayerCell[mParent->m_block->labelIndex]->mCellIndex[*iter]];

		int type = cellIncludeCheck(curCell);

		if (type == 1) {
			mParent->TouchIndex[curCell.index] = true;
			mParent->IntersectIndex[curCell.index] = true;
		}
		else if (type == 2) {
			mParent->TouchIndex[curCell.index] = true;
			mParent->CompleteIndex[curCell.index] = true;
		}
		i++;
	}

	emit progress_update(100);

}


void ThreadSubregion::counterTreeToBinaryData(CounterTreeNode *cur, int level, int index, int blockBaseSize, unsigned char *res) {
	if (cur->type == 0)return;
	if (cur->type == 2) {
		int loc = getBaseLocation(level, index, blockBaseSize);
		int baseX = loc % blockBaseSize;
		int baseY = loc / blockBaseSize % blockBaseSize;
		int baseZ = loc / (blockBaseSize * blockBaseSize);
		int blockSize = blockBaseSize / pow(2, level);
		for (int z = 0; z < blockSize; z++) {
			for (int y = 0; y < blockSize; y++) {
				for (int x = 0; x < blockSize; x++) {
					res[(baseZ + z)*blockBaseSize*blockBaseSize + (baseY + y)*blockBaseSize + baseX + x]=1;
				}
			}
		}
	}
	else {
		for (int i = 0; i < 8; i++) {
			counterTreeToBinaryData(&cur->child[i], level + 1, index * 8 + i, blockBaseSize, res);
		}
	}

}

void ThreadSubregion::cellVolumeCount(CounterTreeNode *cur, int level, int index, int blockBaseSize, unsigned int *cellData,long long *res) {
	if (cur->type == 0)return;
	if (cur->type == 2) {
		int loc = getBaseLocation(level, index, blockBaseSize);
		int baseX = loc % blockBaseSize;
		int baseY = loc / blockBaseSize % blockBaseSize;
		int baseZ = loc / (blockBaseSize * blockBaseSize);
		int blockSize = blockBaseSize / pow(2, level);
		for (int z = 0; z < blockSize; z++) {
			for (int y = 0; y < blockSize; y++) {
				for (int x = 0; x < blockSize; x++) {
					res[cellData[(baseZ + z)*blockBaseSize*blockBaseSize + (baseY + y)*blockBaseSize + baseX + x]]++;
				}
			}
		}
	}
	else {
		for (int i = 0; i < 8; i++) {
			cellVolumeCount(&cur->child[i], level + 1, index * 8 + i, blockBaseSize, cellData, res);
		}
	}
}
int ThreadSubregion::pointIncludeCheck(CounterTreeNode *cur,int level, int index, int blockBaseSize, int x, int y, int z) {
	if (cur->type == 0)return 0;
	if (cur->type == 2)return 1;

	for (int i = 0; i < 8; i++) {
		int loc = getBaseLocation(level+1, index*8+i, blockBaseSize);
		int baseX = loc % blockBaseSize;
		int baseY = loc / blockBaseSize % blockBaseSize;
		int baseZ = loc / (blockBaseSize * blockBaseSize);
		int blockSize = blockBaseSize / pow(2, level+1);
		if (x >= baseX && x < baseX + blockSize 
			&& y >= baseY && y < baseY + blockSize 
			&& z >= baseZ && z < baseZ + blockSize) {
			return pointIncludeCheck(&cur->child[i], level + 1, index * 8 + i, blockBaseSize, x, y, z);
		}
	}
	qDebug() << "What"<<level<<index<<blockBaseSize<<x<<y<<z;
	return -1;
}
int ThreadSubregion::cellIncludeCheck(cell curCell) {
	long long x[2] = { curCell.minbox.x/precision_weight,curCell.maxbox.x / precision_weight };
	long long y[2] = { curCell.minbox.y/precision_weight,curCell.maxbox.y / precision_weight };
	long long z[2] = { curCell.minbox.z/ precision_weight,curCell.maxbox.z / precision_weight };
	
	int sum = 0;
	for (int ix = 0; ix < 2; ix++) {
		for (int iy = 0; iy < 2; iy++) {
			for (int iz = 0; iz < 2; iz++) {
				for (int i = 0; i < counterTreeRootBlockList.size(); i++) {
					long long baseX = counterTreeRootBlockList[i].x()*mGlobals.CurrentProject->DataBlockSize;
					long long baseY = counterTreeRootBlockList[i].y()*mGlobals.CurrentProject->DataBlockSize;
					long long baseZ = counterTreeRootBlockList[i].z()*mGlobals.CurrentProject->DataBlockSize;
					if (x[ix] >= baseX && x[ix] < baseX + mGlobals.CurrentProject->DataBlockSize
						&& y[iy] >= baseY && y[iy] < baseY + mGlobals.CurrentProject->DataBlockSize
						&& z[iz] >= baseZ && z[iz] < baseZ + mGlobals.CurrentProject->DataBlockSize) {
						//qDebug() << "x:"<<baseX<<"|y"<<baseY<<"|z:"<<baseZ;
						sum += pointIncludeCheck(&counterTree[i], 0, 0, mGlobals.CurrentProject->DataBlockSize, x[ix]- baseX, y[iy]- baseY, z[iz]- baseZ);
						//if (res != 0)sum++;
						break;
					}
				}
			}
		}
	}
	if (sum <= 2) {
		return 0;
	}
	//qDebug() << "sum:" << sum;
	if (sum  >= 6) {
		return 2;
	}


	return 1;



}

int ThreadSubregion::getBaseLocation(int level, int p, int blockBaseSize) {
	if (level == 0) {
		return 0;
	}
	int prevLocation = getBaseLocation(level - 1, p / 8, blockBaseSize);
	int blockSize = blockBaseSize / pow(2, level);
	int curP = p % 8;

	return prevLocation + blockSize * (curP / 4) * blockBaseSize * blockBaseSize + blockSize * ((curP / 2) % 2) * blockBaseSize + blockSize * (curP % 2);

}

int ThreadSubregion::getBlockType(int level, int index, unsigned char *blockData,int blockBaseSize) { 
	int baseLocation = getBaseLocation(level, index, blockBaseSize);
	//qDebug() << baseLocation;

	int baseX = baseLocation % blockBaseSize;
	int baseY = baseLocation / blockBaseSize % blockBaseSize;
	int baseZ = baseLocation / (blockBaseSize * blockBaseSize);
	int blockSize = blockBaseSize / pow(2, level);

	int blockBaseSize2=blockBaseSize * blockBaseSize;
	//qDebug() << "x: " << baseX << "| y: " << baseY << "| z: " << baseZ;
	//qDebug() << blockSize;

	int type = -1;//0: not included, 1: partially included, 2: included

//////GPU version
//	if (blockSize < 8) {
//
//		for (int z = baseZ; z < baseZ + blockSize; z++) {
//			for (int y = baseY; y < baseY + blockSize; y++) {
//				for (int x = baseX; x < baseX + blockSize; x++) {
//					if (blockData[z * blockBaseSize * blockBaseSize + y * blockBaseSize + x] == 0) {
//						if (type == 2)return 1;
//						type = 0;
//					}
//					else {
//						if (type == 0)return 1;
//						type = 2;
//					}
//				}
//			}
//		}
//		return type;
//	}
//	int information[5];
//	information[0] = blockBaseSize;
//	information[1] = blockSize;
//	information[2] = baseX;
//	information[3] = baseY;
//	information[4] = baseZ;
//
//
//	type = cuda_parallel_type_decision(blockData, information);
//
////CPU version


	//for (int z = baseZ; z < baseZ + blockSize; z++) {
	//	for (int y = baseY; y < baseY + blockSize; y++) {
	//		for (int x = baseX; x < baseX + blockSize; x++) {
	//			if (blockData[z * blockBaseSize * blockBaseSize + y * blockBaseSize + x] == 0) {
	//				if (type == 2)return 1;
	//				type = 0;
	//			}
	//			else {
	//				if (type == 0)return 1;
	//				type = 2;
	//			}
	//		}
	//	}
	//}
	//return type;

	if (blockSize == 1) {
		if (blockData[baseZ * blockBaseSize2 + baseY * blockBaseSize + baseX] == 0) return 0;
		else return 2;
	}
	else if (blockSize == 2) {
		for (int z = baseZ; z < baseZ + blockSize; z++) {
			for (int y = baseY; y < baseY + blockSize; y++) {
				for (int x = baseX; x < baseX + blockSize; x++) {
					if (blockData[z * blockBaseSize2 + y * blockBaseSize + x] == 0) {
						if (type == 2)return 1;
						type = 0;
					}
					else {
						if (type == 0)return 1;
						type = 2;
					}
				}
			}
		}
		return type;
	}
	else if (blockSize == 4) {

		for (int z = baseZ; z < baseZ + blockSize; z+=3) {
			for (int y = baseY; y < baseY + blockSize; y+=3) {
				for (int x = baseX; x < baseX + blockSize; x+=3) {
					if (blockData[z * blockBaseSize2 + y * blockBaseSize + x] == 0) {
						if (type == 2)return 1;
						type = 0;
					}
					else {
						if (type == 0)return 1;
						type = 2;
					}
				}
			}
		}
		return type;
	}


	int sampling = 3;

	//test 6 side of block
	//yz plane
	for (int z = baseZ; z < baseZ + blockSize; z+= sampling) {
		for (int y = baseY; y < baseY + blockSize; y+= sampling) {
			if (blockData[z * blockBaseSize2 + y * blockBaseSize + baseX] == 0) {
				if (type == 2)return 1;
				type = 0;
			}
			else {
				if (type == 0)return 1;
				type = 2;
			}
			if (blockData[z * blockBaseSize2 + y * blockBaseSize + baseX+ blockSize-1] == 0) {
				if (type == 2)return 1;
				type = 0;
			}
			else {
				if (type == 0)return 1;
				type = 2;
			}
		}
	}

	//xz plane
	for (int z = baseZ; z < baseZ + blockSize; z+= sampling) {
		for (int x = baseX; x < baseX + blockSize; x+= sampling) {
			if (blockData[z * blockBaseSize2 + baseY * blockBaseSize + x] == 0) {
				if (type == 2)return 1;
				type = 0;
			}
			else {
				if (type == 0)return 1;
				type = 2;
			}
			if (blockData[z * blockBaseSize2 + (baseY+ blockSize-1) * blockBaseSize + x] == 0) {
				if (type == 2)return 1;
				type = 0;
			}
			else {
				if (type == 0)return 1;
				type = 2;
			}
		}
	}

	//xy plane
	for (int y = baseY; y < baseY + blockSize; y+= sampling) {
		for (int x = baseX; x < baseX + blockSize; x+= sampling) {
			if (blockData[baseZ * blockBaseSize2 + y * blockBaseSize + x] == 0) {
				if (type == 2)return 1;
				type = 0;
			}
			else {
				if (type == 0)return 1;
				type = 2;
			}
			if (blockData[(baseZ+ blockSize-1) * blockBaseSize2 + y * blockBaseSize + x] == 0) {
				if (type == 2)return 1;
				type = 0;
			}
			else {
				if (type == 0)return 1;
				type = 2;
			}
		}
	}

	//qDebug() << "x: " << baseX << "| y: " << baseY << "| z: " << baseZ;
	//qDebug() << blockSize;
	//qDebug() << type;

//


	return type;

}