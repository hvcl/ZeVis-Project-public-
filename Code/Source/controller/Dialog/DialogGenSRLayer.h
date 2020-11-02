#pragma once

#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include "Source/global.h"
#include "ZebrafishAnalytics.h"

class DialogGenSRLayer : public QDialog
{
	Q_OBJECT

public slots:
	int exec();
	void done(int val);
	void accept();
	void reject();
	void find();

public:
	DialogGenSRLayer(QWidget *parent);
	~DialogGenSRLayer();

private:
	QWidget * pWidget;
	QLineEdit * proj_location_edit;
	QLineEdit *proj_name_edit;
};
