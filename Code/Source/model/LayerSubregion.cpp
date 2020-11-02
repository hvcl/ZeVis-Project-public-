#include "LayerSubregion.h"



LayerSubregion::LayerSubregion() {
}

LayerSubregion::LayerSubregion(std::string name, std::string path) {

	SubregionHeaderPath = path;
	SubregionName = name;

	QString forHash = QString::fromStdString(SubregionHeaderPath + "/" + SubregionName + "/") + QDateTime::currentDateTime().toString();
	
	SubregionID = QString("%1").arg(QString(QCryptographicHash::hash(forHash.toUtf8(), QCryptographicHash::Md5).toHex())).toStdString();

	Utils mUtil;
	std::vector<std::string> temp_array = mUtil.Split(SubregionHeaderPath.c_str(), "header.srl");
	SubregionPath = temp_array.front();

	TouchTablePath = SubregionPath + SubregionName +"_composited.dat_include";
	IntersectTablePath = SubregionPath + SubregionName + "_composited.dat_intersect";

	DataPathXY = SubregionPath + "XY/";
	DataPathYZ = SubregionPath + "YZ/";
	DataPathZX = SubregionPath + "ZX/";

	qDebug() << QString::fromStdString(SubregionID);
	qDebug() << QString::fromStdString(SubregionName);
	qDebug() << QString::fromStdString(SubregionPath);
	qDebug() << QString::fromStdString(SubregionHeaderPath);
	qDebug() << QString::fromStdString(DataPathXY);
	qDebug() << QString::fromStdString(DataPathYZ);
	qDebug() << QString::fromStdString(DataPathZX);

	this->headerReader();
	//this->readIndex();
}

LayerSubregion::~LayerSubregion() {
}

void LayerSubregion::readIndex() {
	
	//std::ifstream mIfs;
	//mIfs.open(TouchTablePath);
	//while (!mIfs.eof()) {
	//	unsigned int index;
	//	mIfs >> index;
	//	TouchIndex[index] = true;
	//	CompleteIndex[index] = true;
	//}
	//mIfs.close();

	//std::ifstream mIfs2;
	//mIfs2.open(IntersectTablePath);
	//while (!mIfs2.eof()) {
	//	unsigned int index;
	//	mIfs2 >> index;
	//	IntersectIndex[index] = true;
	//	CompleteIndex.erase(index);
	//}
	//mIfs2.close();

}


void LayerSubregion::headerReader() {

	std::ifstream mifs(SubregionHeaderPath, std::ifstream::binary);

	Utils mUtil;
	std::string line1;
	std::getline(mifs, line1);
	std::vector<std::string> list_line1 = mUtil.Split(line1.c_str(), " : ");
	DataSizeX = atoi(list_line1.back().c_str());

	std::string line2;
	std::getline(mifs, line2);
	std::vector<std::string> list_line2 = mUtil.Split(line2.c_str(), " : ");
	DataSizeY = atoi(list_line2.back().c_str());

	std::string line3;
	std::getline(mifs, line3);
	std::vector<std::string> list_line3 = mUtil.Split(line3.c_str(), " : ");
	DataSizeZ = atoi(list_line3.back().c_str());

	std::string line4;
	std::getline(mifs, line4);
	std::vector<std::string> list_line4 = mUtil.Split(line4.c_str(), " : ");
	DataBlockSize = atoi(list_line4.back().c_str());

	std::string line5;
	std::getline(mifs, line5);
	std::vector<std::string> list_line5 = mUtil.Split(line5.c_str(), " : ");
	DataLevel = atoi(list_line5.back().c_str());

	std::string line6;
	std::getline(mifs, line6);
	std::vector<std::string> list_line6 = mUtil.Split(line6.c_str(), " : ");
	DataType = list_line6.back().c_str();

	
	mifs.close();
}


int LayerSubregion::LoadBlockBySerialIndex(subregion_layer tempblock) {
	std::ifstream frame;
	std::string frame_path;
	long long lvalue = 1;
	long long seekvalue = lvalue * DataBlockSize * DataBlockSize;


	if (tempblock.load_axis_code == 1) {
		std::string filename = std::to_string(tempblock.index_z / DataBlockSize) + "_" + std::to_string(tempblock.index_y) + "_" + std::to_string(tempblock.index_x) + "_" + std::to_string(tempblock.level) + ".block";
		qDebug() << QString::fromStdString(DataPathXY + filename);
		frame_path = DataPathXY + filename;

		seekvalue *= (tempblock.index_z % DataBlockSize);
		//frame.open(DataPathXY + filename, std::ifstream::binary);
	}
	else if (tempblock.load_axis_code == 2) {
		std::string filename = std::to_string(tempblock.index_z) + "_" + std::to_string(tempblock.index_y) + "_" + std::to_string(tempblock.index_x / DataBlockSize) + "_" + std::to_string(tempblock.level) + ".block";
		qDebug() << QString::fromStdString(DataPathYZ + filename);
		frame_path = DataPathYZ + filename;

		seekvalue *= (tempblock.index_x % DataBlockSize);
		//frame.open(DataPathYZ + filename, std::ifstream::binary);
	}
	else if (tempblock.load_axis_code == 3) {
		std::string filename = std::to_string(tempblock.index_z) + "_" + std::to_string(tempblock.index_y / DataBlockSize) + "_" + std::to_string(tempblock.index_x) + "_" + std::to_string(tempblock.level) + ".block";;
		qDebug() << QString::fromStdString(DataPathZX + filename);
		frame_path = DataPathZX + filename;
		seekvalue *= (tempblock.index_y % DataBlockSize);
		//frame.open(DataPathZX + filename, std::ifstream::binary);
	}
	frame.open(frame_path, std::ifstream::binary);

	if (!frame) {
		qDebug() << "subregionlayer-openfail";
		frame.close();
		/*tempblock.data = NULL;
		tempblock.status = false;
		BlockList.push_back(tempblock);*/
		return -1;
	}
	else {

		tempblock.data = new unsigned char[DataBlockSize * DataBlockSize];
		tempblock.status = true;
		frame.seekg(seekvalue, std::ios::beg);
		frame.read((char *)&tempblock.data[0], DataBlockSize * DataBlockSize);
		BlockList.push_back(tempblock);

		frame.close();
		return BlockList.size() - 1;
	}
}

subregion_layer LayerSubregion::initializeBlock(block_info info) {
	subregion_layer temp;

	if (info.axis == 1) {
		temp.index_x = info.x;
		temp.index_y = info.y;
		temp.index_z = info.z;
	}
	else if (info.axis == 2) {
		temp.index_x = info.z;
		temp.index_y = info.y;
		temp.index_z = info.x;
	}
	else if (info.axis == 3) {
		temp.index_x = info.x;
		temp.index_y = info.z;
		temp.index_z = info.y;
	}

	temp.pos_x = info.size * info.x;
	temp.pos_y = info.size * info.y;
	temp.pos_z = info.z;
	temp.size_x = info.size;
	temp.size_y = info.size;
	temp.size_z = info.size;
	temp.level = info.level;
	temp.load_axis_code = info.axis;
	return temp;
}


int LayerSubregion::checkBlockIndex(int x, int y, int z, int level, int axis) {
	int rt_value = -1;
	int index = 0;

	int idx_x;
	int idx_y;
	int idx_z;

	if (axis == 1) {
		idx_x = x;
		idx_y = y;
		idx_z = z;
	}
	else if (axis == 2) {
		idx_x = z;
		idx_y = y;
		idx_z = x;
	}
	else if (axis == 3) {
		idx_x = x;
		idx_y = z;
		idx_z = y;
	}

	std::list<subregion_layer>::iterator iter;
	for (iter = BlockList.begin(); iter != BlockList.end(); ++iter) {
		if (iter->index_x == idx_x && iter->index_y == idx_y && iter->index_z == idx_z && iter->level == level && iter->load_axis_code == axis) {
			rt_value = 1;
			break;
		}
		index++;
	}
	if (rt_value == 1) {
		return index;
	}
	else {
		return -1;
	}
}

void LayerSubregion::removeBlock() {
	while (BlockList.size() > 100) {
		subregion_layer t = BlockList.front();
		delete[]t.data;

		BlockList.pop_front();
	}
}
