#pragma once

#include <QObject>
#include <QPushButton>
#include <QtWidgets>
#include <QLabel>
#include <QSizePolicy>
#include <math.h>
#include <QAbstractButton>
#include <QGraphicsScene>
#include <QFileDialog>


#include "Source/global.h"
#include "Source/view/ViewAxisGLWidget.h"
#include "Source/view/ViewGraphEditor.h"
#include "Source/view/ViewVolumeGLWidget.h"
#include "Source/controller/Contents_ProjectInfo.h"
#include "Source/controller/Contents_Annotations.h"
#include "Source/controller/CellSubsetExplorer.h"

class ViewAxisGLWidget;
class ViewGraphEditor;
class ViewVolumeGLWidget;
class Contents_ProjectInfo;
class Contents_Annotations;
class CellSubsetExplorer;

class Contents : public QObject
{
	Q_OBJECT

public:
	Contents(QWidget *parent);
	Contents(QWidget *parent, QLayout *parent_layout);
	~Contents();

	void InitGLView();
	void InitGraph();
	void InitProjectInfo();
	void InitSubregionFeatureList();
	void InitAnnotations();

	Contents_Annotations *annotationWidget;
	
	ViewAxisGLWidget *GL_XYAxis_Main=NULL;
	ViewAxisGLWidget *GL_YZAxis_Main = NULL;
	ViewAxisGLWidget *GL_ZXAxis_Main = NULL;
	ViewAxisGLWidget *GL_XYAxis_Sub = NULL;
	ViewAxisGLWidget *GL_YZAxis_Sub = NULL;
	ViewAxisGLWidget *GL_ZXAxis_Sub = NULL;

	ViewVolumeGLWidget *GL_Volume_Main = NULL;
	ViewVolumeGLWidget *GL_Volume_Sub = NULL;

	ViewGraphEditor *GraphEditorView = NULL;
	QVBoxLayout *graphEditorLayout = NULL;
	QVBoxLayout *buttonLayout = NULL;
	QGridLayout *overlapedLayout = NULL;
	QPushButton *b_icon_add = NULL;

	QSlider *GraphEditorViewScale = NULL;
	QLabel *GraphEditorViewScaleLabel = NULL;

	QLineEdit *ProjectName = NULL;
	QLineEdit *ProjectPath = NULL;
	QLineEdit *BackPath = NULL;
	QLineEdit *CellPath = NULL;


	QSplitter *second_lay;
	QHBoxLayout *volume_layout;

	QTabWidget *contents_left_tabwidget = NULL;
	QTabWidget *contents_center_tabwidget = NULL;
	QVBoxLayout *contents_center_layout = NULL;
	QListWidget *labelList = NULL;

	Contents_ProjectInfo *contents_projectinfo_master=NULL;

	void setLabel(QString dir);
	QWidget * mWidget = NULL;

	CellSubsetExplorer *Cell_Explorer=NULL;

public slots:
	void feature_updated();
	void subregion_updated();
	void GraphEditorViewScaleChange(int v);
	void loadSubregionFeature();
	void tabChanged(int index);
	void handleAddNode();

public slots:
	void updateAllWidget(int index, bool scaleflag);
	void handleLabelAddBtn();
	void handleSubregionAddBtn();
	void handleFeatureAddBtn();
	void handleDetailBtn_feature();
	void handleDeleteBtn_feature();
	void handleDeleteBtn_subregion();
	void handleInfoBtn_subregion();
	//void handleCheckBox_subregion(int state);
	void handleColorBtn_subregion();
	void handleOpacity(int value);

	void handleLabelChange(int newLabel);

private:
	
	void updateColorBox(QPushButton *target, QColor color);
	void LeftTabInit(QTabWidget *target, int width, int height);
	void CenterTabInit(QTabWidget *target, int width, int height);
	//void RightTabInit(QTabWidget *target, int width, int height);

	void replaceTab(QTabWidget * tabs, int index, QWidget * replacement, QString label);


	//void RightContentsOn();


	QListWidget *subregionList = NULL;
	QListWidget *featureList = NULL;



	//ViewHistogramBox *MorphoHistBox[5];

	

};
