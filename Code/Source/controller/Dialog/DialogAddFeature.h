#pragma once

#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include "Source/global.h"


class DialogAddFeature : public QDialog
{
	Q_OBJECT

signals:
	void thread_kill();
	void updatedFeature();

public slots:
	int exec();
	void done(int val);
	void accept();
	void reject();

public:
	DialogAddFeature(QWidget *parent);
	~DialogAddFeature();

private slots:
	void handleFindbtn();

private:

	QWidget * pWidget;


	QComboBox * FeatureType;
	QComboBox *	FeatureUnit;
	QLineEdit * FeaturePath;
	QStringList combomenu = { "Constant", "Linear measure", "Square measure", "Cubic measure" };


};
