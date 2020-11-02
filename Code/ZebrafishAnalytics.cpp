#include "ZebrafishAnalytics.h"

ZebrafishAnalytics::ZebrafishAnalytics()
{
	QWidget *main_widget = new QWidget;
	setCentralWidget(main_widget);

	QVBoxLayout *main_layout = new QVBoxLayout;
	main_widget->setLayout(main_layout);

	mGlobals.mDialogManager = new DialogManager(this);

	QHBoxLayout *top_toolbar_layout = new QHBoxLayout;
	top_toolbar_layout->setAlignment(Qt::AlignLeft);
	top_toolbar_layout->setMargin(1);
	QWidget *top_widget = new QWidget;
	mTopToolbar = new TopToolbar(top_widget, top_toolbar_layout);

	QHBoxLayout *top_interface_layout = new QHBoxLayout;
	top_interface_layout->setAlignment(Qt::AlignLeft);
	top_interface_layout->setMargin(1);
	QWidget *top_interface_widget = new QWidget;
	mTopInterface = new TopInterface(top_interface_widget, top_interface_layout);


	QHBoxLayout *contents_layout = new QHBoxLayout;
	contents_layout->setAlignment(Qt::AlignLeft);
	contents_layout->setMargin(1);
	QWidget *contents_widget = new QWidget;
	contents_widget->setStyleSheet("background-color:red;");
	mContents = new Contents(contents_widget, contents_layout);


	main_layout->setMargin(0);
	main_layout->addLayout(top_toolbar_layout);
	main_layout->addLayout(top_interface_layout);
	main_layout->addLayout(contents_layout);


//	createAction();
//	createMenus();


	QString message = tr("");
	statusBar()->showMessage(message);


	connect(mGlobals.CurrentProject, SIGNAL(project_on()), this, SLOT(handleProjectOn()));

	timerID=startTimer(100);
}
void ZebrafishAnalytics::timerEvent(QTimerEvent *event) {
	//repaint();
	if (project_started == false) {
		mGlobals.CurrentProject->NewProject();
		killTimer(timerID);
	}
}
void ZebrafishAnalytics::connectAll() {

	connect(mContents->GL_XYAxis_Main, SIGNAL(update_view_state(int, int, int, float)), mTopInterface, SLOT(update_value(int, int, int, float)));
	connect(mTopInterface, SIGNAL(update_view(bool)), mContents->GL_XYAxis_Main, SLOT(updateByInterface(bool)));

	connect(mContents->GL_YZAxis_Main, SIGNAL(update_view_state(int, int, int, float)), mTopInterface, SLOT(update_value(int, int, int, float)));
	connect(mTopInterface, SIGNAL(update_view(bool)), mContents->GL_YZAxis_Main, SLOT(updateByInterface(bool)));

	connect(mContents->GL_ZXAxis_Main, SIGNAL(update_view_state(int, int, int, float)), mTopInterface, SLOT(update_value(int, int, int, float)));
	connect(mTopInterface, SIGNAL(update_view(bool)), mContents->GL_ZXAxis_Main, SLOT(updateByInterface(bool)));

	connect(mContents->GL_XYAxis_Sub, SIGNAL(update_view_state(int, int, int, float)), mTopInterface, SLOT(update_value(int, int, int, float)));
	connect(mTopInterface, SIGNAL(update_view(bool)), mContents->GL_XYAxis_Sub, SLOT(updateByInterface(bool)));

	connect(mContents->GL_YZAxis_Sub, SIGNAL(update_view_state(int, int, int, float)), mTopInterface, SLOT(update_value(int, int, int, float)));
	connect(mTopInterface, SIGNAL(update_view(bool)), mContents->GL_YZAxis_Sub, SLOT(updateByInterface(bool)));

	connect(mContents->GL_ZXAxis_Sub, SIGNAL(update_view_state(int, int, int, float)), mTopInterface, SLOT(update_value(int, int, int, float)));
	connect(mTopInterface, SIGNAL(update_view(bool)), mContents->GL_ZXAxis_Sub, SLOT(updateByInterface(bool)));

	connect(mContents->GL_ZXAxis_Sub, SIGNAL(update_view_state(int, int, int, float)), mTopInterface, SLOT(update_value(int, int, int, float)));
	connect(mTopInterface, SIGNAL(update_view(bool)), mContents->GL_ZXAxis_Sub, SLOT(updateByInterface(bool)));

	connect(mGlobals.mDialogManager->mDialogAddNode, SIGNAL(makenode(int, QString)), mContents->GraphEditorView, SLOT(addNode(int, QString)));
	connect(mGlobals.mDialogManager->mDialogAddFeature, SIGNAL(updatedFeature()), mContents, SLOT(feature_updated()));
	connect(mGlobals.mDialogManager->mDialogAddSubregion, SIGNAL(updatedSubregion()), mContents, SLOT(subregion_updated()));

	connect(mGlobals.mDialogManager->mDialogAddSubregion, SIGNAL(updatedSubregion()), mContents->GraphEditorView, SLOT(updateSubregionList()));
	connect(mGlobals.mDialogManager->mDialogAddFeature, SIGNAL(updatedFeature()), mContents->GraphEditorView, SLOT(updateFeatureList()));
}


void ZebrafishAnalytics::createMenus() {
	fileMenu = menuBar()->addMenu("File");
	fileMenu->addAction(newProjectAction);
	fileMenu->addAction(generateBackgroundLayer);
	fileMenu->addAction(generateLabelLayer);
	fileMenu->addAction(generateSubregionLayer);
	fileMenu->addAction(preference);
}
void ZebrafishAnalytics::createAction() {
	newProjectAction = new QAction("New Analytics Project", this);
	newProjectAction->setShortcuts(QKeySequence::New);
	newProjectAction->setStatusTip(tr("Generate Background Layer"));
	connect(newProjectAction, &QAction::triggered, this, &ZebrafishAnalytics::handleNewProject);

	newProjectAction = new QAction("Open Analytics Project", this);
	newProjectAction->setShortcuts(QKeySequence::Open);
	newProjectAction->setStatusTip(tr("Open Analytics Project"));
	connect(newProjectAction, &QAction::triggered, this, &ZebrafishAnalytics::handleOpenProject);

	generateBackgroundLayer = new QAction("Generate Background Layer", this);
	generateBackgroundLayer->setShortcut(tr("CTRL+SHIFT+B"));
	generateBackgroundLayer->setStatusTip(tr("Generate Background Layer"));
	connect(generateBackgroundLayer, &QAction::triggered, this, &ZebrafishAnalytics::handleGenerateBGLayer);

	generateLabelLayer = new QAction("Generate Labeled Layer", this);
	generateLabelLayer->setShortcut(tr("CTRL+SHIFT+L"));
	generateLabelLayer->setStatusTip(tr("Generate Labeled Layer"));
	connect(generateLabelLayer, &QAction::triggered, this, &ZebrafishAnalytics::handleGenerateLBLayer);

	generateSubregionLayer = new QAction("Generate Subregion Layer", this);
	generateSubregionLayer->setShortcut(tr("CTRL+SHIFT+S"));
	generateSubregionLayer->setStatusTip(tr("Generate Subregion Layer"));
	connect(generateSubregionLayer, &QAction::triggered, this, &ZebrafishAnalytics::handleGenerateSRLayer);

	preference = new QAction("Preference", this);
	preference->setShortcut(tr("CTRL+SHIFT+P"));
	preference->setStatusTip(tr("Preference"));
	connect(preference, &QAction::triggered, this, &ZebrafishAnalytics::handlePreference);

}

void ZebrafishAnalytics::handleGenerateBGLayer() {
	mGlobals.mDialogManager->mDialogGenBGLayer->exec();
}

void ZebrafishAnalytics::handleGenerateLBLayer() {
	mGlobals.mDialogManager->mDialogGenLBLayer->exec();
}

void ZebrafishAnalytics::handleGenerateSRLayer() {
	mGlobals.mDialogManager->mDialogGenSRLayer->exec();
}

void ZebrafishAnalytics::handleNewProject() {
	mGlobals.mDialogManager->mDialogNewProject->exec();
}

void ZebrafishAnalytics::handlePreference() {
	mGlobals.mDialogManager->mDialogPreference->exec();
}

void ZebrafishAnalytics::handleOpenProject() {
	qDebug("Open Project handle");
	//QString filters("Background Layer file (*.aproj);;");
	//QString dir = QFileDialog::getOpenFileName(this, "Open Directory", QDir::currentPath(), filters);
	//qDebug() << "Open dir";
	//qDebug() << dir;
	//if (!mGlobals.CurrentProject->ProjectStatus) {
	//	mGlobals.CurrentProject->ProjectOpen(dir);
	//}
	///*DialogNewProject *md = new DialogNewProject(mWidget);
	//md->exec();*/

	mGlobals.CurrentProject->ProjectOpen("G:/Brain.aproj");

}


void ZebrafishAnalytics::handleProjectOn() {
	qDebug("Project On master");
	mContents->InitGLView();
	mContents->InitGraph();
	mContents->InitProjectInfo();
	mContents->InitSubregionFeatureList();
	mContents->InitAnnotations();
	mContents->contents_left_tabwidget->setCurrentIndex(0);
	connectAll();
	project_started = true;
}


void ZebrafishAnalytics::handleButton() {
	qDebug("TEST Button");
}

void ZebrafishAnalytics::projectLoad() {
	qDebug("Project Load");

}