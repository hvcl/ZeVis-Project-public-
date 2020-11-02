#pragma once

#include <QThread>
#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include <cmath>
#include <omp.h>
#include <QFileInfo>

#include "Source/global.h"
#include "ZebrafishAnalytics.h"

class ThreadGenLB : public QThread
{
	Q_OBJECT

public:
	ThreadGenLB();
	ThreadGenLB(QObject *parent);
	~ThreadGenLB();
	void setVolumeSize(int ax, int ay, int az);
	void setPathInfo(std::string save_path, std::string data_type, std::string input_path);


private:
	QStringList ImageList;
	std::string InputPath;
	std::string SavePath;
	std::string DataType;

	int whole_volume_x;
	int whole_volume_y;
	int whole_volume_z;
	int sub_volume_size;
	int sub_volume_level;

	void compositingXZ();
	void compositingYZ();
	void multiResolutionXZ();
	void multiResolutionYZ();
	void rescale(unsigned int *src, unsigned int *dst, int factor, int width, int height);
	void crop(unsigned int *src, unsigned int *dst, int x, int y, int rect_width, int rect_height, int src_width, int src_height);
	void run();

	unsigned int *temp_block_slice;
	unsigned int *sub_volume;


signals:
	void finish_generate_lb();
	void progress_generate_lb(float);
	void progress_generate_lb_log(QString);

public slots:
	void kill_thread();




};
