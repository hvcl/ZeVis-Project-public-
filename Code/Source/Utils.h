#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <QColor>
#include <cmath>
#include <QWidget>
#include <QColorDialog>
#include <QDateTime>

typedef struct {
	int x;
	int y;
	int z;
}vertex;



typedef struct {
	int start_x;
	int end_x;
	int start_y;
	int end_y;
	int x;
	int y;
	int z;
	int size;
	int level;
	int axis;
}block_info;



typedef struct {
	unsigned char *data;
	int index_x;
	int index_y;
	int index_z;
	int pos_x;
	int pos_y;
	int pos_z;
	int size_x;
	int size_y;
	int size_z;
	int level;
	bool status;
	int load_axis_code = 0;
}back_layer;

typedef struct {
	unsigned int *data;
	unsigned char *color_data;
	int index_x;
	int index_y;
	int index_z;
	int pos_x;
	int pos_y;
	int pos_z;
	int size_x;
	int size_y;
	int size_z;
	int level;
	bool status;
	int load_axis_code = 0;
}label_layer;


typedef struct {
	unsigned char *data;
	int index_x;
	int index_y;
	int index_z;
	int pos_x;
	int pos_y;
	int pos_z;
	int size_x;
	int size_y;
	int size_z;
	int level;
	bool status;
	int load_axis_code = 0;
}subregion_layer;


typedef struct {
	float r;
	float g;
	float b;
	float a;
}float_color;


typedef struct {
	long long x;
	long long y;
	long long z;
}coord;

typedef struct {
	unsigned int index;
	coord minbox;
	coord maxbox;
	bool status = false;
}cell;

typedef struct {
	QString comment;
	QDateTime time;
}annotation;

class Utils
{
public:
	std::vector<std::string> Split(const char *phrase, std::string delimiter);
	void CalcXYZCount(int *x, int *y, int *z, int level, int x_size, int y_size, int z_size, int blocksize);
	std::string BoolToString(bool b);
	QColor interpolate_color(QColor start, QColor end, double ratio);
	bool fexists(const char *filename);
	

	Utils();
	~Utils();
};

class QColorDialogTester : public QWidget
{
public:

	QString onColor()
	{
		QColor color = QColorDialog::getColor(Qt::yellow, this);
		if (color.isValid())
		{
			//qDebug() << "Color Choosen : " << color.name();
			return color.name();
		}

		return "";
	}

};