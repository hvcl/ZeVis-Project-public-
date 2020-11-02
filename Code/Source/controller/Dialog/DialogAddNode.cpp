#include "DialogAddNode.h"

DialogAddNode::DialogAddNode(QWidget *parent) : QDialog(parent)
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
	dialog_title->setText("Add Node");
	dialog_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	dialog_title->setMinimumWidth(nWidth / 4);

	QFont title_font("Arial", 20, QFont::Bold);
	dialog_title->setFont(title_font);

	QLabel *dialog_contents = new QLabel(this);
	dialog_contents->setMargin(5);
	dialog_contents->setWordWrap(true);
	dialog_contents->setText("Add a data bucket or selection operation.");
	QFont contents_font("Arial", 11, QFont::Normal);
	dialog_contents->setFont(contents_font);

	left_layout->addWidget(dialog_title);
	left_layout->addWidget(dialog_contents);


	QFont right_font("Arial", 10, QFont::Normal);
	QFont right_small_font("Arial", 9, QFont::Bold);
	QFont right_small_contents_font("Arial", 9, QFont::Normal);


	QHBoxLayout *nodename_layout = new QHBoxLayout;
	QLabel *label_nodename = new QLabel(this);
	label_nodename->setText("Node Name   ");
	label_nodename->setFont(right_font);
	node_name = new QLineEdit(this);
	node_name->setPlaceholderText("Enter the node name.");
	node_name->setFocus();
	nodename_layout->addWidget(label_nodename);
	nodename_layout->addWidget(node_name);


	QHBoxLayout *nodetype_layout = new QHBoxLayout;
	QLabel *label_nodetype = new QLabel(this);
	label_nodetype->setText("Node Type   ");
	label_nodetype->setFont(right_font);

	node_type = new QComboBox(this);
	node_type->addItems(node_type_list);
	node_type->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	connect(node_type, SIGNAL(currentIndexChanged(int)), this, SLOT(handleComboMenu(int)));

	nodetype_layout->addWidget(label_nodetype);
	nodetype_layout->addWidget(node_type);



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


	nodetype_layout->setMargin(10);
	right_layout->addLayout(nodetype_layout);
	nodename_layout->setMargin(10);
	right_layout->addLayout(nodename_layout);

	right_layout->addLayout(filler_layout);

	right_layout->addLayout(btn_layout);

	this->setLayout(main_layout);

	main_layout->addLayout(left_layout);
	main_layout->addWidget(vertical_line);
	main_layout->addLayout(right_layout);
}

DialogAddNode::~DialogAddNode()
{
}

void DialogAddNode::handleComboMenu(int index) {
	active_type = index;
}


int DialogAddNode::exec()
{
	qDebug("BG");
	activateWindow();
	this->show();
	return 1;
}
void DialogAddNode::done(int val)
{
	qDebug("done.");

}

void DialogAddNode::accept()
{
	qDebug("Accept.");
	emit makenode(active_type, node_name->text());
	this->hide();
}
void DialogAddNode::reject()
{
	emit thread_kill();

	qDebug("Reject.");
	this->hide();
}
