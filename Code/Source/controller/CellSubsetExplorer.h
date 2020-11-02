#pragma once

#include <QObject>
#include <QPushButton>
#include <QtWidgets>
#include <QIcon>
#include "Source/view/ViewCellGLWidget.h"
#include "Source/view/transferFunction.h"
#include "Source/controller/Contents.h"
#include "Source/global.h"
#include <QThread>
#include "Source/view/View2DPlot.h"


class BlockWidget;


class CellSubsetExplorer : public QWidget
{
	Q_OBJECT

public:
	CellSubsetExplorer(QWidget *parent);
	~CellSubsetExplorer();

	bool onSegment(QVector2D p, QVector2D q, QVector2D r);
	int orientation(QVector2D p, QVector2D q, QVector2D r);
	bool doIntersect(QVector2D p1, QVector2D q1, QVector2D p2, QVector2D q2);
	bool isInside(QVector2D p);

public slots:

	void interacted(int a);
	void cellIncludeCheck();
	void WhiteListAdded(unsigned int label_value);
	void BlackListAdded(unsigned int label_value);
	void WhiteListRemoved(unsigned int label_value);
	void BlackListRemoved(unsigned int label_value);

	void doRandomForest();


signals:
	void cell_Update();

public:
	QWidget * mWidget;

	View2DPlot *reduction_plot=NULL;
	QVector <ViewCellGLWidget*> cell_objects;
	QVector <transferFunction*> tf_objects;

	QGridLayout *cell_layout;
	std::list<unsigned int> * cell_list;
	std::list<unsigned int> * output_cell_list;
	//	std::list<unsigned int> selected_labels;
	QThread workerThread;
	bool IsNewJob = false;
	QVector <unsigned int> *white_list=NULL;
	QVector <unsigned int> *black_list=NULL;
	QVector <unsigned int> *selected_list=NULL;
	int *xaxis=NULL;
	int *yaxis=NULL;
	unsigned char **volume[64];
	unsigned int **label[64];
	int *w;
	int *h;
	int *d;
	float *inputData = NULL;
	bool *IsCellIncluded = NULL;
	bool **IsForegroundByRF = NULL;

	unsigned int *input_vector = NULL;
	int n=0;

	QComboBox *xaxis_combobox = NULL;
	QComboBox *yaxis_combobox = NULL;

	bool *is_selection = NULL;
	QVector<QVector2D> *selectPoints=NULL;

	BlockWidget *curBlock=NULL;

	bool noUpdate = false;

	float max_values[8];
	float min_values[8];

};


class worker : public QObject
{
	Q_OBJECT

public:
	worker(CellSubsetExplorer *parent);

	CellSubsetExplorer *mWidget;

	int *group=NULL;

public slots:
	void generateRepresentativeCells();


};
