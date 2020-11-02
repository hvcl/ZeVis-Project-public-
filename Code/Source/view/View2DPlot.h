#pragma once

#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMatrix4x4>
#include <QVector2D>
#include <QThread>

class CellSubsetExplorer;

class View2DPlot :public QWidget
{
	Q_OBJECT

public:
	View2DPlot(CellSubsetExplorer *parent = 0);
	~View2DPlot();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;

	CellSubsetExplorer *mWidget;
public slots:


signals:


protected:
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;


private:
	
	QPoint m_lastPos;

	QPoint lastPos;
	Qt::MouseButton button;


	QVector2D graphStart; //graph start point
	QVector2D graphEnd; //graph end point

	QVector2D prevP;

	bool doSelect = false;

	bool doMove = false;
	int moveP = -1;

	QVector2D maxValue;
	QVector2D minValue;



};
