#include "DialogNewProject.h"

DialogNewProject::DialogNewProject(QWidget *parent):QDialog(parent)
{
	pWidget = parent;
	setModal(true);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
	int nWidth = 1920 / 2;
	int nHeight = 1080 / 2;
	if (parent != NULL) {
		//setGeometry(parent->x() + parent->width() / 2 - nWidth / 2, parent->y() + parent->height() / 2 - nHeight / 2, nWidth, nHeight);
		setGeometry(0 + 1920 / 2 - nWidth / 2, 0 + 1080 / 2 - nHeight / 2, nWidth, nHeight);
	}
	else {
		resize(nWidth, nHeight);
	}
		


	QHBoxLayout *main_layout = new QHBoxLayout;
	main_layout->setMargin(5);

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->setAlignment(Qt::AlignTop);
	left_layout->setMargin(5);
	left_layout->setContentsMargins(20, 20, 20, 20);
	QVBoxLayout *right_layout = new QVBoxLayout;
	right_layout->setMargin(5);
	right_layout->setContentsMargins(20,20,20,20);
	right_layout->setAlignment(Qt::AlignTop);


	QLabel *vertical_line = new QLabel(this);
	vertical_line->setFrameStyle(QFrame::VLine | QFrame::Plain);
	


	QLabel *dialog_title = new QLabel(this);
	dialog_title->setAlignment(Qt::AlignTop);
	dialog_title->setMargin(5);
	dialog_title->setText("New Analytics Project");
	dialog_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	dialog_title->setMinimumWidth(nWidth/4);

	QFont title_font("Arial", 20, QFont::Bold);
	dialog_title->setFont(title_font);
	
	QLabel *dialog_contents = new QLabel(this);
	dialog_contents->setMargin(5);
	dialog_contents->setWordWrap(true);
	dialog_contents->setText("Create a new project. Please select a project name and location.");
	QFont contents_font("Arial", 11, QFont::Normal);
	dialog_contents->setFont(contents_font);

	left_layout->addWidget(dialog_title);
	left_layout->addWidget(dialog_contents);


	QHBoxLayout *project_name_layout = new QHBoxLayout;

	QFont right_font("Arial", 10, QFont::Normal);
	QLabel *proj_name = new QLabel(this);
	proj_name->setText("Project Name");
	proj_name->setFont(right_font);

	proj_name_edit = new QLineEdit(this);
	proj_name_edit->setPlaceholderText("Type your project name.");
	proj_name_edit->setFocus();

	project_name_layout->addWidget(proj_name);
	project_name_layout->addWidget(proj_name_edit);



	QHBoxLayout *project_location_layout = new QHBoxLayout;
	
	QLabel *proj_location = new QLabel(this);
	proj_location->setText("Project Location");
	proj_location->setFont(right_font);

	proj_location_edit = new QLineEdit(this);
	proj_location_edit->setPlaceholderText("Type your project location");
	proj_location_edit->setFocus();

	QPushButton * proj_location_find_btn = new QPushButton(this);
	proj_location_find_btn->setText("Find");
	connect(proj_location_find_btn, SIGNAL(released()), this, SLOT(find_save_dir()));

	
	project_location_layout->addWidget(proj_location);
	project_location_layout->addWidget(proj_location_edit);
	project_location_layout->addWidget(proj_location_find_btn);
	


	QHBoxLayout *background_location_layout = new QHBoxLayout;

	QLabel *background_location = new QLabel(this);
	background_location->setText("Background Layer Location");
	background_location->setFont(right_font);

	background_location_edit = new QLineEdit(this);
	background_location_edit->setPlaceholderText("Type your background layer location");
	background_location_edit->setFocus();

	QPushButton * background_location_find_btn = new QPushButton(this);
	background_location_find_btn->setText("Find");
	connect(background_location_find_btn, SIGNAL(released()), this, SLOT(find_open_bgl()));


	background_location_layout->addWidget(background_location);
	background_location_layout->addWidget(background_location_edit);
	background_location_layout->addWidget(background_location_find_btn);




	QHBoxLayout *label_location_layout = new QHBoxLayout;

	QLabel *label_location = new QLabel(this);
	label_location->setText("Label Layer Location");
	label_location->setFont(right_font);

	label_location_edit = new QLineEdit(this);
	label_location_edit->setPlaceholderText("Type your label layer location");
	label_location_edit->setFocus();

	QPushButton * label_location_find_btn = new QPushButton(this);
	label_location_find_btn->setText("Find");
	connect(label_location_find_btn, SIGNAL(released()), this, SLOT(find_open_lb()));


	label_location_layout->addWidget(label_location);
	label_location_layout->addWidget(label_location_edit);
	label_location_layout->addWidget(label_location_find_btn);



	//QHBoxLayout *analytics_location_layout = new QHBoxLayout;

	//QLabel *analytics_location = new QLabel(this);
	//analytics_location->setText("Analytics Result Location");
	//analytics_location->setFont(right_font);

	//analytics_location_edit = new QLineEdit(this);
	//analytics_location_edit->setPlaceholderText("If you have Analytics result, type your analytics result location");
	//analytics_location_edit->setFocus();

	//QPushButton * analytics_location_find_btn = new QPushButton(this);
	//analytics_location_find_btn->setText("Find");
	//connect(analytics_location_find_btn, SIGNAL(released()), this, SLOT(find_open_analytics()));


	//analytics_location_layout->addWidget(analytics_location);
	//analytics_location_layout->addWidget(analytics_location_edit);
	//analytics_location_layout->addWidget(analytics_location_find_btn);




	QHBoxLayout *subregions_location_layout = new QHBoxLayout;

	QLabel *subregions_location = new QLabel(this);
	subregions_location->setText("Subregions Location");
	subregions_location->setFont(right_font);

	subregions_location_edit = new QLineEdit(this);
	subregions_location_edit->setPlaceholderText("Type your subregions root location");
	subregions_location_edit->setFocus();

	QPushButton * subregions_location_find_btn = new QPushButton(this);
	subregions_location_find_btn->setText("Find");
	connect(subregions_location_find_btn, SIGNAL(released()), this, SLOT(find_subregion_dir()));


	subregions_location_layout->addWidget(subregions_location);
	subregions_location_layout->addWidget(subregions_location_edit);
	subregions_location_layout->addWidget(subregions_location_find_btn);


	QHBoxLayout *features_location_layout = new QHBoxLayout;

	QLabel *features_location = new QLabel(this);
	features_location->setText("Features Location");
	features_location->setFont(right_font);

	features_location_edit = new QLineEdit(this);
	features_location_edit->setPlaceholderText("Type your features root location");
	features_location_edit->setFocus();

	QPushButton * features_location_find_btn = new QPushButton(this);
	features_location_find_btn->setText("Find");
	connect(features_location_find_btn, SIGNAL(released()), this, SLOT(find_feature_dir()));


	features_location_layout->addWidget(features_location);
	features_location_layout->addWidget(features_location_edit);
	features_location_layout->addWidget(features_location_find_btn);




	QHBoxLayout *filler_layout = new QHBoxLayout;
	QWidget *filler = new QWidget;
	filler->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	filler_layout->addWidget(filler);

		
	QHBoxLayout *btn_layout = new QHBoxLayout;
	QPushButton * done_btn = new QPushButton(this);
	done_btn->setText("Done");
	connect(done_btn, SIGNAL(released()), this, SLOT(accept()));
	
	QPushButton * reject_btn = new QPushButton(this);
	reject_btn->setText("Cancel");
	connect(reject_btn, SIGNAL(released()), this, SLOT(reject()));

	btn_layout->addWidget(done_btn);
	btn_layout->addWidget(reject_btn);
	
	project_name_layout->setMargin(10);
	right_layout->addLayout(project_name_layout);
	project_location_layout->setMargin(10);
	right_layout->addLayout(project_location_layout);
	background_location_layout->setMargin(10);
	right_layout->addLayout(background_location_layout);
	label_location_layout->setMargin(10);
	right_layout->addLayout(label_location_layout);
	subregions_location_layout->setMargin(10);
	right_layout->addLayout(subregions_location_layout);
	features_location_layout->setMargin(10);
	right_layout->addLayout(features_location_layout);
	//analytics_location_layout->setMargin(10);
	//right_layout->addLayout(analytics_location_layout);
	right_layout->addLayout(filler_layout);
	right_layout->addLayout(btn_layout);

	this->setLayout(main_layout);
	
	main_layout->addLayout(left_layout);
	main_layout->addWidget(vertical_line);
	main_layout->addLayout(right_layout);

	//exportButton = new QPushButton(tr("Export"));
	//connect(exportButton, SIGNAL(clicked()), this, SLOT(accept()));
	//cancelButton = new QPushButton(tr("Cancel"));
	//connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	
}

DialogNewProject::~DialogNewProject()
{
}
void DialogNewProject::find_subregion_dir() {
	QString dir = QFileDialog::getExistingDirectory(this, "Select Base Directory of Subregions", QDir::currentPath());
	//QString dir = QFileDialog::getExistingDirectory(this, tr("Open image sequence directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	subregions_location_edit->setText(dir);
}
void DialogNewProject::find_feature_dir() {
	QString dir = QFileDialog::getExistingDirectory(this, "Select Base Directory of Features", QDir::currentPath());
	//QString dir = QFileDialog::getExistingDirectory(this, tr("Open image sequence directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	features_location_edit->setText(dir);
}
void DialogNewProject::find_save_dir() {
	QString filters("Analytics Projects (*.aproj);;");
	QString dir = QFileDialog::getSaveFileName(this, "Save file", QDir::currentPath(), filters);
	//QString dir = QFileDialog::getExistingDirectory(this, tr("Open image sequence directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	proj_location_edit->setText(dir);
}
void DialogNewProject::find_open_bgl() {
	QString filters("Background Layer file (*.bgl);;");
	QString dir = QFileDialog::getOpenFileName(this, "Open Directory", QDir::currentPath(), filters);
	background_location_edit->setText(dir);
}
void DialogNewProject::find_open_lb() {
	QString filters("Label Layer file (*.lbl);;");
	QString dir = QFileDialog::getOpenFileName(this, "Open Directory", QDir::currentPath(), filters);
	label_location_edit->setText(dir);
}
void DialogNewProject::find_open_analytics() {
	QString filters("Cell Morphology Analytics file (*.celldat);;");
	QString dir = QFileDialog::getOpenFileName(this, "Open Directory", QDir::currentPath(), filters);
	analytics_location_edit->setText(dir);
}


int DialogNewProject::exec()
{
	activateWindow();
	
	this->show();
	return 1;
}
void DialogNewProject::done(int val)
{
	printf("done()\n");
}

void DialogNewProject::accept()
{
	QString t1 = proj_name_edit->text();
	QString t2 = proj_location_edit->text();
	QString t3 = background_location_edit->text();
	QString t4 = label_location_edit->text();
	QString t5 = subregions_location_edit->text();
	QString t6 = features_location_edit->text();
	//	QString t5 = analytics_location_edit->text();
	mGlobals.CurrentProject->ProjectMake(t1,t2,t3,t4,t5,t6);
	qDebug("Accept.");
	QMetaObject::invokeMethod(pWidget, "projectLoad");
	mGlobals.mainWindow->mContents->loadSubregionFeature();
	this->hide();
}
void DialogNewProject::reject()
{
	qDebug("Reject.");
	this->hide();
}