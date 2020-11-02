#include "DataCellAnalytics.h"
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

#include "Eigen/Core";
#include "Eigen/Eigenvalues";


DataCellAnalytics::DataCellAnalytics()
{

}
void DataCellAnalytics::Init(std::string path) {
	AnalyticsResultPath = path;
	mifs.open(AnalyticsResultPath);
	qDebug() << "Init Cell";
	qDebug() << QString::fromStdString(AnalyticsResultPath);
	this->LoadCellInformation();
}

void DataCellAnalytics::LoadCellInformation() {
	QTime myTimer;
	myTimer.start();

	while (!mifs.eof()) {
		cell tmp;
		mifs >> tmp.index >> tmp.minbox.x >> tmp.minbox.y >> tmp.minbox.z >> tmp.maxbox.x >> tmp.maxbox.y >> tmp.maxbox.z;
/*
		if (volumeMax < tmp.volume) volumeMax = tmp.volume;
		if (sphericityMax < tmp.sphericity) sphericityMax = tmp.sphericity;
		if (surfaceareaMax < tmp.surfacearea) surfaceareaMax = tmp.surfacearea;
		if (eccentricityMax < tmp.eccentricity) eccentricityMax = tmp.eccentricity;
		if (intensityMax < tmp.intensity) intensityMax = tmp.intensity;

		if (volumeMin > tmp.volume) volumeMin = tmp.volume;
		if (sphericityMin > tmp.sphericity) sphericityMin = tmp.sphericity;
		if (surfaceareaMin > tmp.surfacearea) surfaceareaMin = tmp.surfacearea;
		if (eccentricityMin > tmp.eccentricity) eccentricityMin = tmp.eccentricity;
		if (intensityMin > tmp.intensity) intensityMin = tmp.intensity;*/

		mCellList.push_back(tmp);
	}

	MaxCellCount = mCellList.size();
	MaxCellIndex = mCellList[mCellList.size() - 1].index;
	
	qDebug() << "DataCellAnalytics";
	qDebug() << MaxCellCount;
	qDebug() << MaxCellIndex;
	
	
	//GenerateHistogram();
	int nMilliseconds = myTimer.elapsed();
	int seconds = nMilliseconds / 1000 / 1000;
	qDebug() << nMilliseconds;
}
void DataCellAnalytics::LoadFeature() {


}

//
//void DataCellAnalytics::GenerateHistogram() {
//	int HistSize = HistWidth / HistBarWidth;
//	/*volumeHist.reserve(HistSize);
//	surfaceareaHist.reserve(HistSize);
//	sphericityHist.reserve(HistSize);
//	eccentricityHist.reserve(HistSize);
//	intensityHist.reserve(HistSize);*/
//	for (int i = 0; i < HistSize; ++i) {
//		/*volumeHist[i] = 0;
//		surfaceareaHist[i] = 0;
//		sphericityHist[i] = 0;
//		eccentricityHist[i] = 0;
//		intensityHist[i] = 0;*/
//		std::vector<long long> v;
//
//		volumeHist.push_back(v);
//		surfaceareaHist.push_back(v);
//		sphericityHist.push_back(v);
//		eccentricityHist.push_back(v);
//		intensityHist.push_back(v);
//	}
//	qDebug() << "vector init done";
//
//	std::vector<cell>::iterator iter = mCellList.begin();
//	//for (iter = mCellList.begin(); iter != mCellList.end(); ++iter) {
//	//	if (iter->intensity != -1) {
//	//		int volume_hist_index = ((float)(iter->volume - volumeMin) / (float)(volumeMax - volumeMin)) * (float)(HistSize-1);
//	//		int surfacearea_hist_index = ((float)(iter->surfacearea - surfaceareaMin) / (float)(surfaceareaMax - surfaceareaMin)) * (float)(HistSize - 1);
//	//		int sphericity_hist_index = ((float)(iter->sphericity - sphericityMin) / (float)(sphericityMax - sphericityMin)) * (float)(HistSize - 1);
//	//		int eccentricity_hist_index = ((float)(iter->eccentricity - eccentricityMin) / (float)(eccentricityMax - eccentricityMin)) * (float)(HistSize - 1);\
//	//		int intensity_hist_index = ((float)(iter->intensity - intensityMin) / (float)(intensityMax - intensityMin)) * (float)(HistSize - 1);
//
//	//		//qDebug() << hist_index;
//	//		volumeHist[volume_hist_index].push_back(iter->index);
//	//		surfaceareaHist[surfacearea_hist_index].push_back(iter->index);
//	//		sphericityHist[sphericity_hist_index].push_back(iter->index);
//	//		eccentricityHist[eccentricity_hist_index].push_back(iter->index);
//	//		intensityHist[intensity_hist_index].push_back(iter->index);
//	//	}
//	//}
//}


DataCellAnalytics::~DataCellAnalytics()
{
}

unsigned short ReverseShort(const unsigned short inShort)
{
	unsigned short retVal;
	char *shortToConvert = (char*)& inShort;
	char *returnShort = (char*)& retVal;

	// swap the bytes into a temporary buffer
	returnShort[0] = shortToConvert[1];
	returnShort[1] = shortToConvert[0];
	return retVal;
}

#define BitF8
void DataCellAnalytics::featurePrint(QString inputCoordPath, QString OutputPath) {



	FILE *f = fopen(inputCoordPath.toStdString().c_str(), "rb");
	FILE *f2 = fopen(OutputPath.toStdString().c_str(), "w");
	fprintf(f2, "x,y,z,cell index,volume,sphericity\n");
#ifdef BitF8
	unsigned char *cell_data[3];
	for (int i = 0; i < 3; i++) {
		cell_data[i] = new unsigned char[3072 * 3072];
	}

#else
	unsigned short *cell_data[3];
	for (int i = 0; i < 3; i++) {
		cell_data[i] = new unsigned short[3072 * 3072];
	}
#endif

	int *cnt = new int[65535]();

	long double *volume = new long double[65535]();
	long double *surface = new long double[65535]();
	long double *sphericity = new long double[65535]();
	long double *posx = new long double[65535]();
	long double *posy = new long double[65535]();
	long double *posz = new long double[65535]();

	int *cellState = new int[65535](); //0: not appear 1: fully appera 2: partially appear

#ifdef BitF8
	fread(cell_data[0], 1, 3072 * 3072, f);
	fread(cell_data[1], 1, 3072 * 3072, f);
#else
	fread(cell_data[0], 2, 3072 * 3072, f);
	fread(cell_data[1], 2, 3072 * 3072, f);
#endif

	int cur = 1;

	for (int z = 1; z < 1023; z++) {
		qDebug() << float(z) / 1023;
#ifdef BitF8
		fread(cell_data[(cur + 1) % 3], 1, 3072 * 3072, f);

#else
		fread(cell_data[(cur+1)%3], 2, 3072 * 3072, f);
#endif
		for (int y = 0; y < 3072; y++) {
			for (int x = 0; x < 3072; x++) {
				int pos = y * 3072 + x;
#ifdef BitF8
				int cellN = cell_data[cur][pos];
#else
				int cellN = ReverseShort(cell_data[cur][pos]);
#endif
				if (cellN != 0) {
					posx[cellN] += x;
					posy[cellN] += y;
					posz[cellN] += z;
					volume[cellN]++;
				}

				if (cellN != 0) {
					if (cellState[cellN] == 2)continue;
					cellState[cellN] = 1;
//					volume[cellN]++;

					//int sum = 0;
					//if (x - 1 >= 0) {
					//	if (ReverseShort(cell_data[cur][pos - 1]) == cellN)sum++;
					//}
					if(x-1<0) {
						cellState[cellN] = 2;
					}

					//if (x + 1 < 3072) {
					//	if (ReverseShort(cell_data[cur][pos + 1]) == cellN)sum++;
					//}
					if(x+1==3072) {
						cellState[cellN] = 2;
					}

					//if (y - 1 >= 0) {
					//	if (ReverseShort(cell_data[cur][pos - 3072]) == cellN)sum++;
					//}
					if(y-1<0) {
						cellState[cellN] = 2;
					}
					//if (y + 1 < 3072) {
					//	if (ReverseShort(cell_data[cur][pos + 3072]) == cellN)sum++;
					//}
					if(y+1==3072) {
						cellState[cellN] = 2;
					}
#ifdef BitF8
					if (z - 1 == 0 && cell_data[(cur + 2) % 3][pos] == cellN) {

#else
					if (z - 1 == 0 && ReverseShort(cell_data[(cur+2)%3][pos]) == cellN) {
#endif
						cellState[cellN] = 2;
					}
					//else {
					//	if (ReverseShort(cell_data[(cur+2)%3][pos]) == cellN) {
					//		sum++;
					//	}
					//}
#ifdef BitF8
					if (z + 1 == 1023 && cell_data[(cur + 1) % 3][pos] == cellN) {
#else
					if (z + 1 == 1023 && ReverseShort(cell_data[(cur+1)%3][pos]) == cellN) {
#endif
						cellState[cellN] = 2;
					}
					//else {
					//	if (ReverseShort(cell_data[(cur+1)%3][pos]) == cellN) {
					//		sum++;
					//	}
					//}
					//qDebug() << sum;
					//if (sum == 6) {
					//	volume[cellN]++;

					//}
				}
			}
		}
		cur = (cur + 1) % 3;
	}


	//for (int i = 0; i < 65535; i++) {
	//	if (volume[i] != 0) {
	//		posx[i] /= volume[i];
	//		posy[i] /= volume[i];
	//		posz[i] /= volume[i];
	//	}
	//}


	fclose(f);
	f = fopen(inputCoordPath.toStdString().c_str(), "rb");
#ifdef BitF8
	fread(cell_data[0], 1, 3072 * 3072, f);
#else
	fread(cell_data[0], 2, 3072 * 3072, f);
#endif
	cur = 0;

	bool ck[2][2][2];

	int indexList[100];
	int indexN = 0;

	for (int z = 0; z < 1023; z++) {
		qDebug() << float(z) / 1023;
#ifdef BitF8
		fread(cell_data[(cur + 1) % 2], 1, 3072 * 3072, f);
#else
		fread(cell_data[(cur + 1) % 2], 2, 3072 * 3072, f);
#endif
		for (int y = 0; y < 3071; y++) {
			for (int x = 0; x < 3071; x++) {

				indexN = 0;

				for (int dz = 0; dz < 2; dz++) {
					for (int dy = 0; dy < 2; dy++) {
						for (int dx = 0; dx < 2; dx++) {
							int pos = (y + dy) * 3072 + x + dx;
#ifdef BitF8
							int curI = cell_data[(cur + dz) % 2][pos];
#else
							int curI = ReverseShort(cell_data[(cur + dz) % 2][pos]);
#endif
							if (curI != 0) {
								int q = 0;
								for (; q < indexN; q++) {
									if (indexList[q] == curI)break;

								}
								if (q == indexN) {
									indexList[indexN] = curI;
									indexN++;
								}
							}
						}
					}
				}

				for (int q = 0; q < indexN; q++) {
					for (int dz = 0; dz < 2; dz++) {
						for (int dy = 0; dy < 2; dy++) {
							for (int dx = 0; dx < 2; dx++) {
								int pos = (y + dy) * 3072 + x + dx;
#ifdef BitF8
								int curI = cell_data[(cur + dz) % 2][pos];
#else
								int curI = ReverseShort(cell_data[(cur + dz) % 2][pos]);
#endif
								if (curI != indexList[q]) {
									ck[dx][dy][dz] = false;
								}
								else ck[dx][dy][dz] = true;
							}
						}
					}
					surface[indexList[q]] += getSurfaceUsingMarchingCube(ck);
				}

			}
		}
		cur = (cur + 1) % 2;
	}




	for (int i = 0; i < 65535; i++) {
		if (surface[i] != 0 && cellState[i]==1) {
			posx[i] /= volume[i];
			posy[i] /= volume[i];
			posz[i] /= volume[i];

			float As, radius;
			radius = pow((3 / 12.566368)*volume[i], 1.0 / 3.0);
			As = 4 * M_PI * pow(radius, 2);
			sphericity[i] = As / surface[i];

			fprintf(f2, "%d,%d,%d,%d,%0.2lf,%0.4lf\n", int(posx[i]), int(posy[i]), int(posz[i]), 
				i, volume[i],sphericity[i]);
		}
	}
	fclose(f);
	fclose(f2);

	delete[]cell_data[0];
	delete[]cell_data[1];
	delete[]cell_data[2];
	delete[]volume;
	delete[]surface;
	delete[]sphericity;
	delete[]posx;
	delete[]posy;
	delete[]posz;
	delete[]cnt;


}
double DataCellAnalytics::getSurfaceUsingMarchingCube(bool ck[2][2][2]) {
	int count = 0;
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				if (ck[x][y][z])count++;
			}
		}
	}
	if (count == 8)return 0;
	if (count == 1 || count==7)return sqrt(3.0) / 8.0;
	if (count == 2) {
		int first = -1, second = -1;
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					if (ck[x][y][z]) {
						if (first == -1)first = x + y + z;
						else second = x + y + z;
					}
				}
			}
		}
		if (abs(first - second) == 1) {
			return sqrt(2.0) / 2.0;
		}
		else {
			return sqrt(3.0) / 4.0;
		}
	}
	if (count == 6) {
		int first = -1, second = -1;
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					if (!ck[x][y][z]) {
						if (first == -1)first = x + y + z;
						else second = x + y + z;
					}
				}
			}
		}
		if (abs(first - second) == 1) {
			return sqrt(2.0) / 2.0;
		}
		else {
			return sqrt(3.0) / 4.0;
		}
	}
	if (count == 3) {
		int first = -1, second = -1, third = -1;
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					if (ck[x][y][z]) {
						if (first == -1)first = x + y + z;
						else if (second == -1)second = x + y + z;
						else third = x + y + z;
					}
				}
			}
		}
		if (abs(first - second) + abs(second - third) + abs(first - third) == 4) {
			return 0.5 + 3 * sqrt(3.0) / 8.0;
		}
		else if (abs(first - second) == 2 && abs(second - third) == 2) {
			return 3 * sqrt(3.0) / 8.0;
		}
		else {
			return sqrt(3.0) / 8.0 + sqrt(2.0) / 2.0;
		}
	}
	if (count == 5) {
		int first = -1, second = -1, third = -1;
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					if (!ck[x][y][z]) {
						if (first == -1)first = x + y + z;
						else if (second == -1)second = x + y + z;
						else third = x + y + z;
					}
				}
			}
		}
		if (abs(first - second) + abs(second - third) + abs(first - third) == 4) {
			return 0.5 + 3 * sqrt(3.0) / 8.0;
		}
		else if (abs(first - second) == 2 && abs(second - third) == 2) {
			return 3 * sqrt(3.0) / 8.0;
		}
		else {
			return sqrt(3.0) / 8.0 + sqrt(2.0) / 2.0;
		}
	}

	if (count == 4) {
		int first = -1, second = -1, third = -1, fourth=-1;
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					if (ck[x][y][z]) {
						if (first == -1)first = x + y + z;
						else if (second == -1)second = x + y + z;
						else if (third == -1)third = x + y + z;
						else fourth = x + y + z;
					}
				}
			}
		}
		if (abs(first - second) + abs(first-third) + abs(fourth - first) 
			+ abs(second - third) + abs(second - fourth) 
			+ abs(third - fourth)==8 ) {
			return 1.0;
		}
		if (abs(first - second) + abs(first - third) + abs(fourth - first)
			+ abs(second - third) + abs(second - fourth)
			+ abs(third - fourth) == 11) {
			return 0.5 + sqrt(3.0)*0.5;
		}
		if (abs(first - second) + abs(first - third) + abs(fourth - first)
			+ abs(second - third) + abs(second - fourth)
			+ abs(third - fourth) == 9) {
			return 3.0*sqrt(3.0) / 4.0;
		}
		if (abs(first - second) + abs(first - third) + abs(fourth - first)
			+ abs(second - third) + abs(second - fourth)
			+ abs(third - fourth) == 10) {
			return sqrt(3.0) / 4.0 + sqrt(5.0) / 2.0;
		}

		if (abs(first - second) == abs(first - third)&& abs(first - third) == abs(fourth - first)
			&& abs(fourth - first) == abs(second - third)&& abs(second - third) == abs(second - fourth)
			&& abs(second - fourth) == abs(third - fourth)) {
			return sqrt(3.0) / 2.0;
		}
		return sqrt(2.0);

	}


}

void DataCellAnalytics::CellVolumeSurfaceSphericityCalculation(QString cellDirPath, QString savePath,  int MaxCellN) {
	
	qDebug() << "start cell feature calculation";

	Utils mUtil;
	QDir directory(cellDirPath);
	QStringList target;
	target << "*_0.block";
	QStringList blocks = directory.entryList(target, QDir::Files);
	int bs = 512;
	long long size = bs * bs * bs;
	unsigned int *cell_block = new unsigned int[size];
	//unsigned int *cell_block_x_p = new unsigned int[size];
	//unsigned int *cell_block_x_m = new unsigned int[size];
	//unsigned int *cell_block_y_p = new unsigned int[size];
	//unsigned int *cell_block_y_m = new unsigned int[size];
	//unsigned int *cell_block_z_p = new unsigned int[size];
	//unsigned int *cell_block_z_m = new unsigned int[size];
	//bool x_p, x_m, y_p, y_m, z_p, z_m;



	double *volume = new double[MaxCellN + 1]();
	double *surface = new double[MaxCellN + 1]();
	double *sphericity = new double[MaxCellN + 1]();

	qDebug() << "finished init";

	int cnt = 0;
	int loopsize = blocks.size();
	qDebug() << loopsize;
	foreach(QString filename, blocks) {
		qDebug() << (float)cnt++ / loopsize;

		QString cell_path = cellDirPath + filename;
		std::vector<std::string> str_vec = mUtil.Split(filename.toStdString().c_str(), "_");

		int baseX = (float)stoi(str_vec[2]) * bs;
		int baseY = (float)stoi(str_vec[1]) * bs;
		int baseZ = (float)stoi(str_vec[0]) * bs;

		//QString cell_x_p_Path = cellDirPath + QString::number(stoi(str_vec[0])) + "_"
		//	+ QString::number(stoi(str_vec[1])) + "_" + QString::number(stoi(str_vec[2]) + 1) + "_0.block";
		//QString cell_x_m_Path = cellDirPath + QString::number(stoi(str_vec[0])) + "_"
		//	+ QString::number(stoi(str_vec[1])) + "_" + QString::number(stoi(str_vec[2]) - 1) + "_0.block";
		//QString cell_y_p_Path = cellDirPath + QString::number(stoi(str_vec[0])) + "_"
		//	+ QString::number(stoi(str_vec[1]) + 1) + "_" + QString::number(stoi(str_vec[2])) + "_0.block";
		//QString cell_y_m_Path = cellDirPath + QString::number(stoi(str_vec[0])) + "_"
		//	+ QString::number(stoi(str_vec[1]) - 1) + "_" + QString::number(stoi(str_vec[2])) + "_0.block";
		//QString cell_z_p_Path = cellDirPath + QString::number(stoi(str_vec[0]) + 1) + "_"
		//	+ QString::number(stoi(str_vec[1])) + "_" + QString::number(stoi(str_vec[2])) + "_0.block";
		//QString cell_z_m_Path = cellDirPath + QString::number(stoi(str_vec[0]) - 1) + "_"
		//	+ QString::number(stoi(str_vec[1])) + "_" + QString::number(stoi(str_vec[2])) + "_0.block";
		//x_p = false;
		//x_m = false;
		//y_p = false;
		//y_m = false;
		//z_p = false;
		//z_m = false;
		//if (QFileInfo::exists(cell_x_p_Path) && QFileInfo(cell_x_p_Path).isFile())x_p = true;
		//if (QFileInfo::exists(cell_x_m_Path) && QFileInfo(cell_x_m_Path).isFile())x_m = true;
		//if (QFileInfo::exists(cell_y_p_Path) && QFileInfo(cell_y_p_Path).isFile())y_p = true;
		//if (QFileInfo::exists(cell_y_m_Path) && QFileInfo(cell_x_p_Path).isFile())y_m = true;
		//if (QFileInfo::exists(cell_z_p_Path) && QFileInfo(cell_z_p_Path).isFile())z_p = true;
		//if (QFileInfo::exists(cell_z_m_Path) && QFileInfo(cell_z_m_Path).isFile())z_m = true;



		if (QFileInfo::exists(cell_path) && QFileInfo(cell_path).isFile()) {
			FILE *cell_blck_rfp = fopen(cell_path.toStdString().c_str(), "rb");
			fread(cell_block, size * sizeof(unsigned int), 1, cell_blck_rfp);
			fclose(cell_blck_rfp);

			//if (x_p) {
			//	FILE *cell_blck_t = fopen(cell_x_p_Path.toStdString().c_str(), "rb");
			//	fread(cell_block_x_p, size * sizeof(unsigned int), 1, cell_blck_t);
			//	fclose(cell_blck_t);
			//}
			//if (x_m) {
			//	FILE *cell_blck_t = fopen(cell_x_m_Path.toStdString().c_str(), "rb");
			//	fread(cell_block_x_m, size * sizeof(unsigned int), 1, cell_blck_t);
			//	fclose(cell_blck_t);
			//}
			//if (y_p) {
			//	FILE *cell_blck_t = fopen(cell_y_p_Path.toStdString().c_str(), "rb");
			//	fread(cell_block_y_p, size * sizeof(unsigned int), 1, cell_blck_t);
			//	fclose(cell_blck_t);
			//}
			//if (y_m) {
			//	FILE *cell_blck_t = fopen(cell_y_m_Path.toStdString().c_str(), "rb");
			//	fread(cell_block_y_m, size * sizeof(unsigned int), 1, cell_blck_t);
			//	fclose(cell_blck_t);
			//}
			//if (z_p) {
			//	FILE *cell_blck_t = fopen(cell_z_p_Path.toStdString().c_str(), "rb");
			//	fread(cell_block_z_p, size * sizeof(unsigned int), 1, cell_blck_t);
			//	fclose(cell_blck_t);
			//}
			//if (z_m) {
			//	FILE *cell_blck_t = fopen(cell_z_m_Path.toStdString().c_str(), "rb");
			//	fread(cell_block_z_m, size * sizeof(unsigned int), 1, cell_blck_t);
			//	fclose(cell_blck_t);
			//}


			for (int z = 0; z < bs; z++) {
				for (int y = 0; y < bs; y++) {
					for (int x = 0; x < bs; x++) {
						int pos = z * bs * bs + y * bs + x;
						int cellN = cell_block[pos];
						if (cellN !=0) {
							volume[cellN]++;
		/*					int sum = 0;
							if (x - 1 >= 0) {
								if (cell_block[pos - 1] == cellN)sum++;
							}
							else {
								if (x_m) {
									if (cell_block_x_m[pos + bs - 1] == cellN)sum++;
								}
							}
							if (x + 1 < bs) {
								if (cell_block[pos + 1] == cellN)sum++;
							}
							else {
								if (x_p) {
									if (cell_block_x_p[pos - bs + 1] == cellN)sum++;
								}
							}
							if (y - 1 >= 0) {
								if (cell_block[pos - bs] == cellN)sum++;
							}
							else {
								if (y_m) {
									if (cell_block_y_m[pos + bs * (bs - 1)] == cellN)sum++;
								}
							}
							if (y + 1 < bs) {
								if (cell_block[pos + bs] == cellN)sum++;
							}
							else {
								if (y_p) {
									if (cell_block_y_p[pos - bs * (bs - 1)] == cellN)sum++;
								}
							}
							if (z - 1 >= 0) {
								if (cell_block[pos - bs * bs] == cellN)sum++;
							}
							else {
								if (z_m) {
									if (cell_block_z_m[pos + bs * bs * (bs - 1)] == cellN)sum++;
								}
							}
							if (z + 1 < bs) {
								if (cell_block[pos + bs * bs] == cellN)sum++;
							}
							else {
								if (z_p) {
									if (cell_block_z_p[pos - bs * bs * (bs - 1)] == cellN)sum++;
								}
							}
							if (sum == 6) {
								volume[cellN]++;
							}*/
						}
					}
				}
			}
		}
	}





	unsigned int *cell_block_over[8];
	for (int i = 0; i < 8; i++) {
		cell_block_over[i] = new unsigned int[size];
	}
	bool over[8];


	bool ck[2][2][2];

	int indexList[100];
	int indexN = 0;

	cnt = 0;
	loopsize = blocks.size();
	qDebug() << loopsize;
	foreach(QString filename, blocks) {
		qDebug() << (float)cnt++ / loopsize;

		QString cell_path = cellDirPath + filename;
		std::vector<std::string> str_vec = mUtil.Split(filename.toStdString().c_str(), "_");

		int baseX = (float)stoi(str_vec[2]) * bs;
		int baseY = (float)stoi(str_vec[1]) * bs;
		int baseZ = (float)stoi(str_vec[0]) * bs;
		QString cell_over_path[8];
		for (int o = 1; o < 8; o++) {
			cell_over_path[o]= cellDirPath + QString::number(stoi(str_vec[0])+o/4) + "_"
				+ QString::number(stoi(str_vec[1])+(o/2)%2) + "_" + QString::number(stoi(str_vec[2]) + o%2) + "_0.block";
			over[o] = false;
			if (QFileInfo::exists(cell_over_path[o]) && QFileInfo(cell_over_path[o]).isFile())over[o] = true;
		}

		if (QFileInfo::exists(cell_path) && QFileInfo(cell_path).isFile()) {
			FILE *cell_blck_rfp = fopen(cell_path.toStdString().c_str(), "rb");
			fread(cell_block, size * sizeof(unsigned int), 1, cell_blck_rfp);
			fclose(cell_blck_rfp);
			for (int o = 1; o < 8; o++) {
				if (over[o]) {
					FILE *cell_blck_t = fopen(cell_over_path[o].toStdString().c_str(), "rb");
					fread(cell_block_over[o], size * sizeof(unsigned int), 1, cell_blck_t);
					fclose(cell_blck_t);
				}
			}



			for (int z = 0; z < bs; z++) {
				for (int y = 0; y < bs; y++) {
					for (int x = 0; x < bs; x++) {

						indexN = 0;
						for (int dz = 0; dz < 2; dz++) {
							for (int dy = 0; dy < 2; dy++) {
								for (int dx = 0; dx < 2; dx++) {
									int cellPick = 0;
									int cx = x + dx;
									int cy = y + dy;
									int cz = z + dz;
									if (z + dz == bs)cz=0,cellPick += 4;
									if (y + dy == bs)cy=0,cellPick += 2;
									if (x + dx == bs)cx=0,cellPick += 1;
									int pos = cz * bs * bs + cy * bs + cx;

									int curI = cell_block[pos];
									if (cellPick != 0) {
										if (over[cellPick]) {
											curI = cell_block_over[cellPick][pos];
										}
										else continue;
									}
									if (curI != 0) {
										int q = 0;
										for (; q < indexN; q++) {
											if (indexList[q] == curI)break;

										}
										if (q == indexN) {
											indexList[indexN] = curI;
											indexN++;
										}
									}
								}
							}
						}

						for (int q = 0; q < indexN; q++) {
							for (int dz = 0; dz < 2; dz++) {
								for (int dy = 0; dy < 2; dy++) {
									for (int dx = 0; dx < 2; dx++) {
										int cellPick = 0;
										int cx = x + dx;
										int cy = y + dy;
										int cz = z + dz;
										if (z + dz == bs)cz = 0, cellPick += 4;
										if (y + dy == bs)cy = 0, cellPick += 2;
										if (x + dx == bs)cx = 0, cellPick += 1;
										int pos = cz * bs * bs + cy * bs + cx;

										int curI = cell_block[pos];
										if (cellPick != 0) {
											if (over[cellPick]) {
												curI = cell_block_over[cellPick][pos];
											}
											else curI=0;
										}
										if (curI != indexList[q]) {
											ck[dx][dy][dz] = false;
										}
										else ck[dx][dy][dz] = true;
									}
								}
							}
							surface[indexList[q]] += getSurfaceUsingMarchingCube(ck);
						}

					}
				}
			}
		}
	}
	delete[] cell_block;


	for (int i = 0; i < 8; i++) {
		delete[]cell_block_over[i];
	}


	for (int i = 1; i <= MaxCellN; i++) {
		if (surface[i] != 0) {
			float As, radius;
			radius = pow((3 / 12.566368)*volume[i], 1.0 / 3.0);
			As = 4 * M_PI * pow(radius, 2);
			sphericity[i] = As / surface[i];
		}
	}
	FILE *f1 = fopen((savePath.toStdString() + "volume.dat").c_str(), "w");
	FILE *f2 = fopen((savePath.toStdString() + "surfacearea.dat").c_str(), "w");
	FILE *f3 = fopen((savePath.toStdString() + "shpericity.dat").c_str(), "w");


	for (int i = 1; i <= MaxCellN; i++) {
		fprintf(f1, "%d %lf\n", i, volume[i]);
		fprintf(f2, "%d %lf\n", i, surface[i]);
		if(sphericity[i]>1.0)fprintf(f3, "%d 1.0\n", i);
		else fprintf(f3, "%d %lf\n", i, sphericity[i]);


	}
	fclose(f1);
	fclose(f2);
	fclose(f3);


	delete[]volume;
	delete[]surface;
	delete[]sphericity;

}

QVector2D DataCellAnalytics::getVolumeSurface(std::vector<QVector3D> points) {
	std::vector<QVector3D> tri; //QVector3D: index of each vertex of triangle
	bool *check = new bool[points.size()]();
	QVector2D startP;
	double length = 999999;
	for (int i = 0; i < points.size(); i++) {
		for (int j = i + 1; j < points.size(); j++) {
			double t = (points[i] - points[j]).length();
			if (t < length) {
				length = t;
				startP.setX(i);
				startP.setY(j);
			}
		}
	}
	check[int(startP.x())] = true;
	check[int(startP.y())] = true;
	std::vector<QVector2D> edgeQueue;
	edgeQueue.push_back(startP);
	edgeQueue.push_back(startP);

	while (edgeQueue.size() > 0) {
		std::vector<QVector2D>::iterator itt = edgeQueue.end();
		itt--;
		QVector2D cur = *itt;
		edgeQueue.erase(itt);
		int nextP = -1;
		double len = 999999;
		for (int i = 0; i < points.size(); i++) {

			if (!check[i]) {
				QVector3D first = points[int(cur.x())] - points[i];
				QVector3D second = points[int(cur.y())] - points[i];
				//if (second.x() == 0 || second.y() == 0 || second.z() == 0)continue;
				QVector3D slop = first / second;
				if (abs(slop.x() - slop.y())<0.0001 && abs(slop.y() - slop.z())<0.0001)continue;
				int j = cur.x();
				double t = first.length() + second.length();
				if (t < len) {
					len = t;
					nextP = i;
				}
			}
		}
		if (nextP != -1) {
			edgeQueue.push_back(QVector2D(cur.x(), nextP));
			edgeQueue.push_back(QVector2D(cur.y(), nextP));
			check[nextP] = true;
			tri.push_back(QVector3D(cur.x(), cur.y(), nextP));
		}
	}
	qDebug() << tri.size();

	delete[]check;

	double volume = 0;
	double surface = 0;
	for (int i = 0; i < tri.size(); i++) {
		double a = (points[int(tri[i].x())] - points[int(tri[i].y())]).length();
		double b = (points[int(tri[i].x())] - points[int(tri[i].z())]).length();
		double c = (points[int(tri[i].z())] - points[int(tri[i].y())]).length();
		double s = (a + b + c) / 2;
		surface += sqrt(s*(s - a)*(s - b)*(s - c));
	}
	qDebug() << surface;

	return QVector2D(volume, surface);
}

void DataCellAnalytics::EssentricityCaculation(QString cellDirPath, QString savePath, int StartCellN,int EndCellN,int MaxCellN) {
	std::vector <coord> *points = new std::vector<coord>[MaxCellN + 1];
	qDebug() << "start cell feature calculation";

	Utils mUtil;
	QDir directory(cellDirPath);
	QStringList target;
	target << "*_0.block";
	QStringList blocks = directory.entryList(target, QDir::Files);
	int bs = 512;
	long long size = bs * bs * bs;
	unsigned int *cell_block = new unsigned int[size];

	qDebug() << "finished init";

	int cnt = 0;
	int loopsize = blocks.size();
	qDebug() << loopsize;
	foreach(QString filename, blocks) {
		qDebug() << (float)cnt++ / loopsize;

		QString cell_path = cellDirPath + filename;
		std::vector<std::string> str_vec = mUtil.Split(filename.toStdString().c_str(), "_");

		int baseX = (float)stoi(str_vec[2]) * bs;
		int baseY = (float)stoi(str_vec[1]) * bs;
		int baseZ = (float)stoi(str_vec[0]) * bs;


		if (QFileInfo::exists(cell_path) && QFileInfo(cell_path).isFile()) {
			FILE *cell_blck_rfp = fopen(cell_path.toStdString().c_str(), "rb");

			fread(cell_block, size * sizeof(unsigned int), 1, cell_blck_rfp);
			fclose(cell_blck_rfp);


			for (int z = 0; z < bs; z++) {
				for (int y = 0; y < bs; y++) {
					for (int x = 0; x < bs; x++) {
						int pos = z * bs * bs + y * bs + x;
						int cellN = cell_block[pos];
						if (cellN >= StartCellN && cellN<=EndCellN) {
							coord t;
							t.x = baseX + x;
							t.y = baseY + y;
							t.z = baseZ + z;
							points[cellN].push_back(t);
							
						}
					}
				}
			}
		}
	}

	delete[] cell_block;

	FILE *f1 = fopen((savePath.toStdString() + "eccentricity_"+QString::number(StartCellN).toStdString().c_str()+"_"
		+ QString::number(EndCellN).toStdString().c_str()+".dat").c_str(), "w");
	for (int i = StartCellN; i <= EndCellN; i++) {
		float t = getEccentricity(points[i]);
		if(t>=0 && t<=1)
			fprintf(f1, "%d %lf\n", i, t);
		else fprintf(f1, "%d 0.0\n", i);
	}
	fclose(f1);
	delete[]points;

}
void DataCellAnalytics::CellFeatureCalculation(QString emDirPath,QString cellDirPath,QString savePath,int MaxCellN) {
	qDebug() << "start cell feature calculation";

	Utils mUtil;
	QDir directory(cellDirPath);
	QStringList target;
	target << "*_0.block";
	QStringList blocks = directory.entryList(target, QDir::Files);
	int bs = 512;
	long long size = bs * bs * bs;
	unsigned int *cell_block = new unsigned int[size];
	unsigned int *cell_block_x_p = new unsigned int[size];
	unsigned int *cell_block_x_m = new unsigned int[size];
	unsigned int *cell_block_y_p = new unsigned int[size];
	unsigned int *cell_block_y_m = new unsigned int[size];
	unsigned int *cell_block_z_p = new unsigned int[size];
	unsigned int *cell_block_z_m = new unsigned int[size];
	bool x_p, x_m, y_p, y_m, z_p, z_m;

	unsigned char *em_block = new unsigned char[size];


	long long *volume = new long long[MaxCellN + 1]();
	long long *surface = new long long[MaxCellN + 1]();
	double *sphericity=new double[MaxCellN + 1]();
	double *intensity= new double[MaxCellN + 1]();
	double *eccentricity=new double[MaxCellN + 1]();
	double *position_x = new double[MaxCellN + 1]();
	double *position_y = new double[MaxCellN + 1]();
	double *position_z = new double[MaxCellN + 1]();


	qDebug() << "finished init";

	int cnt = 0;
	int loopsize = blocks.size();
	qDebug() << loopsize;
	foreach(QString filename, blocks) {
		qDebug() << (float)cnt++ / loopsize;

		QString cell_path = cellDirPath + filename;
		QString em_path = emDirPath + filename;
		std::vector<std::string> str_vec = mUtil.Split(filename.toStdString().c_str(), "_");

		int baseX = (float)stoi(str_vec[2]) * bs;
		int baseY = (float)stoi(str_vec[1]) * bs;
		int baseZ = (float)stoi(str_vec[0]) * bs;

		QString cell_x_p_Path = cellDirPath + QString::number(stoi(str_vec[0])) + "_"
			+ QString::number(stoi(str_vec[1])) + "_" + QString::number(stoi(str_vec[2])+1) + "_0.block";
		QString cell_x_m_Path = cellDirPath + QString::number(stoi(str_vec[0])) + "_"
			+ QString::number(stoi(str_vec[1])) + "_" + QString::number(stoi(str_vec[2])-1) + "_0.block";
		QString cell_y_p_Path = cellDirPath + QString::number(stoi(str_vec[0])) + "_"
			+ QString::number(stoi(str_vec[1])+1) + "_" + QString::number(stoi(str_vec[2])) + "_0.block";
		QString cell_y_m_Path = cellDirPath + QString::number(stoi(str_vec[0])) + "_"
			+ QString::number(stoi(str_vec[1])-1) + "_" + QString::number(stoi(str_vec[2])) + "_0.block";
		QString cell_z_p_Path = cellDirPath + QString::number(stoi(str_vec[0])+1) + "_"
			+ QString::number(stoi(str_vec[1])) + "_" + QString::number(stoi(str_vec[2])) + "_0.block";
		QString cell_z_m_Path = cellDirPath + QString::number(stoi(str_vec[0])-1) + "_"
			+ QString::number(stoi(str_vec[1])) + "_" + QString::number(stoi(str_vec[2])) + "_0.block";
		x_p = false;
		x_m = false;
		y_p = false;
		y_m = false;
		z_p = false;
		z_m = false;
		if (QFileInfo::exists(cell_x_p_Path) && QFileInfo(cell_x_p_Path).isFile())x_p = true;
		if (QFileInfo::exists(cell_x_m_Path) && QFileInfo(cell_x_m_Path).isFile())x_m = true;
		if (QFileInfo::exists(cell_y_p_Path) && QFileInfo(cell_y_p_Path).isFile())y_p = true;
		if (QFileInfo::exists(cell_y_m_Path) && QFileInfo(cell_x_p_Path).isFile())y_m = true;
		if (QFileInfo::exists(cell_z_p_Path) && QFileInfo(cell_z_p_Path).isFile())z_p = true;
		if (QFileInfo::exists(cell_z_m_Path) && QFileInfo(cell_z_m_Path).isFile())z_m = true;



		if (QFileInfo::exists(cell_path) && QFileInfo(cell_path).isFile()) {
			FILE *cell_blck_rfp = fopen(cell_path.toStdString().c_str(), "rb");
			FILE *em_blck_rfp = fopen(em_path.toStdString().c_str(), "rb");
			fread(cell_block, size * sizeof(unsigned int), 1, cell_blck_rfp);
			fread(em_block, size, 1, em_blck_rfp);
			fclose(cell_blck_rfp);
			fclose(em_blck_rfp);

			if (x_p) {
				FILE *cell_blck_t = fopen(cell_x_p_Path.toStdString().c_str(), "rb");
				fread(cell_block_x_p, size * sizeof(unsigned int), 1, cell_blck_t);
				fclose(cell_blck_t);
			}
			if (x_m) {
				FILE *cell_blck_t = fopen(cell_x_m_Path.toStdString().c_str(), "rb");
				fread(cell_block_x_m, size * sizeof(unsigned int), 1, cell_blck_t);
				fclose(cell_blck_t);
			}
			if (y_p) {
				FILE *cell_blck_t = fopen(cell_y_p_Path.toStdString().c_str(), "rb");
				fread(cell_block_y_p, size * sizeof(unsigned int), 1, cell_blck_t);
				fclose(cell_blck_t);
			}			
			if (y_m) {
				FILE *cell_blck_t = fopen(cell_y_m_Path.toStdString().c_str(), "rb");
				fread(cell_block_y_m, size * sizeof(unsigned int), 1, cell_blck_t);
				fclose(cell_blck_t);
			}
			if (z_p) {
				FILE *cell_blck_t = fopen(cell_z_p_Path.toStdString().c_str(), "rb");
				fread(cell_block_z_p, size * sizeof(unsigned int), 1, cell_blck_t);
				fclose(cell_blck_t);
			}
			if (z_m) {
				FILE *cell_blck_t = fopen(cell_z_m_Path.toStdString().c_str(), "rb");
				fread(cell_block_z_m, size * sizeof(unsigned int), 1, cell_blck_t);
				fclose(cell_blck_t);
			}


			for (int z = 0; z < bs; z++) {
				for (int y = 0; y < bs; y++) {
					for (int x = 0; x < bs; x++) {
						int pos = z * bs * bs + y * bs + x;
						int cellN = cell_block[pos];
						if (cellN != 0) {
							volume[cellN]++;
							intensity[cellN] += em_block[pos];
							position_x[cellN] += x + baseX;
							position_y[cellN] += y + baseY;
							position_z[cellN] += z + baseZ;
							int sum = 0;
							if (x - 1 >= 0) {
								if (cell_block[pos - 1] == cellN)sum++;
							}
							else {
								if (x_m) {
									if (cell_block_x_m[pos + bs - 1] == cellN)sum++;
								}
							}
							if ( x + 1 < bs) {
								if (cell_block[pos + 1] == cellN)sum++;
							}
							else {
								if (x_p) {
									if (cell_block_x_p[pos-bs+1] == cellN)sum++;
								}
							}
							if (y - 1 >= 0) {
								if (cell_block[pos - bs] == cellN)sum++;
							}
							else {
								if (y_m) {
									if (cell_block_y_m[pos + bs*(bs-1)] == cellN)sum++;
								}
							}
							if (y + 1 < bs) {
								if (cell_block[pos + bs] == cellN)sum++;
							}
							else {
								if (y_p) {
									if (cell_block_y_p[pos - bs*(bs-1)] == cellN)sum++;
								}
							}
							if (z - 1 >= 0) {
								if (cell_block[pos - bs*bs] == cellN)sum++;
							}
							else {
								if (z_m) {
									if (cell_block_z_m[pos + bs * bs * (bs - 1)] == cellN)sum++;
								}
							}
							if (z + 1 < bs) {
								if (cell_block[pos + bs*bs] == cellN)sum++;
							}
							else {
								if (z_p) {
									if (cell_block_z_p[pos - bs * bs * (bs - 1)] == cellN)sum++;
								}
							}
							if (sum != 6)surface[cellN]+=6-sum;
						}
					}
				}
			}


		}
	}

	delete[] cell_block;
	delete[] em_block;

	for (int i = 1; i <= MaxCellN; i++) {
		if (surface != 0) {
			float As, radius;
			radius = pow((3 / 12.566368)*volume[i], 1.0 / 3.0);
			As = 4 * M_PI * pow(radius, 2);
			sphericity[i] = As / surface[i];
		}
		intensity[i] /= volume[i];
		position_x[i] /= volume[i];
		position_y[i] /= volume[i];
		position_z[i] /= volume[i];
	}
	FILE *f1 = fopen((savePath.toStdString()+"volume.dat").c_str(), "w");
	FILE *f2 = fopen((savePath.toStdString() + "surfacearea.dat").c_str(), "w");
	FILE *f3 = fopen((savePath.toStdString() + "sphericity.dat").c_str(), "w");
	FILE *f4 = fopen((savePath.toStdString() + "position_x.dat").c_str(), "w");
	FILE *f5 = fopen((savePath.toStdString() + "position_y.dat").c_str(), "w");
	FILE *f6 = fopen((savePath.toStdString() + "position_z.dat").c_str(), "w");
	FILE *f7 = fopen((savePath.toStdString() + "intensity.dat").c_str(), "w");

	for (int i = 1; i <= MaxCellN; ++i) {
		fprintf(f1, "%d %d\n", i, volume[i]);
		fprintf(f2, "%d %d\n", i, surface[i]);
		fprintf(f3, "%d %lf\n", i, sphericity[i]);
		fprintf(f4, "%d %lf\n", i, position_x[i]);
		fprintf(f5, "%d %lf\n", i, position_y[i]);
		fprintf(f6, "%d %lf\n", i, position_z[i]);
		fprintf(f7, "%d %lf\n", i, intensity[i]);

	}
	fclose(f1);
	fclose(f2);
	fclose(f3);
	fclose(f4);
	fclose(f5);
	fclose(f6);
	fclose(f7);

	delete[]volume;
	delete[]surface;
	delete[]sphericity;
	delete[]eccentricity;
	delete[]position_x;
	delete[]position_y;
	delete[]position_z;
	delete[]intensity;




}


float DataCellAnalytics::getEccentricity(std::vector<coord> points)
{
	if (points.size() < 4)
	{
		return 0;
	}
	float covarianceMatrix[3][3];
	double means[3] = { 0, 0, 0 };
	for (int i = 0; i < points.size(); i++)
	{
		means[0] += points[i].x,
			means[1] += points[i].y,
			means[2] += points[i].z;
	}
	means[0] /= points.size(), means[1] /= points.size(), means[2] /= points.size();

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			covarianceMatrix[i][j] = 0.0;
			for (int k = 0; k < points.size(); k++)
			{
				if (i == 0 && j == 0)
					covarianceMatrix[i][j] += (means[i] - points[k].x) * (means[j] - points[k].x);
				else if (i == 0 && j == 1)
					covarianceMatrix[i][j] += (means[i] - points[k].x) * (means[j] - points[k].y);
				else if (i == 0 && j == 2)
					covarianceMatrix[i][j] += (means[i] - points[k].x) * (means[j] - points[k].z);
				else if (i == 1 && j == 0)
					covarianceMatrix[i][j] += (means[i] - points[k].y) * (means[j] - points[k].x);
				else if (i == 1 && j == 1)
					covarianceMatrix[i][j] += (means[i] - points[k].y) * (means[j] - points[k].y);
				else if (i == 1 && j == 2)
					covarianceMatrix[i][j] += (means[i] - points[k].y) * (means[j] - points[k].z);
				else if (i == 2 && j == 0)
					covarianceMatrix[i][j] += (means[i] - points[k].z) * (means[j] - points[k].x);
				else if (i == 2 && j == 1)
					covarianceMatrix[i][j] += (means[i] - points[k].z) * (means[j] - points[k].y);
				else
					covarianceMatrix[i][j] += (means[i] - points[k].z) * (means[j] - points[k].z);
			}
			covarianceMatrix[i][j] /= points.size() - 1;
		}
	}
	Eigen::MatrixXd Mat(3, 3);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Mat(i, j) = covarianceMatrix[i][j];
		}
	}
	Eigen::EigenSolver<Eigen::MatrixXd> es(Mat);
	std::vector<float> eigenvals;
	float sum = 0;
	for (int i = 0; i < 3; i++)
	{
		eigenvals.push_back(es.eigenvalues()[i].real());
		sum += eigenvals.at(i);

	}
	std::sort(eigenvals.begin(), eigenvals.end(), std::greater<int>());


	//cout << eigenvals[0] << " //// " <<  eigenvals[1] << " //// " <<  eigenvals[2] <<endl;
//	return (3*((eigenvals[1]/sum)+(eigenvals[2]/sum)))/2;
	return sqrt(1 - ((eigenvals[2] * eigenvals[2]) / (eigenvals[0] * eigenvals[0])));
}

