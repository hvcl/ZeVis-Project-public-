#pragma once

#include <QObject>
#include <QPushButton>
#include <QtWidgets>
#include <QIcon>

#include "Dialog/DialogManager.h"
class DialogManager;

class Contents_ProjectInfo : public QObject
{
	Q_OBJECT

public:
	Contents_ProjectInfo(QWidget *parent);
	Contents_ProjectInfo(QWidget *parent, QLayout *parent_layout);
	~Contents_ProjectInfo();
	
	QLineEdit *ProjectName;
	QPushButton *ProjectPath;
	QPushButton *BackPath;
	QLineEdit *CellPath;

	QLineEdit *DataDimX;
	QLineEdit *DataDimY;
	QLineEdit *DataDimZ;

	QLineEdit *ResolutionX;
	QLineEdit *ResolutionY;
	QLineEdit *ResolutionZ;


	QLabel *ResolutionX_Unit;
	QLabel *ResolutionY_Unit;
	QLabel *ResolutionZ_Unit;

	QComboBox *Resolution_Unit;
	int prevResoluiton_Unit = 0;
	
	
	void setLayerBackground(QString dir);

public slots:
	void handleModifyBtn();
	
	void unit_change(int index);

	void handleLoad();

	void handleSetProjectPath();
	void handleSetProjectName(QString a);
	void handleLoadEM();


signals:
	void emLoaded();

private:
	QWidget * mWidget;

	void project_location_layout(QVBoxLayout *layout);
	void project_data_dim_layout(QVBoxLayout *layout);
	void project_resolution_layout(QVBoxLayout *layout);
	void project_subregion_layout(QVBoxLayout *layout);

};
