#include "DialogAddFeature.h"

DialogAddFeature::DialogAddFeature(QWidget *parent) : QDialog(parent)
{
	pWidget = parent;
	setModal(true);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
	int nWidth = 1920 / 2;
	int nHeight = 1080 / 4;
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
	right_layout->setContentsMargins(20, 20, 20, 20);
	right_layout->setAlignment(Qt::AlignTop);


	QLabel *vertical_line = new QLabel(this);
	vertical_line->setFrameStyle(QFrame::VLine | QFrame::Plain);

	QLabel *dialog_title = new QLabel(this);
	dialog_title->setAlignment(Qt::AlignTop);
	dialog_title->setMargin(5);
	dialog_title->setText("Add Feature");
	dialog_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	dialog_title->setMinimumWidth(nWidth / 4);

	QFont title_font("Arial", 20, QFont::Bold);
	dialog_title->setFont(title_font);

	QLabel *dialog_contents = new QLabel(this);
	dialog_contents->setMargin(5);
	dialog_contents->setWordWrap(true);
	dialog_contents->setText("Please select the location of feature data.");
	QFont contents_font("Arial", 11, QFont::Normal);
	dialog_contents->setFont(contents_font);

	left_layout->addWidget(dialog_title);
	left_layout->addWidget(dialog_contents);


	QFont right_font("Arial", 10, QFont::Normal);
	QFont right_small_font("Arial", 9, QFont::Bold);
	QFont right_small_contents_font("Arial", 9, QFont::Normal);


	//QHBoxLayout *featurename_layout = new QHBoxLayout;
	//QLabel *label_featurename = new QLabel(this);
	//label_featurename->setText("Feature Name   ");
	//label_featurename->setFont(right_font);
	//FeatureName = new QLineEdit(this);
	//FeatureName->setPlaceholderText("Enter the subregion name.");
	//FeatureName->setFocus();
	//featurename_layout->addWidget(label_featurename);
	//featurename_layout->addWidget(FeatureName);


	QHBoxLayout *featurepath_layout = new QHBoxLayout;
	QLabel *label_featurepath= new QLabel(this);
	label_featurepath->setText("Feature Path   ");
	label_featurepath->setFont(right_font);
	FeaturePath = new QLineEdit(this);
	FeaturePath->setPlaceholderText("Enter the subregion path.");
	FeaturePath->setFocus();

	QPushButton *FindBtn = new QPushButton(this);
	FindBtn->setText("Find");
	connect(FindBtn, SIGNAL(released()), this, SLOT(handleFindbtn()));

	featurepath_layout->addWidget(label_featurepath);
	featurepath_layout->addWidget(FeaturePath);
	featurepath_layout->addWidget(FindBtn);


	QHBoxLayout *featuretype_layout = new QHBoxLayout;
	QLabel *label_featuretype = new QLabel(this);
	label_featuretype->setText("Feature Type   ");
	label_featuretype->setFont(right_font);
	FeatureType = new QComboBox(this);
	
	FeatureType->addItems(combomenu);
	FeatureType->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		
	featuretype_layout->addWidget(label_featuretype);
	featuretype_layout->addWidget(FeatureType);


	//QHBoxLayout *featureunit_layout = new QHBoxLayout;
	//QLabel *label_featureunit = new QLabel(this);
	//label_featureunit->setText("Feature Unit   ");
	//label_featureunit->setFont(right_font);
	//FeatureUnit = new QComboBox(this);
	//
	//FeatureUnit->addItems(mGlobals.gUnitList);
	//FeatureUnit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//featureunit_layout->addWidget(label_featureunit);
	//featureunit_layout->addWidget(FeatureUnit);

	

	QHBoxLayout *filler_layout = new QHBoxLayout;
	QWidget *filler = new QWidget;
	filler->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	filler_layout->addWidget(filler);



	QHBoxLayout *btn_layout = new QHBoxLayout;

	QPushButton * done_btn = new QPushButton(this);
	done_btn->setText("Add");
	connect(done_btn, SIGNAL(released()), this, SLOT(accept()));

	QPushButton * reject_btn = new QPushButton(this);
	reject_btn->setText("Cancel");
	connect(reject_btn, SIGNAL(released()), this, SLOT(reject()));

	btn_layout->addWidget(done_btn);
	btn_layout->addWidget(reject_btn);


	/*featurename_layout->setMargin(10);
	right_layout->addLayout(featurename_layout);*/
	featurepath_layout->setMargin(10);
	right_layout->addLayout(featurepath_layout);
	featuretype_layout->setMargin(10);
	right_layout->addLayout(featuretype_layout);

	right_layout->addLayout(filler_layout);

	right_layout->addLayout(btn_layout);

	this->setLayout(main_layout);

	main_layout->addLayout(left_layout);
	main_layout->addWidget(vertical_line);
	main_layout->addLayout(right_layout);
}

DialogAddFeature::~DialogAddFeature()
{
}

int DialogAddFeature::exec()
{
	qDebug("BG");
	activateWindow();
	this->show();
	return 1;
}
void DialogAddFeature::done(int val)
{
	qDebug("done.");

}

void DialogAddFeature::accept()
{
	qDebug("Accept.");
	mGlobals.CurrentProject->AddFeature(FeaturePath->text(), FeatureType->currentIndex());
	emit updatedFeature();
	this->hide();
}
void DialogAddFeature::reject()
{
	emit thread_kill();

	qDebug("Reject.");
	this->hide();
}

void DialogAddFeature::handleFindbtn()
{
	QString filters("Feature data (*.dat);;");
	QString dir = QFileDialog::getOpenFileName(this, "Open Directory", QDir::currentPath(), filters);
	FeaturePath->setText(dir);
}