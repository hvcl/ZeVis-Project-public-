#pragma once

#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include "Source/global.h"
#include "ZebrafishAnalytics.h"

class DialogAddNode : public QDialog
{
	Q_OBJECT

signals:
	void thread_kill();
	void makenode(int, QString);

public slots:
	int exec();
	void done(int val);
	void accept();
	void reject();
	void handleComboMenu(int index);

public:
	DialogAddNode(QWidget *parent);
	~DialogAddNode();


private:

	int active_type = 0;
	QStringList node_type_list = {"","Origin", "Sub Data bucket", "Operation(Feature)", "Operation(Masking)",  "Operation(Set)", "Operation(Cut plane)", "Statistic Information","Subset Exploration"};



	QWidget * pWidget;
	

	QLineEdit * node_name;
	QComboBox * node_type;

};
