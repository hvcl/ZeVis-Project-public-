///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////


#include "ViewCellGLWidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QProcess>
#include <QColorDialog>
#include <QVector2D>
#include <QPainter>
#include <QLineEdit>
#include "Source/global.h"


void ViewCellGLWidget::setVolumeData(int w_, int h_, int d_, unsigned char *volume_, unsigned int *label_,int l) {
	label_value = l;
	selectType = 0;
	w = w_;
	h = h_;
	d = d_;
	if (volume_data != NULL)delete[]volume_data;
	if (label_data != NULL)delete[]label_data;

	volume_data = new unsigned char[w*h*d]();
	label_data = new unsigned char[w*h*d]();

	qDebug() << "volume data init";

	sample = sqrt(float(w*w + h * h + d * d));




	for (int iz = 0; iz < d; iz++) {
		for (int iy = 0; iy < h; iy++) {
			for (int ix = 0; ix < w; ix++) {
				int loc = iz * w*h + iy * w + ix;
				volume_data[loc] = volume_[loc];
				if (label_[loc] != l)volume_data[loc] = 250;
			}
		}
	}
	for (int iz = 2; iz < d-2; iz++) {
		for (int iy = 2; iy < h-2; iy++) {
			for (int ix = 2; ix < w-2; ix++) {
				int loc = iz * w*h + iy * w + ix;
				if (label_[loc] == l) {
					if (label_[loc + 1] != l || label_[loc - 1] != l
						|| label_[loc + w] != l || label_[loc - w] != l
						|| label_[loc + w * h] != l || label_[loc - w * h] != l
						|| label_[loc + 2] != l || label_[loc - 2] != l
						|| label_[loc + w*2] != l || label_[loc - w*2] != l
						|| label_[loc + w * h*2] != l || label_[loc - w * h*2] != l) {
						label_data[loc] = 255;
						volume_data[loc] = volume_[loc];
					}
				}
				if (label_[loc] != l) {
					if (label_[loc + 1] == l || label_[loc - 1] == l
						|| label_[loc + w] == l || label_[loc - w] == l
						|| label_[loc + w * h] == l || label_[loc - w * h] == l
						|| label_[loc + 2] == l || label_[loc - 2] == l
						|| label_[loc + w * 2] == l || label_[loc - w * 2] == l
						|| label_[loc + w * h * 2] == l || label_[loc - w * h * 2] == l) {
						label_data[loc] = 255;
						volume_data[loc] = volume_[loc];

					}
				}
			}
		}
	}

	qDebug() << "volume data refine";


	for (int i = w * h * d - 1; i >= 0; i--) {
		if (volume_data[i] < 250)volume_data[i] = 250 - volume_data[i];
		else volume_data[i] = 0;
	}

	unsigned __int64 *histo;
	unsigned __int64 total_histo;
	total_histo = 0;
	histo = new unsigned __int64[256];
	for (int i = 0; i < 256; i++) {
		histo[i] = 0;
	}
	for (int i = 0; i < w*h*d; i++) {
		if (volume_data[i] != 0) {
			total_histo++;
			histo[volume_data[i]]++;
		}
	}
	int peak_ind = 0;
	float peak_v = 0;
	for (int j = 0; j < 256; j++) {
		tf->histoData[j] = float(histo[j]) / total_histo;
		if (tf->histoData[j] > peak_v)peak_v = tf->histoData[j], peak_ind = j;
	}
	peak_pos = float(peak_ind) / 255;
	delete[]histo;

	qDebug() << "histogram init";

	isVolumeChanged = true;

	qDebug() << "volume texture init";


	if (isVolumeInit)update();
	isVolumeInit = true;



}


QVector3D ViewCellGLWidget::cross_product(QVector3D v1, QVector3D v2) {
	QVector3D v;
	v.setX(v1.y()*v2.z() - v2.y()*v1.z());
	v.setY(v1.z()*v2.x() - v2.z()*v1.x());
	v.setZ(v1.x()*v2.y() - v2.x()*v1.y());
	return v;
}

ViewCellGLWidget::ViewCellGLWidget(QWidget *parent)
	: QOpenGLWidget(parent),
	m_program(0)
{

	shift = QVector2D(0, 0);
	z_scale = 1;

	reset_color = 0;

	EYE = 5;

	sampling_rate = 3;

	phong_enable = 1;

	center1 = QVector3D(0, 0, 0);
	eye_position = QVector3D(0, 0, EYE);
	up = QVector3D(0, 1, 0);


	ambi = 0.3, diffu = 1.0, specu = 10, shin = 3;


	tog = 1;
	scale = 1;
//	startTimer(10);
	QWidget::setFocusPolicy(Qt::ClickFocus);

	background_color = QVector4D(0.2078, 0.2078, 0.2078, 1);
	l_t = 0.3;

}

ViewCellGLWidget::~ViewCellGLWidget()
{
	cleanup();
}

QSize ViewCellGLWidget::minimumSizeHint() const
{
	return QSize(20, 20);
}

QSize ViewCellGLWidget::sizeHint() const
{
	return QSize(20, 20);
}



void ViewCellGLWidget::cleanup()
{
	makeCurrent();
	delete m_program;
	m_program = 0;
	doneCurrent();
}


void ViewCellGLWidget::initializeGL()
{
	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ViewCellGLWidget::cleanup);
	initializeOpenGLFunctions();
	glClearColor(0, 0, 0, 1);

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	vshader->compileSourceFile("Resources/glsl/vshader_cellview.glsl");

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	fshader->compileSourceFile("Resources/glsl/fshader_cellview.glsl");


	m_program = new QOpenGLShaderProgram;
	m_program->addShader(vshader);
	m_program->addShader(fshader);

	m_program->link();
	m_program->bind();

	con_volume = m_program->uniformLocation("volume");
	con_label = m_program->uniformLocation("label");

	con_eye_pos = m_program->uniformLocation("eye_position");
	con_box_min = m_program->uniformLocation("box_min");
	con_box_max = m_program->uniformLocation("box_max");
	con_up = m_program->uniformLocation("up");
	con_sample = m_program->uniformLocation("sample");

	con_phong_enable = m_program->uniformLocation("phong_enable");


	con_l_t = m_program->uniformLocation("l_t");
	con_specu = m_program->uniformLocation("Uspecu");
	con_diffu = m_program->uniformLocation("Udiffu");
	con_ambi = m_program->uniformLocation("Uambi");
	con_shin = m_program->uniformLocation("Ushin");

	con_sampling_rate = m_program->uniformLocation("sampling_rate");

	con_window_size = m_program->uniformLocation("window_size");


	glGenTextures(1, &volume);
	glBindTexture(GL_TEXTURE_3D, volume);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, 10, 10, 10, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(con_volume, 0);
	glBindTexture(GL_TEXTURE_3D, volume);

	glGenTextures(1, &label);
	glBindTexture(GL_TEXTURE_3D, label);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, 10, 10, 10, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(con_label, 1);
	glBindTexture(GL_TEXTURE_3D, label);

	con_v_size = m_program->uniformLocation("v_size");

	con_color_table = m_program->uniformLocation("color_table");

	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_1D, color_tex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, 0);
	glActiveTexture(GL_TEXTURE2);
	glUniform1i(con_color_table, 2);
	glBindTexture(GL_TEXTURE_1D, color_tex);

	tf->generate();
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGBA, GL_FLOAT, tf->color_table);


	con_background_color = m_program->uniformLocation("background_color");

	m_program->release();
	glDisable(GL_DEPTH_TEST);

}

void ViewCellGLWidget::ReadColorTable() {
	glBindTexture(GL_TEXTURE_1D, color_tex);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGBA, GL_FLOAT, tf->color_table);
}
void ViewCellGLWidget::paintGL()
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (isVolumeInit == false) {
		return;
	}

	if (isVolumeChanged) {
		isVolumeChanged = false;
		glDeleteTextures(1, &volume);
		glGenTextures(1, &volume);
		glBindTexture(GL_TEXTURE_3D, volume);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, d, 0, GL_RED, GL_UNSIGNED_BYTE, volume_data);
	
		glDeleteTextures(1, &label);
		glGenTextures(1, &label);
		glBindTexture(GL_TEXTURE_3D, label);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, d, 0, GL_RED, GL_UNSIGNED_BYTE, label_data);

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(con_volume, 0);
		glBindTexture(GL_TEXTURE_3D, volume);

		glActiveTexture(GL_TEXTURE1);
		glUniform1i(con_label, 1);
		glBindTexture(GL_TEXTURE_3D, label);

		glActiveTexture(GL_TEXTURE2);
		glUniform1i(con_color_table, 2);
		glBindTexture(GL_TEXTURE_1D, color_tex);

		if (tf->points.length() > 5) {
			tf->points[1].setX(0.1907 - (0.16644 - peak_pos));
			tf->points[2].setX(0.2366 - (0.16644 - peak_pos));
			tf->points[3].setX(0.2937 - (0.16644 - peak_pos));
			tf->points[4].setX(0.3259 - (0.16644 - peak_pos));
			tf->points[5].setX(0.5566 - (0.16644 - peak_pos));
			tf->generate();
			tf->update();
		}
	}

	if (selectType == 0) {
		background_color = QVector4D(1, 1, 1, 1);
	}
	else if (selectType == 1) {
		background_color = QVector4D(0.306, 0.745, 0.667, 1);
	}
	else if (selectType == 2) {
		background_color = QVector4D(1, 0.42, 0.447, 1);
	}

	QVector3D box_t(w, h, d);
	const_box_min = -box_t.normalized();
	const_box_max = box_t.normalized();
	QVector3D right = cross_product(up, center1 - eye_position).normalized();

	box_min = (const_box_min + right * shift.x() + up * shift.y())*scale;
	box_max = (const_box_max + right * shift.x() + up * shift.y())*scale;

	m_program->bind();

	ReadColorTable();
	m_program->setUniformValue(con_window_size, QVector2D(this->width() / 800.0, this->height() / 800.0));
	m_program->setUniformValue(con_eye_pos, eye_position);
	m_program->setUniformValue(con_up, up);
	m_program->setUniformValue(con_box_min, box_min);
	m_program->setUniformValue(con_box_max, box_max);
	m_program->setUniformValue(con_sample, sample / scale);
	m_program->setUniformValue(con_diffu, diffu);
	m_program->setUniformValue(con_ambi, ambi);
	m_program->setUniformValue(con_specu, specu);
	m_program->setUniformValue(con_shin, shin);
	m_program->setUniformValue(con_l_t, l_t);
	m_program->setUniformValue(con_background_color, background_color);
	m_program->setUniformValue(con_phong_enable, phong_enable);
	m_program->setUniformValue(con_v_size, QVector3D(w, h, d));
	m_program->setUniformValue(con_sampling_rate, sampling_rate);

	glBegin(GL_QUADS);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, 1);
	glEnd();

	m_program->release();

	float um_height = (box_max.y()-box_min.y())/this->height() * 800.0 / (h * mGlobals.CurrentProject->ResolutionXnm * 0.001);

	glColor3f(1, 0.5, 0.5);
	glBegin(GL_LINES);
	glLineWidth(2);
	glVertex3f(0.9, -0.5, 1);
	glVertex3f(0.9, -0.5 + um_height*1, 1);
	glVertex3f(0.89, -0.5, 1);
	glVertex3f(0.91, -0.5, 1);
	glVertex3f(0.89, -0.5 + um_height*1, 1);
	glVertex3f(0.91, -0.5 + um_height*1, 1);
	glEnd();


	QPainter painter(this);
	painter.setPen(QColor(255, 128, 128, 255));
	//    painter.drawLine(10,10,100,100);
	QFont t;
	t.setBold(false);
	painter.setFont(t);
	painter.drawText(this->width()*0.9, this->height()*0.73, 30, 20, Qt::AlignCenter, QString("1") + QChar(181) + QString("m"));


	painter.end();


}

void ViewCellGLWidget::resizeGL(int w, int h)
{
	//    int t=w>h?w:h;
	//    this->resize(t,t);
}

void ViewCellGLWidget::mousePressEvent(QMouseEvent *event)
{
	m_lastPos = event->pos();
	m_clickPos = event->pos();

}
void ViewCellGLWidget::mouseReleaseEvent(QMouseEvent *event) {
	float mov = abs(event->pos().x() - m_clickPos.x()) + abs(event->pos().y() - m_clickPos.y());
	if (mov < 2) { //click event
		if (event->modifiers() & Qt::ShiftModifier) {
			int listIndex = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[label_value];
			cell curCell = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[listIndex];

			int xmin = curCell.minbox.x;
			int xmax = curCell.maxbox.x;
			int ymin = curCell.minbox.y;
			int ymax = curCell.maxbox.y;
			int zmin = curCell.minbox.z;
			int zmax = curCell.maxbox.z;

			mGlobals.CurrentProject->ViewPos_X = (xmin+xmax)*0.5;
			mGlobals.CurrentProject->ViewPos_Y = (ymin+ymax)*0.5;
			mGlobals.CurrentProject->ViewPos_Z = (zmin+zmax)*0.5;
			mGlobals.mainWindow->mTopInterface->update_value(mGlobals.CurrentProject->ViewPos_X, mGlobals.CurrentProject->ViewPos_Y, mGlobals.CurrentProject->ViewPos_Z, mGlobals.CurrentProject->ViewZoomLevel);
			mGlobals.mainWindow->mContents->contents_center_tabwidget->setCurrentIndex(0);
			mGlobals.mainWindow->mContents->updateAllWidget(0, 0);

		}
		else {
			if (event->button()==Qt::LeftButton) {
				if (selectType == 0) {
					selectType = 1;
					emit WhiteListAdded(label_value);
					emit TypeUpdated();
					update();
				}
				else if (selectType == 1) {
					selectType = 0;
					emit WhiteListRemoved(label_value);
					emit TypeUpdated();
					update();
				}
				else if (selectType == 2) {
					selectType = 0;
					emit BlackListRemoved(label_value);
					emit TypeUpdated();
					update();
				}
			}
			else if (event->button()==Qt::RightButton) {
				if (selectType == 0) {
					selectType = 2;
					emit BlackListAdded(label_value);
					emit TypeUpdated();
					update();
				}
				else if (selectType == 1) {
					selectType = 0;
					emit WhiteListRemoved(label_value);
					emit TypeUpdated();
					update();
				}
				else if (selectType == 2) {
					selectType = 0;
					emit BlackListRemoved(label_value);
					emit TypeUpdated();
					update();
				}
			}
		}
	}
}

void ViewCellGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - m_lastPos.x();
	int dy = event->y() - m_lastPos.y();
	m_lastPos = event->pos();

	if (event->buttons() & Qt::LeftButton) {
		QVector3D ttt = cross_product(up, center1 - eye_position).normalized();
		eye_position = (eye_position + ttt * dx / 70 / scale * EYE).normalized()*EYE;

		ttt = cross_product(up, center1 - eye_position).normalized();
		eye_position = (eye_position + up * dy / 70 / scale * EYE).normalized()*EYE;

		up = cross_product(center1 - eye_position, ttt).normalized();
	}
	else if (event->buttons() & Qt::RightButton) {
		if (scale > 0.02) {
			scale += 1.0*(-dy) / 1000 * scale*EYE;
		}
		else {
			scale = 0.021;
		}
	}
	else if (event->buttons() & Qt::MiddleButton) {
		shift += QVector2D(-float(dx) / 250, -float(dy) / 250);
	}
	update();
}

void ViewCellGLWidget::timerEvent(QTimerEvent *event) {
	if (tog == 0) {
		QVector3D ttt = cross_product(up, center1 - eye_position).normalized();
		eye_position = (eye_position + ttt * 0.005*EYE).normalized()*EYE;
	}

	update();
}
void ViewCellGLWidget::keyReleaseEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Shift) {
		shift_flag = false;
	}
	update();

}

void ViewCellGLWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_R) {
		tog = abs(tog - 1);

	}
	if (event->key() == Qt::Key_0) {
		FILE *light = fopen("light.txt", "r");
		fscanf(light, "%f %f %f %f %f %f", &ambi, &diffu, &specu, &shin, &l_t,&sampling_rate);
		fclose(light);
	}
	if (event->key() == Qt::Key_P) {
		FILE *position = fopen("position.txt", "w");
		fprintf(position, "%f %f %f\n%f %f %f\n%f\n", eye_position.x(), eye_position.y(), eye_position.z(), up.x(), up.y(), up.z(), scale);
		fclose(position);
	}
	if (event->key() == Qt::Key_O) {

		center1 = QVector3D(0, 0, 0);
		eye_position = QVector3D(0, 0, EYE);
		up = QVector3D(0, 1, 0);
		scale = 1;
		shift = QVector2D(0, 0);
	}

	if (event->key() == Qt::Key_Down) {
		z_scale -= 0.01;
		if (z_scale < 0.01)z_scale = 0.01;

	}
	if (event->key() == Qt::Key_Up) {
		z_scale += 0.01;

	}
	if (event->key() == Qt::Key_B) {
		QColor t_c;
		t_c = QColorDialog::getColor(QColor(int(background_color.x() * 255), int(background_color.y() * 255), int(background_color.z() * 255)));
		if (t_c.isValid()) {
			background_color = QVector4D(float(t_c.red()) / 255, float(t_c.green()) / 255, float(t_c.blue()) / 255, 1);
		}
	}
	if (event->key() == Qt::Key_Shift) {
		shift_flag = true;
	}
	update();

}
