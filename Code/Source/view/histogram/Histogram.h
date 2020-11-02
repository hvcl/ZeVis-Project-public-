#ifndef CHART_H
#define CHART_H
#include <QtCharts>

#include <QtWidgets/QGesture>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>

class QGestureEvent;
QT_CHARTS_USE_NAMESPACE

class Histogram : public QChart
{
public:
	explicit Histogram(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
	~Histogram();

protected:
	bool sceneEvent(QEvent *event);

private:

};
#endif // CHART_H
