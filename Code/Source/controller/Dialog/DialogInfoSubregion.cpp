#include "DialogInfoSubregion.h"



DialogInfoSubregion::DialogInfoSubregion(QWidget *parent) : QDialog(parent)
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
	main_layout = new QHBoxLayout;
	main_layout->setMargin(5);
	this->setLayout(main_layout);
}


DialogInfoSubregion::~DialogInfoSubregion()
{
}



int DialogInfoSubregion::exec()
{
	qDebug("BG");
	activateWindow();
	this->show();
	return 1;
}
void DialogInfoSubregion::done(int val)
{
	qDebug("done.");

}

void DialogInfoSubregion::accept()
{
	qDebug("Accept.");
	// here!!!!!
	this->hide();
}
void DialogInfoSubregion::reject()
{
	emit thread_kill();

	qDebug("Reject.");
	this->hide();
}

void DialogInfoSubregion::setIndexAndDraw(int index) {
	mIndex = index;

	//QWidget *widget = new QWidget;
	//QVBoxLayout * layout = new QVBoxLayout;
	//widget->setLayout(layout);
	//QFont title_font("Arial", 15, QFont::Bold);
	//QFont feature_title_font("Arial", 13, QFont::Bold);
	//QFont small_font("Arial", 10, QFont::Bold);
	//QFont small_contents_font("Arial", 10, QFont::Normal);
	//
	//QLabel *titlelabel = new QLabel;
	//titlelabel->setText(QString::fromStdString(mGlobals.CurrentProject->mSubregion[index].SubregionName));
	//titlelabel->setFont(title_font);
	//layout->addWidget(titlelabel);

	//QHBoxLayout * count_layout = new QHBoxLayout;
	//QLabel *count_label_t = new QLabel;
	//count_label_t->setText("Volume size : ");
	//count_label_t->setFont(small_font);
	//QLineEdit *count_label_c = new QLineEdit;
	//count_label_c->setText(QString::fromStdString(std::to_string(SubregionVolume)));
	//count_label_c->setFont(small_contents_font);
	//count_layout->addWidget(count_label_t);
	//count_layout->addWidget(count_label_c);
	//layout->addLayout(count_layout);

	//QLayoutItem* item;
	//while ((item = main_layout->takeAt(0)) != NULL)
	//{
	//	delete item->widget();
	//	delete item;
	//}

	//main_layout->addWidget(widget);

}
