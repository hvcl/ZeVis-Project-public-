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

#include "Source/controller/Contents.h"
#include "Source/global.h"

class Contents;

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);

class ViewAxisGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Compatibility
{
	Q_OBJECT

public:
	
	explicit ViewAxisGLWidget(QWidget *parent=0);
	~ViewAxisGLWidget();

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;
	void rotateBy(int xAngle, int yAngle, int zAngle);
	void translateBy(float x, float y, float z);
	void scaleBy(float v);
	void setAxisIndex(int axis, int index);
	int AxisCode;
	int GLView_Index;
	void updateByAnotherView(bool);

	bool IsEmLoaded = false;

public slots:
	void updateByInterface(bool);

	void emLoaded();

signals:
	void clicked();
	void update_view_state(int, int, int, float);
	void updateAll(int, bool);
	void addAnnotation(QString m, QColor c, QVector3D sPos, QVector3D ePos, int a);
	void focusAnnotation(QListWidgetItem* item);

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent *event) override;

private:
	float viewScale = 1;
	bool CtrlKeyFlag = false;
	bool paintState = false;
	int WidgetWidth = 0;
	int WidgetHeight = 0;
	int CellTableWidth;
	bool ShiftKeyFlag = false;
	bool MakingAnnotation = false;
	QPoint AnnotationStartPos=QPoint(0,0);
	QPoint AnnotationEndPos=QPoint(0,0);

	int subregion_opacity[10];
	QVector4D subregion_color[10];

	block_info calcBlockIndex();
	unsigned int *emptyTexLabel;
	unsigned char *emptyTexSubregion;
	void drawCenterLine();
	void drawTile(block_info info);
	void uploadCellColor();
	void drawAnnotations(block_info info);

	bool bindEMLayer(block_info info);
	bool bindCellLayer(block_info info);
	int bindSubregionLayer(block_info info);

	unsigned int getCellIndexbyMousePos(QPointF pos);

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


	QOpenGLShaderProgram *program;
	GLuint LBTex;
	int con_LBTex;
	
	GLuint EMTex;
	int con_EMTex;

	GLuint SBTex[10];
	int con_SBTex[10];


	GLuint CellColorBuffer;
	GLuint CellColorTex;
	int con_CellColorTex;
	unsigned int *cell_color_data;


	Contents *mWidget;

};
