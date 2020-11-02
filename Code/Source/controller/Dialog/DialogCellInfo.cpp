#include "DialogCellInfo.h"



DialogCellInfo::DialogCellInfo(QWidget *parent) : QDialog(parent)
{
	pWidget = parent;
	cellTF = new transferFunction();
	cellVis = new ViewCellGLWidget();
	connect(cellTF, SIGNAL(edited()), cellVis, SLOT(update()));
	cellVis->tf = cellTF;

	setModal(true);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
	int nWidth = 1920 / 2;
	int nHeight = 1080 / 2;
	if (parent != NULL) {
		//setGeometry(parent->x() + parent->width() / 2 - nWidth / 2, parent->y() + parent->height() / 2 - nHeight / 2, nWidth, nHeight);
		setGeometry(0 + 1920 / 2 - nWidth / 2, 1080 / 2 - nHeight / 2, nWidth, nHeight);
	}
	else {
		resize(nWidth, nHeight);
	}
	main_layout = new QHBoxLayout;
	main_layout->setMargin(5);
	this->setLayout(main_layout);
}


DialogCellInfo::~DialogCellInfo()
{
}



int DialogCellInfo::exec()
{
	qDebug("BG");
	activateWindow();
	this->show();
	return 1;
}
void DialogCellInfo::done(int val)
{
	qDebug("done.");

}

void DialogCellInfo::accept()
{
	qDebug("Accept.");
	// here!!!!!
	this->hide();
}
void DialogCellInfo::reject()
{
	emit thread_kill();

	qDebug("Reject.");
	this->hide();
}

void DialogCellInfo::setIndexAndDraw(unsigned int index) {
	mIndex = index;
	int feature_size = mGlobals.CurrentProject->mFeature.size();
	
	QFont title_font("Arial", 15, QFont::Bold);
	QFont name_font("Arial", 11, QFont::Normal);
	QFont value_font("Arial", 10, QFont::Normal);

	QWidget *container_widget = new QWidget;
	QVBoxLayout *master_layout = new QVBoxLayout;
	
	QLabel *title_label = new QLabel;
	title_label->setFont(title_font);
	title_label->setText("Cell Information");
	
	QHBoxLayout *information_layout = new QHBoxLayout;
	
	QVBoxLayout *data_layout0 = new QVBoxLayout;
	QLabel *name_label0 = new QLabel;
	QLabel *value_label0 = new QLabel;
	name_label0->setFont(name_font);
	name_label0->setText("Cell Index");
	value_label0->setFont(value_font);
	value_label0->setText(QString::fromStdString(std::to_string(mIndex)));
	data_layout0->addWidget(name_label0);
	data_layout0->addWidget(value_label0);
	information_layout->addLayout(data_layout0);

	int startIndex = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->featureStartIndex;
	int endIndex = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->featureEndIndex;

	for (int i = startIndex; i < endIndex; ++i) {


		int type = mGlobals.CurrentProject->mFeature[i].FeatureType;
		QString unit_str;

		double multi_value;
		if (type == 0 || mGlobals.CurrentProject->ResolutionUnitX == 3) {
			multi_value = 1;
		}
		else if (type == 1) {
			multi_value = (mGlobals.CurrentProject->ResolutionX * mGlobals.CurrentProject->ResolutionY * mGlobals.CurrentProject->ResolutionZ) / 3;
		}
		else if (type == 2) {
			multi_value = (mGlobals.CurrentProject->ResolutionX * mGlobals.CurrentProject->ResolutionY);
		}
		else if (type == 3) {
			multi_value = (mGlobals.CurrentProject->ResolutionX * mGlobals.CurrentProject->ResolutionY * mGlobals.CurrentProject->ResolutionZ);
		}

		if (type == 0) {
			unit_str = "";
		}
		else if (mGlobals.CurrentProject->ResolutionUnitX == 3) {
			unit_str = "voxels";

		}
		else if (type == 1) {
			if (mGlobals.CurrentProject->ResolutionUnitX == 0) {
				unit_str = mGlobals.gUnitList[1];
			}
			else if (mGlobals.CurrentProject->ResolutionUnitX == 1) {
				unit_str = mGlobals.gUnitList[4];
			}
			else if (mGlobals.CurrentProject->ResolutionUnitX == 2) {
				unit_str = mGlobals.gUnitList[7];
			}
		}
		else if (type == 2) {
			if (mGlobals.CurrentProject->ResolutionUnitX == 0) {
				unit_str = mGlobals.gUnitList[2];
			}
			else if (mGlobals.CurrentProject->ResolutionUnitX == 1) {
				unit_str = mGlobals.gUnitList[5];
			}
			else if (mGlobals.CurrentProject->ResolutionUnitX == 2) {
				unit_str = mGlobals.gUnitList[8];
			}
		}
		else if (type == 3) {
			if (mGlobals.CurrentProject->ResolutionUnitX == 0) {
				unit_str = mGlobals.gUnitList[3];
			}
			else if (mGlobals.CurrentProject->ResolutionUnitX == 1) {
				unit_str = mGlobals.gUnitList[6];
			}
			else if (mGlobals.CurrentProject->ResolutionUnitX == 2) {
				unit_str = mGlobals.gUnitList[9];
			}
		}

		QVBoxLayout *data_layout = new QVBoxLayout;
		QLabel *name_label = new QLabel;
		QLabel *value_label = new QLabel;
		name_label->setFont(name_font);
		name_label->setText(QString::fromStdString(mGlobals.CurrentProject->mFeature[i].FeatureName));
		value_label->setFont(value_font);
		value_label->setText(QString::fromStdString(std::to_string(mGlobals.CurrentProject->mFeature[i].FeatureMap[mIndex]* multi_value))+" "+ unit_str);
		data_layout->addWidget(name_label);
		data_layout->addWidget(value_label);
		information_layout->addLayout(data_layout);
	}
	QWidget *filler = new QWidget;
	filler->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	master_layout->addWidget(title_label);
	master_layout->addLayout(information_layout);
	//master_layout->addWidget(filler);

	qDebug() << "start cell vis";

	int listIndex = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[mIndex];
	cell curCell = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[listIndex];

	qDebug() << "listIndex: "<< listIndex;

	int xmin = curCell.minbox.x;
	int xmax = curCell.maxbox.x;
	int ymin = curCell.minbox.y;
	int ymax = curCell.maxbox.y;
	int zmin = curCell.minbox.z;
	int zmax = curCell.maxbox.z;
	xmin = (xmin < 4) ? 0 : xmin - 4;
	ymin = (ymin < 4) ? 0 : ymin - 4;
	zmin = (zmin < 4) ? 0 : zmin - 4;
	xmax = xmax + 4;
	ymax = ymax + 4;
	zmax = zmax + 4;
	while ((xmax - xmin) % 4 != 0 && xmin!=0) {
		xmin--;
	}
	while ((xmax - xmin) % 4 != 0) {
		xmax++;
	}
	qDebug() << "xmin: " << xmin;
	qDebug() << "xmax: " << xmax;
	qDebug() << "ymin: " << ymin;
	qDebug() << "ymax: " << ymax;
	qDebug() << "zmin: " << zmin;
	qDebug() << "zmax: " << zmax;

	int w = xmax - xmin;
	int h = ymax - ymin;
	int d = zmax - zmin;
	qDebug() << "w: " << w;
	qDebug() << "h: " << h;
	qDebug() << "d: " << d;

	int startXInd = xmin / 512;
	int endXInd = (xmax - 1) / 512;
	int startYInd = ymin / 512;
	int endYInd = (ymax - 1) / 512;
	int startZInd = zmin / 512;
	int endZInd = (zmax - 1) / 512;
	qDebug() << "startXInd: " << startXInd;
	qDebug() << "startYInd: " << startYInd;
	qDebug() << "startZInd: " << startZInd;
	qDebug() << "endXInd: " << endXInd;
	qDebug() << "endYInd: " << endYInd;
	qDebug() << "endZInd: " << endZInd;

	std::string labelPath = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->DataPathXY;
	std::string emPath = mGlobals.CurrentProject->mLayerBack->DataPathXY;
	qDebug() << "labelPath: " << labelPath.c_str();
	qDebug() << "emPath: " << emPath.c_str();

	unsigned char *volume = new unsigned char[w*h*d]();
	unsigned int *label = new unsigned int[w*h*d]();

	for (int iz = startZInd; iz <= endZInd; iz++) {
		for (int iy = startYInd; iy <= endYInd; iy++) {
			for (int ix = startXInd; ix <= endXInd; ix++) {
				int startXCoord = xmin - ix * 512;
				if (startXCoord < 0)startXCoord = 0;
				int startYCoord = ymin - iy * 512;
				if (startYCoord < 0)startYCoord = 0;
				int startZCoord = zmin - iz * 512;
				if (startZCoord < 0)startZCoord = 0;

				int endXCoord = xmax - ix * 512;
				if (endXCoord > 512)endXCoord = 512;
				int endYCoord = ymax - iy * 512;
				if (endYCoord > 512)endYCoord = 512;
				int endZCoord = zmax - iz * 512;
				if (endZCoord > 512)endZCoord = 512;

				char fileName[200];
				unsigned char *volume_block = new unsigned char[512 * 512 * 512];
				sprintf(fileName, "%s%d_%d_%d_0.block", emPath.c_str(), iz, iy, ix);
				qDebug() << "volume fileName: " << fileName;

				FILE *f = fopen(fileName, "rb");
				fread(volume_block, 1, 512 * 512 * 512, f);
				fclose(f);


				unsigned int *label_block = new unsigned int[512 * 512 * 512];
				sprintf(fileName, "%s%d_%d_%d_0.block", labelPath.c_str(), iz, iy, ix);
				qDebug() << "label fileName: " << fileName;

				f = fopen(fileName, "rb");
				if (f != NULL) {
					fread(label_block, 4, 512 * 512 * 512, f);
					fclose(f);
				}


				for (int bz = startZCoord; bz < endZCoord; bz++) {
					for (int by = startYCoord; by < endYCoord; by++) {
						for (int bx = startXCoord; bx < endXCoord; bx++) {
							int global_coord = bz * 512 * 512 + by * 512 + bx;
							int local_coord = (bz + iz * 512 - zmin)*w*h + (by + iy * 512 - ymin)*w + (bx + ix * 512 - xmin);
							//if (local_coord >= w*h*d) {
							//	qDebug() << bx << " " << by << " " << bz;
							//	qDebug() << bx + ix * 512 - xmin << " " << by + iy * 512 - ymin << " " << bz + iz * 512 - zmin;
							//	continue;
							//}
							//if (global_coord >= 512*512*512) {
							//	qDebug() << bx << " " << by << " " << bz;
							//	qDebug() << bx + ix * 512 - xmin << " " << by + iy * 512 - ymin << " " << bz + iz * 512 - zmin;
							//	continue;
							//}
							volume[local_coord] = volume_block[global_coord];
							label[local_coord] = label_block[global_coord];
						}
					}
				}
				delete[]volume_block;
				delete[]label_block;

			}
		}
	}
	qDebug() << "end data init";

	cellVis->setVolumeData(w,h,d,volume,label,mIndex);
	qDebug() << "end setVolumeData";

	master_layout->addWidget(cellVis);
	master_layout->addWidget(cellTF);
	cellVis->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	cellTF->setFixedHeight(100);

	delete[]label;
	qDebug() << "end cell vis";


	container_widget->setLayout(master_layout);
	QLayoutItem* item;
	while ((item = main_layout->takeAt(0)) != NULL)
	{
		delete item->widget();
		delete item;
	}
	main_layout->addWidget(container_widget);

}
