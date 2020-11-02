#include "DataProject.h"



DataProject::DataProject()
{
	SelectedColor = QColor(55, 55, 55);
	UnSelectedColor = QColor(125, 11, 11);
	QString a;
	UnitList.append("nm");

	a = QChar(0x00B5);
	a += "m";
	UnitList.append(a);


	UnitList.append("mm");

	UnitList.append("voxels");
	

}
DataProject::~DataProject()
{
}
void DataProject::NewProject() {
	ProjectName = "";
	ProjectPath = "";
	DataSizeX = 0;
	DataSizeY = 0;
	DataSizeZ = 0;
	DataBlockSize = 0;
	DataLevel = 0;
	ViewPos_X = DataSizeX / 2;
	ViewPos_Y = DataSizeY / 2;
	ViewPos_Z = DataSizeZ / 2;
	ViewZoomLevel = 1;

	emit project_on();
}

void DataProject::ProjectMake(QString aProjName, QString aProjPath, QString aLayerBGPath, QString aLayerLBPath, QString aSubregionsPath,QString aFeaturesPath) {
	ProjectName = aProjName;
	ProjectPath = aProjPath;

	SubregionPath = aSubregionsPath;
	FeaturePath = aFeaturesPath;

	mLayerBack = new LayerBackground(aLayerBGPath.toStdString());
	//mLayerCell[maxLayerCell] = new LayerCell(aLayerLBPath.toStdString());
	
	//maxLayerCell++;
	//curLayerCell = 0;

	ProjectStatus = true;
	ProjectSizeLoad();
	ProjectSave(ProjectPath);

}
void DataProject::ProjectOpen(QString aProjPath) {
	Utils mUtil;

	mIFS.open(aProjPath.toStdString());
	std::string line1;
	std::getline(mIFS, line1);
	std::vector<std::string> list_line1 = mUtil.Split(line1.c_str(), " : ");
	ProjectName = QString::fromStdString(list_line1.back());
	qDebug() << ProjectName;

	std::string line2;
	std::getline(mIFS, line2);
	std::vector<std::string> list_line2 = mUtil.Split(line2.c_str(), " : ");
	ProjectPath = QString::fromStdString(list_line2.back());
	qDebug() << ProjectPath;

	std::string line3;
	std::getline(mIFS, line3);
	std::vector<std::string> list_line3 = mUtil.Split(line3.c_str(), " : ");
	QString aLayerBGPath = QString::fromStdString(list_line3.back());
	qDebug() << aLayerBGPath;
	mLayerBack = new LayerBackground(aLayerBGPath.toStdString());
	

	std::string line4;
	std::getline(mIFS, line4);
	std::vector<std::string> list_line4 = mUtil.Split(line4.c_str(), " : ");
	QString aLayerLBPath = QString::fromStdString(list_line4.back());
	qDebug() << aLayerLBPath;
	//mLayerCell[maxLayerCell] = new LayerCell(aLayerLBPath.toStdString());
	//maxLayerCell++;
	//curLayerCell = 0;


	std::string line5;
	std::getline(mIFS, line5);
	std::vector<std::string> list_line5 = mUtil.Split(line5.c_str(), " : ");
	SubregionPath = QString::fromStdString(list_line5.back());
	qDebug() << SubregionPath;

	std::string line6;
	std::getline(mIFS, line6);
	std::vector<std::string> list_line6 = mUtil.Split(line6.c_str(), " : ");
	FeaturePath = QString::fromStdString(list_line6.back());
	qDebug() << FeaturePath;

	//
	//AddFeature(QString::fromStdString(mLayerCell->CellPath) + "MorphologicalFeature/volume.dat");
	//AddFeature(QString::fromStdString(mLayerCell->CellPath) + "MorphologicalFeature/surfacearea.dat");
	//AddFeature(QString::fromStdString(mLayerCell->CellPath) + "MorphologicalFeature/sphericity.dat");
	//AddFeature(QString::fromStdString(mLayerCell->CellPath) + "MorphologicalFeature/position_z.dat");
	//AddFeature(QString::fromStdString(mLayerCell->CellPath) + "MorphologicalFeature/position_y.dat");
	//AddFeature(QString::fromStdString(mLayerCell->CellPath) + "MorphologicalFeature/position_x.dat");
	//AddFeature(QString::fromStdString(mLayerCell->CellPath) + "MorphologicalFeature/intensity.dat");
	//AddFeature(QString::fromStdString(mLayerCell->CellPath) + "MorphologicalFeature/eccentricity.dat");


	ProjectStatus = true;
	ProjectSizeLoad();
//	mGlobals.mainWindow->mContents->loadSubregionFeature();

}
void DataProject::ProjectSave(QString aProjPath) {
	std::ofstream mOFS(aProjPath.toStdString());

	std::string proj_name = "#project name : " + ProjectName.toStdString();
	std::string proj_path = "#project path : " + ProjectPath.toStdString();
	std::string layerbackground = "#layer background path : " + mLayerBack->BackgroundHeaderPath;
	std::string LayerCell = "#layer labeled path : " + mLayerCell[curLayerCell]->CellHeaderPath;
	std::string sub_path = "#subregions path : " + SubregionPath.toStdString();
	std::string feat_path = "#features path : " + FeaturePath.toStdString();

	//std::string analytics_path = "#analytics result path : " + AnalyticsResultPath.toStdString();

	mOFS << proj_name << std::endl;
	mOFS << proj_path << std::endl;
	mOFS << layerbackground << std::endl;
	mOFS << LayerCell << std::endl;
	mOFS << sub_path << std::endl;
	mOFS << feat_path << std::endl;

	//mOFS << analytics_path << std::endl;
	
	mOFS.close();
}

void DataProject::ProjectSizeLoad() {
	Utils mUtil;
	std::ifstream tempIfs(mLayerBack->BackgroundHeaderPath, std::ifstream::binary);
	std::string line1;
	std::getline(tempIfs, line1);
	std::vector<std::string> list_line1 = mUtil.Split(line1.c_str(), ":");
	DataSizeX = atoi(list_line1.back().c_str());

	std::string line2;
	std::getline(tempIfs, line2);
	std::vector<std::string> list_line2 = mUtil.Split(line2.c_str(), ":");
	DataSizeY = atoi(list_line2.back().c_str());

	std::string line3;
	std::getline(tempIfs, line3);
	std::vector<std::string> list_line3 = mUtil.Split(line3.c_str(), ":");
	DataSizeZ = atoi(list_line3.back().c_str());

	std::string line4;
	std::getline(tempIfs, line4);
	std::vector<std::string> list_line4 = mUtil.Split(line4.c_str(), ":");
	DataBlockSize = atoi(list_line4.back().c_str());

	std::string line5;
	std::getline(tempIfs, line5);
	std::vector<std::string> list_line5 = mUtil.Split(line5.c_str(), " : ");
	DataLevel = atoi(list_line5.back().c_str());

	tempIfs.close();

	ViewPos_X = DataSizeX / 2;
	ViewPos_Y = DataSizeY / 2;
	ViewPos_Z = DataSizeZ / 2;
	ViewZoomLevel = 1;
	
	emit project_on();
}

void DataProject::AddFeature(QString aPath, int aType) {
	std::string path = aPath.toStdString();
	Utils mUtil;
	std::vector<std::string> list_line1 = mUtil.Split(path.c_str(), "/");
	std::vector<std::string> list_line2 = mUtil.Split(list_line1.back().c_str(), ".dat");
	std::string name = list_line2.front();

	if (name == "volume") {
		aType = 3;
	}
	else if (name == "surfacearea") {
		aType = 2;
	}
	DataFeature temp(name, path, aType);
	mFeature.push_back(temp);
}

void DataProject::removeFeature(int index) {
	int step = 0;
	std::vector<DataFeature>::iterator iter;
	for (iter = this->mFeature.begin(); iter != this->mFeature.end(); ++iter) {
		if (step == index) {
			this->mFeature.erase(iter);
			break;
		}
	}
	step++;
}

int DataProject::AddSubregion(QString aPath) {
	std::string path = aPath.toStdString();
	Utils mUtil;
	std::vector<std::string> list_line1 = mUtil.Split(path.c_str(), "/");
	std::string name = list_line1[list_line1.size()-2];
	LayerSubregion temp(name, path);
	mSubregion.push_back(temp);
	return mSubregion.size() - 1;
}

void DataProject::removeSubregion(int index) {
	std::vector<LayerSubregion>::iterator iter;
	for (iter = this->mSubregion.begin(); iter != this->mSubregion.end(); ++iter) {
		if (iter->SubregionIndex == index) {
			this->mSubregion.erase(iter);
			break;
		}
	}
	qDebug() << "removeSubregion";
}

int DataProject::getSerialIndex(int x, int y, int z, int lv) {
	Utils mUtil;
	if (x < 0 || y < 0 || z < 0) {
		return -1;
	}
	else {
		int count_sum = 0;
		int size_x;
		int size_y;
		int size_z;
		int x_count;
		int y_count;
		int z_count;

		for (int i = 0; i < lv; ++i) {
			mUtil.CalcXYZCount(&x_count, &y_count, &z_count, i, DataSizeX, DataSizeY, DataSizeZ, DataBlockSize);
			count_sum = x_count * y_count * z_count;
		}
		mUtil.CalcXYZCount(&x_count, &y_count, &z_count, lv, DataSizeX, DataSizeY, DataSizeZ, DataBlockSize);
		if (x < x_count && y < y_count && z < z_count)
			return count_sum + x_count * y_count*z + x_count * y + x;
		else
			return -1;
	}
}

