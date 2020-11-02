#pragma once

#include <QObject>
#include <QtWidgets>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QDebug>
#include <QImage>
#include <algorithm>


#include "Source/Utils.h"
#include "qneblock.h"
#include "Source/view/ViewHeatmapGLWidget.h"
#include "Source/view/ViewHistogramWidget.h"
#include "Source/model/process/ThreadSubregion.h"

class QNEBlock;
class ViewHeatmapGLWidget;
class ViewHistogramWidget;
class ThreadSubregion;

class BlockWidget : public QObject
{
	Q_OBJECT

signals:
	void deleteNode(QNEBlock *ptr);

public slots:
	void handleSubregionInformationUpdate(QString);
	void handleBlockColorBtn();
	void handleCloseBtn();
	void handleAnnotationBtn();
	void handleDropdownChange(int index);
	void handleDropdownChange2(int index);

	void handleHistogramUpdate();
	void handleStatisticsBtn();
	void handleHistogramSetBtn_Percent();
	void handleHistogramSetBtn_Value();
	void handleSubregionSetBtn();
	void handleSubregionLoading();
	void handleIconBtnPress();
	void handleIconBtnRelease();

	void subregion_progess_update(float v);
	void subregion_progess_log_update(QString);



public:
	enum { OriginBlock = 1, DataBlock = 2, FeatureBlock = 3, SubregionBlock = 4,  SetBlock = 5, CutPlaneBlock = 6, StatisticBlock = 7 , SubsetExplorerBlock=8};


	BlockWidget(QObject *parent);
	~BlockWidget();
	void initialize(std::string name, int aflags, int awidth, int aheight, QColor acolor, QNEBlock *parent);

	void generateIcon(int type);

	std::string BlockName;
	QTimer* timer;
	QTimer* timerForMouseInteraction;


	ThreadSubregion *mThread;
	QColor BackgroundColor;
	QString BackgroundColor_style;
	QColor DataColor=QColor(255, 255, 123);;

	QWidget *ToolBox=NULL;
	ViewHeatmapGLWidget *DataHeatmap = NULL;

	QWidget *DataInputOutput = NULL;
	
	QWidget *SubregionInformationMaster = NULL;
	QLabel *subregion_volume_unit = NULL;
	QWidget *SubregionDropdownMaster = NULL;
	QComboBox *SubregionDropdown = NULL;
	QComboBox *SubregionSelectMethod = NULL;



	QWidget *StatisticWidget = NULL;
	QWidget *StatisticLayoutWidget = NULL;

	

	QWidget *FeatureHistogramMaster = NULL;
	ViewHistogramWidget *FeatureHistogram = NULL;


	QWidget  *FeatureDropdownMaster = NULL;
	QComboBox *FeatureDropdown = NULL;
	
	QWidget  *SetDropdownMaster = NULL;
	QComboBox *SetDropdown = NULL;
	
	QWidget *CutDropdownMaster = NULL;
	QComboBox  *CutDropdown = NULL;

	QWidget  *StatisticDropdownMaster = NULL;
	QComboBox  *StatisticDropdown = NULL;

	QWidget *SimilarityTarget = NULL;
	QListWidget *SimilarityList = NULL;

	QGraphicsProxyWidget *proxy=NULL;
	QWidget *openedWidget=NULL;
	QVBoxLayout *openedLayout=NULL;
	QPoint globalCursorPos;
	int iconActivated = 0;
	int selected = 0;

	QPushButton *BucketColorBtn = NULL;
	QPushButton *CloseBtn = NULL;
	QPushButton *AnnotationBtn = NULL;
	QPushButton *StatisticsBtn = NULL;
	QPushButton *IconBtn = NULL;
	bool iconMoved = false;
	bool iconPressed = false;
	QPoint prevPos = QPoint(-1, -1);


	std::list<unsigned int> CellIndexListInput;
	std::list<unsigned int> CellIndexListInput_Sub;

	std::list<unsigned int> CellIndexListOutput;
	std::list<annotation> mAnnotation;

	void addAnnotation(QString cmt);
	void removeAnnotation(int index);
	void updatedInputList();
	void updatedInputListLoading();

	void updatedCellColor();
	void updatedSubregionList();
	void updatedFeatureList();
	void updatedFeatureList2();

	int BlockFlag;

	int SubregionPrecision;
	double SubregionVolume = 0.0f;
	std::map<unsigned int, bool> TouchIndex;
	std::map<unsigned int, bool> IntersectIndex;
	std::map<unsigned int, bool> CompleteIndex;
	double plane_pos_x = 0;
	double plane_pos_y = 0;
	double plane_pos_z = 0;
	double plane_up_x = 1;
	double plane_up_y = 0;
	double plane_up_z = 0;

	QLineEdit *histogram_start_percent = NULL;
	QLineEdit *histogram_end_percent = NULL;
	QLineEdit *histogram_start_value = NULL;
	QLineEdit *histogram_end_value = NULL;
	QLabel *histogram_start_value_unit = NULL;
	QLabel *histogram_end_value_unit = NULL;




	QComboBox *subreigon_precision = NULL;
	QLineEdit *subreigon_volume = NULL;
	QLabel *SubregionProgressbarLog = NULL;
	QProgressBar *SubregionProgressbar = NULL;
	QLineEdit *subregion_pos_x = NULL;
	QLineEdit *subregion_pos_y = NULL;
	QLineEdit *subregion_pos_z = NULL;
	QLineEdit *subregion_up_x = NULL;
	QLineEdit *subregion_up_y = NULL;
	QLineEdit *subregion_up_z = NULL;

	QNEBlock * m_block = NULL;

	bool IsSubregionGenerated = false;


	void generateOneStatistic(int labelIndex, int featureIndex, QString name);


	QLineEdit *count_label_c = new QLineEdit;
	QLineEdit *feature_min_label_c = new QLineEdit;
	QLabel *feature_min_label_u = new QLabel;
	QLineEdit *feature_max_label_c = new QLineEdit;
	QLabel *feature_max_label_u = new QLabel;
	QLineEdit *feature_mean_label_c = new QLineEdit;
	QLabel *feature_mean_label_u = new QLabel;
	QLineEdit *feature_var_label_c = new QLineEdit;
	QLabel *feature_var_label_u = new QLabel;
	QLineEdit *feature_sum_label_c = new QLineEdit;
	QLabel *feature_sum_label_u = new QLabel;
	QLabel *feature_five_label_t = new QLabel;
	QLineEdit * _1_data[5];

	QVBoxLayout * nodeLayout = new QVBoxLayout;

	QLineEdit *blockName_edit=NULL;

private slots:
	void syncPos();
	void mouseInteraction();

public:
	QLabel *count_input1 = NULL;
	QLabel *count_input2 = NULL;
	QLabel *count_output = NULL;


	QVector <unsigned int> white_list;
	QVector <unsigned int> black_list;
	QVector <unsigned int> selected_list;
	QVector<QVector2D> selectPoints;
	bool is_selection = false;
	int xaxis=0;
	int yaxis=0;
	unsigned char *volume[64];
	unsigned int *label[64];
	int w[64];
	int h[64];
	int d[64];
	float means[64][5];

	bool inputUpdated = false;

	bool *IsForegroundByRF = NULL;

	/*bool SubregionActivated = true;
	bool SubregionVolumeStatus = false;*/



	void generate_ToolBox(int aflag);
	void generate_DataHeatmap(int width, int height);
	void generate_DataInputOutput(int width, int height);
	void generate_SubregionInformation(int awidth, int aheight);
	void generate_SubregionDropdown(int width, int height);
	void generate_FeatureHistogram(int width, int height);
	void generate_FeatureDropdown(int width, int height);
	void generate_SetDropdown(int width, int height);
	void generate_CutPlaneDropdown(int width, int height);
	void generate_Statistic(int width, int height);

//	void generate_SubsetExplorer(int width, int height);

	
	void updateColorBox(QPushButton *target, QColor color);

	void checkNextBlock();
	void checkNextBlockLoading();

	void set_operation(int index);


	QFont mFont;

};

