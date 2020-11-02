#include "LayerBackground.h"



LayerBackground::LayerBackground() {
}

LayerBackground::LayerBackground(std::string path) {
	
	BackgroundHeaderPath = path;
	
	Utils mUtil;
	std::vector<std::string> BackDirPath = mUtil.Split(path.c_str(), "header.bgl");
	BackgroundPath = BackDirPath[0];
	BackgroundThumbnailPath = BackDirPath[0] + "thumbnail.png";
	qDebug() << QString::fromStdString(BackgroundPath);

	this->headerReader();
	this->LoadVolumeRenderingTF();
	this->LoadVolumeRenderingSource();
	
}

LayerBackground::~LayerBackground() {
}

void LayerBackground::LoadVolumeRenderingTF() {
	FILE *fp = fopen("Resources/Zebrafish_eye_filled.tf", "rb");
	
	for (int i = 0; i < 256; ++i) {
		float_color temp;
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
		
		fread(&r, 1, 1, fp);
		fread(&g, 1, 1, fp);
		fread(&b, 1, 1, fp);
		fread(&a, 1, 1, fp);

		temp.r = (float)r / 255.0;
		temp.g = (float)g / 255.0;
		temp.b = (float)b / 255.0;
		temp.a = (float)a / 255.0;
		
		//qDebug() << temp.r << " " << temp.g << " " << temp.b << " " << temp.a;
		transferFunc.push_back(temp);
	}
	fclose(fp);
	qDebug() << "load volume rendering tf";
}

void LayerBackground::LoadVolumeRenderingSource() {
	
	int level = HeatmapLevel;
	volumeRenderWidth = DataSizeX / pow(2, level);
	volumeRenderHeight = DataSizeY / pow(2, level);
	volumeRenderDepth = DataSizeZ / pow(2, level);

	qDebug() << level<<volumeRenderWidth << volumeRenderHeight << volumeRenderDepth;


	long long lvalue = 1;
	long long data_size = lvalue * volumeRenderWidth * volumeRenderHeight * volumeRenderDepth;
	long long tile_size = lvalue * DataBlockSize * DataBlockSize;
	long long block_size = lvalue * DataBlockSize * DataBlockSize * DataBlockSize;
	volumeRenderData = new unsigned char[data_size];
	HeatmapData = new float[data_size];

	for (int i = 0; i < data_size; ++i) {
		HeatmapData[i] = 0.0;
	}

	Utils mUtil;
	int x_count;
	int y_count;
	int z_count;
	mUtil.CalcXYZCount(&x_count, &y_count, &z_count, level, DataSizeX, DataSizeY, DataSizeZ, DataBlockSize);
	
	//qDebug() << "VoumeRender Size";
	//qDebug() << x_count << " " << y_count << " " << z_count;
	for (int z = 0; z < volumeRenderDepth; z+= DataBlockSize / pow(2, level)) {
		
		unsigned char *tile = new unsigned char[tile_size];
		for (int y_b = 0; y_b < y_count; ++y_b) {
			for (int x_b = 0; x_b < x_count; ++x_b) {
				std::string blck_path = DataPathXY + std::to_string(int(z*pow(2, level) / DataBlockSize)) + "_" + std::to_string(y_b) + "_" + std::to_string(x_b) + "_" + std::to_string(level) + ".block";
				//qDebug() << QString::fromStdString(blck_path);
				FILE *blck_rfp = fopen(blck_path.c_str(), "rb");
				long long offset_size = lvalue * tile_size;
				for (int k = 0; k< DataBlockSize; k += int(pow(2, level))) {

					
					int z_value = z + k / (int)pow(2, level);
					//qDebug() << z_value;
					if (z_value < volumeRenderDepth) {
						fseek(blck_rfp, offset_size*k, SEEK_SET);
						fread(tile, tile_size, 1, blck_rfp);

						for (int i = 0; i < DataBlockSize; ++i) {
							for (int j = 0; j < DataBlockSize; ++j) {
								if (DataBlockSize * y_b + i < volumeRenderHeight && DataBlockSize * x_b + j < volumeRenderWidth) {
									long long dst_offset =
										lvalue * z_value * volumeRenderWidth*volumeRenderHeight
										+ lvalue * (DataBlockSize * y_b + i) * volumeRenderWidth
										+ lvalue * (DataBlockSize * x_b + j);
									volumeRenderData[dst_offset] = tile[DataBlockSize*i + j];
								}
							}
						}
					}
					
				}
				fclose(blck_rfp);
			}
		}
		delete[] tile;
	}
}


void LayerBackground::headerReader() {
	std::ifstream mifs(BackgroundHeaderPath, std::ifstream::binary);

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


	std::getline(mifs, line3);
	std::getline(mifs, line3);
	std::getline(mifs, line3);



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

	DataPathXY += BackgroundPath + "XY/";
	DataPathYZ += BackgroundPath + "YZ/";
	DataPathZX += BackgroundPath + "ZX/";

	qDebug() << QString::fromStdString(DataPathXY);
	qDebug() << QString::fromStdString(DataPathYZ);
	qDebug() << QString::fromStdString(DataPathZX);
}

int LayerBackground::LoadBlockBySerialIndex(back_layer tempblock){
	std::ifstream frame;
	std::string frame_path;
	long long lvalue = 1;
	long long seekvalue = lvalue * DataBlockSize * DataBlockSize;


	if (tempblock.load_axis_code == 1) {
		std::string filename = std::to_string(tempblock.index_z/DataBlockSize) + "_" + std::to_string(tempblock.index_y) + "_" + std::to_string(tempblock.index_x) + "_" + std::to_string(tempblock.level) + ".block";
		qDebug() << QString::fromStdString(DataPathXY + filename);
		frame_path = DataPathXY + filename;
		
		seekvalue *= (tempblock.index_z % DataBlockSize);
		//frame.open(DataPathXY + filename, std::ifstream::binary);
	}
	else if (tempblock.load_axis_code == 2) {
		std::string filename = std::to_string(tempblock.index_z) + "_" + std::to_string(tempblock.index_y) + "_" + std::to_string(tempblock.index_x/DataBlockSize) + "_" + std::to_string(tempblock.level) + ".block";
		qDebug() << QString::fromStdString(DataPathYZ + filename);
		frame_path = DataPathYZ + filename;

		seekvalue *= (tempblock.index_x % DataBlockSize);
		//frame.open(DataPathYZ + filename, std::ifstream::binary);
	}
	else if (tempblock.load_axis_code == 3) {
		std::string filename = std::to_string(tempblock.index_z) + "_" + std::to_string(tempblock.index_y/DataBlockSize) + "_" + std::to_string(tempblock.index_x) + "_" + std::to_string(tempblock.level) + ".block";;
		qDebug() << QString::fromStdString(DataPathZX + filename);
		frame_path = DataPathZX + filename;
		seekvalue *= (tempblock.index_y % DataBlockSize);
		//frame.open(DataPathZX + filename, std::ifstream::binary);
	}
	frame.open(frame_path, std::ifstream::binary);

	if (!frame) {
		qDebug() << "backlayer-openfail";
		frame.close();
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

back_layer LayerBackground::initializeBlock(block_info info) {
	back_layer temp;
		
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

int LayerBackground::checkBlockIndex(int x, int y, int z, int level, int axis) {
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

	std::list<back_layer>::iterator iter;
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

void LayerBackground::removeBlock() {
	while (BlockList.size() > 100) {
		back_layer t = BlockList.front();
		delete[]t.data;
		BlockList.pop_front();
	}
}
