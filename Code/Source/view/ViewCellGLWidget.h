#pragma once
///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#ifndef ViewCellGLWidget_H
#define ViewCellGLWidget_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QVector2D>
#include "transferFunction.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class ViewCellGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Compatibility
{
	Q_OBJECT

public:
	ViewCellGLWidget(QWidget *parent = 0);
	~ViewCellGLWidget();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;
	int w=-1, h=-1, d=-1;
	QVector4D background_color;
	std::string filename;

	QVector3D eye_position;
	QVector3D center1;
	QVector3D up;

	float scale;
	QVector2D shift;

	void setVolumeData(int w_, int h_, int d_, unsigned char *volume_, unsigned int *label_, int l);

	bool isVolumeInit = false;
	bool isVolumeChanged = false;

	unsigned int label_value=-1;
	int selectType = 0; //0: not selected, 1: white list, 2: black list

	bool shift_flag = false;

public slots:

	void cleanup();

signals:

	void WhiteListAdded(unsigned int label);
	void BlackListAdded(unsigned int label);
	void WhiteListRemoved(unsigned int label);
	void BlackListRemoved(unsigned int label);
	void TypeUpdated();

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void resizeGL(int width, int height) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
	QVector3D cross_product(QVector3D v1, QVector3D v2);
	void ReadColorTable();

	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;


public:

	transferFunction *tf;

	QPoint m_lastPos;
	QPoint m_clickPos;

	QOpenGLShaderProgram *m_program;

	GLuint volume;
	GLuint label;

	QVector3D box_min;
	QVector3D box_max;


	int con_window_size;


	int con_eye_pos;
	int con_box_min;
	int con_box_max;
	int con_up;
	int con_center;

	QString p_fps;
	int fps;
	int fps_start;

	int con_volume;
	int con_label;

	QVector3D const_box_max;
	QVector3D const_box_min;


	int con_color_table;

	GLuint color_tex;



	int con_sample;
	float sample;


	int con_l_t;


	unsigned char *volume_data = NULL;
	unsigned char *label_data = NULL;


	int reset_color;

	float max;
	int con_max;

	float z_scale;

	int con_background_color;

	int con_specu, con_diffu, con_ambi, con_shin;



	int tog;
	float l_t;


	float ambi, diffu, specu, shin;

	float EYE;


	int phong_enable;
	int con_phong_enable;

	int con_v_size;

	float sampling_rate;
	int con_sampling_rate;

	bool samplingAuto = true;

	float prev_sampling_rate = 1;

	float peak_pos = 0;


};

#endif
