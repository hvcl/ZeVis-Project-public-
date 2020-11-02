#pragma once
#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include "Source/global.h"
#include "Source/view/ViewCellGLWidget.h"

class DialogCellInfo : public QDialog
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
	DialogCellInfo(QWidget *parent);
	~DialogCellInfo();
	void setIndexAndDraw(unsigned int index);

private:
	QHBoxLayout * main_layout;
	QWidget * pWidget;
	unsigned int mIndex = 0;

	ViewCellGLWidget *cellVis;
	transferFunction *cellTF;
};