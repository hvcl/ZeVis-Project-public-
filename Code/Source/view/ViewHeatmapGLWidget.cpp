#include "ViewHeatmapGLWidget.h"

ViewHeatmapGLWidget::ViewHeatmapGLWidget(QWidget *parent)
	: QOpenGLWidget(parent),
	clearColor(Qt::black),
	xRot(0),
	yRot(0),
	zRot(0),
	program(0)
{
	setFocusPolicy(Qt::StrongFocus);
	qDebug() << "create gl heatmap";
}

ViewHeatmapGLWidget::~ViewHeatmapGLWidget()
{
	qDebug() << "destroy gl heatmap";
	makeCurrent();

	delete program;
	doneCurrent();
}

QSize ViewHeatmapGLWidget::minimumSizeHint() const
{
	qDebug() << "minimumSizeHint heatmap";
	return QSize(50, 50);
}

QSize ViewHeatmapGLWidget::sizeHint() const
{
	qDebug() << "sizeHint heatmap";
	return QSize(200, 200);
}

void ViewHeatmapGLWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{
	xRot += xAngle;
	yRot += yAngle;
	zRot += zAngle;
	update();
}

void ViewHeatmapGLWidget::initializeGL()
{
	qDebug() << "initalizaeGL heatmap";

	initializeOpenGLFunctions();
	glEnable(GL_TEXTURE_2D);

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	vshader->compileSourceFile("Resources/glsl/vshader_heatmap.glsl");

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	fshader->compileSourceFile("Resources/glsl/fshader_heatmap.glsl");


	program = new QOpenGLShaderProgram;
	program->addShader(vshader);
	program->addShader(fshader);
	program->link();
	program->bind();

	con_thumbnail = program->uniformLocation("thumbnail");
	con_heatmap = program->uniformLocation("heatmap");

	
	thumbnail = changeBackground(ThumbnailPath.c_str(), BackgroundColor);
	qDebug() << "thumbnail format : " << thumbnail.format();
	glGenTextures(1, &thumbnail_tex);
	glBindTexture(GL_TEXTURE_2D, thumbnail_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, thumbnail.width(), thumbnail.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, thumbnail.constBits());
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenTextures(1, &heatmap_tex);
	glBindTexture(GL_TEXTURE_2D, heatmap_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, WidgetWidth, WidgetHeight, 0, GL_RED, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	
	
	//updateHeatmap();
}
void ViewHeatmapGLWidget::setRenderingSize(int width, int height) {
	WidgetWidth = width;
	WidgetHeight = height;
	heatmap_table = new float[WidgetWidth * WidgetHeight]();
}
void ViewHeatmapGLWidget::setTexturePath(std::string path) {
	ThumbnailPath = path;
}
void ViewHeatmapGLWidget::setBackgroundColor(QColor color) {
	BackgroundColor = color;
}

void ViewHeatmapGLWidget::setHeatmapData(std::vector<cell> *cell_list_ptr, int *cell_index_ptr, std::list<unsigned int> *indexlist_ptr, coord data_size) {
	qDebug() << QString::fromStdString(ThumbnailPath) << " " << WidgetWidth << " " << WidgetHeight;

	cell_index = cell_index_ptr;
	cell_table = cell_list_ptr;
	index_list = indexlist_ptr;
	cell_data_size.x = data_size.x;
	cell_data_size.y = data_size.y;
	cell_data_size.z = data_size.z;

	qDebug() << " Heatmap Data on ";
}

void ViewHeatmapGLWidget::updateHeatmap() {
	qDebug() << " Update Heatmap : " << index_list->size();
	memset(heatmap_table, 0, sizeof(float) * WidgetWidth * WidgetHeight);
//	for (int i = 0; i < WidgetWidth*WidgetHeight; i++)heatmap_table[i] = 0;
	//qDebug() << "jjjj1";
	std::list<unsigned int>::iterator iter;
	//qDebug() << "jjjj2";

	heatmap_maxvalue = -9999;
	for (iter = index_list->begin(); iter != index_list->end(); ++iter) {
		cell curCell = cell_table->at(cell_index[*iter]);

		coord minbox = curCell.minbox;
		coord maxbox = curCell.maxbox;
			
		float posX = WidgetHeight - ((float)(minbox.x + maxbox.x) / 2.0 / (float)cell_data_size.x) * WidgetHeight;
		float posZ = ((float)(minbox.z + maxbox.z) / 2.0 / (float)cell_data_size.z) * WidgetWidth;
		heatmap_table[WidgetWidth * (int)posX + (int)posZ] ++;
		if (heatmap_maxvalue < heatmap_table[WidgetWidth * (int)posX + (int)posZ]) {
			heatmap_maxvalue = heatmap_table[WidgetWidth * (int)posX + (int)posZ];
		}
	}
	//qDebug() << "jjjj3";

	this->update();
	//qDebug() << "jjjj4";

	this->repaint();
	//qDebug() << "jjjj5";

}

void ViewHeatmapGLWidget::updateDataColor(QColor color) {
	DataColor = color;
}

void ViewHeatmapGLWidget::paintGL()
{
	qDebug() << "Paint GL heatmap";

	QMatrix4x4 modelViewMatrix;
	//modelViewMatrix.rotate(180, 0.0, 1.0, 0.0);
	
	paintState = true;
	glClearColor(0.2, 0.2, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	program->bind();
	program->setUniformValue("matrix", projMatrix*modelViewMatrix);
	program->setUniformValue("heatmap_max", heatmap_maxvalue);
	program->setUniformValue("heatmap_color", DataColor);

	//qDebug() << "tt1";

	glBindTexture(GL_TEXTURE_2D, heatmap_tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WidgetWidth, WidgetHeight, GL_RED, GL_FLOAT, heatmap_table);
	glBindTexture(GL_TEXTURE_2D, 0);
	//qDebug() << "tt2";

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(con_thumbnail, 0);
	glBindTexture(GL_TEXTURE_2D, thumbnail_tex);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(con_heatmap, 1);
	glBindTexture(GL_TEXTURE_2D, heatmap_tex);

	float posXmin = -0.5f;
	float posXmax = 0.5f;
	float posYmin = -0.5f;
	float posYmax = 0.5f;




	int texcoord_index = program->attributeLocation("TexCoord");

	glBegin(GL_QUADS);
	glVertexAttrib2f(texcoord_index, 0.0f, 0.0f);
	glVertex3f(posXmin, posYmin, 0);
	glVertexAttrib2f(texcoord_index, 1.0f, 0.0f);
	glVertex3f(posXmax, posYmin, 0);
	glVertexAttrib2f(texcoord_index, 1.0f, 1.0f);
	glVertex3f(posXmax, posYmax, 0);
	glVertexAttrib2f(texcoord_index, 0.0f, 1.0f);
	glVertex3f(posXmin, posYmax, 0);
	glEnd();
	

	program->release();
	paintState = false;
}


void ViewHeatmapGLWidget::resizeGL(int width, int height)
{
	qDebug() << "resize GL heatmap";
	WidgetWidth = width;
	WidgetHeight = height;
	projMatrix.setToIdentity();
	projMatrix.viewport(0, 0, width, height);
	//float ratio = (float)height / (float)width;
	projMatrix.ortho(0, (float)width / 2, 0, height / 2, 1, -1);
}

QImage ViewHeatmapGLWidget::changeBackground(const QString resource, QColor backgroundColor)
{
	QImage resSource(resource);
	QImage destBackground(resSource.size(), QImage::Format_RGB32);
	destBackground.fill(QColor(backgroundColor.blue(), backgroundColor.green(), backgroundColor.red()));
	QPainter p(&destBackground);
	p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
	p.drawImage(0, 0, resSource);
	return destBackground;
}