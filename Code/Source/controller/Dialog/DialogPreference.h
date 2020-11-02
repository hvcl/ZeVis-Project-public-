#pragma once

#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <QString>

#include "Source/model/system/systeminfo.h"
#include "Source/global.h"
#include "ZebrafishAnalytics.h"

class DialogPreference : public QDialog
{
	Q_OBJECT

public slots:
	int exec();
	void done(int val);
	void accept();
	void reject();
	void find_save_dir();


public:
	DialogPreference(QWidget *parent);
	~DialogPreference();

private:
	QWidget * pWidget;
	QLineEdit * savepath;
};
