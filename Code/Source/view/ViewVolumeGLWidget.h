#ifndef ViewVolumeGLWidget_H
#define ViewVolumeGLWidget_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QVector2D>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QProcess>
#include <QColorDialog>
#include <QPainter>
#include "Source/Utils.h"
#include "Source/global.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class ViewVolumeGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Compatibility
{
	Q_OBJECT

public:
	ViewVolumeGLWidget(QWidget *parent = 0);
	~ViewVolumeGLWidget();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;

	void setBackgroundVolume(unsigned char *data, int width, int height, int depth);
	void setBackgroundVolumeTF(std::vector<float_color> transferfunction);
	void setVolume(float *data, int width, int height, int depth, float_color color);
	void updateVolume(float *data, int startX, int startY, int startZ, int width, int height, int depth);
	void updatePlaneInfo(QVector3D normal, QVector3D point);
	
	bool IsEmLoaded = false;

public slots:
	void emLoaded();
	void cleanup();
signals:


protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void resizeGL(int width, int height) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;


	QVector3D cross_product(QVector3D v1, QVector3D v2);
	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;


private:
	unsigned char *t_data;
	float *empty_data_for_test;
	int w, h, d;

	float heatmap_resolution_factor = 0.3;

	bool IsReadyBackgorundVolume;
	bool IsReadyVolume;

	QPoint m_lastPos;
	QPoint pressedPos;

	QOpenGLShaderProgram *m_program;

	GLuint Background_tex;
	GLuint Heatmap_tex;

	QVector3D box_min;
	QVector3D box_max;


	int con_eye_pos;
	int con_box_min;
	int con_box_max;
	int con_up;
	int con_center;
	int con_scale;

	int con_window_size;

	QString p_fps;
	int fps;
	int fps_start;

	int con_tex;
	int con_heatmap_tex;

	QVector3D const_box_max;
	QVector3D const_box_min;


	int con_color_table;

	GLuint color_tex;



	int con_sample;
	float sample;


	int con_l_t;


	QVector4D background_color;
	int con_background_color;

	QVector4D Heatmap_color;
	int con_heatmap_color;


	int heatmap_max = -1;

	int con_specu, con_diffu, con_ambi, con_shin;

	QVector2D shift;

	QVector3D eye_position;
	QVector3D center1;
	QVector3D up;

	QVector4D color_table[256];
	int tog;
	float l_t;
	float scale;

	float ambi, diffu, specu, shin;

	float EYE;

	int con_v_size;
	bool shift_flag;

	int con_planeNormal;
	int con_planePoint;

	bool is_selection = false;
	bool doing_selection = false;
	float start_count = 0;
	float end_count = 0;

	bool fix = false;
};

#endif
