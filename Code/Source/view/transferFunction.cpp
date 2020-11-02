///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "transferFunction.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QPainter>
#include <QtGui>
#include <QDockWidget>
#include <QColorDialog>

transferFunction::transferFunction(QWidget *parent)
	: QWidget(parent)
{
	setBackgroundRole(QPalette::Base);
	//    setAutoFillBackground(true);

	histogram_scale = 1;

	float xx, yy, rr, gg, bb;
	FILE *f = fopen("save_condition.txt", "r");
	if (f == NULL) {
		f = fopen("save_condition.txt", "w");
		fprintf(f, "0 0 0 0 0\n1 1 1 1 1\n-1 -1 -1 -1 -1");
		fclose(f);
		f = fopen("save_condition.txt", "r");
	}

	while (1) {
		fscanf(f, "%f %f %f %f %f", &xx, &yy, &rr, &gg, &bb);
		if (xx == -1)break;
		points.push_back(QVector2D(xx, yy));   //resolution
		colors.push_back(QVector3D(rr, gg, bb));
	}
	fclose(f);

	for (int i = 0; i < 256; i++)histoData[i] = 0, color_table[i] = QVector4D(0, 0, 0, 0);
	creat_delete = 0;
	QWidget::setFocusPolicy(Qt::ClickFocus);


}

transferFunction::~transferFunction() {
	FILE *f = fopen("save_condition.txt", "w");
	int size = points.size();
	for (int i = 0; i < size; i++) {
		fprintf(f, "%f %f %f %f %f\n"
			, points[i].x(), points[i].y()
			, colors[i].x(), colors[i].y(), colors[i].z());
	}
	fprintf(f, "-1 -1 -1 -1 -1");
	fclose(f);
}
void transferFunction::resizeEvent(QResizeEvent *event) {
	int w = event->size().width();
	int h = event->size().height();
	this->resize(w, h);
}

QSize transferFunction::minimumSizeHint() const
{
	return QSize(800, 800);
}

QSize transferFunction::sizeHint() const
{
	return QSize(800, 800);
}

void transferFunction::paintEvent(QPaintEvent *)
{

//	qDebug() << "tf2 start";
	int size = points.size();
	pen_size = this->width()*0.010;

	TF_start = QVector2D(this->width()*0.055, this->height()*0.88);
	TF_stop = QVector2D(this->width()*0.92, this->height()*0.15);


	QPainter painter(this);
	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
//	painter.drawPixmap(0, 0, this->width(), this->height(), background);//resolution




	int start_index = 0, stop_index = 1;
	for (int i = 0; i < 256; i++) {
		float t_num = histoData[i];
		QPen histo_pen;
		histo_pen.setWidthF((TF_stop.x() - TF_start.x()) / 255);
		QPointF start_t, stop_t;
		start_t = QPointF(TF_start.x() + i * (TF_stop.x() - TF_start.x()) / 255, TF_start.y());
		stop_t = QPointF(TF_start.x() + i * (TF_stop.x() - TF_start.x()) / 255, (TF_start.y() + sqrt(t_num)*(TF_stop.y() - TF_start.y())*histogram_scale));


		if (float(i) / 255 > points[stop_index].x())stop_index++, start_index++;

		QVector3D t_color;
		float t_a = ((float(i) / 255 - points[start_index].x())*points[stop_index].y() + (points[stop_index].x() - float(i) / 255)*points[start_index].y()) / (points[stop_index].x() - points[start_index].x());
		t_a = t_a * 3;
		if (t_a > 1)t_a = 1;
		t_color = ((float(i) / 255 - points[start_index].x())*colors[stop_index] + (points[stop_index].x() - float(i) / 255)*colors[start_index]) / (points[stop_index].x() - points[start_index].x());
		histo_pen.setColor(QColor(t_color.x() * 255 * t_a, t_color.y() * 255 * t_a, t_color.z() * 255 * t_a));
		painter.setPen(histo_pen);
		painter.drawLine(start_t, stop_t);
	}


//	qDebug() << "tf2 histogram drawed";


	start_index = 0, stop_index = 1;
	for (int i = 0; i < 256; i++) {
		QPen colormap_pen;
		QPointF start_t, stop_t;
		start_t = QPointF(TF_start.x() + i * (TF_stop.x() - TF_start.x()) / 255, TF_start.y() + 70);
		stop_t = QPointF(TF_start.x() + i * (TF_stop.x() - TF_start.x()) / 255, TF_start.y() + 20);

		if (float(i) / 255 > points[stop_index].x())stop_index++, start_index++;

		QVector3D t_color;
		t_color = ((float(i) / 255 - points[start_index].x())*colors[stop_index] + (points[stop_index].x() - float(i) / 255)*colors[start_index]) / (points[stop_index].x() - points[start_index].x());
		colormap_pen.setColor(QColor(t_color.x() * 255, t_color.y() * 255, t_color.z() * 255));
		colormap_pen.setWidth((TF_stop.x() - TF_start.x()) / 255.0 * 2);
		painter.setPen(colormap_pen);
		painter.drawLine(start_t, stop_t);
	}


//	qDebug() << "tf2 line drawed";


	float a = points[0].y();
	float r = colors[0].x();
	float g = colors[0].y();
	float b = colors[0].z();
	color_table[0] = QVector4D(r, g, b, a);
	for (int i = 0; i < size - 1; i++) {
		float a1 = points[i].y();
		float a2 = points[i + 1].y();
		float r1 = colors[i].x();
		float r2 = colors[i + 1].x();
		float g1 = colors[i].y();
		float g2 = colors[i + 1].y();
		float b1 = colors[i].z();
		float b2 = colors[i + 1].z();
		int x1 = int(points[i].x() * 255);
		int x2 = int(points[i + 1].x() * 255);

		for (int j = x1 + 1; j <= x2; j++) {

			float rr1 = (r2 - r1) / (x2 - x1)*(j - x1) + r1;
			float gg1 = (g2 - g1) / (x2 - x1)*(j - x1) + g1;
			float bb1 = (b2 - b1) / (x2 - x1)*(j - x1) + b1;
			float aa1 = (a2 - a1) / (x2 - x1)*(j - x1) + a1;
			float ta = aa1;
			r = rr1;
			g = gg1;
			b = bb1;
			a = ta;
			color_table[j] = QVector4D(r, g, b, a);
		}
	}

//	qDebug() << "tf2 generated";


	QPen t_pen;
	t_pen.setWidth(pen_size * 2);
	t_pen.setColor(QColor(int(colors[0].x() * 255), int(colors[0].y() * 255), int(colors[0].z() * 255)));
	painter.setPen(t_pen);
	painter.drawPoint(TF_start.x() + points[0].x()*(TF_stop.x() - TF_start.x()),
		TF_start.y() + points[0].y()*(TF_stop.y() - TF_start.y()));

	for (int i = 1; i < size; i++) {
		painter.setPen(Qt::NoPen);
		QPoint start_t, stop_t;
		QColor start_c, stop_c;
		start_c = QColor(int(colors[i - 1].x() * 255), int(colors[i - 1].y() * 255), int(colors[i - 1].z() * 255));
		stop_c = QColor(int(colors[i].x() * 255), int(colors[i].y() * 255), int(colors[i].z() * 255));

		start_t.setX(int(TF_start.x() + points[i - 1].x()*(TF_stop.x() - TF_start.x())));
		start_t.setY(int(TF_start.y() + points[i - 1].y()*(TF_stop.y() - TF_start.y())));
		stop_t.setX(int(TF_start.x() + points[i].x()*(TF_stop.x() - TF_start.x())));
		stop_t.setY(int(TF_start.y() + points[i].y()*(TF_stop.y() - TF_start.y())));

		QLinearGradient gradient(start_t, stop_t);
		gradient.setColorAt(0.0, start_c);
		gradient.setColorAt(1.0, stop_c);
		painter.setBrush(gradient);
		QPoint tt[4];
		tt[0] = start_t - QPoint(0, 1);
		tt[1] = start_t + QPoint(0, 1);
		tt[2] = stop_t + QPoint(0, 1);
		tt[3] = stop_t - QPoint(0, 1);

		painter.drawPolygon(tt, 4);
		t_pen.setWidth(pen_size * 2 + 1);
		t_pen.setColor(QColor(0, 0, 0));
		painter.setBrush(Qt::NoBrush);
		painter.setPen(t_pen);
		painter.drawPoint(stop_t);

		t_pen.setWidth(pen_size * 2);
		t_pen.setColor(stop_c);
		painter.setBrush(Qt::NoBrush);
		painter.setPen(t_pen);
		painter.drawPoint(stop_t);
	}

//	qDebug() << "tf2 finish";

	emit edited();

}
void transferFunction::generate() {
	int size = points.size();

	float a = points[0].y();
	float r = colors[0].x();
	float g = colors[0].y();
	float b = colors[0].z();
	color_table[0] = QVector4D(r, g, b, a);
	for (int i = 0; i < size - 1; i++) {
		float a1 = points[i].y();
		float a2 = points[i + 1].y();
		float r1 = colors[i].x();
		float r2 = colors[i + 1].x();
		float g1 = colors[i].y();
		float g2 = colors[i + 1].y();
		float b1 = colors[i].z();
		float b2 = colors[i + 1].z();
		int x1 = int(points[i].x() * 255);
		int x2 = int(points[i + 1].x() * 255);

		for (int j = x1 + 1; j <= x2; j++) {

			float rr1 = (r2 - r1) / (x2 - x1)*(j - x1) + r1;
			float gg1 = (g2 - g1) / (x2 - x1)*(j - x1) + g1;
			float bb1 = (b2 - b1) / (x2 - x1)*(j - x1) + b1;
			float aa1 = (a2 - a1) / (x2 - x1)*(j - x1) + a1;
			float ta = aa1;
			r = rr1;
			g = gg1;
			b = bb1;
			a = ta;
			color_table[j] = QVector4D(r, g, b, a);
		}
	}


}

void transferFunction::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
	button = event->button();

	QPoint t_p;

	button = event->button();
	if (event->button() == Qt::MidButton) {
		int size = points.size();
		for (int i = 0; i < size; i++) {
			t_p = QPoint(int(TF_start.x() + points[i].x()*(TF_stop.x() - TF_start.x())),
				int(TF_start.y() + points[i].y()*(TF_stop.y() - TF_start.y())));
			if ((lastPos - t_p).x() < pen_size && (lastPos - t_p).x() > -pen_size && (lastPos - t_p).y() < pen_size && (lastPos - t_p).y() > -pen_size) {
				QColor t_c;
				t_c.setRgb((QColorDialog::getColor(QColor(int(colors[i].x() * 255), int(colors[i].y() * 255), int(colors[i].z() * 255)))).rgb());
				colors[i] = QVector3D(float(t_c.red()) / 255, float(t_c.green()) / 255, float(t_c.blue()) / 255);
				break;
			}
		}
	}
	else if (event->button() == Qt::RightButton) {

		if (creat_delete == 1) {
			int size = points.size();
			for (int i = 1; i < size - 1; i++) {
				t_p = QPoint(int(TF_start.x() + points[i].x()*(TF_stop.x() - TF_start.x())),
					int(TF_start.y() + points[i].y()*(TF_stop.y() - TF_start.y())));

				if ((lastPos - t_p).x() < pen_size && (lastPos - t_p).x() > -pen_size && (lastPos - t_p).y() < pen_size && (lastPos - t_p).y() > -pen_size) {
					points.remove(i);
					colors.remove(i);
					break;
				}
			}
		}
		else {
			if (lastPos.x() > TF_start.x() && lastPos.x() < TF_stop.x() && lastPos.y() <= TF_start.y() && lastPos.y() >= TF_stop.y()) {//resolution
				int size = points.size();
				for (int i = 0; i < size; i++) {
					t_p = QPoint(int(TF_start.x() + points[i].x()*(TF_stop.x() - TF_start.x())),
						int(TF_start.y() + points[i].y()*(TF_stop.y() - TF_start.y())));

					if (t_p.x() > lastPos.x()) {
						points.insert(i, QVector2D((lastPos.x() - TF_start.x()) / (TF_stop.x() - TF_start.x()),
							(lastPos.y() - TF_start.y()) / (TF_stop.y() - TF_start.y())));
						colors.insert(i, QVector3D((colors[i - 1] + colors[i]) / 2));
						break;
					}
				}
			}
		}
	}
	else if (event->button() == Qt::LeftButton) {
		int size = points.size();
		for (int i = 0; i < size; i++) {
			t_p = QPoint(int(TF_start.x() + points[i].x()*(TF_stop.x() - TF_start.x())),
				int(TF_start.y() + points[i].y()*(TF_stop.y() - TF_start.y())));
			if ((lastPos - t_p).x() < pen_size && (lastPos - t_p).x() > -pen_size && (lastPos - t_p).y() < pen_size && (lastPos - t_p).y() > -pen_size) {
				num = i;
				break;
			}
			num = -1;
		}

	}
	update();

}

void transferFunction::mouseMoveEvent(QMouseEvent *event)
{
	lastPos = event->pos();
	if (button == Qt::LeftButton) {
		if (num != -1) {
			if (lastPos.x() <= TF_start.x())lastPos.setX(TF_start.x() + 1); //resolution
			if (lastPos.x() >= TF_stop.x())lastPos.setX(TF_stop.x() - 1);//resolution
			if (lastPos.y() > TF_start.y())lastPos.setY(TF_start.y());//resolution
			if (lastPos.y() < TF_stop.y())lastPos.setY(TF_stop.y());//resolution
			int size = points.size();
			if (num == 0 || num == size - 1) {
				points[num].setY((lastPos.y() - TF_start.y()) / (TF_stop.y() - TF_start.y()));
			}
			else {
				for (int i = 0; i < size; i++) {
					QPoint t_p = QPoint(int(TF_start.x() + points[i].x()*(TF_stop.x() - TF_start.x())),
						int(TF_start.y() + points[i].y()*(TF_stop.y() - TF_start.y())));
					if (t_p.x() > lastPos.x()) {
						points.remove(num);
						QVector3D t = colors[num];
						colors.remove(num);
						if (num < i) {
							points.insert(i - 1, QVector2D((lastPos.x() - TF_start.x()) / (TF_stop.x() - TF_start.x()),
								(lastPos.y() - TF_start.y()) / (TF_stop.y() - TF_start.y())));
							colors.insert(i - 1, t);
							num = i - 1;
						}
						else {
							points.insert(i, QVector2D((lastPos.x() - TF_start.x()) / (TF_stop.x() - TF_start.x()),
								(lastPos.y() - TF_start.y()) / (TF_stop.y() - TF_start.y())));
							colors.insert(i, t);
							num = i;
						}
						break;
					}
				}
			}
		}

	}
	else if (button == Qt::RightButton) {
	}
	update();

}

void transferFunction::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Insert) {
		creat_delete = (creat_delete == 0) ? 1 : 0;
	}
}


void transferFunction::set_histogram_scale(int n) {
	histogram_scale = n;
	update();
}

void transferFunction::timerEvent(QTimerEvent *event) {

}

