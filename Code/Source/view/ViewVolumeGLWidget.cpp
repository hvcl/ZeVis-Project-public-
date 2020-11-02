#include "ViewVolumeGLWidget.h"


void ViewVolumeGLWidget::setBackgroundVolume(unsigned char *data, int width, int height, int depth) {
	qDebug() << "setBackgroundVolume start";
	if (IsReadyBackgorundVolume) {
		glBindTexture(GL_TEXTURE_3D, Background_tex);
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, depth, GL_RED, GL_UNSIGNED_BYTE, data);
		return;
	}

	w = width;
	h = height;
	d = depth;

	sample = sqrt(float(w*w + h * h + d * d));

	glBindTexture(GL_TEXTURE_3D, Background_tex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, d, 0, GL_RED, GL_UNSIGNED_BYTE, data);

	QVector3D t(w, h, d);
	const_box_min = -t.normalized();
	const_box_max = t.normalized();

	IsReadyBackgorundVolume = true;
	qDebug() << "setBackgroundVolume end";
}

void ViewVolumeGLWidget::setVolume(float *data, int width, int height, int depth, float_color color) {
	updateVolume(data, 0, 0, 0, width, height, depth);
	Heatmap_color.setX(color.r);
	Heatmap_color.setY(color.g);
	Heatmap_color.setZ(color.b);
	Heatmap_color.setW(color.a);
}
void ViewVolumeGLWidget::updateVolume(float *data, int startX, int startY, int startZ, int width, int height, int depth) {
	glBindTexture(GL_TEXTURE_3D, Heatmap_tex);
	glTexSubImage3D(GL_TEXTURE_3D, 0, startX, startY, startZ, width, height, depth, GL_RED, GL_FLOAT, data);
}

QVector3D ViewVolumeGLWidget::cross_product(QVector3D v1, QVector3D v2) {
	QVector3D v;
	v.setX(v1.y()*v2.z() - v2.y()*v1.z());
	v.setY(v1.z()*v2.x() - v2.z()*v1.x());
	v.setZ(v1.x()*v2.y() - v2.x()*v1.y());
	return v;
}

ViewVolumeGLWidget::ViewVolumeGLWidget(QWidget *parent)
	: QOpenGLWidget(parent),
	m_program(0)
{

	shift_flag = false;
	IsReadyBackgorundVolume = true;
	IsReadyVolume = true;

	shift = QVector2D(0, 0);
	EYE = 5;
	center1 = QVector3D(0, 0, 0);
	eye_position = QVector3D(0, 0, EYE);
	up = QVector3D(0, 1, 0);

	ambi = 0.5, diffu = 0.7, specu = 0.1, shin = 8;
	l_t = 0.1;
	tog = 0;
	scale = 1;
//	startTimer(30);
	QWidget::setFocusPolicy(Qt::ClickFocus);

	qDebug() << "volume create";
}

ViewVolumeGLWidget::~ViewVolumeGLWidget()
{
	cleanup();
}

QSize ViewVolumeGLWidget::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize ViewVolumeGLWidget::sizeHint() const
{
	return QSize(50, 50);
}



void ViewVolumeGLWidget::cleanup()
{
	makeCurrent();
	delete m_program;
	m_program = 0;
	doneCurrent();
}

void ViewVolumeGLWidget::initializeGL()
{
	qDebug() << "volume initgl";
	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ViewVolumeGLWidget::cleanup);
	initializeOpenGLFunctions();
	glClearColor(1, 1, 1, 1);
	m_program = new QOpenGLShaderProgram;

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	vshader->compileSourceFile("Resources/glsl/vshader_3dview.glsl");

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	fshader->compileSourceFile("Resources/glsl/fshader_3dview.glsl");

	m_program->addShader(vshader);
	m_program->addShader(fshader);
	m_program->link();
	m_program->bind();
	con_tex = m_program->uniformLocation("tex");
	con_heatmap_tex = m_program->uniformLocation("heatmap_tex");

	con_eye_pos = m_program->uniformLocation("eye_position");
	con_box_min = m_program->uniformLocation("box_min");
	con_box_max = m_program->uniformLocation("box_max");
	con_up = m_program->uniformLocation("up");
	con_sample = m_program->uniformLocation("sample");

	con_l_t = m_program->uniformLocation("l_t");
	con_specu = m_program->uniformLocation("Uspecu");
	con_diffu = m_program->uniformLocation("Udiffu");
	con_ambi = m_program->uniformLocation("Uambi");
	con_shin = m_program->uniformLocation("Ushin");

	con_v_size = m_program->uniformLocation("v_size");

	con_scale = m_program->uniformLocation("scale");

	con_window_size = m_program->uniformLocation("window_size");

	con_color_table = m_program->uniformLocation("color_table");
	for (int i = 0; i < 256; i++) {
		color_table[i] = QVector4D(0,0,0,0);
	}

	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_1D, color_tex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, color_table);



	glGenTextures(1, &Background_tex);
	glBindTexture(GL_TEXTURE_3D, Background_tex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	glGenTextures(1, &Heatmap_tex);
	glBindTexture(GL_TEXTURE_3D, Heatmap_tex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	Heatmap_color.setX(1);
	Heatmap_color.setY(0);
	Heatmap_color.setZ(0);
	Heatmap_color.setW(1);



	glActiveTexture(GL_TEXTURE0);
	glUniform1i(con_color_table, 0);
	glBindTexture(GL_TEXTURE_1D, color_tex);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(con_tex, 1);
	glBindTexture(GL_TEXTURE_3D, Background_tex);

	glActiveTexture(GL_TEXTURE2);
	glUniform1i(con_heatmap_tex, 2);
	glBindTexture(GL_TEXTURE_3D, Heatmap_tex);



	QVector3D t(w, h, d);
	const_box_min = -t.normalized();
	const_box_max = t.normalized();


	background_color = QVector4D(0, 0, 0, 1);
	con_background_color = m_program->uniformLocation("background_color");
	con_heatmap_color = m_program->uniformLocation("heatmap_color");

	mGlobals.CurrentProject->planeNormal = QVector3D(0, 0, 1);
	mGlobals.CurrentProject->planePoint = QVector3D(0, 0, 0);
	con_planeNormal = m_program->uniformLocation("plane_normal");
	con_planePoint = m_program->uniformLocation("plane_point");


	//m_program->release();

}
void ViewVolumeGLWidget::emLoaded() {
	m_program->bind();


	for (int i = 0; i < 256; i++) {
		float r = mGlobals.CurrentProject->mLayerBack->transferFunc[i].r;
		float g = mGlobals.CurrentProject->mLayerBack->transferFunc[i].g;
		float b = mGlobals.CurrentProject->mLayerBack->transferFunc[i].b;
		float a = mGlobals.CurrentProject->mLayerBack->transferFunc[i].a;
		color_table[i] = QVector4D(r, g, b, a);
	}

	glBindTexture(GL_TEXTURE_1D, color_tex);
	glTexSubImage1D(GL_TEXTURE_1D, 0,0,256, GL_RGBA, GL_FLOAT, color_table);


	w = mGlobals.CurrentProject->mLayerBack->volumeRenderWidth;
	h = mGlobals.CurrentProject->mLayerBack->volumeRenderHeight;
	d = mGlobals.CurrentProject->mLayerBack->volumeRenderDepth;

	sample = sqrt(float(w*w + h * h + d * d));

	glBindTexture(GL_TEXTURE_3D, Background_tex);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, d, 0, GL_RED, GL_UNSIGNED_BYTE, mGlobals.CurrentProject->mLayerBack->volumeRenderData);


	glBindTexture(GL_TEXTURE_3D, Heatmap_tex);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w*heatmap_resolution_factor, h*heatmap_resolution_factor, d*heatmap_resolution_factor, 0, GL_RED, GL_FLOAT, mGlobals.CurrentProject->mLayerBack->HeatmapData);




	IsEmLoaded = true;
	qDebug() << "VolumeGL em loaded";


}

void ViewVolumeGLWidget::setBackgroundVolumeTF(std::vector<float_color> transferfunction) {
	for (int i = 0; i<256; i++) {
		color_table[i] = QVector4D(transferfunction[i].r, transferfunction[i].g, transferfunction[i].b, transferfunction[i].a);
		qDebug() << color_table[i];
	}
}

void ViewVolumeGLWidget::paintGL()
{
	if (IsEmLoaded == false) {
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;

	}
	//if (!(IsReadyBackgorundVolume && IsReadyVolume))return;

	fps++;

	float max = -999999;

	if (mGlobals.CurrentProject->mLayerBack->HeatmapUpdated && mGlobals.CurrentProject->curLayerCell!=-1 && fix==false) {
		qDebug() << "3d distribution update";
		mGlobals.CurrentProject->mLayerBack->HeatmapUpdated = false;
		if(scale<3)heatmap_resolution_factor = scale*0.1;
		else heatmap_resolution_factor = 0.3;
		int heatmap_size = int(w * heatmap_resolution_factor)*int(h  * heatmap_resolution_factor)*int(d * heatmap_resolution_factor);
		for (int i = 0; i < heatmap_size; ++i) {
			mGlobals.CurrentProject->mLayerBack->HeatmapData[i] = 0;
		}

		for (int i = 0; i < mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.size(); ++i) {
			if (mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.at(i).status) {
				float cx = (float)(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.at(i).maxbox.x + mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.at(i).minbox.x) / 2.0;
				float cy = (float)(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.at(i).maxbox.y + mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.at(i).minbox.y) / 2.0;
				float cz = (float)(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.at(i).maxbox.z + mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.at(i).minbox.z) / 2.0;

				cx = (cx / (float)mGlobals.CurrentProject->DataSizeX) * int(w * heatmap_resolution_factor);
				cy = (cy / (float)mGlobals.CurrentProject->DataSizeY) * int(h * heatmap_resolution_factor);
				cz = (cz / (float)mGlobals.CurrentProject->DataSizeZ) * int(d * heatmap_resolution_factor);

				int loc = (int)cz*int(w * heatmap_resolution_factor)*int(h * heatmap_resolution_factor) + (int)cy*int(w * heatmap_resolution_factor) + (int)cx;
				mGlobals.CurrentProject->mLayerBack->HeatmapData[loc] += 1.0;
				if (max < mGlobals.CurrentProject->mLayerBack->HeatmapData[loc]) {
					max = mGlobals.CurrentProject->mLayerBack->HeatmapData[loc];
				}
			}
		}
		heatmap_max = max;
		if (is_selection) {
			for (int i = 0; i < heatmap_size; ++i) {
				//			mGlobals.CurrentProject->mLayerBack->HeatmapData[i] = pow(mGlobals.CurrentProject->mLayerBack->HeatmapData[i]/max,0.3);
				if (mGlobals.CurrentProject->mLayerBack->HeatmapData[i] >= start_count* heatmap_max && mGlobals.CurrentProject->mLayerBack->HeatmapData[i] <= end_count* heatmap_max) {
					mGlobals.CurrentProject->mLayerBack->HeatmapData[i] = mGlobals.CurrentProject->mLayerBack->HeatmapData[i] / max;
				}
				else mGlobals.CurrentProject->mLayerBack->HeatmapData[i] = 0;
			}
		}
		else {
			for (int i = 0; i < heatmap_size; ++i) {
				//			mGlobals.CurrentProject->mLayerBack->HeatmapData[i] = pow(mGlobals.CurrentProject->mLayerBack->HeatmapData[i]/max,0.3);
				mGlobals.CurrentProject->mLayerBack->HeatmapData[i] = mGlobals.CurrentProject->mLayerBack->HeatmapData[i] / max;
			}
		}

		glDeleteTextures(1, &Heatmap_tex);
		glGenTextures(1, &Heatmap_tex);
		glBindTexture(GL_TEXTURE_3D, Heatmap_tex);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w*heatmap_resolution_factor, h*heatmap_resolution_factor, d*heatmap_resolution_factor, 0, GL_RED, GL_FLOAT, mGlobals.CurrentProject->mLayerBack->HeatmapData);
		
		
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(con_color_table, 0);
		glBindTexture(GL_TEXTURE_1D, color_tex);

		glActiveTexture(GL_TEXTURE1);
		glUniform1i(con_tex, 1);
		glBindTexture(GL_TEXTURE_3D, Background_tex);

		glActiveTexture(GL_TEXTURE2);
		glUniform1i(con_heatmap_tex, 2);
		glBindTexture(GL_TEXTURE_3D, Heatmap_tex);

		//		updateVolume(mGlobals.CurrentProject->mLayerBack->HeatmapData, 0, 0, 0, w * heatmap_resolution_factor, h * heatmap_resolution_factor, d * heatmap_resolution_factor);
		Heatmap_color.setX(mGlobals.CurrentProject->mLayerBack->HeatmapColor.r);
		Heatmap_color.setY(mGlobals.CurrentProject->mLayerBack->HeatmapColor.g);
		Heatmap_color.setZ(mGlobals.CurrentProject->mLayerBack->HeatmapColor.b);
		Heatmap_color.setW(mGlobals.CurrentProject->mLayerBack->HeatmapColor.a);
		qDebug() << "3d distribution update end";

	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	QVector3D box_t(w, h, d);
	const_box_min = -box_t.normalized();
	const_box_max = box_t.normalized();
	QVector3D right = cross_product(up, center1 - eye_position).normalized();

	box_min = (const_box_min + right * shift.x() + up * shift.y())*scale;
	box_max = (const_box_max + right * shift.x() + up * shift.y())*scale;

	m_program->bind();
	m_program->setUniformValue(con_v_size, QVector3D(w, h, d));
	m_program->setUniformValue(con_eye_pos, eye_position);
	m_program->setUniformValue(con_up, up);
	m_program->setUniformValue(con_box_min, box_min);
	m_program->setUniformValue(con_box_max, box_max);
	m_program->setUniformValue(con_sample, sample / scale * 2);
	m_program->setUniformValue(con_scale, scale);

	m_program->setUniformValue(con_diffu, diffu);
	m_program->setUniformValue(con_ambi, ambi);
	m_program->setUniformValue(con_specu, specu);
	m_program->setUniformValue(con_shin, shin);

	m_program->setUniformValue(con_l_t, l_t);
	m_program->setUniformValue(con_background_color, background_color);
	m_program->setUniformValue(con_heatmap_color, Heatmap_color);

	m_program->setUniformValue(con_planeNormal, mGlobals.CurrentProject->planeNormal);
	m_program->setUniformValue(con_planePoint, mGlobals.CurrentProject->planePoint);

	m_program->setUniformValue(con_window_size, QVector2D(this->width() / 800.0, this->height() / 800.0));

	glBegin(GL_QUADS);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, 1);
	glEnd();

	m_program->release();

	QPainter painter(this);
	
	QLinearGradient gradient;
	gradient.setStart(this->width()-10,this->height());
	gradient.setFinalStop(this->width() - 10, 0);
	gradient.setColorAt(0.0, QColor(0, 0, 255,0));
	gradient.setColorAt(1.0/3.0, QColor(0, 255, 255,255*1.0/3.0));
	gradient.setColorAt(2.0/3.0, QColor(0, 255, 0,255*2.0/3.0));
	gradient.setColorAt(1.0, QColor(255, 0, 0,255));
	QPen t_pen = QPen(gradient, 4);
	t_pen.setWidth(20);
	painter.setPen(t_pen);
	painter.drawLine(this->width() - 10, this->height()
		, this->width() - 10, 0);

	if (is_selection || doing_selection) {
		float s=start_count, e=end_count;
		if (s > e) {
			float t = s;
			s = e;
			e = t;
		}

		t_pen = QPen(QColor(0, 0, 0, 200));
		t_pen.setWidth(20);
		t_pen.setCapStyle(Qt::FlatCap);
		painter.setPen(t_pen);
		painter.drawLine(this->width() - 10, this->height()
			, this->width() - 10, this->height() - this->height()*s);
		painter.drawLine(this->width() - 10, 0
			, this->width() - 10, this->height() - this->height()*e);
	}

	
	QFont tFont;
	tFont.setBold(true);
	painter.setFont(tFont);
	t_pen = QPen(QColor(255, 255, 255, 255));
	painter.setPen(t_pen);
	painter.drawText(this->width() - 40, this->height() - 15, "0");
	if (heatmap_max != -1) {
		painter.drawText(this->width() - 40, 15, QString::number(heatmap_max));
		if (is_selection) {
			painter.drawText(this->width() - 40, this->height() - this->height()*start_count, QString::number(start_count*heatmap_max));
			painter.drawText(this->width() - 40, this->height() - this->height()*end_count, QString::number(end_count*heatmap_max));

		}
	}

	painter.end();

	
	//painter.setPen(QColor(255 - background_color.x() * 255, 255 - background_color.y() * 255, 255 - background_color.z() * 255));
	//painter.drawText(30, 30, "FPS: " + p_fps);
	//painter.end();

}

void ViewVolumeGLWidget::resizeGL(int w, int h)
{
	//int t = w > h ? w : h;
	//this->resize(t, t);
}

void ViewVolumeGLWidget::mousePressEvent(QMouseEvent *event)
{
	m_lastPos = event->pos();
	pressedPos = event->pos();
	if (event->button() == Qt::LeftButton) {
		if (event->pos().x() >= this->width()-40.0 && event->pos().x() <= this->width()) {
			is_selection = false;
			doing_selection = true;
			start_count = 1.0-float(event->pos().y()) / this->height();
			end_count = 1.0-float(event->pos().y()) / this->height();
		}
		update();
	}
}
void ViewVolumeGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::RightButton) {
		mGlobals.CurrentProject->mLayerBack->HeatmapUpdated = true;
		update();
	}
	if (event->button() == Qt::LeftButton) {
		if (doing_selection) {
			doing_selection = false;
			if (start_count > end_count) {
				float t = start_count;
				start_count = end_count;
				end_count = t;
			}
			if (end_count-start_count<0.01) {
				is_selection = false;
				mGlobals.CurrentProject->mLayerBack->HeatmapUpdated = true;
			}
			else {
				is_selection = true;
				mGlobals.CurrentProject->mLayerBack->HeatmapUpdated = true;
			}
		}
		update();
	}

}

void ViewVolumeGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - m_lastPos.x();
	int dy = event->y() - m_lastPos.y();
	m_lastPos = event->pos();

	if (event->buttons() & Qt::LeftButton) {
		if (doing_selection) {
			end_count= 1.0-float(event->pos().y()) / this->height();

		}
		else {

			if (shift_flag) {
				shift += QVector2D(-float(dx) / 250 / scale, -float(dy) / 250 / scale);
			}
			else {
				QVector3D ttt = cross_product(up, center1 - eye_position).normalized();
				eye_position = (eye_position + ttt * dx / 70 / scale * EYE).normalized()*EYE;

				ttt = cross_product(up, center1 - eye_position).normalized();
				eye_position = (eye_position + up * dy / 70 / scale * EYE).normalized()*EYE;

				up = cross_product(center1 - eye_position, ttt).normalized();
			}
		}
	}
	else if(event->buttons() & Qt::MidButton ){
		shift += QVector2D(-float(dx) / 250 / scale, -float(dy) / 250 / scale);
	}
	else if (event->buttons() & Qt::RightButton) {
		if (scale>0.02) {
			scale += 1.0*(-dy) / 1000 * scale*EYE;
		}
		else {
			scale = 0.021;
		}
	}
	//else if (event->buttons() & Qt::MiddleButton) {
	//	shift += QVector2D(-float(dx) / 250 / scale, -float(dy) / 250 / scale);
	//}
	update();
}

void ViewVolumeGLWidget::timerEvent(QTimerEvent *event) {
	if (tog == 0) {
		QVector3D ttt = cross_product(up, center1 - eye_position).normalized();
		eye_position = (eye_position + ttt * 0.005*EYE * 3).normalized()*EYE;
	}


	if (fps_start == -1) {
		fps_start = GetTickCount();
	}
	else {
		if (GetTickCount() - fps_start>1000) {
			char itoa_t[10];
			p_fps = QString(itoa(fps, itoa_t, 10));
			fps = 0;
			fps_start = GetTickCount();
		}
	}

	update();
}
void ViewVolumeGLWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_R) {
		tog = abs(tog - 1);

	}
	if (event->key() == Qt::Key_Shift) {
		shift_flag = true;

	}
	if (event->key() == Qt::Key_F) {
		fix = !fix;

	}
}
void ViewVolumeGLWidget::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Shift) {
		shift_flag = false;

	}
}
void ViewVolumeGLWidget::updatePlaneInfo(QVector3D normal, QVector3D point) {
//	planeNormal = normal;
//	planePoint = point;
}
