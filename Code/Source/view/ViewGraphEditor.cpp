#include "ViewGraphEditor.h"

ViewGraphEditor::ViewGraphEditor(QWidget *parent)
	: QWidget(parent)
{
	mWidget = parent;
	//setStyleSheet("QMessageBox{background: rgb(55,55,55);  border: none;font-family: Arial; font-style: normal;  font-size: 10pt; color: #ffffff ; }, QMessageBox QLabel{background: rgb(55,55,55);  border: none;font-family: Arial; font-style: normal;  font-size: 10pt; color: #ffffff ; }");
	setMouseTracking(true);
}

ViewGraphEditor::~ViewGraphEditor()
{
	delete GraphScene;
	delete GraphView;
	delete GraphEditor;

}


void ViewGraphEditor::initialize(QLayout *parent_layout) {
	GraphScene = new QGraphicsScene();
	GraphView = new QGraphicsView();
	GraphView->setScene(GraphScene);
	
	GraphView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

	
	GraphEditor = new QNodesEditor(this);
	GraphEditor->install(GraphScene, GraphView);

	

	parent_layout->addWidget(GraphView);

}

void ViewGraphEditor::GraphViewScale(int v) {
	GraphView->scale(float(v) / curScale, float(v) / curScale);
	
	curScale = v;
}

void ViewGraphEditor::addOrigin(std::string name,double x,double y) {
	QNEBlock *b = new QNEBlock(0);
	GraphScene->addItem(b);
	b->labelIndex = mGlobals.CurrentProject->curLayerCell;
	b->setBlockFlagAndSize(name, QNEBlock::OriginBlock, 180, 215, data_node_color, this);
	b->setInputDataOrigin(&mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList);
	//qDebug() << "j2";
	b->addPort(QString(name.c_str()), 0, 0, QNEPort::NamePort, 0, QNEPort::Left);
	//qDebug() << "j3";
	QString t_label_note = QString("Label: ") + QString(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->CellPath.c_str());
	
	if (t_label_note.length() > 25) {
		QString tt = "...";
		for (int i = t_label_note.length() - 25; i < t_label_note.length(); i++) {
			tt = tt + t_label_note[i];
		}
		t_label_note = tt;
	}
	b->addPort(t_label_note, 0, 0, QNEPort::TypePort, 0, QNEPort::Left);
	//qDebug() << "j4";
	b->addPort("", 0, 0, QNEPort::DataWidgetPort, 0, QNEPort::Center);
	//qDebug() << "j5";
	b->addPort("", 0, 0, QNEPort::DataSizePort, 0, QNEPort::Center);
	//qDebug() << "j6";
	b->addPort("", 0, 0, QNEPort::ToolBoxPort, 0, QNEPort::Right);
	//qDebug() << "j7";
	connect(b->mBlock, SIGNAL(deleteNode(QNEBlock*)), this, SLOT(deleteNode(QNEBlock*)));
	//qDebug() << "j8";

	b->addOutputPort("");
	//qDebug() << "j9";

	b->setPos(x, y);


}

void ViewGraphEditor::addDataBucket(std::string name, double x, double y) {
	QNEBlock *b = new QNEBlock(0);
	GraphScene->addItem(b);
	b->setBlockFlagAndSize(name, QNEBlock::DataBlock, 180, 215, data_node_color, this);
	b->addPort(QString(name.c_str()), 0, 0, QNEPort::NamePort, 0, QNEPort::Left);
	b->addPort("Sub Data bucket", 0, 0, QNEPort::TypePort, 0, QNEPort::Left);
	b->addPort("", 0, 0, QNEPort::DataWidgetPort, 0, QNEPort::Center);
	b->addPort("", 0, 0, QNEPort::DataSizePort, 0, QNEPort::Center);
	b->addPort("", 0, 0, QNEPort::ToolBoxPort, 0, QNEPort::Right);
	b->addInputPort("");
	b->addOutputPort("");
	connect(b->mBlock, SIGNAL(deleteNode(QNEBlock*)), this, SLOT(deleteNode(QNEBlock*)));
	b->setPos(x, y);
}
void ViewGraphEditor::addStatistic(std::string name, double x, double y) {
	QNEBlock *b = new QNEBlock(0);
	GraphScene->addItem(b);
	b->setBlockFlagAndSize(name, QNEBlock::StatisticBlock, 330, 450, operation_node_color, this);
	b->addPort(QString(name.c_str()), 0, 0, QNEPort::NamePort, 0, QNEPort::Left);
	b->addPort("Statistic information", 0, 0, QNEPort::TypePort, 0, QNEPort::Left);
	b->addPort("", 0, 0, QNEPort::StatisticPort, 0, QNEPort::Left);
	b->addPort("", 0, 0, QNEPort::ToolBoxPort, 0, QNEPort::Right);
	b->addInputPort("");
	b->addOutputPort("");
	connect(b->mBlock, SIGNAL(deleteNode(QNEBlock*)), this, SLOT(deleteNode(QNEBlock*)));
	b->setPos(x, y);
}
void ViewGraphEditor::addSubregion(std::string name, double x, double y) {
	QNEBlock *b = new QNEBlock(0);
	GraphScene->addItem(b);
	b->setBlockFlagAndSize(name, QNEBlock::SubregionBlock, 80, 80, operation_node_color, this);
	//b->addPort(name.c_str(), 0, 0, QNEPort::NamePort, 0, QNEPort::Left);
	//b->addPort("Operation(Subregion)", 0, 0, QNEPort::TypePort, 0, QNEPort::Left);
	//b->addPort("", 0, 0, QNEPort::DataSizePort, 0, QNEPort::Left);
	b->addPort("", 0, 0, QNEPort::SubregionWidgetPort, 0, QNEPort::Icon);
	//b->addPort("", 0, 0, QNEPort::SubregionDropdownPort, 0, QNEPort::Center);
	b->addPort("", 0, 0, QNEPort::ToolBoxPort, 0, QNEPort::Right);
	b->addInputPort("");
	b->addOutputPort("");
	connect(b->mBlock, SIGNAL(deleteNode(QNEBlock*)), this, SLOT(deleteNode(QNEBlock*)));
	b->setPos(x, y);

}
void ViewGraphEditor::addSet(std::string name, double x, double y) {
	QNEBlock *b = new QNEBlock(0);
	GraphScene->addItem(b);
	b->setBlockFlagAndSize(name, QNEBlock::SetBlock, 80, 80, operation_node_color, this);
	//b->addPort(name.c_str(), 0, 0, QNEPort::NamePort, 0, QNEPort::Left);
	//b->addPort("Operation(Set)", 0, 0, QNEPort::TypePort, 0, QNEPort::Left);
	//b->addPort("", 0, 0, QNEPort::DataSizePort, 0, QNEPort::Left);
	b->addPort("", 0, 0, QNEPort::SetWidgetPort, 0, QNEPort::Icon);
	b->addPort("", 0, 0, QNEPort::ToolBoxPort, 0, QNEPort::Right);
	b->addInputPort("");
	b->addSubInputPort("");
	b->addOutputPort("");
	connect(b->mBlock, SIGNAL(deleteNode(QNEBlock*)), this, SLOT(deleteNode(QNEBlock*)));
	b->setPos(x, y);

	//b->setOpacity(0.5);

	//QGraphicsColorizeEffect *m_effect = new QGraphicsColorizeEffect();
	//m_effect->setColor(QColor(255,0,0));
	//b->setGraphicsEffect(m_effect);

//	QNEBlock *b = new QNEBlock(0);
//	GraphScene->addItem(b);
//	b->setBlockFlagAndSize(name, QNEBlock::SetBlock, 250, 180, operation_node_color, this);
////	b->addPort(name.c_str(), 0, 0, QNEPort::NamePort, 0, QNEPort::Left);
////	b->addPort("Operation(Set)", 0, 0, QNEPort::TypePort, 0, QNEPort::Left);
////	b->addPort("", 0, 0, QNEPort::DataSizePort, 0, QNEPort::Left);
////	b->addPort("", 0, 0, QNEPort::SetDropdownPort, 0, QNEPort::Center);
////	b->addPort("", 0, 0, QNEPort::ToolBoxPort, 0, QNEPort::Right);
//	b->addPort("", 0, 0, QNEPort::SetIcon, 0, QNEPort::Center);
//	b->addInputPort("");
//	b->addSubInputPort("");
//	b->addOutputPort("");
//	connect(b->mBlock, SIGNAL(deleteNode(QNEBlock*)), this, SLOT(deleteNode(QNEBlock*)));
//	b->setPos(x, y);


}
void ViewGraphEditor::addFeature(std::string name, double x, double y) {
	QNEBlock *b = new QNEBlock(0);
	GraphScene->addItem(b);
	qDebug() << "0000000";

	b->setBlockFlagAndSize(name, QNEBlock::FeatureBlock, 80, 80, operation_node_color, this);
	//b->addPort(name.c_str(), 0, 0, QNEPort::NamePort, 0, QNEPort::Left);
	//b->addPort("Operation(Feature)", 0, 0, QNEPort::TypePort, 0, QNEPort::Left);
	//b->addPort("", 0, 0, QNEPort::DataSizePort, 0, QNEPort::Left);
	b->addPort("", 0, 0, QNEPort::FeatureWidgetPort, 0, QNEPort::Icon);

	//b->addPort("", 0, 0, QNEPort::FeatureDropdownPort, 0, QNEPort::Center);
	b->addPort("", 0, 0, QNEPort::ToolBoxPort, 0, QNEPort::Right);

	b->addInputPort("");
	b->addOutputPort("");
	connect(b->mBlock, SIGNAL(deleteNode(QNEBlock*)), this, SLOT(deleteNode(QNEBlock*)));
	b->setPos(x, y);


}
void ViewGraphEditor::addSimilarity(std::string name, double x, double y) {

}
void ViewGraphEditor::addCutPlane(std::string name, double x, double y) {
	QNEBlock *b = new QNEBlock(0);
	GraphScene->addItem(b);
	b->setBlockFlagAndSize(name, QNEBlock::CutPlaneBlock, 80, 80, operation_node_color, this);
	//b->addPort(name.c_str(), 0, 0, QNEPort::NamePort, 0, QNEPort::Left);
	//b->addPort("Operation(Subregion)", 0, 0, QNEPort::TypePort, 0, QNEPort::Left);
	//b->addPort("", 0, 0, QNEPort::DataSizePort, 0, QNEPort::Left);
	b->addPort("", 0, 0, QNEPort::CutPlanePort, 0, QNEPort::Icon);
	//b->addPort("", 0, 0, QNEPort::SubregionDropdownPort, 0, QNEPort::Center);
	b->addPort("", 0, 0, QNEPort::ToolBoxPort, 0, QNEPort::Right);
	b->addInputPort("");
	b->addOutputPort("");
	connect(b->mBlock, SIGNAL(deleteNode(QNEBlock*)), this, SLOT(deleteNode(QNEBlock*)));
	b->setPos(x, y);

}
void ViewGraphEditor::addSubsetExplorer(std::string name, double x, double y) {
	QNEBlock *b = new QNEBlock(0);
	GraphScene->addItem(b);
	qDebug() << "0000000";

	b->setBlockFlagAndSize(name, QNEBlock::SubsetExplorer, 80, 80, operation_node_color, this);
	b->addPort("", 0, 0, QNEPort::SubsetExplorerPort, 0, QNEPort::Icon);
	b->addPort("", 0, 0, QNEPort::ToolBoxPort, 0, QNEPort::Right);

	b->addInputPort("");
	b->addOutputPort("");
	connect(b->mBlock, SIGNAL(deleteNode(QNEBlock*)), this, SLOT(deleteNode(QNEBlock*)));
	b->setPos(x, y);


}


void ViewGraphEditor::addNode(int type, QString name) {

	//QRect visibleR = GraphView->visibleRegion().boundingRect();

	QRect visibleR = GraphView->frameGeometry(); // current local rect
	QRectF visibleR2 = GraphScene->sceneRect();
	QPointF mid=GraphView->mapToScene(visibleR.width()*0.5, visibleR.height()*0.5);
	float mid_x = mid.x();
	float mid_y = mid.y();
	if (type == BlockWidget::OriginBlock) {
		qDebug() << "Origin / " << name;
		if (mGlobals.CurrentProject->curLayerCell == -1) {
			QMessageBox::warning(0, "Error", "Please select a label first");
		}
		else {
			addOrigin(name.toStdString(), mid_x, mid_y);
		}
	}
	else if (type == BlockWidget::DataBlock) {
		qDebug() << "Sub Data bucket / " << name;
		addDataBucket(name.toStdString(), mid_x, mid_y);
	}
	else if (type == BlockWidget::FeatureBlock) {
		qDebug() << "Feature / " << name;
		if (mGlobals.CurrentProject->mFeature.size() == 0) {
			QMessageBox::warning(0, "Error", "Please add a feature first");
		}
		else {
			addFeature(name.toStdString(), mid_x, mid_y);
		}
	}
	else if (type == BlockWidget::SubregionBlock) {
		qDebug() << "Subregion / " << name;
		if (mGlobals.CurrentProject->mSubregion.size() == 0) {
			QMessageBox::warning(0, "Error", "Please add a subregion first");
		}
		else {
			addSubregion(name.toStdString(), mid_x, mid_y);
		}
	}
	else if (type == BlockWidget::SetBlock) {
		qDebug() << "Set / " << name;
		addSet(name.toStdString(), mid_x, mid_y);
	}
	else if (type == BlockWidget::CutPlaneBlock) {
		qDebug() << "cut plane / " << name;
		addCutPlane(name.toStdString(), mid_x, mid_y);
	}
	else if (type == BlockWidget::StatisticBlock) {
		qDebug() << "stat / " << name;
		addStatistic(name.toStdString(), mid_x, mid_y);
	}
	else if (type == BlockWidget::SubsetExplorerBlock) {
		qDebug() << "explorer / " << name;
		addSubsetExplorer(name.toStdString(), mid_x, mid_y);
	}
}


void ViewGraphEditor::deleteNode(QNEBlock *ptr) {
	qDebug() << "deleteNode";
		
	
	foreach(QNEPort *port, ptr->ports()) {
		if (port->portAlign() == QNEPort::Input) {
			foreach(QNEConnection *conn, port->connections()) {
				GraphScene->removeItem((QGraphicsItem*)conn);
			}
		}
		if (port->portAlign() == QNEPort::Output) {
			foreach(QNEConnection *conn, port->connections()) {
				QNEBlock *next = conn->port2()->m_block;
				next->mBlock->CellIndexListInput.clear();
				next->mBlock->updatedInputList();
				GraphScene->removeItem((QGraphicsItem*)conn);
			}
			
		}
	}

	GraphScene->removeItem((QGraphicsItem*)ptr);
}

void ViewGraphEditor::updateSubregionList() {
	qDebug() << "Subregion updated";
	foreach(QGraphicsItem *ptr, GraphScene->items()) {
		if (ptr->type() == QNEBlock::Type) {
			QNEBlock *temp = (QNEBlock *)ptr;
			if (temp->mBlockFlags == QNEBlock::SubregionBlock) {
				temp->mBlock->updatedSubregionList();
			}
		}
	}
}

void ViewGraphEditor::updateFeatureList() {
	qDebug() << "Feature updated";
	foreach(QGraphicsItem *ptr, GraphScene->items()) {
		if (ptr->type() == QNEBlock::Type) {
			QNEBlock *temp = (QNEBlock *)ptr;
			if (temp->mBlockFlags == QNEBlock::FeatureBlock) {
				temp->mBlock->updatedFeatureList();
			}
		}
	}
}

void ViewGraphEditor::mousePressEvent(QMouseEvent *event)
{
	if (event->x() < 0 || event->x() > this->width() || event->y() < 0 || event->y() > this->height())return;

	if (event->button() == Qt::LeftButton) {


	}
	else if (event->button() == Qt::MidButton) {
	}
	else if (event->button() == Qt::RightButton) {
		//rightButtonFlag = true;

	}


}
void ViewGraphEditor::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::RightButton) {
		//rightButtonFlag = false;
	}
}
void ViewGraphEditor::mouseMoveEvent(QMouseEvent *event)
{
	setFocus();

	if (event->x() < 0 || event->x() > this->width() || event->y() < 0 || event->y() > this->height())return;

	float tx = event->x();
	float ty = event->y();

	//if (rightButtonFlag) {

	//}


	
}

void ViewGraphEditor::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape) {

	}
	if (event->key() == Qt::Key_Shift) {

	}
}
void ViewGraphEditor::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Shift) {
	}
}