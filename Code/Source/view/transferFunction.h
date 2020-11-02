#pragma once
///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#ifndef transferFunction_H
#define transferFunction_H

#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMatrix4x4>
#include <QVector2D>


QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)



class transferFunction :public QWidget
{
	Q_OBJECT

public:
	transferFunction(QWidget *parent = 0);
	~transferFunction();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
	void set_histogram_scale(int n);

signals:

	void color_change(float r, float g, float b);
	void sigapply();
	void edited();

protected:
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
public:

	void generate();

	QPoint m_lastPos;

	QVector3D eye_position;
	QVector3D center;
	QVector3D up;


	int slice;
	float size_w, size_h;

	QVector4D color_table[256];

	float histoData[256];

	unsigned char *data;
	float *n_data;



	float scale;
	int w, h, d;
	int dx, dy;
	std::string filename;

	int select_tog;

	int axx, axy, axz;

	int slx, sly, slz;


	QVector3D label_color;




	QPixmap background;
	QPixmap manual;
	QVector<QVector2D> points;
	QVector<QVector3D> colors;
	QPoint lastPos;
	int num;
	Qt::MouseButton button;
	int creat_delete;


	int basic_start_x;
	int basic_start_y;
	int basic_stop_x;
	int basic_stop_y;
	int basic_width;
	int basic_height;
	int basic_size;
	float increase_factor_x;
	float increase_factor_y;
	float basic_frame_width;
	float basic_frame_height;
	float i_f;


	bool first_flag;

	int label_number;

	int pen_size;
	QVector2D TF_start, TF_stop;
	int histogram_scale;

	int tf_index = 0;

};

#endif
