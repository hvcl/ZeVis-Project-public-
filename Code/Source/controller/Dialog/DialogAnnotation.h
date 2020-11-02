#pragma once
#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include "Source/global.h"
#include "Source/Utils.h"
#include "Source/controller/node/BlockWidget.h"


class BlockWidget;
class DialogAnnotation : public QDialog
{
	Q_OBJECT

signals:
	void thread_kill();

public slots:
	int exec();
	void done(int val);
	void accept();
	void reject();
	void addAnnotation();
	void removeAnnotation();


public:
	DialogAnnotation(QWidget *parent);
	~DialogAnnotation();
	void setData(BlockWidget *ptr);

private:
	QVBoxLayout * main_layout;
	QWidget * pWidget;
	int mIndex = 0;
	QListWidget *annotationList;
	QLineEdit *commentEdit;

	BlockWidget *m_block_ptr;


	void draw();
};