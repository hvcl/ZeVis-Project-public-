#pragma once

#include <QObject>
#include <QtWidgets>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QSizePolicy>

#include <iostream>
#include <vector>
#include <math.h>
#include <QColorDialog>

#include "Source/Utils.h"
#include "Source/view/histogram/Histogram.h"
#include "Source/view/histogram/HistogramViewer.h"
#include "Source/global.h"

class uuint {
public:
	int v;
	int index = 0;
	uuint(int a, int i) {
		v = a;
		index = i;
	}
	bool operator<(uuint t) const;
};

class ViewHistogramWidget : public QWidget
{
	Q_OBJECT

signals:
	void OutputUpdated();

public:


	ViewHistogramWidget(QWidget *parent);
	void setData(int feature_index, std::list<unsigned int> *in_ptr, std::list<unsigned int> *out_ptr);
	void setInterface_percent(QLineEdit *s, QLineEdit *e);
	void setInterface_value(QLineEdit *s, QLineEdit *e);
	void setInterface_unit(QLabel *s, QLabel *e);

	void setRenderingSize(int width, int height);
	void redraw();
	void sortCellbyFeature( int data_index);
	//bool sortCmp(const unsigned int &a, const unsigned int &b);
	~ViewHistogramWidget();
	
	QLineEdit *start_edit_percent;
	QLineEdit *end_edit_percent;
	
	QLineEdit *start_edit_value;
	QLineEdit *end_edit_value;

	QLabel *start_edit_value_unit;
	QLabel *end_edit_value_unit;
	double start_cell_value = 0;
	double end_cell_value = 0;

public slots:
	void update_move_box(double start, double end, int flag);
	void update_release_box(double start, double end, int flag);
	void clear_box();


private:
	int mWidth;
	int mHeight;
	QVBoxLayout * HistBoxLayout;
	
	int HistStartIdx = 0;
	int HistEndIdx = 0;


	QWidget *mWidget;
	std::list<unsigned int> *mInput;
	std::list<unsigned int> *mOutput;
	std::vector<std::vector<uuint>> HistogramData;
	double mDataStep;
	double mDataMaximum;
	double mDataMinimum;
	int mFeatureIndex=0;

	QList<QBarSet *> mBarsets;

	void clearHistogramData();
	//void updateColorBox(QPushButton *target, QColor color);
	//void updateCellColor();
};


