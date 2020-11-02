#pragma once

#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include "Source/global.h"
#include "Source/model/process/ThreadGenLB.h"
#include "ZebrafishAnalytics.h"

using namespace std;

class DialogGenLBLayer : public QDialog
{
	Q_OBJECT


signals:
	void thread_kill();

public slots:
	int exec();
	void done(int val);
	void accept();
	void generate();
	void reject();
	void find();
	void find_save();
	void update_progress(float v);
	void update_progress_log(QString str);


public:
	DialogGenLBLayer(QWidget *parent);
	~DialogGenLBLayer();

private:
	bool save_path_check = false;
	bool image_path_check = false;

	int whole_volume_x;
	int whole_volume_y;
	int whole_volume_z;

	int sub_volume_x_count;
	int sub_volume_y_count;
	int sub_volume_z_count;

	int sub_volume_size;

	QStringList mImageList;
	QLabel * exampleImage;

	QLabel * image_type;
	QLabel * image_type_title;
	QLabel * image_size;
	QLabel * image_size_title;




	QWidget * pWidget;
	QLineEdit * seedImagePath;

	QLineEdit * label_size_x;
	QLineEdit * label_size_y;
	QLineEdit * label_size_z;

	QLineEdit * savepath;

	QProgressBar *mProgressbar;
	QLabel * mProgressLog;
};
