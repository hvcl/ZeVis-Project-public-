#include "Contents_ProjectInfo.h"



Contents_ProjectInfo::Contents_ProjectInfo(QWidget *parent)
	: QObject(parent)
{
	mWidget = parent;
	QVBoxLayout *layout = new QVBoxLayout;
	project_location_layout(layout);
	project_data_dim_layout(layout);
	project_resolution_layout(layout);
	project_subregion_layout(layout);


	QHBoxLayout *filler_layout = new QHBoxLayout;
	QWidget *filler = new QWidget;
	filler->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	filler_layout->addWidget(filler);

	layout->addLayout(filler_layout);
	mWidget->setLayout(layout);

}

Contents_ProjectInfo::~Contents_ProjectInfo()
{
}

Contents_ProjectInfo::Contents_ProjectInfo(QWidget *parent, QLayout *parent_layout)
	: QObject(parent)
{
	
}

void Contents_ProjectInfo::project_location_layout(QVBoxLayout *layout) {
	QHBoxLayout *proj_name_layout = new QHBoxLayout;
	QHBoxLayout *proj_path_layout = new QHBoxLayout;
	QHBoxLayout *proj_back_layout = new QHBoxLayout;
	QHBoxLayout *proj_cell_layout = new QHBoxLayout;
	
	QFont title_font("Arial", 11, QFont::Bold);
	QLabel *title_label = new QLabel(mWidget);
	title_label->setFont(title_font);
	title_label->setText("Project Information");
	layout->addWidget(title_label);


	QLabel *proj_name_label = new QLabel(mWidget);
	proj_name_label->setAlignment(Qt::AlignCenter);
	proj_name_label->setFixedSize(100, 30);
	proj_name_label->setText("Project Name");
	ProjectName = new QLineEdit(mWidget);
	//ProjectName->setReadOnly(true);
	//ProjectName->setText(mGlobals.CurrentProject->ProjectName);
	proj_name_layout->addWidget(proj_name_label);
	proj_name_layout->addWidget(ProjectName);
	connect(ProjectName, SIGNAL(textEdited(QString)), this, SLOT(handleSetProjectName(QString)));
	QLabel *proj_path_label = new QLabel(mWidget);
	proj_path_label->setAlignment(Qt::AlignCenter);
	proj_path_label->setFixedSize(100, 30);
	proj_path_label->setText("Project Path");
	ProjectPath = new QPushButton(mWidget);
	//ProjectPath->setReadOnly(true);
	//ProjectPath->setText(mGlobals.CurrentProject->ProjectPath);
	proj_path_layout->addWidget(proj_path_label);
	proj_path_layout->addWidget(ProjectPath);
	connect(ProjectPath, SIGNAL(pressed()), this, SLOT(handleSetProjectPath()));
	//proj_path_label->hide();
	//ProjectPath->hide();

	QLabel *proj_back_label = new QLabel(mWidget);
	proj_back_label->setAlignment(Qt::AlignCenter);
	proj_back_label->setFixedSize(100, 30);
	proj_back_label->setText("EM Image");
	BackPath = new QPushButton(mWidget);
	//BackPath->setReadOnly(true);
	//BackPath->setText(QString::fromStdString(mGlobals.CurrentProject->mLayerBack->BackgroundPath));
	proj_back_layout->addWidget(proj_back_label);
	proj_back_layout->addWidget(BackPath);
	connect(BackPath, SIGNAL(pressed()), this, SLOT(handleLoadEM()));


	QLabel *proj_cell_label = new QLabel(mWidget);
	proj_cell_label->setAlignment(Qt::AlignCenter);
	proj_cell_label->setFixedSize(100, 30);
	proj_cell_label->setText("Cell Path");
	CellPath = new QLineEdit(mWidget);
	CellPath->setReadOnly(true);
	//CellPath->setText(QString::fromStdString(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->CellPath));
	proj_cell_layout->addWidget(proj_cell_label);
	proj_cell_layout->addWidget(CellPath);
	proj_cell_label->hide();
	CellPath->hide();


	QFrame *buffer = new QFrame(mWidget);
	buffer->setFrameStyle(QFrame::HLine | QFrame::Plain);
	buffer->setStyleSheet("color:rgb(100,100,100)");
	buffer->setLineWidth(2.5);
	

	layout->addLayout(proj_name_layout);
	layout->addLayout(proj_path_layout);
	layout->addLayout(proj_back_layout);
	//layout->addLayout(proj_cell_layout);
	layout->addWidget(buffer);
	
}
void Contents_ProjectInfo::handleSetProjectPath() {
	QString filters("Project path (*.ZeVis);;");
	QString dir = QFileDialog::getSaveFileName(mWidget, "Project save path", QDir::currentPath(), filters);
	if (dir != "") {
		ProjectPath->setText(dir);
		mGlobals.CurrentProject->ProjectPath = dir;
		mGlobals.mainWindow->mTopToolbar->handleSaveStateProject();
	}
}

void Contents_ProjectInfo::handleSetProjectName(QString a) {
	mGlobals.CurrentProject->ProjectName = a;
}
void Contents_ProjectInfo::handleLoadEM() {
	QString filters("EM path (*.bgl);;");
	QString dir = QFileDialog::getOpenFileName(mWidget, "EM image path", QDir::currentPath(), filters);
	if (dir != "") {
		setLayerBackground(dir);
	}
}

void Contents_ProjectInfo::setLayerBackground(QString dir) {
	BackPath->setText(dir);
	Utils mUtil;
	std::ifstream tempIfs(dir.toStdString(), std::ifstream::binary);
	std::string line1;
	std::getline(tempIfs, line1);
	std::vector<std::string> list_line1 = mUtil.Split(line1.c_str(), ":");
	mGlobals.CurrentProject->DataSizeX = atoi(list_line1.back().c_str());

	std::string line2;
	std::getline(tempIfs, line2);
	std::vector<std::string> list_line2 = mUtil.Split(line2.c_str(), ":");
	mGlobals.CurrentProject->DataSizeY = atoi(list_line2.back().c_str());

	std::string line3;
	std::getline(tempIfs, line3);
	std::vector<std::string> list_line3 = mUtil.Split(line3.c_str(), ":");
	mGlobals.CurrentProject->DataSizeZ = atoi(list_line3.back().c_str());

	std::string line31;
	std::getline(tempIfs, line31);
	std::vector<std::string> list_line31 = mUtil.Split(line31.c_str(), ":");
	mGlobals.CurrentProject->ResolutionX = atof(list_line31.back().c_str());
	std::string line32;
	std::getline(tempIfs, line32);
	std::vector<std::string> list_line32 = mUtil.Split(line32.c_str(), ":");
	mGlobals.CurrentProject->ResolutionY = atof(list_line32.back().c_str());
	std::string line33;
	std::getline(tempIfs, line33);
	std::vector<std::string> list_line33 = mUtil.Split(line33.c_str(), ":");
	mGlobals.CurrentProject->ResolutionZ = atof(list_line33.back().c_str());

	mGlobals.CurrentProject->ResolutionXnm = mGlobals.CurrentProject->ResolutionX;
	mGlobals.CurrentProject->ResolutionYnm = mGlobals.CurrentProject->ResolutionY;
	mGlobals.CurrentProject->ResolutionZnm = mGlobals.CurrentProject->ResolutionZ;

	mGlobals.CurrentProject->ResolutionUnitX = 0;
	mGlobals.CurrentProject->ResolutionUnitY = 0;
	mGlobals.CurrentProject->ResolutionUnitZ = 0;


	std::string line4;
	std::getline(tempIfs, line4);
	std::vector<std::string> list_line4 = mUtil.Split(line4.c_str(), ":");
	mGlobals.CurrentProject->DataBlockSize = atoi(list_line4.back().c_str());

	std::string line5;
	std::getline(tempIfs, line5);
	std::vector<std::string> list_line5 = mUtil.Split(line5.c_str(), " : ");
	mGlobals.CurrentProject->DataLevel = atoi(list_line5.back().c_str());

	tempIfs.close();

	mGlobals.CurrentProject->ViewPos_X = mGlobals.CurrentProject->DataSizeX / 2;
	mGlobals.CurrentProject->ViewPos_Y = mGlobals.CurrentProject->DataSizeY / 2;
	mGlobals.CurrentProject->ViewPos_Z = mGlobals.CurrentProject->DataSizeZ / 2;
	mGlobals.CurrentProject->ViewZoomLevel = 1;

	DataDimX->setText(QString::fromStdString(std::to_string(mGlobals.CurrentProject->DataSizeX)));
	DataDimY->setText(QString::fromStdString(std::to_string(mGlobals.CurrentProject->DataSizeY)));
	DataDimZ->setText(QString::fromStdString(std::to_string(mGlobals.CurrentProject->DataSizeZ)));

	char t[20];
	sprintf(t, "%.3f", mGlobals.CurrentProject->ResolutionX);
	ResolutionX->setText(QString(t));
	sprintf(t, "%.3f", mGlobals.CurrentProject->ResolutionY);
	ResolutionY->setText(QString(t));
	sprintf(t, "%.3f", mGlobals.CurrentProject->ResolutionZ);
	ResolutionZ->setText(QString(t));

	ResolutionX_Unit->setText(mGlobals.CurrentProject->UnitList[0]);
	ResolutionY_Unit->setText(mGlobals.CurrentProject->UnitList[0]);
	ResolutionZ_Unit->setText(mGlobals.CurrentProject->UnitList[0]);


	ResolutionX->setDisabled(false);
	ResolutionY->setDisabled(false);
	ResolutionZ->setDisabled(false);

	Resolution_Unit->setCurrentIndex(0);


	mGlobals.CurrentProject->mLayerBack = new LayerBackground(dir.toStdString());


	mGlobals.CurrentProject->ProjectStatus = true;

	emit emLoaded();
}

void Contents_ProjectInfo::project_data_dim_layout(QVBoxLayout *layout) {
	QHBoxLayout *dim_x_layout = new QHBoxLayout;
	QHBoxLayout *dim_y_layout = new QHBoxLayout;
	QHBoxLayout *dim_z_layout = new QHBoxLayout;
		
	QFont  title_font("Arial", 11, QFont::Bold);
	QLabel *title_label = new QLabel(mWidget);
	title_label->setFont(title_font);
	title_label->setText("Project Data Dimension");
	layout->addWidget(title_label);


	QLabel *dim_x_label = new QLabel(mWidget);
	dim_x_label->setAlignment(Qt::AlignCenter);
	dim_x_label->setFixedSize(100, 30);
	dim_x_label->setText("X");
	DataDimX = new QLineEdit(mWidget);
	DataDimX->setReadOnly(true);
	DataDimX->setAlignment(Qt::AlignCenter);
	DataDimX->setText(QString::fromStdString(std::to_string(mGlobals.CurrentProject->DataSizeX)));
	QLabel *px_x_label = new QLabel(mWidget);
	px_x_label->setAlignment(Qt::AlignCenter);
	px_x_label->setFixedSize(50, 30);
	px_x_label->setText("px");
	dim_x_layout->addWidget(dim_x_label);
	dim_x_layout->addWidget(DataDimX);
	dim_x_layout->addWidget(px_x_label);
	

	QLabel *dim_y_label = new QLabel(mWidget);
	dim_y_label->setAlignment(Qt::AlignCenter);
	dim_y_label->setFixedSize(100, 30);
	dim_y_label->setText("Y");
	DataDimY = new QLineEdit(mWidget);
	DataDimY->setReadOnly(true);
	DataDimY->setAlignment(Qt::AlignCenter);
	DataDimY->setText(QString::fromStdString(std::to_string(mGlobals.CurrentProject->DataSizeY)));
	QLabel *px_y_label = new QLabel(mWidget);
	px_y_label->setAlignment(Qt::AlignCenter);
	px_y_label->setFixedSize(50, 30);
	px_y_label->setText("px");
	dim_y_layout->addWidget(dim_y_label);
	dim_y_layout->addWidget(DataDimY);
	dim_y_layout->addWidget(px_y_label);


	QLabel *dim_z_label = new QLabel(mWidget);
	dim_z_label->setAlignment(Qt::AlignCenter);
	dim_z_label->setFixedSize(100, 30);
	dim_z_label->setText("Z");
	DataDimZ = new QLineEdit(mWidget);
	DataDimZ->setReadOnly(true);
	DataDimZ->setAlignment(Qt::AlignCenter);
	DataDimZ->setText(QString::fromStdString(std::to_string(mGlobals.CurrentProject->DataSizeZ)));
	QLabel *px_z_label = new QLabel(mWidget);
	px_z_label->setAlignment(Qt::AlignCenter);
	px_z_label->setFixedSize(50, 30);
	px_z_label->setText("px");
	dim_z_layout->addWidget(dim_z_label);
	dim_z_layout->addWidget(DataDimZ);
	dim_z_layout->addWidget(px_z_label);

	QFrame *buffer = new QFrame(mWidget);
	buffer->setFrameStyle(QFrame::HLine | QFrame::Plain);
	buffer->setStyleSheet("color:rgb(100,100,100)");
	buffer->setLineWidth(2.5);

	layout->addLayout(dim_x_layout);
	layout->addLayout(dim_y_layout);
	layout->addLayout(dim_z_layout);
	layout->addWidget(buffer);

}
void Contents_ProjectInfo::project_resolution_layout(QVBoxLayout *layout) {
	QHBoxLayout *res_x_layout = new QHBoxLayout;
	QHBoxLayout *res_y_layout = new QHBoxLayout;
	QHBoxLayout *res_z_layout = new QHBoxLayout;
	QHBoxLayout *unit_layout = new QHBoxLayout;


	QFont  title_font("Arial", 11, QFont::Bold);
	QLabel *title_label = new QLabel(mWidget);
	title_label->setFont(title_font);
	title_label->setText("Project Resolution");
	layout->addWidget(title_label);

	QLabel *res_x_label = new QLabel(mWidget);
	res_x_label->setAlignment(Qt::AlignCenter);
	res_x_label->setFixedSize(100, 30);
	res_x_label->setText("X");

	char t[20];
	sprintf(t, "%.3f", mGlobals.CurrentProject->ResolutionX);


	ResolutionX = new QLineEdit(mWidget);
	ResolutionX->setAlignment(Qt::AlignCenter);
	ResolutionX->setText(QString(t));
	ResolutionX_Unit = new QLabel(mGlobals.CurrentProject->UnitList[mGlobals.CurrentProject->ResolutionUnitX]);
	ResolutionX_Unit->setAlignment(Qt::AlignCenter);
	ResolutionX_Unit->setFixedWidth(50);

	res_x_layout->addWidget(res_x_label);
	res_x_layout->addWidget(ResolutionX);
	res_x_layout->addWidget(ResolutionX_Unit);
	
	
	QLabel *res_y_label = new QLabel(mWidget);
	res_y_label->setAlignment(Qt::AlignCenter);
	res_y_label->setFixedSize(100, 30);
	res_y_label->setText("Y");
	
	
	sprintf(t, "%.3f", mGlobals.CurrentProject->ResolutionY);

	ResolutionY = new QLineEdit(mWidget);
	ResolutionY->setAlignment(Qt::AlignCenter);
	ResolutionY->setText(QString(t));
	ResolutionY_Unit = new QLabel(mGlobals.CurrentProject->UnitList[mGlobals.CurrentProject->ResolutionUnitX]);
	ResolutionY_Unit->setAlignment(Qt::AlignCenter);
	ResolutionY_Unit->setFixedWidth(50);
	//ResolutionY_Unit->setDisabled(true);

	res_y_layout->addWidget(res_y_label);
	res_y_layout->addWidget(ResolutionY);
	res_y_layout->addWidget(ResolutionY_Unit);

	QLabel *res_z_label = new QLabel(mWidget);
	res_z_label->setAlignment(Qt::AlignCenter);
	res_z_label->setFixedSize(100, 30);
	res_z_label->setText("Z");

	sprintf(t, "%.3f", mGlobals.CurrentProject->ResolutionZ);

	ResolutionZ = new QLineEdit(mWidget);
	ResolutionZ->setAlignment(Qt::AlignCenter);
	ResolutionZ->setText(QString(t));
	ResolutionZ_Unit = new QLabel(mGlobals.CurrentProject->UnitList[mGlobals.CurrentProject->ResolutionUnitX]);
	ResolutionZ_Unit->setAlignment(Qt::AlignCenter);
	ResolutionZ_Unit->setFixedWidth(50);
	//ResolutionZ_Unit->setDisabled(true);

	res_z_layout->addWidget(res_z_label);
	res_z_layout->addWidget(ResolutionZ);
	res_z_layout->addWidget(ResolutionZ_Unit);

	QLabel *set_unit_label = new QLabel("Unit");
	set_unit_label->setAlignment(Qt::AlignCenter);
	set_unit_label->setFixedSize(100, 30);


	Resolution_Unit = new QComboBox;
//	Resolution_Unit->setFixedWidth(50);
	Resolution_Unit->addItems(mGlobals.CurrentProject->UnitList);
	Resolution_Unit->setEditable(true);
	Resolution_Unit->lineEdit()->setReadOnly(true);
	Resolution_Unit->lineEdit()->setAlignment(Qt::AlignCenter);
	for (int i = 0; i < Resolution_Unit->count(); ++i) {
		Resolution_Unit->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
	}
	Resolution_Unit->setFixedWidth(200);
	connect(Resolution_Unit, SIGNAL(currentIndexChanged(int)), this, SLOT(unit_change(int)));

	prevResoluiton_Unit = 0;

	unit_layout->addWidget(set_unit_label);
	unit_layout->addWidget(Resolution_Unit);

	QFrame *buffer = new QFrame(mWidget);
	buffer->setFrameStyle(QFrame::HLine | QFrame::Plain);
	buffer->setStyleSheet("color:rgb(100,100,100)");
	buffer->setLineWidth(2.5);

	QPushButton *modifybtn = new QPushButton;
	modifybtn->setText("Apply");
	connect(modifybtn, SIGNAL(released()), this, SLOT(handleModifyBtn()));


	layout->addLayout(res_x_layout);
	layout->addLayout(res_y_layout);
	layout->addLayout(res_z_layout);
	layout->addSpacing(10);
	layout->addLayout(unit_layout);
	//layout->addWidget(modifybtn);
	layout->addSpacing(10);

	
	layout->addWidget(buffer);
}

void Contents_ProjectInfo::project_subregion_layout(QVBoxLayout *layout) {
	

}

void Contents_ProjectInfo::unit_change(int index) {
	qDebug("UnitChange");

	if (index == 3) {
		ResolutionX->setDisabled(true);
		ResolutionY->setDisabled(true);
		ResolutionZ->setDisabled(true);
	}
	else {
		ResolutionX->setDisabled(false);
		ResolutionY->setDisabled(false);
		ResolutionZ->setDisabled(false);
		double tX = std::stod(ResolutionX->text().toStdString());
		ResolutionX->setText(QString::number(tX*pow(1000,  prevResoluiton_Unit- index)));
		double tY = std::stod(ResolutionY->text().toStdString());
		ResolutionY->setText(QString::number(tY*pow(1000, prevResoluiton_Unit-index)));
		double tZ = std::stod(ResolutionZ->text().toStdString());
		ResolutionZ->setText(QString::number(tZ*pow(1000, prevResoluiton_Unit - index)));
		prevResoluiton_Unit = index;
	}

	mGlobals.CurrentProject->ResolutionUnitX = index;
	mGlobals.CurrentProject->ResolutionUnitY = index;
	mGlobals.CurrentProject->ResolutionUnitZ = index;

	ResolutionX_Unit->setText(mGlobals.CurrentProject->UnitList[index]);
	ResolutionY_Unit->setText(mGlobals.CurrentProject->UnitList[index]);
	ResolutionZ_Unit->setText(mGlobals.CurrentProject->UnitList[index]);

	mGlobals.CurrentProject->ResolutionX = std::stod(ResolutionX->text().toStdString());
	mGlobals.CurrentProject->ResolutionY = std::stod(ResolutionY->text().toStdString());
	mGlobals.CurrentProject->ResolutionZ = std::stod(ResolutionZ->text().toStdString());

	foreach(QGraphicsItem *ptr, mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->items()) {
		if (ptr->type() == QNEBlock::Type) {
			QNEBlock *temp = (QNEBlock *)ptr;
			if (temp->mBlockFlags == QNEBlock::SubregionBlock || temp->mBlockFlags == QNEBlock::CutPlaneBlock) {
				double volume = temp->mBlock->SubregionVolume;
				double multi_value = (mGlobals.CurrentProject->ResolutionX * mGlobals.CurrentProject->ResolutionY * mGlobals.CurrentProject->ResolutionZ);
				if (mGlobals.CurrentProject->ResolutionUnitX != 3)volume *= multi_value;

				//subreigon_precision->setText("level " + QString::fromStdString(std::to_string(SubregionPrecision)));


				std::stringstream ss;
				ss << std::fixed << std::setprecision(4) << volume;
				std::string mystring = ss.str();
				temp->mBlock->subreigon_volume->setText(QString::fromStdString(mystring));
				//	subreigon_volume[SubregionPrecision-1]->setText("level " + QString::number(SubregionPrecision) + ": " + QString::fromStdString(mystring));


				QString unit_str;
				if (mGlobals.CurrentProject->ResolutionUnitX == 0) {
					unit_str = mGlobals.gUnitList[3];
				}
				else if (mGlobals.CurrentProject->ResolutionUnitX == 1) {
					unit_str = mGlobals.gUnitList[6];
				}
				else if (mGlobals.CurrentProject->ResolutionUnitX == 2) {
					unit_str = mGlobals.gUnitList[9];
				}
				else if (mGlobals.CurrentProject->ResolutionUnitX == 3) {
					unit_str = "voxels";
				}

				temp->mBlock->subregion_volume_unit->setText(unit_str);
				//temp->mBlock->handleSubregionLoading();
			}
			if (temp->mBlockFlags == QNEBlock::StatisticBlock) {
				temp->mBlock->handleDropdownChange2(temp->mBlock->FeatureDropdown->currentIndex());
			}
			if (temp->mBlockFlags == QNEBlock::FeatureBlock) {
				int type = mGlobals.CurrentProject->mFeature[temp->mBlock->FeatureDropdown->currentIndex()].FeatureType;
				QString unit_str;
				if (type == 0 || mGlobals.CurrentProject->ResolutionUnitX == 3) {
					unit_str = "v";
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


				temp->mBlock->FeatureHistogram->start_edit_value_unit->setText(QString("Start(") + unit_str + QString(")"));
				temp->mBlock->FeatureHistogram->end_edit_value_unit->setText(QString("End(") + unit_str + QString(")"));

				temp->mBlock->FeatureHistogram->start_edit_value->setText(QString::number(temp->mBlock->FeatureHistogram->start_cell_value*multi_value, 'f', 4));
				temp->mBlock->FeatureHistogram->end_edit_value->setText(QString::number(temp->mBlock->FeatureHistogram->end_cell_value*multi_value, 'f', 4));

//				temp->mBlock->FeatureHistogram->update_release_box((double)temp->mBlock->histogram_start_percent->text().toDouble() / 100.0, (double)temp->mBlock->histogram_end_percent->text().toDouble() / 100.0, 0);
			}
		}
	}
}

void Contents_ProjectInfo::handleModifyBtn() {
	qDebug("handleModifyBtn");
	mGlobals.CurrentProject->ResolutionX = std::stod(ResolutionX->text().toStdString());
	mGlobals.CurrentProject->ResolutionY = std::stod(ResolutionY->text().toStdString());
	mGlobals.CurrentProject->ResolutionZ = std::stod(ResolutionZ->text().toStdString());

	mGlobals.CurrentProject->ResolutionUnitX = Resolution_Unit->currentIndex();
	mGlobals.CurrentProject->ResolutionUnitY = Resolution_Unit->currentIndex();
	mGlobals.CurrentProject->ResolutionUnitZ = Resolution_Unit->currentIndex();

	foreach(QGraphicsItem *ptr, mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->items()) {
		if (ptr->type() == QNEBlock::Type) {
			QNEBlock *temp = (QNEBlock *)ptr;
			if (temp->mBlockFlags == QNEBlock::SubregionBlock || temp->mBlockFlags == QNEBlock::CutPlaneBlock) {
				temp->mBlock->handleSubregionLoading();
			}
		}
	}
}
void Contents_ProjectInfo::handleLoad() {
	qDebug("handleLoad");

	char t[20];
	sprintf(t, "%.3f", mGlobals.CurrentProject->ResolutionX);
	ResolutionX->setText(QString(t));
	sprintf(t, "%.3f", mGlobals.CurrentProject->ResolutionY);
	ResolutionY->setText(QString(t));
	sprintf(t, "%.3f", mGlobals.CurrentProject->ResolutionZ);
	ResolutionZ->setText(QString(t));
	prevResoluiton_Unit = mGlobals.CurrentProject->ResolutionUnitX;

	Resolution_Unit->setCurrentIndex(mGlobals.CurrentProject->ResolutionUnitX);


	ResolutionX_Unit->setText(mGlobals.CurrentProject->UnitList[mGlobals.CurrentProject->ResolutionUnitX]);
	ResolutionY_Unit->setText(mGlobals.CurrentProject->UnitList[mGlobals.CurrentProject->ResolutionUnitX]);
	ResolutionZ_Unit->setText(mGlobals.CurrentProject->UnitList[mGlobals.CurrentProject->ResolutionUnitX]);


	foreach(QGraphicsItem *ptr, mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->items()) {
		if (ptr->type() == QNEBlock::Type) {
			QNEBlock *temp = (QNEBlock *)ptr;
			if (temp->mBlockFlags == QNEBlock::SubregionBlock || temp->mBlockFlags == QNEBlock::CutPlaneBlock) {
				temp->mBlock->handleSubregionInformationUpdate(" ");
			}
		}
	}

}