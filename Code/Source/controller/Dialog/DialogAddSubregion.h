#pragma once

#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include "Source/global.h"


class DialogAddSubregion: public QDialog
{
	Q_OBJECT

signals:
	void thread_kill();
	void updatedSubregion();

public slots:
	int exec();
	void done(int val);
	void accept();
	void reject();

public:
	DialogAddSubregion(QWidget *parent);
	~DialogAddSubregion();


private slots:
	void handleFindbtn();

private:

	QWidget * pWidget;
	
	QLineEdit * SubregionName;
	QLineEdit * SubregionPath;

};
