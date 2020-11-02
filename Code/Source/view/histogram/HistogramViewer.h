
#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QObject>
#include <QtCharts/QChartView>
#include <QtWidgets/QRubberBand>
#include <QtGui/QMouseEvent>
#include <QDebug>
QT_CHARTS_USE_NAMESPACE

class HistogramViewer : public QChartView
{
	Q_OBJECT
public:
	HistogramViewer(QChart *chart, QWidget *parent = 0);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);

signals:
	void update_move_pos(double s, double e, int f);
	void update_release_pos(double s, double e, int f);
	void clear_mouse();

private:
	void init();

	bool isPressed;
	int chartWidth;
	double XStartPos;
	double XEndPos;
};

#endif