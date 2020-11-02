#pragma once

#include <QObject>
#include <QtWidgets>
#include <QLabel>
#include <QSizePolicy>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFileDialog>
#include <QColor>
#include <QDebug>
#include <QMessageBox>


#include "Source/controller/node/qneblock.h"
#include "Source/controller/node/qneconnection.h"
#include "Source/controller/node/qnodeseditor.h"
#include "Source/controller/node/qneport.h"


#include "Source/global.h"
#include "Source/Utils.h"



class QNodesEditor;
class QNEBlock;


class ViewGraphEditor : public QWidget
{
	Q_OBJECT

public slots:
	void addNode(int type, QString name);
	void deleteNode(QNEBlock *ptr);
	void updateSubregionList();
	void updateFeatureList();

	void GraphViewScale(int v);

public:
	ViewGraphEditor(QWidget *parent);
	~ViewGraphEditor();
	void initialize(QLayout *parent_layout);
	void addOrigin(std::string name,double x, double y);

	QGraphicsScene* GraphScene;
	QGraphicsView *GraphView;
	QNodesEditor *GraphEditor;

	int curScale = 100;




	void addDataBucket(std::string name, double x, double y);
	void addSubregion(std::string name, double x, double y);
	void addSet(std::string name, double x, double y);
	void addFeature(std::string name, double x, double y);
	void addSimilarity(std::string name, double x, double y);
	void addCutPlane(std::string name, double x, double y);
	void addStatistic(std::string name, double x, double y);
	void addSubsetExplorer(std::string name, double x, double y);

private:
	QWidget * mWidget;

	QColor data_node_color = QColor("#3A79A0");
	QColor operation_node_color = QColor("#555555");


protected:

	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;


};
