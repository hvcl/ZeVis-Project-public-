#include "DialogAddSubregion.h"

DialogAddSubregion::DialogAddSubregion(QWidget *parent) : QDialog(parent)
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
	dialog_title->setText("Add Subregion");
	dialog_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	dialog_title->setMinimumWidth(nWidth / 4);

	QFont title_font("Arial", 20, QFont::Bold);
	dialog_title->setFont(title_font);

	QLabel *dialog_contents = new QLabel(this);
	dialog_contents->setMargin(5);
	dialog_contents->setWordWrap(true);
	dialog_contents->setText("Please select the location of subregion data.");
	QFont contents_font("Arial", 11, QFont::Normal);
	dialog_contents->setFont(contents_font);

	left_layout->addWidget(dialog_title);
	left_layout->addWidget(dialog_contents);


	QFont right_font("Arial", 10, QFont::Normal);
	QFont right_small_font("Arial", 9, QFont::Bold);
	QFont right_small_contents_font("Arial", 9, QFont::Normal);


	//QHBoxLayout *subregionname_layout = new QHBoxLayout;
	//QLabel *label_subregionname = new QLabel(this);
	//label_subregionname->setText("Subregion Name   ");
	//label_subregionname->setFont(right_font);
	//SubregionName = new QLineEdit(this);
	//SubregionName->setPlaceholderText("Enter the subregion name.");
	//SubregionName->setFocus();
	//subregionname_layout->addWidget(label_subregionname);
	//subregionname_layout->addWidget(SubregionName);


	QHBoxLayout *subregionpath_layout = new QHBoxLayout;
	QLabel *label_subregionpath= new QLabel(this);
	label_subregionpath->setText("Subregion Path   ");
	label_subregionpath->setFont(right_font);

	SubregionPath = new QLineEdit(this);
	SubregionPath->setPlaceholderText("Enter the subregion path.");
	SubregionPath->setFocus();

	QPushButton *FindBtn = new QPushButton(this);
	FindBtn->setText("Find");
	connect(FindBtn, SIGNAL(released()), this, SLOT(handleFindbtn()));
	
	subregionpath_layout->addWidget(label_subregionpath);
	subregionpath_layout->addWidget(SubregionPath);
	subregionpath_layout->addWidget(FindBtn);



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


	//subregionname_layout->setMargin(10);
	//right_layout->addLayout(subregionname_layout);
	subregionpath_layout->setMargin(10);
	right_layout->addLayout(subregionpath_layout);

	right_layout->addLayout(filler_layout);

	right_layout->addLayout(btn_layout);

	this->setLayout(main_layout);

	main_layout->addLayout(left_layout);
	main_layout->addWidget(vertical_line);
	main_layout->addLayout(right_layout);
}

DialogAddSubregion::~DialogAddSubregion()
{
}


int DialogAddSubregion::exec()
{
	qDebug("BG");
	activateWindow();
	this->show();
	return 1;
}
void DialogAddSubregion::done(int val)
{
	qDebug("done.");

}

void DialogAddSubregion::accept()
{
	qDebug("Accept.");
	mGlobals.CurrentProject->AddSubregion(SubregionPath->text());
	emit updatedSubregion();
	this->hide();
}
void DialogAddSubregion::reject()
{
	emit thread_kill();
	qDebug("Reject.");
	this->hide();
}

void DialogAddSubregion::handleFindbtn()
{
	QString filters("Subregion Layer File (*.srl);;");
	QString dir = QFileDialog::getOpenFileName(this, "Open Directory", QDir::currentPath(), filters);
	SubregionPath->setText(dir);
}