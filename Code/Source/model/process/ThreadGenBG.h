#pragma once

#include <QThread>
#include <QObject>
#include <QDialog>
#include <QtWidgets>
#include <omp.h>
#include <string>
#include <iostream>
#include <string>
#include <cmath>
#include <assert.h>
#include <QFileInfo>
#include <stdio.h>
#include <stdlib.h>

#include<sstream>
#include <fstream>
#include <iomanip>


#include "Source/global.h"
#include "ZebrafishAnalytics.h"

class ThreadGenBG : public QThread
{
	Q_OBJECT

public:
	ThreadGenBG();
	ThreadGenBG(QObject *parent);
	~ThreadGenBG();
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

	void compositingXZYZ();
	
	void multiResolution(int axis);
	void multiResolutionXZ();
	void multiResolutionYZ();


	void rescale(unsigned char *src, unsigned char *dst, int factor, int width, int height);
	void crop(unsigned char *src, unsigned char *dst, int x, int y, int rect_width, int rect_height, int src_width, int src_height);
	void run();

	unsigned char *temp_block_slice;
	unsigned char *sub_volume;


signals:
	void finish_generate_bg();
	void progress_generate_bg(float);
	void progress_generate_bg_log(QString);

public slots:
	void kill_thread();
	



};
