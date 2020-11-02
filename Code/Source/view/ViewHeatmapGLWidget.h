#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

#include <QMatrix4x4>


#include <QVector2D>
#include <cmath>

#include "Source/global.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);

class ViewHeatmapGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Compatibility
{
	Q_OBJECT

public:

	explicit ViewHeatmapGLWidget(QWidget *parent = 0);
	~ViewHeatmapGLWidget();

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;
	void rotateBy(int xAngle, int yAngle, int zAngle);
	void translateBy(float x, float y, float z);
	void scaleBy(float v);
	
	void setHeatmapData(std::vector<cell> *cell_list_ptr, int *cell_index_ptr, std::list<unsigned int> *indexlist_ptr, coord DataSize);
	void updateHeatmap();
	void updateDataColor(QColor color);
	void setBackgroundColor(QColor color);
	void setRenderingSize(int width, int height);
	void setTexturePath(std::string path);


	
protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

public:

	QImage changeBackground(const QString resource, QColor backgroundColor);


	bool paintState = false;
	int WidgetWidth = 0;
	int WidgetHeight = 0;
	std::string ThumbnailPath;

	
	QColor clearColor;
	QPoint lastPos;
	QMatrix4x4 projMatrix;


	int xRot;
	int yRot;
	int zRot;
	float xTrans_value = 0.0f;
	float yTrans_value = 0.0f;
	float zTrans_value = 0.0f;
	float prev_zoomlevel = 1.0;

	QColor BackgroundColor;
	QColor DataColor;

	std::vector<cell> *cell_table;
	int *cell_index;
	std::list<unsigned int> *index_list;
	coord cell_data_size;

	QOpenGLShaderProgram *program;
	GLuint thumbnail_tex;
	int con_thumbnail;
	GLuint heatmap_tex;
	int con_heatmap;

	QImage thumbnail;

	float *heatmap_table;
	int heatmap_maxvalue;


};
