#include "TopInterface.h"

TopInterface::TopInterface(QWidget *parent)
	: QObject(parent)
{
	mWidget = parent;
}
TopInterface::TopInterface(QWidget *parent, QLayout *parent_layout)
	: QObject(parent)
{
	mWidget = parent;
	QLabel *label1 = new QLabel(mWidget);
	label1->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	label1->setAlignment(Qt::AlignCenter);
	label1->setFixedSize(40, 30);
	label1->setText("X");
	//label1->setAlignment(Qt::AlignBottom | Qt::AlignRight);
	x_value_edit = new QLineEdit(mWidget);
	x_value_edit->setFixedSize(70, 30);
	x_value_edit->setFocus();

	QLabel *label2 = new QLabel(mWidget);
	label2->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	label2->setAlignment(Qt::AlignCenter);
	label2->setFixedSize(40, 30);
	label2->setText("Y");
	//label2->setAlignment(Qt::AlignBottom | Qt::AlignRight);
	y_value_edit = new QLineEdit(mWidget);
	y_value_edit->setFixedSize(70, 30);
	y_value_edit->setFocus();

	QLabel *label3 = new QLabel(mWidget);
	label3->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	label3->setAlignment(Qt::AlignCenter);
	label3->setFixedSize(40, 30);
	label3->setText("Z");
	//label3->setAlignment(Qt::AlignBottom | Qt::AlignRight);
	z_value_edit = new QLineEdit(mWidget);
	z_value_edit->setFixedSize(70, 30);
	z_value_edit->setFocus();

	QLabel *label4 = new QLabel(mWidget);
	label4->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	label4->setAlignment(Qt::AlignCenter);
	label4->setFixedSize(100, 30);
	label4->setText("Scale");
	//label4->setAlignment(Qt::AlignBottom | Qt::AlignRight);
	level_value_edit = new QLineEdit(mWidget);
	level_value_edit->setFixedSize(70, 30);
	level_value_edit->setFocus();

	interface_set_btn = new QPushButton("Set", mWidget);
	interface_set_btn->setFixedSize(QSize(40, 30));
	connect(interface_set_btn, SIGNAL(released()), this, SLOT(handleButton()));

	QFrame *buffer = new QFrame(mWidget);
	buffer->setFrameStyle(QFrame::VLine | QFrame::Plain);
	buffer->setStyleSheet("color:rgb(72,72,72)");
	buffer->setLineWidth(1.5);
	buffer->setFixedWidth(30);

	QLabel *label5 = new QLabel(mWidget);
	label5->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	label5->setAlignment(Qt::AlignCenter);
	label5->setFixedSize(150, 30);
	label5->setText("Cell Visualize Method");
	rendering_menu = new QComboBox(mWidget);
	QStringList commands = { "Random color", "Selected cell" };
	rendering_menu->addItems(commands);
	rendering_menu->setFixedWidth(250);
	connect(rendering_menu, SIGNAL(currentIndexChanged(int)), this, SLOT(handleComboMenu(int)));
	//connect(interface_set_btn, SIGNAL(released()), this, SLOT(handleButton()));

	QLabel *label6 = new QLabel(mWidget);
	label6->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	label6->setAlignment(Qt::AlignCenter);
	label6->setFixedSize(100, 30);
	label6->setText("Cell opacity");
	cell_opacity = new QSlider(Qt::Horizontal);
	cell_opacity->setFixedWidth(100);
	cell_opacity->setFocusPolicy(Qt::StrongFocus);
	cell_opacity->setTickPosition(QSlider::TicksBothSides);
	cell_opacity->setTickInterval(10);
	cell_opacity->setSingleStep(1);
	cell_opacity->setValue(50);
	connect(cell_opacity, SIGNAL(valueChanged(int)), this, SLOT(handleOpacity(int)));
	//connect(cell_opacity, SIGNAL(currentIndexChanged(int)), this, SLOT(handleComboMenu(int)));
	//connect(interface_set_btn, SIGNAL(released()), this, SLOT(handleButton()));




	parent_layout->addWidget(label1);
	parent_layout->addWidget(x_value_edit);
	parent_layout->addWidget(label2);
	parent_layout->addWidget(y_value_edit);
	parent_layout->addWidget(label3);
	parent_layout->addWidget(z_value_edit);
	parent_layout->addWidget(label4);
	parent_layout->addWidget(level_value_edit);
	parent_layout->addWidget(interface_set_btn);
	parent_layout->addWidget(buffer);
	parent_layout->addWidget(label5);
	parent_layout->addWidget(rendering_menu);
	parent_layout->addWidget(buffer);
	parent_layout->addWidget(label6);
	parent_layout->addWidget(cell_opacity);
}


void TopInterface::handleButton() {
	float mul = pow(2, 2 - mGlobals.CurrentProject->ViewZoomLevel);

	mGlobals.CurrentProject->ViewPos_X = x_value_edit->text().toFloat();
	mGlobals.CurrentProject->ViewPos_Y = y_value_edit->text().toFloat();
	mGlobals.CurrentProject->ViewPos_Z = z_value_edit->text().toFloat();
	mGlobals.CurrentProject->ViewZoomLevel = 2-log2(level_value_edit->text().toFloat());
	emit update_view(true);
}
void TopInterface::handleComboMenu(int index) {
	mGlobals.CurrentProject->VisualizeMethod_Index = index;
	emit update_view(true);
}

void TopInterface::handleOpacity(int value) {
	mGlobals.CurrentProject->CellOpacity = (float)value / 100.0f;
	//if(mGlobals.CurrentProject->curLayerCell!=-1)
	//	mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->Opacity = (float)value / 100.0f;
	emit update_view(true);
}

void TopInterface::update_value(int x, int y, int z, float level) {
	//qDebug("update value");
	x_value_edit->setText(QString::fromStdString(std::to_string(x)));
	y_value_edit->setText(QString::fromStdString(std::to_string(y)));
	z_value_edit->setText(QString::fromStdString(std::to_string(z)));
	level_value_edit->setText(QString::fromStdString(std::to_string(pow(2,2-level))));
}

TopInterface::~TopInterface()
{

}
