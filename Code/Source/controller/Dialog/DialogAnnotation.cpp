#include "DialogAnnotation.h"



DialogAnnotation::DialogAnnotation(QWidget *parent) : QDialog(parent)
{
	pWidget = parent;
	setModal(true);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
	int nWidth = 850;
	int nHeight = 1080 / 2;
	if (parent != NULL) {
		//setGeometry(parent->x() + parent->width() / 2 - nWidth / 2, parent->y() + parent->height() / 2 - nHeight / 2, nWidth, nHeight);
		setGeometry(0 + 1920 / 2 - nWidth / 2, 0 + 1080 / 2 - nHeight / 2, nWidth, nHeight);
	}
	else {
		resize(nWidth, nHeight);
	}
	annotationList = new QListWidget(pWidget);
	main_layout = new QVBoxLayout;
	main_layout->setMargin(5);

	main_layout->addWidget(annotationList);

	QHBoxLayout *write_layout = new QHBoxLayout;
	commentEdit = new QLineEdit;
	commentEdit->setFixedWidth(700);
	QPushButton *writeBtn = new QPushButton(pWidget);
	writeBtn->setText("Add");
	write_layout->addWidget(commentEdit);
	write_layout->addWidget(writeBtn);
	connect(writeBtn, SIGNAL(released()), this, SLOT(addAnnotation()));
	main_layout->addLayout(write_layout);
	this->setLayout(main_layout);
}


DialogAnnotation::~DialogAnnotation()
{
}



int DialogAnnotation::exec()
{
	qDebug("BG");
	activateWindow();
	this->show();
	return 1;
}
void DialogAnnotation::done(int val)
{
	qDebug("done.");

}

void DialogAnnotation::accept()
{
	qDebug("Accept.");
	// here!!!!!
	this->hide();
}
void DialogAnnotation::reject()
{
	emit thread_kill();

	qDebug("Reject.");
	this->hide();
}
void DialogAnnotation::addAnnotation()
{
	m_block_ptr->addAnnotation(commentEdit->text());
	qDebug() << "Add!!";
	draw();
}

void DialogAnnotation::removeAnnotation() {
	QObject *senderObj = sender(); // This will give Sender object
	QString senderObjName = senderObj->objectName();
	int index = std::stoi(senderObjName.toStdString());
	qDebug() << "delete Button: " << senderObjName;
	m_block_ptr->removeAnnotation(index);
	draw();
}

void DialogAnnotation::draw() {
	annotationList->clear();

	int index = 0;
	std::list<annotation>::iterator iter;
	for (iter = m_block_ptr->mAnnotation.begin(); iter != m_block_ptr->mAnnotation.end(); ++iter) {

		QWidget *master_widget = new QWidget(pWidget);
		QHBoxLayout *master_layout = new QHBoxLayout;
		QFont textfont;
		textfont.setPointSize(10);
		textfont.setBold(true);

		QFont timefont;
		timefont.setPointSize(8);

		QVBoxLayout *row_layout1 = new QVBoxLayout;
		QLabel *comment_label = new QLabel;
		comment_label->setMaximumWidth(600);
		comment_label->setText(iter->comment);
		comment_label->setWordWrap(true);
		comment_label->setFont(textfont);
		comment_label->setStyleSheet("color:white;");
		row_layout1->addWidget(comment_label);

		QLabel *time_label = new QLabel;
		time_label->setWordWrap(false);
		time_label->setFont(timefont);
		time_label->setText(iter->time.toString("yyyy MMMM dd ap h:m:s"));
		time_label->setStyleSheet("color:gray;");
		row_layout1->addWidget(time_label);

		
		QPushButton *deleteBtn = new QPushButton(pWidget);
		deleteBtn->setIcon(QIcon("Resources/icon_trash.png"));
		deleteBtn->setObjectName(QString::fromStdString(std::to_string(index++)));
		deleteBtn->setIconSize(QSize(30, 30));
		deleteBtn->setFixedSize(QSize(40, 40));
		connect(deleteBtn, SIGNAL(clicked()), this, SLOT(removeAnnotation()));

		master_layout->addLayout(row_layout1);
		master_layout->addWidget(deleteBtn);
		master_widget->setLayout(master_layout);

		QListWidgetItem *item = new QListWidgetItem();

		item->setSizeHint(master_widget->sizeHint());

		annotationList->addItem(item);
		annotationList->setItemWidget(item, master_widget);

	}
}


void DialogAnnotation::setData(BlockWidget *ptr){
	m_block_ptr = ptr;
	draw();
}
