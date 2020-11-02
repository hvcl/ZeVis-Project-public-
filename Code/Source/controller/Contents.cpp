#include "Contents.h"

Contents::Contents(QWidget *parent)
	: QObject(parent)
{
	mWidget = parent;
}
Contents::Contents(QWidget *parent, QLayout *parent_layout)
	: QObject(parent)
{
	mWidget = parent;
	
	

	QSplitter * splitter = new QSplitter(mWidget);

	contents_left_tabwidget = new QTabWidget(mWidget);
	contents_center_tabwidget = new QTabWidget(mWidget);


	contents_left_tabwidget->addTab(new QWidget(), "Project");
	contents_left_tabwidget->addTab(new QWidget(), "Labels && Subregions");
	contents_left_tabwidget->addTab(new QWidget(), "Annotations");

	contents_left_tabwidget->setMouseTracking(true);


	volume_layout = new QHBoxLayout;
	QWidget *volume_widget = new QWidget;
	volume_widget->setLayout(volume_layout);


	contents_center_tabwidget->addTab(new QWidget(), "4-View");
	contents_center_tabwidget->addTab(new QWidget(), "XY");
	contents_center_tabwidget->addTab(new QWidget(), "YZ");
	contents_center_tabwidget->addTab(new QWidget(), "ZX");
	contents_center_tabwidget->addTab(volume_widget, "3D");
	contents_center_tabwidget->addTab(new QWidget(), "Cell Subset Explorer");

	contents_center_tabwidget->addTab(new QWidget(), "Graph");
	connect(contents_center_tabwidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

	splitter->addWidget(contents_left_tabwidget);
	splitter->addWidget(contents_center_tabwidget);

	QList<int> sizes;
	sizes << 480 << 1440;
	splitter->setSizes(sizes);

	parent_layout->addWidget(splitter);


}


Contents::~Contents()
{
}
void Contents::tabChanged(int index) {
	if (GL_Volume_Main == NULL)return;
	if (index == 0) {
		second_lay->addWidget(GL_Volume_Main);
		QList<int> sizes;
		sizes << 720 << 720;
		second_lay->setSizes(sizes);
	}
	else if (index == 4) {
		volume_layout->addWidget(GL_Volume_Main);
	}
}
void Contents::LeftTabInit(QTabWidget *target, int width, int height) {
	//target->setFixedSize(width, height);

	target->addTab(new QWidget(), "Project");
	target->addTab(new QWidget(), "Labels && Subregions");
}
void Contents::CenterTabInit(QTabWidget *target, int width, int height) {
	//target->setFixedSize(width, height);
	
	target->addTab(new QWidget(), "4-View");
	target->addTab(new QWidget(), "XY");
	target->addTab(new QWidget(), "YZ");
	target->addTab(new QWidget(), "ZX");
	target->addTab(new QWidget(), "3D");
	target->addTab(new QWidget(), "Graph");


}

void Contents::replaceTab(QTabWidget * tabs, int index, QWidget * replacement, QString label)
{
	if (replacement == NULL)return;
	Q_ASSERT(tabs && tabs->count() > index);
	tabs->removeTab(index);
	tabs->insertTab(index, replacement, label);
}
void Contents::updateAllWidget(int index, bool scaleflag) {
	GL_XYAxis_Main->update();
	GL_YZAxis_Main->update();
	GL_ZXAxis_Main->update();
	GL_XYAxis_Sub->update();
	GL_YZAxis_Sub->update();
	GL_ZXAxis_Sub->update();
}


void Contents::InitGLView() {

	if (GL_XYAxis_Main != NULL) {
		delete GL_XYAxis_Main;
	}
	GL_XYAxis_Main = new ViewAxisGLWidget;
	GL_XYAxis_Main->setAxisIndex(1, 0);
	connect(GL_XYAxis_Main, SIGNAL(updateAll(int, bool)), this, SLOT(updateAllWidget(int, bool)));

	if (GL_YZAxis_Main != NULL) {
		delete GL_YZAxis_Main;
	}
	GL_YZAxis_Main = new ViewAxisGLWidget;
	GL_YZAxis_Main->setAxisIndex(2, 1);
	connect(GL_YZAxis_Main, SIGNAL(updateAll(int, bool)), this, SLOT(updateAllWidget(int, bool)));

	if (GL_ZXAxis_Main != NULL) {
		delete GL_ZXAxis_Main;
	}
	GL_ZXAxis_Main = new ViewAxisGLWidget;
	GL_ZXAxis_Main->setAxisIndex(3, 2);
	connect(GL_ZXAxis_Main, SIGNAL(updateAll(int, bool)), this, SLOT(updateAllWidget(int, bool)));


	//GL_Volume_Main->setBackgroundVolumeTF(mGlobals.CurrentProject->mLayerBack->transferFunc);

	if (GL_Volume_Main != NULL) {
		delete GL_Volume_Main;
	}
	GL_Volume_Main = new ViewVolumeGLWidget;
	
	//if (GL_Volume_Sub != NULL) {
	//	delete GL_Volume_Sub;
	//}
	//GL_Volume_Sub = new ViewVolumeGLWidget;

	replaceTab(contents_center_tabwidget, 1, GL_XYAxis_Main, "XY");
	replaceTab(contents_center_tabwidget, 2, GL_YZAxis_Main, "YZ");
	replaceTab(contents_center_tabwidget, 3, GL_ZXAxis_Main, "ZX");
	//replaceTab(contents_center_tabwidget, 4, GL_Volume_Main, "3D");

	//GL_Volume_Main->setBackgroundVolume(mGlobals.CurrentProject->mLayerBack->volumeRenderData, width, height, depth);
	//GL_Volume_Main->setBackgroundVolumeTF(mGlobals.CurrentProject->mLayerBack->transferFunc);

	if (GL_XYAxis_Sub != NULL) {
		delete GL_XYAxis_Sub;
	}
	GL_XYAxis_Sub = new ViewAxisGLWidget;
	GL_XYAxis_Sub->setAxisIndex(1, 3);
	connect(GL_XYAxis_Sub, SIGNAL(updateAll(int, bool)), this, SLOT(updateAllWidget(int, bool)));

	if (GL_YZAxis_Sub != NULL) {
		delete GL_YZAxis_Sub;
	}
	GL_YZAxis_Sub = new ViewAxisGLWidget;
	GL_YZAxis_Sub->setAxisIndex(2, 4);
	connect(GL_YZAxis_Sub, SIGNAL(updateAll(int, bool)), this, SLOT(updateAllWidget(int, bool)));

	if (GL_ZXAxis_Sub != NULL) {
		delete GL_ZXAxis_Sub;
	}
	GL_ZXAxis_Sub = new ViewAxisGLWidget;
	GL_ZXAxis_Sub->setAxisIndex(3, 5);
	connect(GL_ZXAxis_Sub, SIGNAL(updateAll(int, bool)), this, SLOT(updateAllWidget(int, bool)));

	//GL_Volume_Sub = new ViewVolumeGLWidget;


	QSplitter *view4_layout = new QSplitter;
	view4_layout->setOrientation(Qt::Vertical);
	QSplitter *first_lay = new QSplitter;
	first_lay->setOrientation(Qt::Horizontal);
	second_lay = new QSplitter;
	second_lay->setOrientation(Qt::Horizontal);
	first_lay->addWidget(GL_XYAxis_Sub);
	first_lay->addWidget(GL_YZAxis_Sub);
	second_lay->addWidget(GL_ZXAxis_Sub);
	//second_lay->addWidget(GL_Volume_Main);

	QList<int> sizes;
	sizes << 720 << 720;
	second_lay->setSizes(sizes);
	first_lay->setSizes(sizes);

	view4_layout->addWidget(first_lay);
	view4_layout->addWidget(second_lay);
	
	
	replaceTab(contents_center_tabwidget, 0, view4_layout, "4-View");

	if (Cell_Explorer != NULL) {
		delete Cell_Explorer;
	}
	Cell_Explorer = new CellSubsetExplorer(new QWidget());

	replaceTab(contents_center_tabwidget, 5, Cell_Explorer, "Cell Subset Explorer");

}

void Contents::InitGraph() {
	QWidget *widget = new QWidget;
	graphEditorLayout = new QVBoxLayout;
	if (GraphEditorView != NULL) {
		delete GraphEditorView;
	}

	GraphEditorViewScale = new QSlider(Qt::Horizontal);
	GraphEditorViewScale->setMinimum(1);
	GraphEditorViewScale->setMaximum(300);
	GraphEditorViewScale->setValue(100);

	GraphEditorViewScaleLabel = new QLabel("100%");
	connect(GraphEditorViewScale, SIGNAL(valueChanged(int)), this, SLOT(GraphEditorViewScaleChange(int)));

	QHBoxLayout *layout2 = new QHBoxLayout;
	layout2->addSpacing(200);
	layout2->addWidget(GraphEditorViewScale);
	layout2->addWidget(GraphEditorViewScaleLabel);



	b_icon_add = new QPushButton;
	b_icon_add->setIcon(QIcon("Resources/icon_addnode.png"));
	b_icon_add->setIconSize(QSize(30, 30));
	b_icon_add->setFixedSize(QSize(45, 45));
	connect(b_icon_add, SIGNAL(released()), this, SLOT(handleAddNode()));
	b_icon_add->setWindowFlags(Qt::WindowStaysOnTopHint);

	QWidget *filler0 = new QWidget;
	filler0->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);


	buttonLayout = new QVBoxLayout;
	buttonLayout->setContentsMargins(10, 10, 10, 10);
	buttonLayout->addWidget(b_icon_add);
	buttonLayout->addWidget(filler0);

	


	overlapedLayout = new QGridLayout;
	overlapedLayout->setAlignment(Qt::AlignLeft);
	overlapedLayout->addLayout(graphEditorLayout, 0, 0);
	overlapedLayout->addLayout(buttonLayout, 0, 0);

	QVBoxLayout *layout3 = new QVBoxLayout;
	layout3->addLayout(overlapedLayout);
	layout3->addLayout(layout2);


	GraphEditorView = new ViewGraphEditor(mWidget);
	GraphEditorView->initialize(graphEditorLayout);
	//GraphEditorView->addOrigin();
	widget->setLayout(layout3);
	replaceTab(contents_center_tabwidget, 6, widget, "Graph");
}
void Contents::handleAddNode() {
	qDebug("Open Add node handle");
	mGlobals.mDialogManager->mDialogAddNode->exec();
}
void Contents::InitProjectInfo() {
	QWidget *ProjectInfo_Container = new QWidget;
	contents_projectinfo_master = new Contents_ProjectInfo(ProjectInfo_Container);
	connect(mGlobals.mainWindow->mTopToolbar,SIGNAL(loadProjectInfo()) , contents_projectinfo_master, SLOT(handleLoad()));
	connect(contents_projectinfo_master, SIGNAL(emLoaded()), GL_XYAxis_Main, SLOT(emLoaded()));
	connect(contents_projectinfo_master, SIGNAL(emLoaded()), GL_XYAxis_Sub, SLOT(emLoaded()));
	connect(contents_projectinfo_master, SIGNAL(emLoaded()), GL_YZAxis_Main, SLOT(emLoaded()));
	connect(contents_projectinfo_master, SIGNAL(emLoaded()), GL_YZAxis_Sub, SLOT(emLoaded()));
	connect(contents_projectinfo_master, SIGNAL(emLoaded()), GL_ZXAxis_Main, SLOT(emLoaded()));
	connect(contents_projectinfo_master, SIGNAL(emLoaded()), GL_ZXAxis_Sub, SLOT(emLoaded()));
	connect(contents_projectinfo_master, SIGNAL(emLoaded()), GL_Volume_Main, SLOT(emLoaded()));

	replaceTab(contents_left_tabwidget, 0, ProjectInfo_Container, "Project");
}
void Contents::InitAnnotations() {
	QWidget *Annotations_Container = new QWidget;
	annotationWidget = new Contents_Annotations(Annotations_Container);
	replaceTab(contents_left_tabwidget, 2, Annotations_Container, "Annotations");

	connect(GL_XYAxis_Main, SIGNAL(addAnnotation(QString, QColor, QVector3D, QVector3D, int)),
		annotationWidget, SLOT(addAnnotation(QString, QColor, QVector3D, QVector3D, int)));
	connect(GL_XYAxis_Sub, SIGNAL(addAnnotation(QString, QColor, QVector3D, QVector3D, int)),
		annotationWidget, SLOT(addAnnotation(QString, QColor, QVector3D, QVector3D, int)));

	connect(GL_ZXAxis_Main, SIGNAL(addAnnotation(QString, QColor, QVector3D, QVector3D, int)),
		annotationWidget, SLOT(addAnnotation(QString, QColor, QVector3D, QVector3D, int)));
	connect(GL_ZXAxis_Sub, SIGNAL(addAnnotation(QString, QColor, QVector3D, QVector3D, int)),
		annotationWidget, SLOT(addAnnotation(QString, QColor, QVector3D, QVector3D, int)));

	connect(GL_YZAxis_Main, SIGNAL(addAnnotation(QString, QColor, QVector3D, QVector3D, int)),
		annotationWidget, SLOT(addAnnotation(QString, QColor, QVector3D, QVector3D, int)));
	connect(GL_YZAxis_Sub, SIGNAL(addAnnotation(QString, QColor, QVector3D, QVector3D, int)),
		annotationWidget, SLOT(addAnnotation(QString, QColor, QVector3D, QVector3D, int)));

}

void Contents::InitSubregionFeatureList() {
	QVBoxLayout *layout = new QVBoxLayout;
	QWidget *label = new QWidget(mWidget);
	QWidget *feature = new QWidget(mWidget);
	QVBoxLayout *label_layout = new QVBoxLayout;
	QHBoxLayout *label_title_layout = new QHBoxLayout;

	QVBoxLayout *feature_layout = new QVBoxLayout;
	QHBoxLayout *feature_title_layout = new QHBoxLayout;

	if (labelList != NULL) {
		delete labelList;
	}
	labelList = new QListWidget(mWidget);
	labelList->setContentsMargins(1, 1, 1, 1);
	//QListWidgetItem *newItem = new QListWidgetItem(QString(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->CellHeaderPath.c_str()));
	//newItem->setSizeHint(QSize(labelList->width(), 30));
	//labelList->addItem(newItem);

	connect(labelList, SIGNAL(currentRowChanged(int)), this, SLOT(handleLabelChange(int)));


	if (featureList != NULL) {
		delete featureList;
	}
	featureList = new QListWidget(mWidget);

	QFont title_font("Arial", 11, QFont::Bold);


	QLabel *label_title = new QLabel(mWidget);
	label_title->setText("Cell Label");
	label_title->setFont(title_font);
	QWidget *filler0 = new QWidget;
	filler0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QPushButton *labelAddBtn = new QPushButton(mWidget);
	labelAddBtn->setText("Add");
	label_title_layout->addWidget(label_title);
	label_title_layout->addWidget(filler0);
	label_title_layout->addWidget(labelAddBtn);
	connect(labelAddBtn, SIGNAL(released()), this, SLOT(handleLabelAddBtn()));




	QLabel *feature_title = new QLabel(mWidget);
	feature_title->setText("Feature");
	feature_title->setFont(title_font);
	QPushButton *featureAddBtn = new QPushButton(mWidget);
	featureAddBtn->setText("Add");
	QWidget *filler2 = new QWidget;
	filler2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	feature_title_layout->addWidget(feature_title);
	feature_title_layout->addWidget(filler2);
	feature_title_layout->addWidget(featureAddBtn);
	connect(featureAddBtn, SIGNAL(released()), this, SLOT(handleFeatureAddBtn()));


	label_layout->addLayout(label_title_layout);
	label_layout->addWidget(labelList);



	QWidget *subregion = new QWidget(mWidget);
	QVBoxLayout *subregion_layout = new QVBoxLayout;
	QHBoxLayout *subregion_title_layout = new QHBoxLayout;

	if (subregionList != NULL) {
		delete subregionList;
	}
	subregionList = new QListWidget(mWidget);


	QLabel *subregion_title = new QLabel(mWidget);
	subregion_title->setText("Subregion");
	subregion_title->setFont(title_font);
	QWidget *filler1 = new QWidget;
	filler1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QPushButton *subregionAddBtn = new QPushButton(mWidget);
	subregionAddBtn->setText("Add");
	subregion_title_layout->addWidget(subregion_title);
	subregion_title_layout->addWidget(filler1);
	subregion_title_layout->addWidget(subregionAddBtn);
	connect(subregionAddBtn, SIGNAL(released()), this, SLOT(handleSubregionAddBtn()));
	subregion_layout->addLayout(subregion_title_layout);
	subregion_layout->addWidget(subregionList);
	//feature_layout->addLayout(feature_title_layout);
	feature_layout->addWidget(featureList);

	label->setLayout(label_layout);
	subregion->setLayout(subregion_layout);
	feature->setLayout(feature_layout);


	QFrame *buffer = new QFrame(mWidget);
	buffer->setFrameStyle(QFrame::HLine | QFrame::Plain);
	buffer->setStyleSheet("color:rgb(100,100,100)");
	buffer->setLineWidth(2.5);

	layout->addWidget(label);
	layout->addWidget(feature);
	layout->addWidget(buffer);
	layout->addWidget(subregion);

	QWidget *FeatureList_Container = new QWidget;
	FeatureList_Container->setLayout(layout);
	replaceTab(contents_left_tabwidget, 1, FeatureList_Container, "Labels && Subregions");
	feature_updated();

}
void Contents::handleLabelChange(int newLabel) {
	mGlobals.CurrentProject->curLayerCell = newLabel;
	feature_updated();
	updateAllWidget(0, 0);
}
void Contents::updateColorBox(QPushButton *target, QColor color) {
	target->setStyleSheet("");
	QPalette mPalette = target->palette();
	mPalette.setColor(target->backgroundRole(), color);
	target->setPalette(mPalette);
	target->setStyleSheet(QString("background-color: %1;foreground-color: %1; border-style: none;").arg(color.name()));
	target->setAutoFillBackground(true);
	target->setFlat(true);
	target->repaint();
}

void Contents::handleLabelAddBtn() {

	QString filters("Cell label file (*.lbl);;");
	QString dir = QFileDialog::getOpenFileName(mWidget, "Open cell label header", QDir::currentPath(), filters);
	if (dir == "") {
		return;
	}
	mGlobals.mainWindow->mTopToolbar->curState->setText("Cell label loading...");
	mGlobals.mainWindow->mTopToolbar->curState->update();
	mGlobals.mainWindow->mTopToolbar->curState->repaint();
	setLabel(dir);
	mGlobals.mainWindow->mTopToolbar->curState->setText("");
	mGlobals.mainWindow->mTopToolbar->curState->update();
	mGlobals.mainWindow->mTopToolbar->curState->repaint();
	//emit mGlobals.mDialogManager->mDialogAddFeature->updatedFeature();

}
void Contents::setLabel(QString dir) {
	int curIndex = mGlobals.CurrentProject->maxLayerCell;
	mGlobals.CurrentProject->mLayerCell[curIndex] = new LayerCell(dir.toStdString());
	mGlobals.CurrentProject->mLayerCell[curIndex]->featureStartIndex = mGlobals.CurrentProject->mFeature.size();
	//mGlobals.CurrentProject->curLayerCell = mGlobals.CurrentProject->maxLayerCell;
	mGlobals.CurrentProject->maxLayerCell++;

	QListWidgetItem *newItem = new QListWidgetItem(dir);
	newItem->setSizeHint(QSize(labelList->width(), 30));
	labelList->addItem(newItem);

	mWidget->update();

	Utils mUtil;
	std::vector<std::string> BackDirPath = mUtil.Split(dir.toStdString().c_str(), "header.lbl");

	//	mGlobals.CurrentProject->mFeature.clear();
	QStringList strFilter2;
	strFilter2 += "*.dat";
	QDirIterator files2(QString(BackDirPath[0].c_str()) + "MorphologicalFeature/", strFilter2, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	while (files2.hasNext()) {
		files2.next();
		mGlobals.CurrentProject->AddFeature(files2.filePath(), 0);
		qDebug() << files2.filePath();
	}
	mGlobals.CurrentProject->mLayerCell[curIndex]->featureEndIndex = mGlobals.CurrentProject->mFeature.size();

}

void Contents::handleFeatureAddBtn() {
	mGlobals.mDialogManager->mDialogAddFeature->exec();
}
void Contents::feature_updated() {
	qDebug() << "feature_updated";
	featureList->clear();
	featureList->setContentsMargins(1, 1, 1, 1);
	if (mGlobals.CurrentProject->curLayerCell == -1)return;
	int index = 0;
//	std::vector<DataFeature>::iterator iter;
//	for (iter = mGlobals.CurrentProject->mFeature.begin(); iter != mGlobals.CurrentProject->mFeature.end(); ++iter) {

	int startIndex = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->featureStartIndex;
	int endIndex = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->featureEndIndex;

	for(int i= startIndex;i<endIndex;i++){

		QWidget *widget = new QWidget(mWidget);
		QHBoxLayout *list_item_layout= new QHBoxLayout;
		QFont font;
		font.setPointSize(10);
		font.setBold(true);

		QLabel *name= new QLabel(mWidget);
		name->setFont(font);
		name->setText(QString::fromStdString(mGlobals.CurrentProject->mFeature[i].FeatureName));
		
		QPushButton *detailBtn = new QPushButton(mWidget);
		detailBtn->setIcon(QIcon("Resources/icon_detail.png"));
		detailBtn->setObjectName(QString::fromStdString(std::to_string(i)));
		detailBtn->setIconSize(QSize(15, 15));
		detailBtn->setFixedSize(QSize(20, 20));
		connect(detailBtn, SIGNAL(clicked()), this, SLOT(handleDetailBtn_feature()));

		QPushButton *deleteBtn = new QPushButton(mWidget);
		deleteBtn->setIcon(QIcon("Resources/icon_trash.png"));
		deleteBtn->setObjectName(QString::fromStdString(std::to_string(i)));
		deleteBtn->setIconSize(QSize(15, 15));
		deleteBtn->setFixedSize(QSize(20, 20));
		connect(deleteBtn, SIGNAL(clicked()), this, SLOT(handleDeleteBtn_feature()));
		

		QWidget *filler2 = new QWidget;
		filler2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		list_item_layout->addWidget(name);
		list_item_layout->addWidget(filler2);
		list_item_layout->addWidget(detailBtn);
		list_item_layout->addWidget(deleteBtn);
		widget->setLayout(list_item_layout);
		QListWidgetItem *item = new QListWidgetItem();
		
		item->setSizeHint(widget->sizeHint());

		featureList->addItem(item);
		featureList->setItemWidget(item, widget);

		index++;
	}


}



void Contents::handleDetailBtn_feature() {
	QObject *senderObj = sender(); // This will give Sender object
	QString senderObjName = senderObj->objectName();
	qDebug() << "detail Button: " << senderObjName;
	int index = std::stoi(senderObjName.toStdString());
	mGlobals.mDialogManager->mDialogDetailFeature->setIndexAndDraw(index);
	mGlobals.mDialogManager->mDialogDetailFeature->exec();
}



void Contents::handleDeleteBtn_feature() {
	QObject *senderObj = sender(); // This will give Sender object
	QString senderObjName = senderObj->objectName();
	int index = std::stoi(senderObjName.toStdString());
	qDebug() << "delete Button feature: " << senderObjName;

	mGlobals.CurrentProject->removeFeature(index);
	featureList->removeItemWidget(featureList->takeItem(index));
}





void Contents::loadSubregionFeature() {
	mGlobals.CurrentProject->mSubregion.clear();
	QStringList strFilter;
	strFilter += "*.srl";
	QDirIterator files(mGlobals.CurrentProject->SubregionPath + "/", strFilter, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	while (files.hasNext()) {
		files.next();
		mGlobals.CurrentProject->AddSubregion(files.filePath());
		qDebug() << files.filePath();
	}
	emit mGlobals.mDialogManager->mDialogAddSubregion->updatedSubregion();


	mGlobals.CurrentProject->mFeature.clear();
	QStringList strFilter2;
	strFilter2 += "*.dat";
	QDirIterator files2(mGlobals.CurrentProject->FeaturePath + "/", strFilter2, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	while (files2.hasNext()) {
		files2.next();
		mGlobals.CurrentProject->AddFeature(files2.filePath(), 0);
		qDebug() << files2.filePath();
	}
	emit mGlobals.mDialogManager->mDialogAddFeature->updatedFeature();
}

void Contents::handleSubregionAddBtn() {
//	mGlobals.mDialogManager->mDialogAddSubregion->exec();
	QString filters("Subregion Layer File (*.srl);;");
	QString dir = QFileDialog::getOpenFileName(mWidget, "Open Directory", QDir::currentPath(), filters);
	if (dir == "")return;
//	SubregionPath->setText(dir); 
	mGlobals.CurrentProject->AddSubregion(dir);
	emit mGlobals.mDialogManager->mDialogAddSubregion->updatedSubregion();
}

void Contents::subregion_updated() {


	subregionList->clear();
	subregionList->setContentsMargins(1, 1, 1, 1);
	int index = 0;
	std::vector<LayerSubregion>::iterator iter;
	for (iter = mGlobals.CurrentProject->mSubregion.begin(); iter != mGlobals.CurrentProject->mSubregion.end(); ++iter) {

		QWidget *widget = new QWidget(mWidget);
		QHBoxLayout *list_item_layout = new QHBoxLayout;
		QFont font;
		font.setPointSize(10);
		font.setBold(true);

		iter->SubregionIndex = index;

		QLabel *name = new QLabel(mWidget);
		name->setFont(font);
		name->setText(QString::fromStdString(iter->SubregionName));

		//QPushButton *infoBtn = new QPushButton(mWidget);
		//infoBtn->setIcon(QIcon("Resources/icon_info.png"));
		//infoBtn->setObjectName(QString::fromStdString(std::to_string(index)));
		//infoBtn->setIconSize(QSize(15, 15));
		//infoBtn->setFixedSize(QSize(20, 20));
		//connect(infoBtn, SIGNAL(clicked()), this, SLOT(handleInfoBtn_subregion()));


		QPushButton *colorbtn = new QPushButton;
		colorbtn->setObjectName(QString::fromStdString(std::to_string(index)));
		colorbtn->setFixedSize(QSize(15, 15));
		colorbtn->setFlat(true);
		colorbtn->setAutoFillBackground(true);
		updateColorBox(colorbtn, iter->SubregionColor);
		connect(colorbtn, SIGNAL(released()), this, SLOT(handleColorBtn_subregion()));

		QPushButton *deleteBtn = new QPushButton(mWidget);
		deleteBtn->setIcon(QIcon("Resources/icon_trash.png"));
		deleteBtn->setObjectName(QString::fromStdString(std::to_string(index)));
		deleteBtn->setIconSize(QSize(15, 15));
		deleteBtn->setFixedSize(QSize(20, 20));
		connect(deleteBtn, SIGNAL(clicked()), this, SLOT(handleDeleteBtn_subregion()));

		QSlider *slider = new QSlider(Qt::Horizontal);
		slider->setObjectName(QString::fromStdString(std::to_string(index)));
		slider->setFixedWidth(100);
		slider->setFocusPolicy(Qt::StrongFocus);
		slider->setTickPosition(QSlider::TicksBothSides);
		slider->setTickInterval(10);
		slider->setSingleStep(1);
		slider->setValue(iter->SubregionOpacity * 100);
		connect(slider, SIGNAL(valueChanged(int)), this, SLOT(handleOpacity(int)));


		QWidget *filler2 = new QWidget;
		filler2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		list_item_layout->addWidget(name);
		list_item_layout->addWidget(filler2);
		//list_item_layout->addWidget(infoBtn);
		list_item_layout->addWidget(slider);
		list_item_layout->addWidget(colorbtn);
		//list_item_layout->addWidget(CheckBox);
		list_item_layout->addWidget(deleteBtn);
		widget->setLayout(list_item_layout);
		QListWidgetItem *item = new QListWidgetItem();

		item->setSizeHint(widget->sizeHint());

		subregionList->addItem(item);
		subregionList->setItemWidget(item, widget);

		index++;
	}
	updateAllWidget(0, false);
}


void Contents::handleOpacity(int value) {
	QObject *senderObj = sender(); // This will give Sender object
	QString senderObjName = senderObj->objectName();
	int index = std::stoi(senderObjName.toStdString());
	mGlobals.CurrentProject->mSubregion[index].SubregionOpacity = (float)value / 100.0f;
	qDebug() << index << " " << mGlobals.CurrentProject->mSubregion[index].SubregionOpacity;
	updateAllWidget(0, false);
}


void Contents::handleDeleteBtn_subregion() {
	QObject *senderObj = sender(); // This will give Sender object
	QString senderObjName = senderObj->objectName();
	int index = std::stoi(senderObjName.toStdString());
	qDebug() << "delete Button: subregion" << senderObjName;

	mGlobals.CurrentProject->removeSubregion(index);
	subregion_updated();
}
void Contents::handleInfoBtn_subregion() {
	QObject *senderObj = sender(); // This will give Sender object
	QString senderObjName = senderObj->objectName();
	int index = std::stoi(senderObjName.toStdString());
	qDebug() << "info Button: subregion" << senderObjName;
	mGlobals.mDialogManager->mDialogInfoSubregion->setIndexAndDraw(index);
	mGlobals.mDialogManager->mDialogInfoSubregion->exec();
}


void Contents::handleColorBtn_subregion() {
	QObject *senderObj = sender(); // This will give Sender object
	QString senderObjName = senderObj->objectName();
	int index = std::stoi(senderObjName.toStdString());
	QColorDialogTester color_test;
	QString color_str = color_test.onColor();
	qDebug() << color_str;

	if (color_str.length() > 2) {
		mGlobals.CurrentProject->mSubregion[index].SubregionColor = QColor(color_str);
		subregion_updated();
	}

}

void Contents::GraphEditorViewScaleChange(int v) {
	GraphEditorViewScaleLabel->setText(QString::number(v) + "%");
	GraphEditorView->GraphViewScale(v);
}