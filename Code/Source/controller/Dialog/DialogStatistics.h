#pragma once

#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <fstream>
#include <string>
#include <algorithm>
#include <iostream>
#include <string>
#include "Source/global.h"

typedef struct {
	double min;
	double max;
	double mean;
	double variance;
	long double sum;
	long long count;
	double five[5];
}statistic;

class DialogStatistics : public QDialog
{
	Q_OBJECT

signals:
	void thread_kill();

public slots:
	int exec();
	void done(int val);
	void accept();
	void reject();

public:
	DialogStatistics(QWidget *parent);
	~DialogStatistics();

	void generateStatistics(int labelIndex, QString name, std::list<unsigned int> *data_ptr);


	statistic getBasicStatistic(int findex);
	std::list<unsigned int> *CellIndexList;


private:
	QHBoxLayout * main_layout;
	QWidget * pWidget;
	QString TitleName;

	

};
