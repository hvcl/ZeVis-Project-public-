#include "ViewAxisGLWidget.h"

ViewAxisGLWidget::ViewAxisGLWidget(QWidget *parent)
	: QOpenGLWidget(parent),
	clearColor(Qt::black),
	xRot(0),
	yRot(0),
	zRot(0),
	program(0)
{
	mWidget = mGlobals.mainWindow->mContents;
	setFocusPolicy(Qt::ClickFocus);
	setMouseTracking(true);
	qDebug() << "create gl";
}
void ViewAxisGLWidget::setAxisIndex(int axis, int index) {
	AxisCode = axis;
	GLView_Index = index;
	qDebug() << "set axis";
	qDebug() << axis;
}

ViewAxisGLWidget::~ViewAxisGLWidget()
{
	qDebug() << "destroy gl";
	makeCurrent();

	delete program;
	doneCurrent();
}

QSize ViewAxisGLWidget::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize ViewAxisGLWidget::sizeHint() const
{
	return QSize(200, 200);
}

void ViewAxisGLWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{
	xRot += xAngle;
	yRot += yAngle;
	zRot += zAngle;
	update();
}

void ViewAxisGLWidget::updateByInterface(bool scaleflag) {
	qDebug() << "updateByInterface";
	update();
}
void ViewAxisGLWidget::updateByAnotherView(bool scaleflag) {
	qDebug() << "updateByAnotherView";
	update();
}

void ViewAxisGLWidget::initializeGL()
{
	qDebug() << "init GL";
	initializeOpenGLFunctions();
	//glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	//lBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);

	vshader->compileSourceFile("Resources/glsl/vshader_axisview.glsl");

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	
	fshader->compileSourceFile("Resources/glsl/fshader_axisview.glsl");

	program = new QOpenGLShaderProgram;
	
	program->addShader(vshader);
	program->addShader(fshader);
	program->link();
	program->bind();

}

void ViewAxisGLWidget::emLoaded() {
	program->bind();

	int blocksize = mGlobals.CurrentProject->DataBlockSize;

	con_EMTex = program->uniformLocation("em_texture");
	con_LBTex = program->uniformLocation("cell_texture");
	for (int i = 0; i < 10; i++) {
		char t[30];
		sprintf(t, "subregion_texture[%d]", i);
		con_SBTex[i] = program->uniformLocation(t);
	}


	con_CellColorTex = program->uniformLocation("cell_color_texture");

	glGenTextures(1, &EMTex);
	glBindTexture(GL_TEXTURE_2D, EMTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, blocksize, blocksize, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	for (int i = 0; i < 10; ++i) {
		glGenTextures(1, &SBTex[i]);
		glBindTexture(GL_TEXTURE_2D, SBTex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, blocksize, blocksize, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	}

	glGenTextures(1, &LBTex);
	glBindTexture(GL_TEXTURE_2D, LBTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, blocksize, blocksize, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);

	//CellTableWidth = sqrt(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->MaxCellCount)+1;
	//long long lvalue = 1;
	//long long tablesize = lvalue * CellTableWidth * CellTableWidth;
	//cell_color_data = new unsigned int[tablesize];

	cell_color_data = NULL;

	//qDebug() << " Table Width : " << CellTableWidth;
	//qDebug() << " Cell Count : " << mGlobals.CurrentProject->mLayerCell->MaxCellCount;

	glGenTextures(1, &CellColorTex);
	glBindTexture(GL_TEXTURE_2D, CellColorTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, blocksize, blocksize, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	//emptyTexLabel = new unsigned int[mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize];
	//memset(emptyTexLabel, 0, mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize*sizeof(unsigned int));

	IsEmLoaded = true;

	qDebug() << "ViewGL em loaded";
}

block_info ViewAxisGLWidget::calcBlockIndex() {
	block_info info;
	
	float leftTopPosX;
	float leftTopPosY;
	float rightBottomPosX;
	float rightBottomPosY;
	int level = mGlobals.CurrentProject->ViewZoomLevel;
	if (level < 0)level = 0;
	if (level > mGlobals.CurrentProject->DataLevel)level = mGlobals.CurrentProject->DataLevel;
	int tilevalue = mGlobals.CurrentProject->DataBlockSize * pow(2,level);

	int tIdx_StartX;
	int tIdx_StartY;
	int tIdx_EndX;
	int tIdx_EndY;
	int tIdx_Z;
	
	int x_count;
	int y_count;
	int z_count;
	Utils mUtil;
	mUtil.CalcXYZCount(&x_count, &y_count, &z_count, level, mGlobals.CurrentProject->DataSizeX, mGlobals.CurrentProject->DataSizeY, mGlobals.CurrentProject->DataSizeZ, mGlobals.CurrentProject->DataBlockSize);

	float mul =1.0/ (pow(2, viewScale - mGlobals.CurrentProject->ViewZoomLevel)*2);
	if (AxisCode == 1) {
		leftTopPosX = mGlobals.CurrentProject->ViewPos_X - WidgetWidth*mul-1;
		leftTopPosY = mGlobals.CurrentProject->ViewPos_Y - WidgetHeight* mul-1;
		rightBottomPosX = mGlobals.CurrentProject->ViewPos_X + WidgetWidth* mul+1;
		rightBottomPosY = mGlobals.CurrentProject->ViewPos_Y + WidgetHeight* mul+1;
		
		tIdx_Z = (int)(mGlobals.CurrentProject->ViewPos_Z);
	}
	else if (AxisCode == 2) {
		leftTopPosX = mGlobals.CurrentProject->ViewPos_Z - WidgetWidth * mul -1;
		leftTopPosY = mGlobals.CurrentProject->ViewPos_Y - WidgetHeight * mul-1;
		rightBottomPosX = mGlobals.CurrentProject->ViewPos_Z + WidgetWidth * mul+1;
		rightBottomPosY = mGlobals.CurrentProject->ViewPos_Y + WidgetHeight * mul+1;
		
		tIdx_Z = (int)(mGlobals.CurrentProject->ViewPos_X);
	}
	else if (AxisCode == 3) {
		leftTopPosX = mGlobals.CurrentProject->ViewPos_X - WidgetWidth * mul-1;
		leftTopPosY = mGlobals.CurrentProject->ViewPos_Z - WidgetHeight * mul-1;
		rightBottomPosX = mGlobals.CurrentProject->ViewPos_X + WidgetWidth * mul+1;
		rightBottomPosY = mGlobals.CurrentProject->ViewPos_Z + WidgetHeight * mul+1;
		
		tIdx_Z = (int)(mGlobals.CurrentProject->ViewPos_Y);
	}
	tIdx_StartX = (int)(leftTopPosX / tilevalue);
	tIdx_StartY = (int)(leftTopPosY / tilevalue);
	tIdx_EndX = (int)(rightBottomPosX / tilevalue) + 1;
	tIdx_EndY = (int)(rightBottomPosY / tilevalue) + 1;

	info.start_x = tIdx_StartX;
	info.end_x = tIdx_EndX;
	info.start_y = tIdx_StartY;
	info.end_y = tIdx_EndY;
	info.z = tIdx_Z;
	info.size = tilevalue;
	info.level =level;
	info.axis = AxisCode;
	return info;
}

void ViewAxisGLWidget::uploadCellColor(){
	//CellTableWidth = sqrt(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->MaxCellCount) + 1;
	//long long lvalue = 1;
	//long long tablesize = lvalue * CellTableWidth * CellTableWidth;
	//if (cell_color_data != NULL) {
	//	delete[]cell_color_data;
	//}
	//cell_color_data = new unsigned int[tablesize];

	//if (mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->CellColorGPU_On == false) {
	//	for (int i = 0; i < mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.size(); ++i) {
	//		unsigned int index = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.at(i).index;
	//		if (mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList.at(i).status) {
	//			cell_color_data[index] = 1;
	//		}
	//		else {
	//			cell_color_data[index] = 0;
	//		}
	//		//cell_color_data[index] = mGlobals.CurrentProject->mLayerCell->mCellList.at(i).color.red();
	//	}
	//	mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->CellColorGPU_On = true;
	//
	//	glBindTexture(GL_TEXTURE_2D, CellColorTex);
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CellTableWidth, CellTableWidth, GL_RED_INTEGER, GL_UNSIGNED_INT, cell_color_data);
	//	glBindTexture(GL_TEXTURE_2D, 0);

	//}
	
}

bool ViewAxisGLWidget::bindEMLayer(block_info info) {
	int stat = mGlobals.CurrentProject->mLayerBack->checkBlockIndex(info.x, info.y, info.z, info.level, info.axis);
	if (stat == -1) {
		back_layer mblock = mGlobals.CurrentProject->mLayerBack->initializeBlock(info);
		stat = mGlobals.CurrentProject->mLayerBack->LoadBlockBySerialIndex(mblock);
	}

	if (stat != -1) {
		std::list<back_layer>::iterator iter_em = mGlobals.CurrentProject->mLayerBack->BlockList.begin();
		std::advance(iter_em, stat);
		glBindTexture(GL_TEXTURE_2D, EMTex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mGlobals.CurrentProject->DataBlockSize, mGlobals.CurrentProject->DataBlockSize, GL_RED, GL_UNSIGNED_BYTE, iter_em->data);
		glBindTexture(GL_TEXTURE_2D, 0);
		return 1;
	}
	else {
		return 0;
	}
}
bool ViewAxisGLWidget::bindCellLayer(block_info info) {
	int stat = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->checkBlockIndex(info.x, info.y, info.z, info.level, info.axis);
	if (stat == -1) {
		label_layer mblock = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->initializeBlock(info);
		stat = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->LoadBlockBySerialIndex(mblock);
	}

	if (stat != -1) {

		std::list<label_layer>::iterator iter_lb = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->BlockList.begin();
		std::advance(iter_lb, stat);
		int size = mGlobals.CurrentProject->DataBlockSize * mGlobals.CurrentProject->DataBlockSize;
		for (int i = 0; i < size; ++i) {
			//qDebug() << i;
			unsigned int lb = iter_lb->data[i];
			//if(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex.find(lb)!= mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex.end())
			if(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[lb]!=-1)
			{
				if (mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[lb]].status) {
					iter_lb->color_data[i] = 255;
				}
				else {
					iter_lb->color_data[i] = 50;
				}
			}
			else {
				iter_lb->color_data[i] = 0;
			}
		}

		glBindTexture(GL_TEXTURE_2D, LBTex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mGlobals.CurrentProject->DataBlockSize, mGlobals.CurrentProject->DataBlockSize, GL_RED_INTEGER, GL_UNSIGNED_INT, iter_lb->data);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, CellColorTex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mGlobals.CurrentProject->DataBlockSize, mGlobals.CurrentProject->DataBlockSize, GL_RED, GL_UNSIGNED_BYTE, iter_lb->color_data);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}
	else {
		return false;
	}

}

int ViewAxisGLWidget::bindSubregionLayer(block_info info) {
	//qDebug() << "bindSubregionLayer";
	for (int i = 0; i < 10; ++i) {
		subregion_opacity[i] = 0;
		subregion_color[i][0] = 0;
		subregion_color[i][1] = 0;
		subregion_color[i][2] = 0;
		subregion_color[i][3] = 0;
	}
	int subregion_index = 0;
	std::vector<LayerSubregion>::iterator iter;
	for (iter = mGlobals.CurrentProject->mSubregion.begin(); iter != mGlobals.CurrentProject->mSubregion.end(); ++iter) {
		if (iter->SubregionOpacity > 0 ) {
			int stat = iter->checkBlockIndex(info.x, info.y, info.z, info.level, info.axis);

			if (stat == -1) {
				subregion_layer mblock = iter->initializeBlock(info);
				stat = iter->LoadBlockBySerialIndex(mblock);
			}

			if (stat != -1) {
				std::list<subregion_layer>::iterator iter_sb = iter->BlockList.begin();
				std::advance(iter_sb, stat);
				glBindTexture(GL_TEXTURE_2D, SBTex[subregion_index]);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mGlobals.CurrentProject->DataBlockSize, mGlobals.CurrentProject->DataBlockSize, GL_RED, GL_UNSIGNED_BYTE, iter_sb->data);
				glBindTexture(GL_TEXTURE_2D, 0);
				subregion_color[subregion_index][0] = iter->SubregionColor.redF();
				subregion_color[subregion_index][1] = iter->SubregionColor.greenF();
				subregion_color[subregion_index][2] = iter->SubregionColor.blueF();
				subregion_color[subregion_index][3] = iter->SubregionColor.alphaF();

				subregion_opacity[subregion_index] = iter->SubregionOpacity * 100;
				subregion_index++;
			}

			//subregion_color[subregion_index] = iter->SubregionColor;


		}
	}
	return subregion_index;
}

void ViewAxisGLWidget::paintGL()
{
	//qDebug() << "Paint GL : " << GLView_Index << " " << WidgetWidth << "/" << WidgetHeight;
	
	if (IsEmLoaded == false) {
		glClearColor(0.5, 0.5, 0.5, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}
	QMatrix4x4 modelViewMatrix;
	//modelViewMatrix.rotate(180, 0.0, 1.0, 0.0);
	float mul = pow(2, viewScale - mGlobals.CurrentProject->ViewZoomLevel);
	modelViewMatrix.scale(mul, -mul, 1);
	float xt;
	float yt;
	float zt;

	if (AxisCode == 1) {
		xt = (mGlobals.CurrentProject->ViewPos_X) / WidgetHeight * (-1);
		yt = (mGlobals.CurrentProject->ViewPos_Y) / WidgetHeight * (-1);
		zt = 0;
	}
	else if (AxisCode == 2) {
		xt = mGlobals.CurrentProject->ViewPos_Z / WidgetHeight * (-1);
		//yt = (mGlobals.CurrentProject->DataSizeY - mGlobals.CurrentProject->ViewPos_Y) / WidgetHeight * (-1);
		yt = (mGlobals.CurrentProject->ViewPos_Y) / WidgetHeight * (-1);
		zt = 0;

	}
	else if (AxisCode == 3) {
		xt = (mGlobals.CurrentProject->ViewPos_X) / WidgetHeight * (-1);
		yt = (mGlobals.CurrentProject->ViewPos_Z) / WidgetHeight * (-1);
		zt = 0;
		//zt = (mGlobals.CurrentProject->ViewPos_Y) / WidgetHeight * (-1);

	}

	modelViewMatrix.translate(xt, yt, zt);
	paintState = true;
	//glEnable(GL_CULL_FACE);
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	block_info BlockInfo = calcBlockIndex();

	////qDebug() << "paint";
	program->bind();
	program->setUniformValue("matrix", projMatrix*modelViewMatrix);
	program->setUniformValue("visualize_method", mGlobals.CurrentProject->VisualizeMethod_Index);
	program->setUniformValue("em_opacity", 1.0f);
//	program->setUniformValue("cell_opacity", 0);
	if(mGlobals.CurrentProject->curLayerCell!=-1)
		program->setUniformValue("cell_opacity", mGlobals.CurrentProject->CellOpacity);
	program->setUniformValue("selected_color", mGlobals.CurrentProject->SelectedColor);
	program->setUniformValue("unselected_color", mGlobals.CurrentProject->UnSelectedColor);

	for (int x = BlockInfo.start_x; x < BlockInfo.end_x; ++x) {
		for (int y = BlockInfo.start_y; y < BlockInfo.end_y; ++y) {

			BlockInfo.x = x;
			BlockInfo.y = y;
			bool EMStat = bindEMLayer(BlockInfo);
			bool CellStat = false;
			if (mGlobals.CurrentProject->curLayerCell != -1)
				CellStat = bindCellLayer(BlockInfo);

			int subregion_count = bindSubregionLayer(BlockInfo);
			//uploadCellColor();

			if (EMStat) {
				glActiveTexture(GL_TEXTURE0);
				glUniform1i(con_EMTex, 0);
				glBindTexture(GL_TEXTURE_2D, EMTex);
			}
			else {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			if (CellStat) {

				glActiveTexture(GL_TEXTURE1);
				glUniform1i(con_LBTex, 1);
				glBindTexture(GL_TEXTURE_2D, LBTex);

				glActiveTexture(GL_TEXTURE2);
				glUniform1i(con_CellColorTex, 2);
				glBindTexture(GL_TEXTURE_2D, CellColorTex);

			}
			else {

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
						


			
			for (int subregion_index = 0; subregion_index < subregion_count; subregion_index++) {
				glActiveTexture(GL_TEXTURE3 + subregion_index);
				glUniform1i(con_SBTex[subregion_index], subregion_index + 3);
				glBindTexture(GL_TEXTURE_2D, SBTex[subregion_index]);
			}
			for (; subregion_count < 10; subregion_count++) {
				glActiveTexture(GL_TEXTURE3 + subregion_count);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			program->setUniformValueArray("subregion_opacity", subregion_opacity, 10);
			program->setUniformValueArray("subregion_color", (QVector4D*)subregion_color, 10);

			drawTile(BlockInfo);	
		}
	}		
	program->release();

	
	
	mGlobals.CurrentProject->mLayerBack->removeBlock();
	if (mGlobals.CurrentProject->curLayerCell != -1)
		mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->removeBlock();
	std::vector<LayerSubregion>::iterator iter;
	for (iter = mGlobals.CurrentProject->mSubregion.begin(); iter != mGlobals.CurrentProject->mSubregion.end(); ++iter) {
		iter->removeBlock();
	}

	drawCenterLine();

	drawAnnotations(BlockInfo);
	paintState = false;

}
void ViewAxisGLWidget::drawAnnotations(block_info info) {
	if (mWidget->annotationWidget->onoff->isChecked() == false)return;
	QPainter painter(this);
	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
	QPen t_pen;
	QBrush t_brush(Qt::SolidPattern);
	t_pen.setColor(QColor(0, 255, 255, 100));
	t_pen.setWidth(3);
	t_brush.setColor(QColor(0, 0, 255, 50));
	painter.setBrush(t_brush);
	painter.setPen(t_pen);
	painter.drawRect(min(AnnotationStartPos.x(), AnnotationEndPos.x()),
					min(AnnotationStartPos.y(), AnnotationEndPos.y()),
					abs(AnnotationStartPos.x() - AnnotationEndPos.x()),
					abs(AnnotationStartPos.y() - AnnotationEndPos.y()));

	for (int i = 0; i < mWidget->annotationWidget->annotations.size(); i++) {
	
		if (mWidget->annotationWidget->annotations[i]->axis != AxisCode
			&& mWidget->annotationWidget->annotations[i]->axis!=0)continue;
		t_brush.setColor(mWidget->annotationWidget->annotations[i]->color);
		painter.setBrush(t_brush);

		float mul = pow(2, viewScale - mGlobals.CurrentProject->ViewZoomLevel);

		QPoint s, e;
		if (AxisCode == 1) {
			if (mWidget->annotationWidget->annotations[i]->startPos.z() > mGlobals.CurrentProject->ViewPos_Z
				|| mWidget->annotationWidget->annotations[i]->endPos.z() < mGlobals.CurrentProject->ViewPos_Z)continue;
			s = QPoint((mWidget->annotationWidget->annotations[i]->startPos.x() - mGlobals.CurrentProject->ViewPos_X)*mul + WidgetWidth / 2,
				(mWidget->annotationWidget->annotations[i]->startPos.y() - mGlobals.CurrentProject->ViewPos_Y)*mul + WidgetHeight / 2);
			e = QPoint((mWidget->annotationWidget->annotations[i]->endPos.x() - mGlobals.CurrentProject->ViewPos_X)*mul + WidgetWidth / 2,
				(mWidget->annotationWidget->annotations[i]->endPos.y() - mGlobals.CurrentProject->ViewPos_Y)*mul + WidgetHeight / 2);
		}
		else if (AxisCode == 2) {
			if (mWidget->annotationWidget->annotations[i]->startPos.x() > mGlobals.CurrentProject->ViewPos_X
				|| mWidget->annotationWidget->annotations[i]->endPos.x() < mGlobals.CurrentProject->ViewPos_X)continue;

			s = QPoint((mWidget->annotationWidget->annotations[i]->startPos.z() - mGlobals.CurrentProject->ViewPos_Z)*mul + WidgetWidth / 2,
				(mWidget->annotationWidget->annotations[i]->startPos.y() - mGlobals.CurrentProject->ViewPos_Y)*mul + WidgetHeight / 2);
			e = QPoint((mWidget->annotationWidget->annotations[i]->endPos.z() - mGlobals.CurrentProject->ViewPos_Z)*mul + WidgetWidth / 2,
				(mWidget->annotationWidget->annotations[i]->endPos.y() - mGlobals.CurrentProject->ViewPos_Y)*mul + WidgetHeight / 2);

		}
		else if (AxisCode == 3) {
			if (mWidget->annotationWidget->annotations[i]->startPos.y() > mGlobals.CurrentProject->ViewPos_Y
				|| mWidget->annotationWidget->annotations[i]->endPos.y() < mGlobals.CurrentProject->ViewPos_Y)continue;

			s = QPoint((mWidget->annotationWidget->annotations[i]->startPos.x() - mGlobals.CurrentProject->ViewPos_X)*mul + WidgetWidth / 2,
				(mWidget->annotationWidget->annotations[i]->startPos.z() - mGlobals.CurrentProject->ViewPos_Z)*mul + WidgetHeight / 2);
			e = QPoint((mWidget->annotationWidget->annotations[i]->endPos.x() - mGlobals.CurrentProject->ViewPos_X)*mul + WidgetWidth / 2,
				(mWidget->annotationWidget->annotations[i]->endPos.z() - mGlobals.CurrentProject->ViewPos_Z)*mul + WidgetHeight / 2);

		}

		if (s.x() < WidgetWidth && s.y() < WidgetHeight && e.x() > 0 && e.y() > 0) {
			t_brush.setColor(mWidget->annotationWidget->annotations[i]->color);
			painter.setBrush(t_brush);
			painter.drawRect(s.x(), s.y(), e.x() - s.x(), e.y() - s.y());
		}
	}
	painter.end();


}
void ViewAxisGLWidget::drawTile(block_info info) {
	float mul = pow(2, viewScale - mGlobals.CurrentProject->ViewZoomLevel);

	float posXmin = (float)(info.size * info.x) / (float)WidgetHeight;
	float posXmax = (float)(info.size * info.x + info.size) / (float)WidgetHeight;
	float posYmin = (float)(info.size * info.y) / (float)WidgetHeight;
	float posYmax = (float)(info.size * info.y + info.size) / (float)WidgetHeight;

	int texcoord_index = program->attributeLocation("TexCoord");

	glBegin(GL_QUADS);
	glVertexAttrib2f(texcoord_index, 0.0f, 0.0f);
	glVertex2f(posXmin, posYmin);
	glVertexAttrib2f(texcoord_index, 1.0f, 0.0f);
	glVertex2f(posXmax, posYmin);
	glVertexAttrib2f(texcoord_index, 1.0f, 1.0f);
	glVertex2f(posXmax, posYmax);
	glVertexAttrib2f(texcoord_index, 0.0f, 1.0f);
	glVertex2f(posXmin, posYmax);
	glEnd();

}

void ViewAxisGLWidget::drawCenterLine() {
	glLineWidth(0.5);
	glColor4f(0.1f, 0.1f, 0.1f, 1.0f);

	glBegin(GL_LINES);
	glVertex2f(-1.0f, 0.0f);	
	glVertex2f(1.0f, 0.0f);

	glVertex2f(0.0f, -1.0f);
	glVertex2f(0.0f, 1.0f);
	glEnd();
}


void ViewAxisGLWidget::resizeGL(int width, int height)
{
	qDebug() << "resize GL";
	WidgetWidth = width;
	WidgetHeight = height;

	projMatrix.setToIdentity();
	projMatrix.viewport(0, 0, width, height);
	float ratio = (float)height / (float)width;
	

	projMatrix.ortho(0, (float)width/2/ratio, 0, height/2, 1, -1);

	emit update_view_state(mGlobals.CurrentProject->ViewPos_X, mGlobals.CurrentProject->ViewPos_Y, mGlobals.CurrentProject->ViewPos_Z, mGlobals.CurrentProject->ViewZoomLevel);
}

unsigned int ViewAxisGLWidget::getCellIndexbyMousePos(QPointF pos) {
	if (mGlobals.CurrentProject->curLayerCell == -1)return 0;
	block_info BlockInfo = calcBlockIndex();

	for (int x = BlockInfo.start_x; x < BlockInfo.end_x; ++x) {
		for (int y = BlockInfo.start_y; y < BlockInfo.end_y; ++y) {
			BlockInfo.x = x;
			BlockInfo.y = y;
			
			int stat = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->checkBlockIndex(BlockInfo.x, BlockInfo.y, BlockInfo.z, BlockInfo.level, BlockInfo.axis);

			if (stat != -1) {
				std::list<label_layer>::iterator iter_lb = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->BlockList.begin();
				std::advance(iter_lb, stat);
				//BlockInfo.size = mGlobals.CurrentProject->DataBlockSize;

				float posXmin = (float)(BlockInfo.size * BlockInfo.x);
				float posXmax = (float)(BlockInfo.size * BlockInfo.x + BlockInfo.size);
				float posYmin = (float)(BlockInfo.size * BlockInfo.y);
				float posYmax = (float)(BlockInfo.size * BlockInfo.y + BlockInfo.size);

				float targetX;
				float targetY;
				float mul =1.0/ pow(2, viewScale - mGlobals.CurrentProject->ViewZoomLevel);
				if (AxisCode == 1) {
					targetX = mGlobals.CurrentProject->ViewPos_X + (pos.x() - (WidgetWidth / 2))* mul;
					targetY = mGlobals.CurrentProject->ViewPos_Y + (pos.y() - (WidgetHeight / 2))* mul;
				}
				else if (AxisCode == 2) {
					targetX = mGlobals.CurrentProject->ViewPos_Z + (pos.x() - (WidgetWidth / 2))* mul;
					targetY = mGlobals.CurrentProject->ViewPos_Y + (pos.y() - (WidgetHeight / 2))* mul;
				}
				else if (AxisCode == 3) {
					targetX = mGlobals.CurrentProject->ViewPos_X + (pos.x() - (WidgetWidth / 2))* mul;
					targetY = mGlobals.CurrentProject->ViewPos_Z + (pos.y() - (WidgetHeight / 2))* mul;
				}
				
				//qDebug() << posXmin << " " << posXmax << " " << targetX;
				//qDebug() << posYmin << " " << posYmax << " " << targetY;
				if (posXmin < targetX && posXmax > targetX && posYmin < targetY && posYmax > targetY) {
					/*qDebug() << "This Block!!!!!";
					qDebug() << posXmin << " " << posXmax << " " << targetX;
					qDebug() << posYmin << " " << posYmax << " " << targetY;*/

					int tX = (targetX - posXmin) / pow(2, BlockInfo.level);
					int tY = (targetY - posYmin) / pow(2, BlockInfo.level);
					return iter_lb->data[tY*mGlobals.CurrentProject->DataBlockSize + tX];

				}
				
			}

		}
	}
	
	return 0;

}

void ViewAxisGLWidget::mousePressEvent(QMouseEvent *event)
{
	//qDebug() << "mouse Press GL";
	lastPos = event->pos();

	if (CtrlKeyFlag) {
		qDebug() << "Ctrl key pressed (" << event->x() << " " << event->y() << ")";
		QPointF pos(event->x(), event->y());
		unsigned int index = getCellIndexbyMousePos(pos);
		if (index != 0) {
			CtrlKeyFlag = false;
			mGlobals.mDialogManager->mDialogCellInfo->setIndexAndDraw(index);
			mGlobals.mDialogManager->mDialogCellInfo->exec();

		}
	}
	else if (event->buttons() & Qt::LeftButton) {
		if (ShiftKeyFlag) {
			MakingAnnotation = true;
			AnnotationStartPos = lastPos;
			AnnotationEndPos = lastPos;
		}
		else {
			for (int i = 0; i < mWidget->annotationWidget->annotations.size(); i++) {
				if (mWidget->annotationWidget->annotations[i]->axis != AxisCode)continue;
				float mul = pow(2, viewScale - mGlobals.CurrentProject->ViewZoomLevel);
				QPoint s, e;
				if (AxisCode == 1) {
					if (mWidget->annotationWidget->annotations[i]->startPos.z() != mGlobals.CurrentProject->ViewPos_Z)continue;
					s = QPoint((mWidget->annotationWidget->annotations[i]->startPos.x() - mGlobals.CurrentProject->ViewPos_X)*mul + WidgetWidth / 2,
						(mWidget->annotationWidget->annotations[i]->startPos.y() - mGlobals.CurrentProject->ViewPos_Y)*mul + WidgetHeight / 2);
					e = QPoint((mWidget->annotationWidget->annotations[i]->endPos.x() - mGlobals.CurrentProject->ViewPos_X)*mul + WidgetWidth / 2,
						(mWidget->annotationWidget->annotations[i]->endPos.y() - mGlobals.CurrentProject->ViewPos_Y)*mul + WidgetHeight / 2);
				}
				else if (AxisCode == 2) {
					if (mWidget->annotationWidget->annotations[i]->startPos.x() != mGlobals.CurrentProject->ViewPos_X)continue;

					s = QPoint((mWidget->annotationWidget->annotations[i]->startPos.z() - mGlobals.CurrentProject->ViewPos_Z)*mul + WidgetWidth / 2,
						(mWidget->annotationWidget->annotations[i]->startPos.y() - mGlobals.CurrentProject->ViewPos_Y)*mul + WidgetHeight / 2);
					e = QPoint((mWidget->annotationWidget->annotations[i]->endPos.z() - mGlobals.CurrentProject->ViewPos_Z)*mul + WidgetWidth / 2,
						(mWidget->annotationWidget->annotations[i]->endPos.y() - mGlobals.CurrentProject->ViewPos_Y)*mul + WidgetHeight / 2);

				}
				else if (AxisCode == 3) {
					if (mWidget->annotationWidget->annotations[i]->startPos.y() != mGlobals.CurrentProject->ViewPos_Y)continue;

					s = QPoint((mWidget->annotationWidget->annotations[i]->startPos.x() - mGlobals.CurrentProject->ViewPos_X)*mul + WidgetWidth / 2,
						(mWidget->annotationWidget->annotations[i]->startPos.z() - mGlobals.CurrentProject->ViewPos_Z)*mul + WidgetHeight / 2);
					e = QPoint((mWidget->annotationWidget->annotations[i]->endPos.x() - mGlobals.CurrentProject->ViewPos_X)*mul + WidgetWidth / 2,
						(mWidget->annotationWidget->annotations[i]->endPos.z() - mGlobals.CurrentProject->ViewPos_Z)*mul + WidgetHeight / 2);

				}

				if (s.x() < lastPos.x() && s.y() < lastPos.y()
					&& e.x() > lastPos.x() && e.y() > lastPos.y()) {
					mWidget->annotationWidget->focusItem(i);
					break;
				}
			}
		}
	}
	if (event->buttons() & Qt::RightButton) {

	}


}

void ViewAxisGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	setFocus();
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		if (MakingAnnotation) {
			AnnotationEndPos = event->pos();
			update();
		}
		else {
			//qDebug() << "mouse Move GL";
			float mul = pow(2, viewScale - mGlobals.CurrentProject->ViewZoomLevel);

			if (AxisCode == 1) {
				mGlobals.CurrentProject->ViewPos_X -= (float)dx / mul;
				mGlobals.CurrentProject->ViewPos_Y -= (float)dy / mul;
			}
			else if (AxisCode == 2) {
				mGlobals.CurrentProject->ViewPos_Z -= (float)dx / mul;
				mGlobals.CurrentProject->ViewPos_Y -= (float)dy / mul;
			}
			else if (AxisCode == 3) {
				mGlobals.CurrentProject->ViewPos_X -= (float)dx / mul;
				mGlobals.CurrentProject->ViewPos_Z -= (float)dy / mul;
			}

			emit update_view_state(mGlobals.CurrentProject->ViewPos_X, mGlobals.CurrentProject->ViewPos_Y, mGlobals.CurrentProject->ViewPos_Z, mGlobals.CurrentProject->ViewZoomLevel);
			emit updateAll(GLView_Index, false);
			update();
		}
	}
	lastPos = event->pos();
}

void ViewAxisGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (MakingAnnotation) {
		QVector3D s, e;

		if (abs(AnnotationStartPos.x() - AnnotationEndPos.x())<5 && abs(AnnotationStartPos.y() - AnnotationEndPos.y())<5) {
			QPointF pos(AnnotationStartPos.x(), AnnotationStartPos.y());
			unsigned int index = getCellIndexbyMousePos(pos);
			if (index == 0) {
				MakingAnnotation = false;
				AnnotationEndPos = QPoint(0, 0);
				AnnotationStartPos = QPoint(0, 0);
				return;
			}
			s = QVector3D(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[index]].minbox.x,
				mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[index]].minbox.y,
				mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[index]].minbox.z);
			e = QVector3D(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[index]].maxbox.x,
				mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[index]].maxbox.y,
				mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[index]].maxbox.z);

			char tt[20];
			sprintf(tt, "Cell %d", index);
			emit addAnnotation(QString(tt), QColor(0, 255, 255, 100), s, e, 0);
			MakingAnnotation = false;
			AnnotationEndPos = QPoint(0, 0);
			AnnotationStartPos = QPoint(0, 0);
			update();
			emit updateAll(GLView_Index, false);
			emit clicked();
			return;
		}

		if (AnnotationStartPos.x() > AnnotationEndPos.x()) {
			int t = AnnotationStartPos.x();
			AnnotationStartPos.setX(AnnotationEndPos.x());
			AnnotationEndPos.setX(t);
		}
		if (AnnotationStartPos.y() > AnnotationEndPos.y()) {
			int t = AnnotationStartPos.y();
			AnnotationStartPos.setY(AnnotationEndPos.y());
			AnnotationEndPos.setY(t);
		}

		float mul = 1.0 / pow(2, viewScale - mGlobals.CurrentProject->ViewZoomLevel);
		if (AxisCode == 1) {
			s= QVector3D(mGlobals.CurrentProject->ViewPos_X + (AnnotationStartPos.x() - (WidgetWidth / 2))* mul,
						mGlobals.CurrentProject->ViewPos_Y + (AnnotationStartPos.y() - (WidgetHeight / 2))* mul,
						mGlobals.CurrentProject->ViewPos_Z);
		}
		else if (AxisCode == 2) {
			s=QVector3D(mGlobals.CurrentProject->ViewPos_X,
						mGlobals.CurrentProject->ViewPos_Y + (AnnotationStartPos.y() - (WidgetHeight / 2))* mul,
				mGlobals.CurrentProject->ViewPos_Z + (AnnotationStartPos.x() - (WidgetWidth / 2))* mul);
		}
		else if (AxisCode == 3) {
			s=QVector3D(mGlobals.CurrentProject->ViewPos_X + (AnnotationStartPos.x() - (WidgetWidth / 2))* mul,
						mGlobals.CurrentProject->ViewPos_Y,
						mGlobals.CurrentProject->ViewPos_Z + (AnnotationStartPos.y() - (WidgetHeight / 2))* mul);
		}
		if (AxisCode == 1) {
			e = QVector3D(mGlobals.CurrentProject->ViewPos_X + (AnnotationEndPos.x() - (WidgetWidth / 2))* mul,
				mGlobals.CurrentProject->ViewPos_Y + (AnnotationEndPos.y() - (WidgetHeight / 2))* mul,
				mGlobals.CurrentProject->ViewPos_Z);
		}
		else if (AxisCode == 2) {
			e = QVector3D(mGlobals.CurrentProject->ViewPos_X,
				mGlobals.CurrentProject->ViewPos_Y + (AnnotationEndPos.y() - (WidgetHeight / 2))* mul,
				mGlobals.CurrentProject->ViewPos_Z + (AnnotationEndPos.x() - (WidgetWidth / 2))* mul);
		}
		else if (AxisCode == 3) {
			e = QVector3D(mGlobals.CurrentProject->ViewPos_X + (AnnotationEndPos.x() - (WidgetWidth / 2))* mul,
				mGlobals.CurrentProject->ViewPos_Y,
				mGlobals.CurrentProject->ViewPos_Z + (AnnotationEndPos.y() - (WidgetHeight / 2))* mul);
		}

		emit addAnnotation("",QColor(0,0,255,100),s,e,AxisCode);
		MakingAnnotation = false;
		AnnotationEndPos = QPoint(0, 0);
		AnnotationStartPos = QPoint(0, 0);
//		emit update_view_state(mGlobals.CurrentProject->ViewPos_X, mGlobals.CurrentProject->ViewPos_Y, mGlobals.CurrentProject->ViewPos_Z, mGlobals.CurrentProject->ViewZoomLevel);
		update();
	}

	//qDebug() << "mouse Release GL";
	emit clicked();
}

void ViewAxisGLWidget::wheelEvent(QWheelEvent *event){
	float delta = (float)event->angleDelta().y();

	if (delta > 0) {
		mGlobals.CurrentProject->ViewZoomLevel -= 0.05;
		emit update_view_state(mGlobals.CurrentProject->ViewPos_X, mGlobals.CurrentProject->ViewPos_Y, mGlobals.CurrentProject->ViewPos_Z, mGlobals.CurrentProject->ViewZoomLevel);
		emit updateAll(GLView_Index, true);
		update();
	}
	else {
		mGlobals.CurrentProject->ViewZoomLevel += 0.05;
		emit update_view_state(mGlobals.CurrentProject->ViewPos_X, mGlobals.CurrentProject->ViewPos_Y, mGlobals.CurrentProject->ViewPos_Z, mGlobals.CurrentProject->ViewZoomLevel);
		emit updateAll(GLView_Index, true);
		update();
	}
}

void ViewAxisGLWidget::keyPressEvent(QKeyEvent* event) {
	if (event->key() == Qt::Key_P) {
		FILE *f = fopen("inputCoords.txt", "r");
		if (f != NULL) {
			FILE *f2 = fopen("outputCoords.csv", "w");
			fprintf(f2, "x,y,z,cell index,volume,sphericity,eccentricity\n");

			int cellN = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->MaxCellCount;
			int volumeIndex = 0;
			int sphericityIndex = 0;
			int eccentricityIndex = 0;

			for (int i = 0; i < mGlobals.CurrentProject->mFeature.size(); i++) {
				if (mGlobals.CurrentProject->mFeature[i].FeatureName == "volume") {
					volumeIndex = i;
				}
				if (mGlobals.CurrentProject->mFeature[i].FeatureName == "sphericity") {
					sphericityIndex = i;
				}
				if (mGlobals.CurrentProject->mFeature[i].FeatureName == "eccentricity") {
					eccentricityIndex = i;
				}
			}
			while (1) {
				int x, y, z;
				fscanf(f, "%d %d %d", &x, &y, &z);
				if (x == -1)break;
				int len = 99999;
				int index = -1;
				for (int i = 0; i <= cellN; i++) {
					coord t1 = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[i].minbox;
					coord t2 = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[i].maxbox;
					if (x >= t1.x && y >= t1.y && z >= t1.z
						&& x <= t2.x && y <= t2.y && z <= t2.z) {
						int t = abs((t1.x + t2.x)*0.5 - x) + abs((t1.y + t2.y)*0.5 - y) + abs((t1.z + t2.z)*0.5 - z);
						if (t < len) {
							len = t;

							if (index != -1) {
								qDebug() << x << y << z << "overlap";
							}
							index = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[i].index;
							fprintf(f2, "%d,%d,%d,%d,%0.2lf,%0.4lf,,%0.4lf\n", x, y, z, index, mGlobals.CurrentProject->mFeature[volumeIndex].FeatureMap[index],
								mGlobals.CurrentProject->mFeature[sphericityIndex].FeatureMap[index],
								mGlobals.CurrentProject->mFeature[eccentricityIndex].FeatureMap[index]);

						}
					}
				}
				if (index == -1) {
					fprintf(f2, "%d,%d,%d,-1,-1,-1,-1\n", x, y, z);
				}
			}
			fclose(f);
			fclose(f2);
		}
	}

	if (event->key() == Qt::Key_Left) {
		qDebug() << "left";
	}
	else if (event->key() == Qt::Key_Right) {
		qDebug() << "right";
	}
	else if (event->key() == Qt::Key_Up) {

		if (AxisCode == 1) {
			if (mGlobals.CurrentProject->ViewPos_Z > 0) {
				mGlobals.CurrentProject->ViewPos_Z -= 1.0;
			}
		}
		else if (AxisCode == 2) {
			if (mGlobals.CurrentProject->ViewPos_X > 0) {
				mGlobals.CurrentProject->ViewPos_X -= 1.0;
			}
		}
		else if (AxisCode == 3) {
			if (mGlobals.CurrentProject->ViewPos_Y > 0) {
				mGlobals.CurrentProject->ViewPos_Y -= 1.0;
			}
		}
		emit update_view_state(mGlobals.CurrentProject->ViewPos_X, mGlobals.CurrentProject->ViewPos_Y, mGlobals.CurrentProject->ViewPos_Z, mGlobals.CurrentProject->ViewZoomLevel);
		emit updateAll(GLView_Index, false);
		update();
	}
	else if (event->key() == Qt::Key_Down) {
		if (AxisCode == 1) {
			if (mGlobals.CurrentProject->ViewPos_Z > 0) {
				mGlobals.CurrentProject->ViewPos_Z += 1.0;
			}
		}
		else if (AxisCode == 2) {
			if (mGlobals.CurrentProject->ViewPos_X > 0) {
				mGlobals.CurrentProject->ViewPos_X += 1.0;
			}
		}
		else if (AxisCode == 3) {
			if (mGlobals.CurrentProject->ViewPos_Y > 0) {
				mGlobals.CurrentProject->ViewPos_Y += 1.0;
			}
		}
		emit update_view_state(mGlobals.CurrentProject->ViewPos_X, mGlobals.CurrentProject->ViewPos_Y, mGlobals.CurrentProject->ViewPos_Z, mGlobals.CurrentProject->ViewZoomLevel);
		emit updateAll(GLView_Index, false);
		update();
	}
	else if (event->key() == Qt::Key_Control) {
		CtrlKeyFlag = true;
	}
	else if (event->key() == Qt::Key_Shift) {
		ShiftKeyFlag = true;
	}
}


void ViewAxisGLWidget::keyReleaseEvent(QKeyEvent* event) {
	if (event->key() == Qt::Key_Control) {
		CtrlKeyFlag = false;
	}
	else if (event->key() == Qt::Key_Shift) {
		ShiftKeyFlag = false;
	}
	
}

