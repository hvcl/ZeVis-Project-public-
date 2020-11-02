///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "View2DPlot.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QPainter>
#include <QtGui>
#include <QDockWidget>
#include <QColorDialog>

#include <iostream>
#include <QDebug>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
#include "Source/controller/CellSubsetExplorer.h"


View2DPlot::View2DPlot(CellSubsetExplorer *parent)
	: QWidget(parent)
{
	mWidget = parent;

	doSelect = false;
	
	startTimer(200);
	setBackgroundRole(QPalette::Base);
	QWidget::setFocusPolicy(Qt::ClickFocus);
	setMouseTracking(true);

}

View2DPlot::~View2DPlot() {
}

void View2DPlot::resizeEvent(QResizeEvent *event) {
	int w = event->size().width();
	int h = event->size().height();
	int t = w > h ? h : w;
	this->resize(t, t);
}

QSize View2DPlot::minimumSizeHint() const
{
	return QSize(100, 100);
}

QSize View2DPlot::sizeHint() const
{
	return QSize(800, 800);
}

void View2DPlot::paintEvent(QPaintEvent *)
{

	graphStart = QVector2D(this->width()*0.1, this->height()*0.9);
	graphEnd = QVector2D(this->width()*0.9, this->height()*0.1);


	QPainter painter(this);
	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
	//     painter.drawPixmap(0,0,this->width(),this->height(),background);//resolution

	QPen t_pen;
	QBrush t_brush(Qt::SolidPattern);

	////Draw frame
	///
	///
	t_pen.setColor(QColor(150, 150, 150));
	t_pen.setWidth(1);
	painter.setPen(t_pen);
	painter.drawLine(this->width()*0.02, this->height()*0.02, this->width()*0.98, this->height()*0.02);
	painter.drawLine(this->width()*0.98, this->height()*0.02, this->width()*0.98, this->height()*0.98);
	painter.drawLine(this->width()*0.98, this->height()*0.98, this->width()*0.02, this->height()*0.98);
	painter.drawLine(this->width()*0.02, this->height()*0.98, this->width()*0.02, this->height()*0.02);

	t_pen.setColor(QColor(200, 200, 200));
	t_pen.setStyle(Qt::DashLine);
	painter.setPen(t_pen);
	for (int i = 0; i <= 10; i++) {
		painter.drawLine(this->width()*0.02, graphStart.y() + (graphEnd.y() - graphStart.y()) / 10.0*i, this->width()*0.98, graphStart.y() + (graphEnd.y() - graphStart.y()) / 10.0*i);
	}
	for (int i = 0; i <= 10; i++) {
		painter.drawLine(graphStart.x() + (graphEnd.x() - graphStart.x()) / 10.0*i, this->height()*0.02, graphStart.x() + (graphEnd.x() - graphStart.x()) / 10.0*i, this->height()*0.98);
	}

	if (mWidget->inputData == NULL) {
		return;
	}
	if (mWidget->IsCellIncluded == NULL) {
		return;
	}
	if (mWidget->input_vector == NULL) {
		return;
	}

	painter.setPen(Qt::NoPen);

	int xaxis = mWidget->xaxis_combobox->currentIndex();
	int yaxis = mWidget->yaxis_combobox->currentIndex();

	for (int i = 0; i < mWidget->n; i++) {
		int type = 0;
		float radius = 2;
		for (int j = 0; j < 64; j++) {
			if (mWidget->cell_objects[j]->label_value == mWidget->input_vector[i]) {
				type = mWidget->cell_objects[j]->selectType;
				break;
			}
		}
		if (type == 0) {
			if(mWidget->IsCellIncluded[i])
				t_brush.setColor(QColor(255,255,255));
			else
				t_brush.setColor(QColor(0, 0, 0));
			radius = 7;
			painter.setBrush(t_brush);
			painter.drawEllipse(graphStart.x() + (graphEnd.x() - graphStart.x()) * mWidget->inputData[i * 8 + xaxis]-radius/2,
				graphStart.y() + (graphEnd.y() - graphStart.y()) * mWidget->inputData[i * 8 + yaxis] - radius / 2,
				radius, radius);
		}
	}


	for (int i = 0; i < mWidget->n; i++) {
		int type = 0;
		float radius = 2;
		for (int j = 0; j < 64; j++) {
			if (mWidget->cell_objects[j]->label_value == mWidget->input_vector[i]) {
				type = mWidget->cell_objects[j]->selectType;
				if (type == 1) {
					t_brush.setColor(QColor(0.306 * 255, 0.745 * 255, 0.667 * 255));
					radius = 10;
					painter.setBrush(t_brush);

					painter.drawEllipse(graphStart.x() + (graphEnd.x() - graphStart.x()) * mWidget->inputData[i * 8 + xaxis] - radius / 2,
						graphStart.y() + (graphEnd.y() - graphStart.y()) * mWidget->inputData[i * 8 + yaxis] - radius / 2,
						radius, radius);

				}
				else if (type == 2) {
					t_brush.setColor(QColor(255, 0, 0));
					radius = 10;
					painter.setBrush(t_brush);

					painter.drawEllipse(graphStart.x() + (graphEnd.x() - graphStart.x()) * mWidget->inputData[i * 8 + xaxis] - radius / 2,
						graphStart.y() + (graphEnd.y() - graphStart.y()) * mWidget->inputData[i * 8 + yaxis] - radius / 2,
						radius, radius);
				}
			}
		}




		//painter.drawPoint(graphStart.x() + (graphEnd.x() - graphStart.x()) * mWidget->inputData[i * 8 + xaxis],
		//	graphStart.y() + (graphEnd.y() - graphStart.y()) * mWidget->inputData[i * 8 + yaxis]);
	}


	if (doSelect) {
		t_brush.setColor(QColor(0, 0, 255));
		float radius = 10;
		painter.setBrush(t_brush);

		for (int i = 0; i < mWidget->selectPoints->size(); i++) {
			QVector2D p1 = QVector2D(graphStart.x() + (graphEnd.x() - graphStart.x()) * (*mWidget->selectPoints)[i].x(),
				graphStart.y() + (graphEnd.y() - graphStart.y()) * (*mWidget->selectPoints)[i].y());
			painter.drawEllipse(p1.x() - radius / 2, p1.y() - radius / 2,
				radius, radius);

		}
		t_pen.setColor(QColor(0, 0, 255));
		t_pen.setWidth(2);
		painter.setPen(t_pen);
		for (int i = 0; i < mWidget->selectPoints->size() - 1; i++) {
			QVector2D p1 = QVector2D(graphStart.x() + (graphEnd.x() - graphStart.x()) * (*mWidget->selectPoints)[i].x(),
				graphStart.y() + (graphEnd.y() - graphStart.y()) * (*mWidget->selectPoints)[i].y());
			QVector2D p2 = QVector2D(graphStart.x() + (graphEnd.x() - graphStart.x()) * (*mWidget->selectPoints)[i + 1].x(),
				graphStart.y() + (graphEnd.y() - graphStart.y()) * (*mWidget->selectPoints)[i + 1].y());
			painter.drawLine(p1.x(), p1.y(), p2.x(), p2.y());
		}
	}
	if (*mWidget->is_selection) {
		t_brush.setColor(QColor(0, 0, 255));
		float radius = 10;
		painter.setBrush(t_brush);

		for (int i = 0; i < mWidget->selectPoints->size(); i++) {
			QVector2D p1 = QVector2D(graphStart.x() + (graphEnd.x() - graphStart.x()) * (*mWidget->selectPoints)[i].x(),
				graphStart.y() + (graphEnd.y() - graphStart.y()) * (*mWidget->selectPoints)[i].y());
			painter.drawEllipse(p1.x() - radius / 2, p1.y() - radius / 2,
				radius, radius);
		}
		t_pen.setColor(QColor(0, 0, 255));
		t_pen.setWidth(2);
		t_pen.setStyle(Qt::SolidLine);

		painter.setPen(t_pen);
		for (int i = 0; i < mWidget->selectPoints->size(); i++) {
			int next = (i + 1) % mWidget->selectPoints->size();
			QVector2D p1 = QVector2D(graphStart.x() + (graphEnd.x() - graphStart.x()) * (*mWidget->selectPoints)[i].x(),
				graphStart.y() + (graphEnd.y() - graphStart.y()) * (*mWidget->selectPoints)[i].y());
			QVector2D p2 = QVector2D(graphStart.x() + (graphEnd.x() - graphStart.x()) * (*mWidget->selectPoints)[next].x(),
				graphStart.y() + (graphEnd.y() - graphStart.y()) * (*mWidget->selectPoints)[next].y());
			painter.drawLine(p1.x(), p1.y(), p2.x(), p2.y());
		}
		painter.setPen(Qt::NoPen);
		t_brush.setColor(QColor(0, 0, 255, 50));
		painter.setBrush(t_brush);
		QPoint poly[50];
		for (int i = 0; i < mWidget->selectPoints->size(); i++) {
			poly[i].setX(graphStart.x() + (graphEnd.x() - graphStart.x()) * (*mWidget->selectPoints)[i].x());
			poly[i].setY(graphStart.y() + (graphEnd.y() - graphStart.y()) * (*mWidget->selectPoints)[i].y());
		}
		painter.drawPolygon(poly, mWidget->selectPoints->size());


		t_pen.setColor(QColor(0, 255, 255));
		t_pen.setWidth(2);
		t_pen.setStyle(Qt::SolidLine);
		painter.setPen(t_pen);
		QFont t;
		t.setBold(true);
		painter.setFont(t);
		int xaxis = *mWidget->xaxis;
		int yaxis = *mWidget->yaxis;
		for (int i = 0; i < mWidget->selectPoints->size(); i++) {
			QVector2D p1 = QVector2D(graphStart.x() + (graphEnd.x() - graphStart.x()) * (*mWidget->selectPoints)[i].x(),
				graphStart.y() + (graphEnd.y() - graphStart.y()) * (*mWidget->selectPoints)[i].y());
			float xvalue = (*mWidget->selectPoints)[i].x()*(mWidget->max_values[xaxis] - mWidget->min_values[xaxis]) + mWidget->min_values[xaxis];
			float yvalue = (*mWidget->selectPoints)[i].y()*(mWidget->max_values[yaxis] - mWidget->min_values[yaxis]) + mWidget->min_values[yaxis];
			painter.drawText(p1.x(), p1.y(), "(" + QString::number(xvalue));
			painter.drawText(p1.x(), p1.y() + 20, "," + QString::number(yvalue) + ")");
		}
		painter.setPen(Qt::NoPen);
	}

}

void View2DPlot::mousePressEvent(QMouseEvent *event)
{
	if (event->x() < 0 || event->x() > this->width() || event->y() < 0 || event->y() > this->height())return;

	lastPos = event->pos();
	button = event->button();


	if (event->button() == Qt::LeftButton) {
		if (mWidget->is_selection == NULL)return;

		if (*mWidget->is_selection == true) {
			for (int i = 0; i < mWidget->selectPoints->size(); i++) {
				QVector2D p1 = QVector2D(graphStart.x() + (graphEnd.x() - graphStart.x()) * (*mWidget->selectPoints)[i].x(),
					graphStart.y() + (graphEnd.y() - graphStart.y()) * (*mWidget->selectPoints)[i].y());
				QVector2D p2 = QVector2D(event->pos().x(), event->pos().y());
					//QVector2D((event->x() - graphStart.x()) / (graphEnd.x() - graphStart.x()),
//				(event->y() - graphStart.y()) / (graphEnd.y() - graphStart.y()));
				if ((p1 - p2).length() < 10) {
					doMove = true;
					moveP = i;
					break;
				}
			}
			if (!doMove) {
				*mWidget->is_selection = false;
				mWidget->selectPoints->clear();
				doSelect = false;
				mWidget->cellIncludeCheck();
			}
		}
		else {
			bool flag = false;

			for (int i = 0; i < mWidget->selectPoints->size(); i++) {
				QVector2D p1 = QVector2D(graphStart.x() + (graphEnd.x() - graphStart.x()) * (*mWidget->selectPoints)[i].x(),
					graphStart.y() + (graphEnd.y() - graphStart.y()) * (*mWidget->selectPoints)[i].y());
				QVector2D p2 = QVector2D(event->pos().x(), event->pos().y());
				if ((p1 - p2).length() < 10) {
					if (mWidget->selectPoints->size() < 3) {
						*mWidget->is_selection = false;
						mWidget->selectPoints->clear();
						doSelect = false;
					}
					else {
						*mWidget->is_selection = true;
						mWidget->cellIncludeCheck();
						doSelect = false;
					}
					flag = true;
					break;
				}
			}
			if (flag == false) {
				for (int i = 0; i < mWidget->selectPoints->size()-2; i++) {
					QVector2D p1 = (*mWidget->selectPoints)[i];
					QVector2D q1 = (*mWidget->selectPoints)[i+1];

					QVector2D p2 = (*mWidget->selectPoints)[mWidget->selectPoints->size() - 1];
					QVector2D q2 = QVector2D((event->x() - graphStart.x()) / (graphEnd.x() - graphStart.x()),
						(event->y() - graphStart.y()) / (graphEnd.y() - graphStart.y()));

					if (mWidget->doIntersect(p1, q1, p2, q2)) {
						*mWidget->is_selection = false;
						mWidget->selectPoints->clear();
						doSelect = false;
						flag = true;
						break;
					}
				}
			}
			if (flag == false) {
				QVector2D p1 = QVector2D((event->x() - graphStart.x()) / (graphEnd.x() - graphStart.x()),
					(event->y() - graphStart.y()) / (graphEnd.y() - graphStart.y()));
				mWidget->selectPoints->push_back(p1);
				doSelect = true;
			}

		}

	}
	else if (event->button() == Qt::MidButton) {

	}
	else if (event->button() == Qt::RightButton) {

	}
	update();


}
void View2DPlot::mouseReleaseEvent(QMouseEvent *event) {

	if (event->button() == Qt::LeftButton) {
		if (doMove) {
			doMove = false;
			mWidget->cellIncludeCheck();
		}
	}
	if (event->button() == Qt::RightButton) {

	}
	update();
}
void View2DPlot::mouseMoveEvent(QMouseEvent *event)
{
	setFocus();



	if (event->x() < 0 || event->x() > this->width() || event->y() < 0 || event->y() > this->height())return;

	float tx = event->x();
	float ty = event->y();


	if (event->buttons() & Qt::LeftButton) {
		if (doMove) {
			(*mWidget->selectPoints)[moveP].setX((event->x() - graphStart.x()) / (graphEnd.x() - graphStart.x()));
			(*mWidget->selectPoints)[moveP].setY((event->y() - graphStart.y()) / (graphEnd.y() - graphStart.y()));
			update();
		}
	}



}

void View2DPlot::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape) {
		if (doSelect) {
			*mWidget->is_selection = false;
			mWidget->selectPoints->clear();
			doSelect = false;
			update();
		}
	}

}

void View2DPlot::keyReleaseEvent(QKeyEvent *event)
{
	
}

void View2DPlot::timerEvent(QTimerEvent *event) {
}


