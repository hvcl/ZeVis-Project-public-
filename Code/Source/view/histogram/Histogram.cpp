#include "Histogram.h"


Histogram::Histogram(QGraphicsItem *parent, Qt::WindowFlags wFlags)
	: QChart(QChart::ChartTypeCartesian, parent, wFlags)
{
	// Seems that QGraphicsView (QChartView) does not grab gestures.
	// They can only be grabbed here in the QGraphicsWidget (QChart).
	//grabGesture(Qt::PanGesture);
	//grabGesture(Qt::PinchGesture);
}

Histogram::~Histogram()
{

}

bool Histogram::sceneEvent(QEvent *event)
{
	qDebug() << "Event";
	return QChart::event(event);
}


