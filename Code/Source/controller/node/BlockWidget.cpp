#include "BlockWidget.h"

BlockWidget::BlockWidget(QObject *parent)
	: QObject(parent)
{
	for (int i = 0; i < 64; i++) {
		volume[i] = NULL;
		label[i] = NULL;
	}
}

BlockWidget::~BlockWidget()
{
}

void BlockWidget::initialize(std::string name, int aflags, int awidth, int aheight, QColor acolor, QNEBlock *parent) {
	m_block = parent;
	BlockName = name;
	BackgroundColor = acolor;
	BackgroundColor_style = "background-color : rgba(0,0,0,0)";
	BlockFlag = aflags;
	generate_ToolBox(aflags);
	mThread = new ThreadSubregion;
	connect(mThread, SIGNAL(subregion_information_update(QString)), this, SLOT(handleSubregionInformationUpdate(QString)));
	mFont.setPointSize(8);
	mFont.setBold(true);

	switch (aflags) {
		case OriginBlock:
			CellIndexListOutput = CellIndexListInput;
			generate_DataHeatmap(awidth, aheight);
			generate_DataInputOutput(awidth, aheight);
			break;
		case DataBlock:
			CellIndexListOutput = CellIndexListInput;
			generate_DataHeatmap(awidth, aheight);
			generate_DataInputOutput(awidth, aheight);
			break;
		case FeatureBlock:
			CellIndexListOutput = CellIndexListInput;
			generate_FeatureHistogram(awidth, aheight);
			generate_FeatureDropdown(awidth, aheight);
			generate_DataInputOutput(awidth, aheight);

			break;
		case SubregionBlock:
			mThread->start();
			CellIndexListOutput = CellIndexListInput;
			generate_SubregionInformation(awidth, aheight);
			generate_SubregionDropdown(awidth, aheight);
			generate_DataInputOutput(awidth, aheight);
			break;


		case SetBlock:
//			mThread->start();//ToDo;
			CellIndexListOutput = CellIndexListInput;
			generate_SetDropdown(awidth, aheight);
			generate_DataInputOutput(awidth, aheight);
			break;
		case CutPlaneBlock:
			mThread->start();

			CellIndexListOutput = CellIndexListInput;
			generate_SubregionInformation(awidth, aheight);
			generate_CutPlaneDropdown(awidth, aheight);

			//generate_CutDropdown(awidth, aheight);
			generate_DataInputOutput(awidth, aheight);
			break;
		case StatisticBlock:
			CellIndexListOutput = CellIndexListInput;
			generate_Statistic(awidth, aheight);
			generate_DataInputOutput(awidth, aheight);
			break;
		case SubsetExplorerBlock:
			CellIndexListOutput = CellIndexListInput;
			generate_DataInputOutput(awidth, aheight);
//			generate_SubsetExplorer(awidth, aheight);
			break;
	}
	



	timerForMouseInteraction = new QTimer(this);
	timerForMouseInteraction->start(50);
	connect(timerForMouseInteraction, &QTimer::timeout, this, &BlockWidget::mouseInteraction);
}
void BlockWidget::mouseInteraction() {
	if (iconPressed) {
		if (prevPos != QCursor::pos()) {
			m_block->setPos(m_block->pos() + (QCursor::pos() - prevPos) / (float(mGlobals.mainWindow->mContents->GraphEditorView->curScale) / 100));
			prevPos = QCursor::pos();
			iconMoved = true;
		}
	}
	if (BlockFlag == FeatureBlock || BlockFlag==SubsetExplorerBlock) {
		QNEBlock *cur = m_block;
		QNEConnection *conn = NULL;

		foreach(QNEPort *port, cur->ports()) {
			if (port->portAlign() == QNEPort::Input) {
				if (port->connections().size() == 0)break;
				conn = port->connections()[0];
				break;
			}
		}
		if (conn == NULL) {
			QPalette pal = IconBtn->palette();
			pal.setColor(QPalette::Background, Qt::darkGray);
			IconBtn->setPalette(pal);
		}
		else {
			QPalette pal = IconBtn->palette();
			pal.setColor(QPalette::Background, Qt::green);
			IconBtn->setPalette(pal);
		}
	}
	else if (BlockFlag == SubregionBlock || BlockFlag == CutPlaneBlock) {
		QNEBlock *cur = m_block;
		QNEConnection *conn = NULL;

		foreach(QNEPort *port, cur->ports()) {
			if (port->portAlign() == QNEPort::Input) {
				if (port->connections().size() == 0)break;
				conn = port->connections()[0];
				break;
			}
		}
		if (conn == NULL) {
			QPalette pal = IconBtn->palette();
			pal.setColor(QPalette::Background, Qt::darkGray);
			IconBtn->setPalette(pal);
		}
		else {
			if (IsSubregionGenerated) {
				QPalette pal = IconBtn->palette();
				pal.setColor(QPalette::Background, Qt::green);
				IconBtn->setPalette(pal);
			}
			else {
				QPalette pal = IconBtn->palette();
				pal.setColor(QPalette::Background, Qt::red);
				IconBtn->setPalette(pal);
			}
		}
	}
	else if (BlockFlag == SetBlock) {
		QNEBlock *cur = m_block;
		QNEConnection *conn = NULL;
		int count = 0;

		foreach(QNEPort *port, cur->ports()) {
			if (port->portAlign() == QNEPort::Input) {
				if (port->connections().size() == 0)break;
				count++;
			}
		}
		if (count<2) {
			QPalette pal = IconBtn->palette();
			pal.setColor(QPalette::Background, Qt::darkGray);
			IconBtn->setPalette(pal);
		}
		else {
			if (m_block->labelIndex != m_block->labelIndex2) {
				QPalette pal = IconBtn->palette();
				pal.setColor(QPalette::Background, Qt::red);
				IconBtn->setPalette(pal);

			}
			else {
				QPalette pal = IconBtn->palette();
				pal.setColor(QPalette::Background, Qt::green);
				IconBtn->setPalette(pal);
			}
		}
	}

}
void BlockWidget::generate_ToolBox(int aflag) {
	
	ToolBox = new QWidget;
	ToolBox->setAttribute(Qt::WA_TranslucentBackground);
	QHBoxLayout *layout = new QHBoxLayout;
	layout->setContentsMargins(0, 5, 2, 5);

	if (aflag == OriginBlock || aflag == DataBlock) {
		BucketColorBtn = new QPushButton;
		BucketColorBtn->setFixedSize(QSize(15, 15));
		BucketColorBtn->setFlat(true);
		BucketColorBtn->setAutoFillBackground(true);
		DataColor = QColor(255, 255, 123);
		updateColorBox(BucketColorBtn, DataColor);
		layout->addWidget(BucketColorBtn);
		connect(BucketColorBtn, SIGNAL(released()), this, SLOT(handleBlockColorBtn()));

	}

	AnnotationBtn = new QPushButton;
	AnnotationBtn->setIcon(QIcon("Resources/icon_annotation.png"));
	AnnotationBtn->setIconSize(QSize(15, 15));
	AnnotationBtn->setFixedSize(QSize(18, 18));
	connect(AnnotationBtn, SIGNAL(released()), this, SLOT(handleAnnotationBtn()));
	layout->addWidget(AnnotationBtn);

	if (aflag == OriginBlock || aflag == DataBlock) {

		StatisticsBtn = new QPushButton;
		StatisticsBtn->setIcon(QIcon("Resources/icon_statistics.png"));
		StatisticsBtn->setIconSize(QSize(15, 15));
		StatisticsBtn->setFixedSize(QSize(18, 18));
		connect(StatisticsBtn, SIGNAL(released()), this, SLOT(handleStatisticsBtn()));
		layout->addWidget(StatisticsBtn);
	}

	//if (aflag != OriginBlock) {
		CloseBtn = new QPushButton;
		CloseBtn->setIcon(QIcon("Resources/icon_blockclose.png"));
		CloseBtn->setIconSize(QSize(15, 15));
		CloseBtn->setFixedSize(QSize(18, 18));
		connect(CloseBtn, SIGNAL(released()), this, SLOT(handleCloseBtn()));
		layout->addWidget(CloseBtn);
	//}
	

	ToolBox->setLayout(layout);
}
void BlockWidget::generate_Statistic(int width, int height) {

	StatisticWidget = new QWidget;
	StatisticWidget->setAttribute(Qt::WA_TranslucentBackground);
	QVBoxLayout *layout = new QVBoxLayout;

	QLabel *label = new QLabel;
	label->setText("Current Feature");
	label->setFont(mFont);
	layout->addWidget(label);

	FeatureDropdown = new QComboBox;
	//for each (DataFeature feature in mGlobals.CurrentProject->mFeature)
	//{
	//	FeatureDropdown->addItem(QString::fromStdString(feature.FeatureName));
	//}

	connect(FeatureDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange2(int)));
	layout->addWidget(FeatureDropdown);
	//layout->addSpacing(10);

	StatisticLayoutWidget = new QWidget;
	layout->addWidget(StatisticLayoutWidget);


	StatisticWidget->setLayout(layout);
	



	QFont title_font("Arial", 15, QFont::Bold);
	QFont feature_title_font("Arial", 13, QFont::Bold);
	QFont small_font("Arial", 10, QFont::Bold);
	QFont small_contents_font("Arial", 10, QFont::Normal);


	QHBoxLayout * count_layout = new QHBoxLayout;
	QLabel *count_label_t = new QLabel;
	count_label_t->setText("Cell Count : ");
	count_label_t->setFont(small_font);

	count_label_c->setText("");
	count_label_c->setFont(small_contents_font);

	QLabel *count_label_u = new QLabel;
	count_label_u->setText("cells");
	count_label_u->setAlignment(Qt::AlignCenter);
	count_label_u->setFixedSize(60, 30);
	count_label_u->setFont(small_font);

	count_layout->addWidget(count_label_t);
	count_layout->addWidget(count_label_c);
	count_layout->addWidget(count_label_u);

	QLabel *hline = new QLabel(StatisticLayoutWidget);
	hline->setFrameStyle(QFrame::HLine | QFrame::Plain);

	nodeLayout->addWidget(hline);
	nodeLayout->addLayout(count_layout);


	QVBoxLayout * feature_layout = new QVBoxLayout;
	QLabel *featurename_label = new QLabel;
	featurename_label->setText("");
	featurename_label->setFont(feature_title_font);

	QHBoxLayout * feature_min_layout = new QHBoxLayout;
	QLabel *feature_min_label_t = new QLabel;
	feature_min_label_t->setText("Minimum : ");
	feature_min_label_t->setFont(small_font);

	feature_min_label_c->setText("");
	feature_min_label_c->setFont(small_contents_font);
	feature_min_label_u->setText("");
	feature_min_label_u->setAlignment(Qt::AlignCenter);
	feature_min_label_u->setFixedSize(60, 30);
	feature_min_label_u->setFont(small_font);
	feature_min_layout->addWidget(feature_min_label_t);
	feature_min_layout->addWidget(feature_min_label_c);
	feature_min_layout->addWidget(feature_min_label_u);

	QHBoxLayout * feature_max_layout = new QHBoxLayout;
	QLabel *feature_max_label_t = new QLabel;
	feature_max_label_t->setText("Maximum : ");
	feature_max_label_t->setFont(small_font);
	feature_max_label_c->setText("");
	feature_max_label_c->setFont(small_contents_font);
	feature_max_label_u->setText("");
	feature_max_label_u->setAlignment(Qt::AlignCenter);
	feature_max_label_u->setFixedSize(60, 30);
	feature_max_label_u->setFont(small_font);
	feature_max_layout->addWidget(feature_max_label_t);
	feature_max_layout->addWidget(feature_max_label_c);
	feature_max_layout->addWidget(feature_max_label_u);

	QHBoxLayout * feature_mean_layout = new QHBoxLayout;
	QLabel *feature_mean_label_t = new QLabel;
	feature_mean_label_t->setText("Mean : ");
	feature_mean_label_t->setFont(small_font);
	feature_mean_label_c->setText("");
	feature_mean_label_c->setFont(small_contents_font);
	feature_mean_label_u->setText("");
	feature_mean_label_u->setAlignment(Qt::AlignCenter);
	feature_mean_label_u->setFixedSize(60, 30);
	feature_mean_label_u->setFont(small_font);
	feature_mean_layout->addWidget(feature_mean_label_t);
	feature_mean_layout->addWidget(feature_mean_label_c);
	feature_mean_layout->addWidget(feature_mean_label_u);

	QHBoxLayout * feature_var_layout = new QHBoxLayout;
	QLabel *feature_var_label_t = new QLabel;
	feature_var_label_t->setText("Variance : ");
	feature_var_label_t->setFont(small_font);
	feature_var_label_c->setText("");
	feature_var_label_c->setFont(small_contents_font);
	feature_var_label_u->setText("");
	feature_var_label_u->setAlignment(Qt::AlignCenter);
	feature_var_label_u->setFixedSize(60, 30);
	feature_var_label_u->setFont(small_font);
	feature_var_layout->addWidget(feature_var_label_t);
	feature_var_layout->addWidget(feature_var_label_c);
	feature_var_layout->addWidget(feature_var_label_u);

	QHBoxLayout * feature_sum_layout = new QHBoxLayout;
	QLabel *feature_sum_label_t = new QLabel;
	feature_sum_label_t->setText("Sum : ");
	feature_sum_label_t->setFont(small_font);
	feature_sum_label_c->setText("");
	feature_sum_label_c->setFont(small_contents_font);
	feature_sum_label_u->setText("");
	feature_sum_label_u->setAlignment(Qt::AlignCenter);
	feature_sum_label_u->setFixedSize(60, 30);
	feature_sum_label_u->setFont(small_font);
	feature_sum_layout->addWidget(feature_sum_label_t);
	feature_sum_layout->addWidget(feature_sum_label_c);
	feature_sum_layout->addWidget(feature_sum_label_u);

	QHBoxLayout * feature_five_layout = new QHBoxLayout;
	feature_five_label_t->setText("Five Number Summary()  : ");
	feature_five_label_t->setFont(small_font);
	//feature_five_layout->addWidget(feature_five_label_t);
	for (int i = 0; i < 5; ++i) {
		QVBoxLayout * _1_layout = new QVBoxLayout;

		QLabel * _1_title = new QLabel;
		_1_title->setAlignment(Qt::AlignCenter);
		_1_title->setText(QString::number(i * 25) + QString("%"));
		_1_title->setFont(small_font);
		_1_data[i] = new QLineEdit;
		_1_data[i]->setText("");
		_1_data[i]->setFont(small_contents_font);
		_1_data[i]->setFixedWidth(52);
		_1_layout->addWidget(_1_title);
		_1_layout->addWidget(_1_data[i]);
		feature_five_layout->addLayout(_1_layout);
	}

	nodeLayout->addLayout(feature_min_layout);
	nodeLayout->addLayout(feature_max_layout);
	nodeLayout->addLayout(feature_mean_layout);
	nodeLayout->addLayout(feature_var_layout);
	nodeLayout->addLayout(feature_sum_layout);
	nodeLayout->addWidget(feature_five_label_t);
	nodeLayout->addLayout(feature_five_layout);


	StatisticLayoutWidget->setLayout(nodeLayout);




}




void BlockWidget::generate_DataHeatmap(int width, int height) {
	
	DataHeatmap = new ViewHeatmapGLWidget;
	width = width - 2;
	QImage temp(QString::fromStdString(mGlobals.CurrentProject->mLayerBack->BackgroundThumbnailPath));
	float ratio = (float)temp.height() / (float)temp.width();
	qDebug() << "ratio : " << ratio;

	DataHeatmap->setRenderingSize(width, width * ratio);
	DataHeatmap->setTexturePath(mGlobals.CurrentProject->mLayerBack->BackgroundThumbnailPath);
	DataHeatmap->setBackgroundColor(BackgroundColor);
	DataHeatmap->setFixedSize(QSize(width, width * ratio));
	DataHeatmap->setContentsMargins(0, 0, 0, 0);

	coord Datasize;
	Datasize.x = mGlobals.CurrentProject->DataSizeX;
	Datasize.y = mGlobals.CurrentProject->DataSizeY;
	Datasize.z = mGlobals.CurrentProject->DataSizeZ;
	DataHeatmap->setHeatmapData(&mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList, mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex, &this->CellIndexListInput, Datasize);
	DataHeatmap->updateDataColor(DataColor);


}
void BlockWidget::generate_DataInputOutput(int width, int height) {
	DataInputOutput = new QWidget;
	DataInputOutput->setAttribute(Qt::WA_TranslucentBackground);
	QVBoxLayout *layout = new QVBoxLayout;
	
	QHBoxLayout *layout_input = new QHBoxLayout;
	//layout_input->setContentsMargins(10, 0, 0, 0);
	QLabel *title_input = new QLabel;
	
	title_input->setFont(mFont);
	if (BlockFlag == SetBlock) {
		title_input->setText("Input 1 :");
	}
	else {
		title_input->setText("Input :");
	}


	count_input1= new QLabel;
	mFont.setBold(true);
	count_input1->setFont(mFont);
	int size_input = CellIndexListInput.size();
	count_input1->setText(QString::fromStdString(std::to_string(size_input)) + " cells");

	//layout_input->addWidget(title_input);
	layout_input->addWidget(count_input1);

	QHBoxLayout *layout_input2 = new QHBoxLayout;
	layout_input2->setContentsMargins(10, 0, 0, 0);
	QLabel *title_input2 = new QLabel;
	
	mFont.setPointSize(8);
	mFont.setBold(true);
	title_input2->setFont(mFont);
	title_input2->setText("Input 2 :");

	count_input2 = new QLabel;
	mFont.setBold(false);
	count_input2->setFont(mFont);
	int size_input2 = CellIndexListInput_Sub.size();
	count_input2->setText(QString::fromStdString(std::to_string(size_input2)) + " cells");
	layout_input2->addWidget(title_input2);
	layout_input2->addWidget(count_input2);

	QHBoxLayout *layout_output = new QHBoxLayout;
	layout_output->setContentsMargins(10, 0, 0, 0);
	QLabel *title_output = new QLabel;
	mFont.setPointSize(8);
	mFont.setBold(true);
	title_output->setFont(mFont);
	title_output->setText("Output :");

	count_output = new QLabel;
	mFont.setBold(false);
	count_output->setFont(mFont);
	int size_output = CellIndexListOutput.size();
	count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	layout_output->addWidget(title_output);
	layout_output->addWidget(count_output);

	layout->addLayout(layout_input);
	//if (BlockFlag == SetBlock) {
	//	layout->addLayout(layout_input2);
	//}
	//layout->addLayout(layout_output);

	
	DataInputOutput->setLayout(layout);

}
void BlockWidget::generate_SubregionInformation(int width, int height) {
	SubregionInformationMaster = new QWidget;
	
	//SubregionInformationMaster->setFixedWidth(width - 20);
	SubregionInformationMaster->setAttribute(Qt::WA_TranslucentBackground);

	QVBoxLayout *layout = new QVBoxLayout;
	QHBoxLayout *precision_layout = new QHBoxLayout;
	precision_layout->setContentsMargins(10, 0, 0, 0);
	QLabel *label0 = new QLabel;
	label0->setText("Precision :");
	mFont.setPointSize(8);
	mFont.setBold(true);
	label0->setFont(mFont);
	precision_layout->addWidget(label0);
	subreigon_precision = new QComboBox;
	for (int i = 0; i < mGlobals.CurrentProject->DataLevel; i++) {
		subreigon_precision->addItem(QString::number(100.0/pow(2,i))+QString("%"));
	}
	mFont.setPointSize(8);
	mFont.setBold(false);
	subreigon_precision->setFont(mFont);
	precision_layout->addWidget(subreigon_precision);

	QHBoxLayout *volume_layout = new QHBoxLayout;
	volume_layout->setContentsMargins(10, 0, 0, 0);
	QLabel *label1 = new QLabel;
	label1->setText("Volume :");
	mFont.setPointSize(8);
	mFont.setBold(true);
	label1->setFont(mFont);
	volume_layout->addWidget(label1);

	subreigon_volume = new QLineEdit;
	subreigon_volume->setText("0");
	mFont.setPointSize(8);
	mFont.setBold(false);
	subreigon_volume->setFont(mFont);
	volume_layout->addWidget(subreigon_volume);

//	QVBoxLayout *subregion_volume_layout = new QVBoxLayout;
//
//	for (int i = 0; i < 4; i++) {
//		subreigon_volume[i] = new QLineEdit;
//		subreigon_volume[i]->setText("level "+QString::number(i+1)+": ");
//		mFont.setPointSize(8);
//		mFont.setBold(false);
//		subreigon_volume[i]->setFont(mFont);
//		subregion_volume_layout->addWidget(subreigon_volume[i]);
//	}
//	volume_layout->addLayout(subregion_volume_layout);
////	volume_layout->addWidget(subreigon_volume);

	subregion_volume_unit = new QLabel;
	subregion_volume_unit->setText("");
	mFont.setPointSize(8);
	mFont.setBold(true);
	subregion_volume_unit->setFont(mFont);
	volume_layout->addWidget(subregion_volume_unit);


	layout->addLayout(precision_layout);
	layout->addLayout(volume_layout);

	SubregionInformationMaster->setLayout(layout);

}



void BlockWidget::generate_SubregionDropdown(int width, int height) {
	SubregionDropdownMaster = new QWidget;
	//SubregionDropdownMaster->setFixedWidth(width-20);
	SubregionDropdownMaster->setAttribute(Qt::WA_TranslucentBackground);
	
	QVBoxLayout *layout = new QVBoxLayout;
	
	SubregionProgressbarLog = new QLabel;
	SubregionProgressbarLog->setText("Progress");
	SubregionProgressbarLog->setFont(mFont);
	layout->addWidget(SubregionProgressbarLog);

	SubregionProgressbar = new QProgressBar;
	connect(mThread, SIGNAL(progress_update(float)), this, SLOT(subregion_progess_update(float)));
	connect(mThread, SIGNAL(progress_log_update(QString)), this, SLOT(subregion_progess_log_update(QString)));
	layout->addWidget(SubregionProgressbar);

	QLabel *label = new QLabel;
	label->setText("Current Subregion");
	label->setFont(mFont);
	layout->addWidget(label);
	
	SubregionDropdown = new QComboBox;
	for each (LayerSubregion subregion in mGlobals.CurrentProject->mSubregion) {
		SubregionDropdown->addItem(QString::fromStdString(subregion.SubregionName));
	}
	//connect(SubregionDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange(int)));
	layout->addWidget(SubregionDropdown);


	//subregion_pos_x = new QLineEdit;
	//subregion_pos_y = new QLineEdit;
	//subregion_pos_z = new QLineEdit;
	//subregion_up_x = new QLineEdit;
	//subregion_up_y = new QLineEdit;
	//subregion_up_z = new QLineEdit;

	//QHBoxLayout *interface_pos_layout = new QHBoxLayout;
	//QLabel *label2 = new QLabel;
	//label2->setText("Pos X");
	////label2->setFont(mFont);
	//interface_pos_layout->addWidget(label2);
	//subregion_pos_x->setText(QString::number(plane_pos_x, 'g', 4));
	//interface_pos_layout->addWidget(subregion_pos_x);
	//
	//QLabel *label3 = new QLabel;
	//label3->setText("Pos Y");
	////label3->setFont(mFont);
	//interface_pos_layout->addWidget(label3);
	//subregion_pos_y->setText(QString::number(plane_pos_y, 'g', 4));
	//interface_pos_layout->addWidget(subregion_pos_y);

	//QLabel *label4 = new QLabel;
	//label4->setText("Pos Z");
	////label4->setFont(mFont);
	//interface_pos_layout->addWidget(label4);
	//subregion_pos_z->setText(QString::number(plane_pos_z, 'g', 4));
	//interface_pos_layout->addWidget(subregion_pos_z);

	//QHBoxLayout *interface_up_layout = new QHBoxLayout;
	//QLabel *label5 = new QLabel;
	//label5->setText("Up X");
	////label5->setFont(mFont);
	//interface_up_layout->addWidget(label5);
	//subregion_up_x->setText(QString::number(plane_up_x, 'g', 4));
	//interface_up_layout->addWidget(subregion_up_x);

	//QLabel *label6 = new QLabel;
	//label6->setText("Up Y");
	////label6->setFont(mFont);
	//interface_up_layout->addWidget(label6);
	//subregion_up_y->setText(QString::number(plane_up_y, 'g', 4));
	//interface_up_layout->addWidget(subregion_up_y);

	//QLabel *label7 = new QLabel;
	//label7->setText("Up Z");
	////label7->setFont(mFont);
	//interface_up_layout->addWidget(label7);
	//subregion_up_z->setText(QString::number(plane_up_z, 'g', 4));
	//interface_up_layout->addWidget(subregion_up_z);


	QLabel *label8 = new QLabel;
	label8->setText("Cell Selection Option");
	label8->setFont(mFont);
	layout->addWidget(label8);

	SubregionSelectMethod = new QComboBox;
	QStringList optionlist = { "Complete Overlap", "Intersection", "Touch" };
	SubregionSelectMethod->addItems(optionlist);
	//connect(SubregionSelectMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange(int)));
	layout->addWidget(SubregionSelectMethod);


	layout->addSpacing(10);
	//QLabel *label9 = new QLabel;
	//label9->setText("Cut Plane");
	//label9->setFont(mFont);
	//layout->addWidget(label9);


	QPushButton *setbtn = new QPushButton;

	setbtn->setText("Generate");
	connect(setbtn, SIGNAL(released()), this, SLOT(handleSubregionSetBtn()));
	//layout->addLayout(interface_pos_layout);
	//layout->addLayout(interface_up_layout);
	layout->addWidget(setbtn);
	
	SubregionDropdownMaster->setLayout(layout);
}
void BlockWidget::generate_CutPlaneDropdown(int width, int height) {
	SubregionDropdownMaster = new QWidget;
	//SubregionDropdownMaster->setFixedWidth(width - 20);
	SubregionDropdownMaster->setAttribute(Qt::WA_TranslucentBackground);

	QVBoxLayout *layout = new QVBoxLayout;

	SubregionProgressbarLog = new QLabel;
	SubregionProgressbarLog->setText("Progress");
	SubregionProgressbarLog->setFont(mFont);
	layout->addWidget(SubregionProgressbarLog);

	SubregionProgressbar = new QProgressBar;
	connect(mThread, SIGNAL(progress_update(float)), this, SLOT(subregion_progess_update(float)));
	connect(mThread, SIGNAL(progress_log_update(QString)), this, SLOT(subregion_progess_log_update(QString)));
	layout->addWidget(SubregionProgressbar);

	//QLabel *label = new QLabel;
	//label->setText("Current Subregion");
	//label->setFont(mFont);
	//layout->addWidget(label);

	//SubregionDropdown = new QComboBox;
	//for each (LayerSubregion subregion in mGlobals.CurrentProject->mSubregion) {
	//	SubregionDropdown->addItem(QString::fromStdString(subregion.SubregionName));
	//}
	////connect(SubregionDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange(int)));
	//layout->addWidget(SubregionDropdown);


	subregion_pos_x = new QLineEdit;
	subregion_pos_y = new QLineEdit;
	subregion_pos_z = new QLineEdit;
	subregion_up_x = new QLineEdit;
	subregion_up_y = new QLineEdit;
	subregion_up_z = new QLineEdit;

	QHBoxLayout *interface_pos_layout = new QHBoxLayout;
	QLabel *label2 = new QLabel;
	label2->setText("Pos X");
	//label2->setFont(mFont);
	interface_pos_layout->addWidget(label2);
	subregion_pos_x->setText(QString::number(plane_pos_x, 'g', 4));
	interface_pos_layout->addWidget(subregion_pos_x);
	
	QLabel *label3 = new QLabel;
	label3->setText("Pos Y");
	//label3->setFont(mFont);
	interface_pos_layout->addWidget(label3);
	subregion_pos_y->setText(QString::number(plane_pos_y, 'g', 4));
	interface_pos_layout->addWidget(subregion_pos_y);

	QLabel *label4 = new QLabel;
	label4->setText("Pos Z");
	//label4->setFont(mFont);
	interface_pos_layout->addWidget(label4);
	subregion_pos_z->setText(QString::number(plane_pos_z, 'g', 4));
	interface_pos_layout->addWidget(subregion_pos_z);

	QHBoxLayout *interface_up_layout = new QHBoxLayout;
	QLabel *label5 = new QLabel;
	label5->setText("Up X");
	//label5->setFont(mFont);
	interface_up_layout->addWidget(label5);
	subregion_up_x->setText(QString::number(plane_up_x, 'g', 4));
	interface_up_layout->addWidget(subregion_up_x);

	QLabel *label6 = new QLabel;
	label6->setText("Up Y");
	//label6->setFont(mFont);
	interface_up_layout->addWidget(label6);
	subregion_up_y->setText(QString::number(plane_up_y, 'g', 4));
	interface_up_layout->addWidget(subregion_up_y);

	QLabel *label7 = new QLabel;
	label7->setText("Up Z");
	//label7->setFont(mFont);
	interface_up_layout->addWidget(label7);
	subregion_up_z->setText(QString::number(plane_up_z, 'g', 4));
	interface_up_layout->addWidget(subregion_up_z);


	QLabel *label8 = new QLabel;
	label8->setText("Cell Selection Option");
	label8->setFont(mFont);
	layout->addWidget(label8);

	SubregionSelectMethod = new QComboBox;
	QStringList optionlist = { "Complete Overlap", "Intersection", "Touch" };
	SubregionSelectMethod->addItems(optionlist);
	//connect(SubregionSelectMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange(int)));
	layout->addWidget(SubregionSelectMethod);


	layout->addSpacing(10);
	QLabel *label9 = new QLabel;
	label9->setText("Cut Plane");
	label9->setFont(mFont);
	layout->addWidget(label9);


	QPushButton *setbtn = new QPushButton;

	setbtn->setText("Generate");
	connect(setbtn, SIGNAL(released()), this, SLOT(handleSubregionSetBtn()));
	layout->addLayout(interface_pos_layout);
	layout->addLayout(interface_up_layout);
	layout->addWidget(setbtn);

	SubregionDropdownMaster->setLayout(layout);
}

void BlockWidget::updatedSubregionList() {
	int current_index = SubregionDropdown->currentIndex();
	disconnect(SubregionDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange(int)));
	SubregionDropdown->clear();
	for each (LayerSubregion subregion in mGlobals.CurrentProject->mSubregion) {
		SubregionDropdown->addItem(QString::fromStdString(subregion.SubregionName));
	}
	SubregionDropdown->setCurrentIndex(current_index);
	connect(SubregionDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange(int)));
}

void BlockWidget::subregion_progess_update(float v) {
	if (v > 99) {
		SubregionProgressbar->setValue(100);
	}
	else {
		SubregionProgressbar->setValue((int)v);
	}
}

void BlockWidget::subregion_progess_log_update(QString str) {
	SubregionProgressbarLog->setText(str);
}

void BlockWidget::generate_FeatureHistogram(int width, int height) {
	FeatureHistogramMaster = new QWidget;
	//FeatureHistogramMaster->setFixedWidth(width - 10);
	FeatureHistogramMaster->setAttribute(Qt::WA_TranslucentBackground);
	QVBoxLayout *layout = new QVBoxLayout;
	QLabel *label = new QLabel;
	label->setText("Feature Histogram");
	label->setFont(mFont);
	layout->addWidget(label);
	
	histogram_start_percent = new QLineEdit;
	histogram_start_percent->setText("0");
	histogram_end_percent= new QLineEdit;
	histogram_end_percent->setText("0");
	histogram_start_value = new QLineEdit;
	histogram_start_value->setText("0");
	histogram_end_value = new QLineEdit;
	histogram_end_value->setText("0");


	QWidget *FeatureHistBox = new QWidget;
	FeatureHistBox->setContentsMargins(1, 1, 1, 1);
	FeatureHistBox->setAutoFillBackground(true);
	FeatureHistogram = new ViewHistogramWidget(FeatureHistBox);
	FeatureHistogram->setInterface_percent(histogram_start_percent, histogram_end_percent);
	FeatureHistogram->setInterface_value(histogram_start_value, histogram_end_value);
	FeatureHistogram->setRenderingSize(350, 100);
	FeatureHistBox->setFixedSize(350, 100);
	FeatureHistogram->setData(0, &CellIndexListInput, &CellIndexListOutput);
	connect(FeatureHistogram, SIGNAL(OutputUpdated()), this, SLOT(handleHistogramUpdate()));
	layout->addWidget(FeatureHistBox);

	QHBoxLayout *percent_layout = new QHBoxLayout;
	QLabel *label2 = new QLabel;
	label2->setText("Start(%)");
	label2->setFont(mFont);
	percent_layout->addWidget(label2);
	percent_layout->addWidget(histogram_start_percent);
	QLabel *label3 = new QLabel;
	label3->setText("End(%)");
	label3->setFont(mFont);
	percent_layout->addWidget(label3);
	percent_layout->addWidget(histogram_end_percent);
	QPushButton *setbtn1 = new QPushButton;
	setbtn1->setText("Set");
	connect(setbtn1, SIGNAL(released()), this, SLOT(handleHistogramSetBtn_Percent()));
	percent_layout->addWidget(setbtn1);



	QFont small_font("Arial", 10, QFont::Bold);

	QHBoxLayout *value_layout = new QHBoxLayout;
	histogram_start_value_unit = new QLabel;
	histogram_start_value_unit->setText(QString("Start(v)"));
	histogram_start_value_unit->setFont(small_font);
	value_layout->addWidget(histogram_start_value_unit);
	value_layout->addWidget(histogram_start_value);
	histogram_end_value_unit = new QLabel;
	histogram_end_value_unit->setText(QString("End(v)"));
	histogram_end_value_unit->setFont(small_font);
	value_layout->addWidget(histogram_end_value_unit);
	value_layout->addWidget(histogram_end_value);
	QPushButton *setbtn2 = new QPushButton;
	setbtn2->setText("Set");
	connect(setbtn2, SIGNAL(released()), this, SLOT(handleHistogramSetBtn_Value()));
	value_layout->addWidget(setbtn2);


	FeatureHistogram->setInterface_unit(histogram_start_value_unit, histogram_end_value_unit);

	layout->addLayout(percent_layout);
	layout->addLayout(value_layout);


	FeatureHistogramMaster->setLayout(layout);
}

void BlockWidget::generate_FeatureDropdown(int width, int height) {

	FeatureDropdownMaster = new QWidget;
	//FeatureDropdownMaster->setFixedWidth(width - 20);
	FeatureDropdownMaster->setAttribute(Qt::WA_TranslucentBackground);

	QVBoxLayout *layout = new QVBoxLayout;
	QLabel *label = new QLabel;
	label->setText("Current Feature");
	label->setFont(mFont);
	layout->addWidget(label);
	
	FeatureDropdown = new QComboBox;
	//for each (DataFeature feature in mGlobals.CurrentProject->mFeature)
	//{
	//	FeatureDropdown->addItem(QString::fromStdString(feature.FeatureName));
	//}

	connect(FeatureDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange(int)));
	layout->addWidget(FeatureDropdown);
	FeatureDropdownMaster->setLayout(layout);
}

void BlockWidget::updatedFeatureList() {
	int current = FeatureDropdown->currentIndex();
	disconnect(FeatureDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange(int)));
	FeatureDropdown->clear();
	if (m_block->labelIndex == -1)return;

	int startIndex = mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureStartIndex;
	int endIndex = mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureEndIndex;

	for (int i = startIndex; i < endIndex; i++) {
		FeatureDropdown->addItem(QString::fromStdString(mGlobals.CurrentProject->mFeature[i].FeatureName));
	}
	if (current >= 0 && current < endIndex - startIndex) {
		FeatureDropdown->setCurrentIndex(current);
	}
	else {
		FeatureDropdown->setCurrentIndex(0);
		current = 0;
	}
	int type = mGlobals.CurrentProject->mFeature[startIndex+ current].FeatureType;
	QString unit_str;
	if (type == 0 || mGlobals.CurrentProject->ResolutionUnitX==3) {
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
	histogram_start_value_unit->setText(QString("Start(") + unit_str + QString(")"));
	histogram_end_value_unit->setText(QString("End(") + unit_str + QString(")"));


	connect(FeatureDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange(int)));
}

void BlockWidget::updatedFeatureList2() {
	int current = FeatureDropdown->currentIndex();
	disconnect(FeatureDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange2(int)));
	FeatureDropdown->clear();
	if (m_block->labelIndex == -1)return;

	int startIndex = mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureStartIndex;
	int endIndex = mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureEndIndex;

	for (int i = startIndex; i < endIndex; i++) {
		FeatureDropdown->addItem(QString::fromStdString(mGlobals.CurrentProject->mFeature[i].FeatureName));
	}
	if (current>=0 && current <endIndex-startIndex) {
		FeatureDropdown->setCurrentIndex(current);
		handleDropdownChange2(current);
	}
	else {
		FeatureDropdown->setCurrentIndex(0);
		handleDropdownChange2(0);
	}
	connect(FeatureDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange2(int)));
}


void BlockWidget::generate_SetDropdown(int width, int height) {
	
	SetDropdownMaster = new QWidget;
	//SetDropdownMaster->setFixedWidth(width - 20);
	SetDropdownMaster->setAttribute(Qt::WA_TranslucentBackground);

	QVBoxLayout *layout = new QVBoxLayout;
	QLabel *label = new QLabel;
	label->setText("Set operation");
	label->setFont(mFont);
	layout->addWidget(label);

	SetDropdown = new QComboBox;
	QStringList operation = { "Union", "Subtraction", "Intersection" };
	SetDropdown->addItems(operation);
	connect(SetDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(handleDropdownChange(int)));
	layout->addWidget(SetDropdown);

	SetDropdownMaster->setLayout(layout);
}


void BlockWidget::handleBlockColorBtn() {
	QColorDialog color_test;
	QColor color_str = color_test.getColor(DataColor);
	qDebug() << color_str;
	if (color_str.isValid()) {
		DataColor = color_str;
		updateColorBox(BucketColorBtn, DataColor);
	}

	DataHeatmap->updateDataColor(DataColor);
}

void BlockWidget::handleCloseBtn() {
	qDebug() << "CloseBtn";
//	openedWidget->close();
	if(proxy!=NULL)m_block->pScene->removeItem(proxy);
	emit deleteNode(m_block);
}

void BlockWidget::handleAnnotationBtn() {
	qDebug() << "AnnotationBtn";
	mGlobals.mDialogManager->mDialogAnnotation->setData(this);
	mGlobals.mDialogManager->mDialogAnnotation->exec();
}
void BlockWidget::handleStatisticsBtn() {
	qDebug() << "StatisticsBtn";
	if (m_block->labelIndex == -1)return;
	mGlobals.mDialogManager->mDialogStatistics->generateStatistics(m_block->labelIndex,QString::fromStdString(BlockName), &CellIndexListInput);
	mGlobals.mDialogManager->mDialogStatistics->exec();
}

void BlockWidget::updateColorBox(QPushButton *target, QColor color) {
	qDebug() << "updateColorBox" << " " << color.name();
	target->setStyleSheet("");
	QPalette mPalette = target->palette();
	mPalette.setColor(target->backgroundRole(), color);
	target->setPalette(mPalette);
	target->setStyleSheet(QString("background-color: %1;foreground-color: %1; border-style: none;").arg(color.name()));
	target->setAutoFillBackground(true);
	target->setFlat(true);
	target->repaint();
}
void BlockWidget::handleDropdownChange(int index) {
	updatedInputList();
}
void BlockWidget::handleDropdownChange2(int index) {
	//updatedInputList();
	if (m_block->labelIndex == -1)return;
	int startIndex = mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureStartIndex;
	generateOneStatistic(m_block->labelIndex, FeatureDropdown->currentIndex() + startIndex, QString::fromStdString(BlockName));
	
}


void BlockWidget::generateOneStatistic(int labelIndex, int featureIndex, QString name) {
	mGlobals.mDialogManager->mDialogStatistics->CellIndexList = &CellIndexListInput;
	count_label_c->setText(QString::fromStdString(std::to_string(CellIndexListInput.size())));
	if (CellIndexListInput.size() > 0) {
		int i = featureIndex;
		statistic stat = mGlobals.mDialogManager->mDialogStatistics->getBasicStatistic(i);
		int type = mGlobals.CurrentProject->mFeature[i].FeatureType;
		QString unit_str;
		if (type == 0){
			unit_str = "constant";
		}
		else if(mGlobals.CurrentProject->ResolutionUnitX == 3){
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
		feature_min_label_c->setText(QString::number(stat.min, 'f', 3));
		feature_min_label_u->setText(unit_str);

		feature_max_label_c->setText(QString::number(stat.max, 'f', 3));
		feature_max_label_u->setText(unit_str);

		feature_mean_label_c->setText(QString::number(stat.mean, 'f', 3));
		feature_mean_label_u->setText(unit_str);

		feature_var_label_c->setText(QString::number(stat.variance, 'f', 3));

		feature_sum_label_c->setText(QString::number(stat.sum, 'f', 3));
		feature_sum_label_u->setText(unit_str);

		feature_five_label_t->setText("Five Number Summary(" + unit_str + ")  : ");
		//feature_five_layout->addWidget(feature_five_label_t);
		for (int i = 0; i < 5; ++i) {
			_1_data[i]->setText(QString::number(stat.five[i], 'f', 3));
		}
	}

}




void BlockWidget::handleHistogramUpdate() {
	int size_output = CellIndexListOutput.size();
	count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	checkNextBlock();
}
void BlockWidget::handleHistogramSetBtn_Percent() {
	FeatureHistogram->update_release_box((double)histogram_start_percent->text().toDouble() / 100.0, (double)histogram_end_percent->text().toDouble() / 100.0, 0);
	//histogram_start_value->setText(QString::number((double)histogram_start_percent->text().toDouble() / 100.0));
	//histogram_end_value->setText(QString::number((double)histogram_end_percent->text().toDouble() / 100.0));

}
void BlockWidget::handleHistogramSetBtn_Value() {
	FeatureHistogram->update_release_box(histogram_start_value->text().toDouble(), histogram_end_value->text().toDouble(), 1);
	//histogram_start_percent->setText(QString::number(histogram_start_value->text().toDouble() * 100.0));
	//histogram_end_percent->setText(QString::number(histogram_end_value->text().toDouble() * 100.0));

}

void BlockWidget::handleSubregionSetBtn() {
	//FeatureHistogram->update_release_box((float)histogram_start->text().toInt() / 100.0, (float)histogram_end->text().toInt() / 100.0);
	qDebug() << "handleSubregionSetBtn";
	int subregion_index=-1;
	if (BlockFlag == CutPlaneBlock) {
		plane_pos_x = stod(subregion_pos_x->text().toStdString());
		plane_pos_y = stod(subregion_pos_y->text().toStdString());
		plane_pos_z = stod(subregion_pos_z->text().toStdString());
		plane_up_x = stod(subregion_up_x->text().toStdString());
		plane_up_y = stod(subregion_up_y->text().toStdString());
		plane_up_z = stod(subregion_up_z->text().toStdString());

		mGlobals.CurrentProject->planeNormal[0] = (float)plane_up_x;
		mGlobals.CurrentProject->planeNormal[1] = (float)plane_up_y;
		mGlobals.CurrentProject->planeNormal[2] = (float)plane_up_z;
		mGlobals.CurrentProject->planePoint[0] = (float)plane_pos_x / (float)mGlobals.CurrentProject->DataSizeX;
		mGlobals.CurrentProject->planePoint[1] = (float)plane_pos_y / (float)mGlobals.CurrentProject->DataSizeY;
		mGlobals.CurrentProject->planePoint[2] = (float)plane_pos_z / (float)mGlobals.CurrentProject->DataSizeZ;
	}
	else if (BlockFlag == SubregionBlock) {
		subregion_index = SubregionDropdown->currentIndex();

	}

	
	if (subregion_index == -1) {
		mThread->addJob("-1", this);

	}
	else mThread->addJob(mGlobals.CurrentProject->mSubregion[subregion_index].SubregionID, this);


	updatedInputList();
}

void BlockWidget::handleSubregionLoading() {
	//FeatureHistogram->update_release_box((float)histogram_start->text().toInt() / 100.0, (float)histogram_end->text().toInt() / 100.0);
	qDebug() << "handleSubregionLoading";
	int subregion_index = -1;
	if (BlockFlag == CutPlaneBlock) {

		plane_pos_x = stod(subregion_pos_x->text().toStdString());
		plane_pos_y = stod(subregion_pos_y->text().toStdString());
		plane_pos_z = stod(subregion_pos_z->text().toStdString());
		plane_up_x = stod(subregion_up_x->text().toStdString());
		plane_up_y = stod(subregion_up_y->text().toStdString());
		plane_up_z = stod(subregion_up_z->text().toStdString());

		mGlobals.CurrentProject->planeNormal[0] = (float)plane_up_x;
		mGlobals.CurrentProject->planeNormal[1] = (float)plane_up_y;
		mGlobals.CurrentProject->planeNormal[2] = (float)plane_up_z;
		mGlobals.CurrentProject->planePoint[0] = (float)plane_pos_x / (float)mGlobals.CurrentProject->DataSizeX;
		mGlobals.CurrentProject->planePoint[1] = (float)plane_pos_y / (float)mGlobals.CurrentProject->DataSizeY;
		mGlobals.CurrentProject->planePoint[2] = (float)plane_pos_z / (float)mGlobals.CurrentProject->DataSizeZ;
	}
	else if (BlockFlag == SubregionBlock) {

		subregion_index = SubregionDropdown->currentIndex();
	}



	double volume = SubregionVolume;
	double multi_value = (mGlobals.CurrentProject->ResolutionX * mGlobals.CurrentProject->ResolutionY * mGlobals.CurrentProject->ResolutionZ);
	if (mGlobals.CurrentProject->ResolutionUnitX != 3)volume *= multi_value;

	//subreigon_precision->setText("level " + QString::fromStdString(std::to_string(SubregionPrecision)));

	
	std::stringstream ss;
	ss << std::fixed << std::setprecision(4) << volume;
	std::string mystring = ss.str();
	subreigon_volume->setText(QString::fromStdString(mystring));
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

	subregion_volume_unit->setText(unit_str);

	SubregionProgressbarLog->setText("Finish");
	SubregionProgressbar->setValue(100);

	int size_output = CellIndexListOutput.size();
	count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	

	checkNextBlockLoading();
}


void BlockWidget::handleSubregionInformationUpdate(QString id) {
	
	double volume= SubregionVolume;
	double multi_value = (mGlobals.CurrentProject->ResolutionX * mGlobals.CurrentProject->ResolutionY * mGlobals.CurrentProject->ResolutionZ);
	if (mGlobals.CurrentProject->ResolutionUnitX != 3)volume *= multi_value;


	
	int subregion_index = -1;
	if(BlockFlag==SubregionBlock)
		SubregionDropdown->currentIndex();
	//subreigon_precision->setText("level " + QString::fromStdString(std::to_string(SubregionPrecision)));

	std::stringstream ss;
	ss << std::fixed << std::setprecision(4) << volume;
	std::string mystring = ss.str();
	//subreigon_volume[SubregionPrecision-1]->setText("level " + QString::number(SubregionPrecision) + ": " + QString::fromStdString(mystring));
	subreigon_volume->setText(QString::fromStdString(mystring));
	
	
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

	subregion_volume_unit->setText(unit_str);
	
	//subreigon_precision->setText(QString::fromStdString());
	

	CellIndexListOutput.clear();
	std::list<unsigned int>::iterator iter = CellIndexListInput.begin();
	for (iter = CellIndexListInput.begin(); iter != CellIndexListInput.end(); ++iter) {
		if (SubregionSelectMethod->currentIndex() == 0) {
			std::map<unsigned int, bool>::iterator target_iter = CompleteIndex.end();
			if (CompleteIndex.find(*iter) != target_iter) {
				CellIndexListOutput.push_back(*iter);
			}
		}
		else if (SubregionSelectMethod->currentIndex() == 1) {
			std::map<unsigned int, bool>::iterator target_iter = IntersectIndex.end();
			if (IntersectIndex.find(*iter) != target_iter) {
				CellIndexListOutput.push_back(*iter);
			}
		}
		else if (SubregionSelectMethod->currentIndex() == 2) {
			std::map<unsigned int, bool>::iterator target_iter = TouchIndex.end();
			if (TouchIndex.find(*iter) != target_iter) {
				CellIndexListOutput.push_back(*iter);
			}
		}
	}
	int size_output = CellIndexListOutput.size();
	count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	checkNextBlock();
}


void BlockWidget::addAnnotation(QString cmt) {
	annotation temp;
	temp.comment = cmt;
	temp.time = QDateTime::currentDateTime();
	mAnnotation.push_back(temp);
}
void BlockWidget::removeAnnotation(int index) {
	int step = 0;
	std::list<annotation>::iterator iter;
	for (iter = this->mAnnotation.begin(); iter != this->mAnnotation.end(); ++iter) {
		if (step == index) {
			this->mAnnotation.erase(iter);
			break;
		}
		step++;
	}
}

void BlockWidget::updatedCellColor() {
	if (m_block->labelIndex == -1)return;
	std::vector<cell>::iterator iter_global= mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->mCellList.begin();
	for (iter_global = mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->mCellList.begin(); iter_global != mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->mCellList.end(); ++iter_global) {
		iter_global->status = false;
	}
	
	std::list<unsigned int>::iterator iter_selected = CellIndexListOutput.begin();
	for (iter_selected = CellIndexListOutput.begin(); iter_selected != CellIndexListOutput.end(); ++iter_selected) {
		mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->mCellList.at(mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->mCellIndex[*iter_selected]).status = true;
	}
	mGlobals.CurrentProject->SelectedColor = DataColor;
	
	mGlobals.CurrentProject->mLayerBack->HeatmapColor.r = DataColor.redF();
	mGlobals.CurrentProject->mLayerBack->HeatmapColor.g = DataColor.greenF();
	mGlobals.CurrentProject->mLayerBack->HeatmapColor.b = DataColor.blueF();
	mGlobals.CurrentProject->mLayerBack->HeatmapColor.a = 1.0;
	mGlobals.CurrentProject->mLayerBack->HeatmapUpdated = true;
}


void BlockWidget::updatedInputList() {
	
	int size_input = CellIndexListInput.size();
	count_input1->setText(QString::fromStdString(std::to_string(size_input)) + " cells");

	if (BlockFlag == OriginBlock) {
		CellIndexListOutput = CellIndexListInput;
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
		DataHeatmap->updateHeatmap();
		mGlobals.mainWindow->mContents->GraphEditorView->GraphView->scale(2.0, 1);
		mGlobals.mainWindow->mContents->GraphEditorView->GraphView->scale(0.5, 1);
	}
	else if (BlockFlag == DataBlock) {
		if (m_block->labelIndex == -1)return;
		CellIndexListOutput = CellIndexListInput;
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
		coord Datasize;
		Datasize.x = mGlobals.CurrentProject->DataSizeX;
		Datasize.y = mGlobals.CurrentProject->DataSizeY;
		Datasize.z = mGlobals.CurrentProject->DataSizeZ;
		DataHeatmap->setHeatmapData(&mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->mCellList, mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->mCellIndex, &this->CellIndexListInput, Datasize);
		DataHeatmap->updateHeatmap();

		mGlobals.mainWindow->mContents->GraphEditorView->GraphView->scale(2.0, 1);
		mGlobals.mainWindow->mContents->GraphEditorView->GraphView->scale(0.5, 1);

	}
	else if (BlockFlag == SubregionBlock | BlockFlag==CutPlaneBlock) {
		IsSubregionGenerated = false;
		CellIndexListOutput.clear();
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	}
	else if (BlockFlag == FeatureBlock) {
		if (m_block->labelIndex == -1)return;
		CellIndexListOutput.clear();
		FeatureHistogram->start_edit_percent->setText("0");
		FeatureHistogram->end_edit_percent->setText("0");

		FeatureHistogram->start_edit_value->setText("0");
		FeatureHistogram->end_edit_value->setText("0");
		if (mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureStartIndex == mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureEndIndex) {
			return;
		}
		updatedFeatureList();
		int startIndex = mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureStartIndex;
		int curindex = startIndex + FeatureDropdown->currentIndex();
		if (FeatureDropdown->currentIndex() == -1)curindex = startIndex;
		FeatureHistogram->setData(curindex, &CellIndexListInput, &CellIndexListOutput);
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	}

	else if (BlockFlag == SetBlock) {
		int size_input2 = CellIndexListInput_Sub.size();
		count_input2->setText(QString::fromStdString(std::to_string(size_input2)) + " cells");
		CellIndexListOutput.clear();
		set_operation(SetDropdown->currentIndex());
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	}
	else if (BlockFlag == StatisticBlock) {
		if (m_block->labelIndex == -1)return;
		CellIndexListOutput = CellIndexListInput;
		if (mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureStartIndex == mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureEndIndex) {
			return;
		}
		updatedFeatureList2();
	}
	else if (BlockFlag == SubsetExplorerBlock) {

		CellIndexListOutput.clear();
		std::list<unsigned int>::iterator iter = CellIndexListInput.begin();
		int cnt = 0;
		for (; iter != CellIndexListInput.end(); ++iter) {
			CellIndexListOutput.push_back(*iter);
		}
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
		inputUpdated = true;
	}
	checkNextBlock();
}

void BlockWidget::updatedInputListLoading() {

	int size_input = CellIndexListInput.size();
	count_input1->setText(QString::fromStdString(std::to_string(size_input)) + " cells");

	if (BlockFlag == OriginBlock) {
		CellIndexListOutput = CellIndexListInput;
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
		DataHeatmap->updateHeatmap();

	}
	else if (BlockFlag == DataBlock) {
		if (m_block->labelIndex == -1)return;
		CellIndexListOutput = CellIndexListInput;
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
		coord Datasize;
		Datasize.x = mGlobals.CurrentProject->DataSizeX;
		Datasize.y = mGlobals.CurrentProject->DataSizeY;
		Datasize.z = mGlobals.CurrentProject->DataSizeZ;
		DataHeatmap->setHeatmapData(&mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->mCellList, mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->mCellIndex, &this->CellIndexListInput, Datasize);
		DataHeatmap->updateHeatmap();
	}
	else if (BlockFlag == SubregionBlock || BlockFlag==CutPlaneBlock) {
//		CellIndexListOutput.clear();
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	}
	else if (BlockFlag == FeatureBlock) {
//		CellIndexListOutput.clear();
		if (m_block->labelIndex == -1)return;
		if (mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureStartIndex == mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureEndIndex) {
			return;
		}
		updatedFeatureList();

		int startIndex = mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureStartIndex;
		int curindex = startIndex + FeatureDropdown->currentIndex();
		if (FeatureDropdown->currentIndex() == -1)curindex = startIndex;
		FeatureHistogram->setData(curindex, &CellIndexListInput, &CellIndexListOutput);
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	}

	else if (BlockFlag == SetBlock) {
		int size_input2 = CellIndexListInput_Sub.size();
		count_input2->setText(QString::fromStdString(std::to_string(size_input2)) + " cells");
	//	CellIndexListOutput.clear();
		set_operation(SetDropdown->currentIndex());
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	}
	else if (BlockFlag == StatisticBlock) {
		if (m_block->labelIndex == -1)return;
		if (mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureStartIndex == mGlobals.CurrentProject->mLayerCell[m_block->labelIndex]->featureEndIndex) {
			return;
		}
		updatedFeatureList2();
	}
	else if (BlockFlag == SubsetExplorerBlock) {
		int size_output = CellIndexListOutput.size();
		count_output->setText(QString::fromStdString(std::to_string(size_output)) + " cells");
	}
	checkNextBlockLoading();
}


void BlockWidget::checkNextBlock() {
	foreach(QNEPort *port, m_block->ports()) {
		if (port->portAlign() == QNEPort::Output) {
			foreach(QNEConnection *conn, port->connections()) {
				QNEBlock *next = conn->port2()->m_block;
				if (conn->port2()->portFlags() == QNEPort::SetSubInputPort) {
					next->mBlock->CellIndexListInput_Sub.clear();
					std::copy(CellIndexListOutput.begin(), CellIndexListOutput.end(), std::back_inserter(next->mBlock->CellIndexListInput_Sub));
					next->mBlock->m_block->labelIndex2 = m_block->labelIndex;
					next->mBlock->updatedInputList();
				}
				else {
					next->mBlock->CellIndexListInput.clear();
					std::copy(CellIndexListOutput.begin(), CellIndexListOutput.end(), std::back_inserter(next->mBlock->CellIndexListInput));

					next->mBlock->m_block->labelIndex = m_block->labelIndex;
					next->mBlock->updatedInputList();

				}
				
			}

		}
	}
}


void BlockWidget::checkNextBlockLoading() {
	foreach(QNEPort *port, m_block->ports()) {
		if (port->portAlign() == QNEPort::Output) {
			foreach(QNEConnection *conn, port->connections()) {
				QNEBlock *next = conn->port2()->m_block;
				if (conn->port2()->portFlags() == QNEPort::SetSubInputPort) {
					next->mBlock->CellIndexListInput_Sub.clear();
					std::copy(CellIndexListOutput.begin(), CellIndexListOutput.end(), std::back_inserter(next->mBlock->CellIndexListInput_Sub));
					next->mBlock->m_block->labelIndex2 = m_block->labelIndex;
					next->mBlock->updatedInputListLoading();

				}
				else {
					next->mBlock->CellIndexListInput.clear();
					std::copy(CellIndexListOutput.begin(), CellIndexListOutput.end(), std::back_inserter(next->mBlock->CellIndexListInput));

					next->mBlock->m_block->labelIndex = m_block->labelIndex;
					next->mBlock->updatedInputListLoading();

				}

			}

		}
	}
}


void BlockWidget::set_operation(int index) {
	if (m_block->labelIndex==-1 || m_block->labelIndex != m_block->labelIndex2) {
		return;
	}
	std::map<unsigned int, bool> temp;
	if (index == 0) { // Union
		std::list<unsigned int>::iterator iter = CellIndexListInput.begin();
		for (iter = CellIndexListInput.begin(); iter != CellIndexListInput.end(); ++iter) {
			temp[*iter] = true;
		}
		for (iter = CellIndexListInput_Sub.begin(); iter != CellIndexListInput_Sub.end(); ++iter) {
			temp[*iter] = true;
		}
		std::map<unsigned int, bool>::iterator mapiter = temp.begin();
		for (mapiter = temp.begin(); mapiter != temp.end(); ++mapiter) {
			CellIndexListOutput.push_back(mapiter->first);
		}
	}
	else if (index == 1) { // Subtraction
		std::list<unsigned int>::iterator iter = CellIndexListInput.begin();
		for (iter = CellIndexListInput.begin(); iter != CellIndexListInput.end(); ++iter) {
			temp[*iter] = true;
		}
		for (iter = CellIndexListInput_Sub.begin(); iter != CellIndexListInput_Sub.end(); ++iter) {
			std::map<unsigned int, bool>::iterator mapiter = temp.find(*iter);
			if (mapiter != temp.end()) {
				temp.erase(mapiter);
			}
		}
		std::map<unsigned int, bool>::iterator mapiter = temp.begin();
		for (mapiter = temp.begin(); mapiter != temp.end(); ++mapiter) {
			CellIndexListOutput.push_back(mapiter->first);
		}
	}
	else if (index == 2) { //Intersection
		std::list<unsigned int>::iterator iter = CellIndexListInput.begin();
		for (iter = CellIndexListInput.begin(); iter != CellIndexListInput.end(); ++iter) {
			temp[*iter] = true;
		}
		for (iter = CellIndexListInput_Sub.begin(); iter != CellIndexListInput_Sub.end(); ++iter) {
			if (temp.find(*iter) != temp.end()) {
				CellIndexListOutput.push_back(*iter);
			}
		}
	}
}

void BlockWidget::handleIconBtnRelease() {
	iconPressed = false;
	if (iconMoved) {
		iconMoved = false;
		return;
	}

	//	globalCursorPos = QCursor::pos();
	QPoint start = QCursor::pos();
	QPoint before = QCursor::pos();
	QPoint now;

	if (BlockFlag == SubsetExplorerBlock) {
		if (inputUpdated) {
			inputUpdated = false;
			white_list.clear();
			black_list.clear();
			selected_list.clear();
			selectPoints.clear();
			is_selection = false;
			if (IsForegroundByRF != NULL) {
				delete[]IsForegroundByRF;
			}
			IsForegroundByRF = new bool[CellIndexListInput.size()];
			for (int i = 0; i < CellIndexListInput.size(); i++)IsForegroundByRF[i] = true;
//			CellIndexListOutput.clear();
//			CellIndexListOutput = CellIndexListInput;
		}
		mGlobals.mainWindow->mContents->Cell_Explorer->curBlock = this;
		mGlobals.mainWindow->mContents->Cell_Explorer->IsNewJob = true;
		emit mGlobals.mainWindow->mContents->Cell_Explorer->cell_Update();
		return;

	}

	if (iconActivated == 1) {
		proxy->hide();
//		m_block->pScene->removeItem(proxy);
		timer->stop();
		//openedWidget->close();
		iconActivated = 0;
		return;
	}
	qDebug() << "handleIconBtn";
	proxy->show();
	timer->start(100);
	
//	openedWidget->setGeometry(globalCursorPos.x(), globalCursorPos.y(), 300, 400);
//	openedWidget->show();
	iconActivated = 1;
}
void BlockWidget::handleIconBtnPress() {
	iconPressed = true;
	prevPos = QCursor::pos();
}
void BlockWidget::syncPos() {
	proxy->setPos(m_block->pos()+QPointF(40,40));
	proxy->setZValue(50);

}
void BlockWidget::generateIcon(int type) {
	IconBtn = new QPushButton;
	QFont tf;
	tf.setBold(true);
	tf.setPointSize(12);
	IconBtn->setFont(tf);
	IconBtn->setFixedSize(40, 40);
	QPalette pal=IconBtn->palette();
	pal.setColor(QPalette::Background, Qt::darkGray);
	IconBtn->setPalette(pal);

	connect(IconBtn, SIGNAL(released()), this, SLOT(handleIconBtnRelease()));
	connect(IconBtn, SIGNAL(pressed()), this, SLOT(handleIconBtnPress()));
	openedWidget = new QWidget;
	//openedWidget->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
	openedLayout = new QVBoxLayout;
	openedWidget->setLayout(openedLayout);
	proxy = m_block->pScene->addWidget(openedWidget);
	//proxy->setPos(m_block->pos());
	proxy->setGeometry(QRectF(m_block->pos().x() + 40, m_block->pos().y() + 40, 300, 400));
	proxy->hide();
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &BlockWidget::syncPos);
	QFont font;
	font.setPointSize(12);
	font.setBold(true);
	//font.setBold(false);
	blockName_edit = new QLineEdit(QString(BlockName.c_str()));
	blockName_edit->setFont(font);
	blockName_edit->setAutoFillBackground(false);
	QPalette pal2 = blockName_edit->palette();
	pal2.setColor(QPalette::Background,QColor(0,0,0,0));
	pal2.setColor(QPalette::Foreground, QColor(0, 0, 0, 0));
	pal2.setColor(QPalette::Base, QColor(0, 0, 0, 0));
	blockName_edit->setPalette(pal2);


	font.setPointSize(8);
	font.setBold(false);
	QLabel *nodeType = new QLabel;
	nodeType->setFont(font);

	openedLayout->addWidget(blockName_edit);
	openedLayout->addWidget(nodeType);

	if (type == QNEPort::SubregionWidgetPort) {
		IconBtn->setText("M");
		nodeType->setText("Subregion Operation (Masking)");
		openedLayout->addSpacing(10);

		openedLayout->addWidget(SubregionInformationMaster);
		openedLayout->addWidget(SubregionDropdownMaster);
		openedWidget->setFixedHeight(330);
	}
	else if (type == QNEPort::CutPlanePort) {
		IconBtn->setText("C");
		nodeType->setText("Subregion Operation (Cut plane)");
		openedLayout->addSpacing(10);

		openedLayout->addWidget(SubregionInformationMaster);
		openedLayout->addWidget(SubregionDropdownMaster);
		openedWidget->setFixedHeight(350);

	}
	else if (type == QNEPort::SetWidgetPort) {
		IconBtn->setText("S");
		nodeType->setText("Cell Operation (Set)");
		openedLayout->addWidget(SetDropdownMaster);
		openedWidget->setFixedSize(200,130);

	}
	else if (type == QNEPort::FeatureWidgetPort) {
		IconBtn->setText("F");
		nodeType->setText("Cell Operation (Thresholding)");
		openedLayout->addWidget(FeatureHistogramMaster);
		openedLayout->addWidget(FeatureDropdownMaster);


	}
	else if (type == QNEPort::SubsetExplorerPort) {
		IconBtn->setText("E");
		nodeType->setText("Cell Operation (Subset exploration)");
		openedWidget->hide();

	}
}