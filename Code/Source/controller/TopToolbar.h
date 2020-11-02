#pragma once

#include <QObject>
#include <QPushButton>
#include <QtWidgets>
#include <QIcon>

#include "Source/controller/node/qneblock.h"


#include "Dialog/DialogManager.h"
//#include "DialogNewProject.h"
//#include "DialogAddNode.h"
class DialogManager;
class QNEBlock;

class TopToolbar : public QObject
{
	Q_OBJECT

public:
	TopToolbar(QWidget *parent);
	TopToolbar(QWidget *parent, QLayout *parent_layout);
	~TopToolbar();

	//void setDialogManager(DialogManager *ptr);
	QLabel *curState = NULL;

	void graphTraverse(QNEBlock *block);
	std::ofstream f;

signals:
	void loadProjectInfo();

public slots:
	void handleNewProject();
	void handleSaveProject();
	void handleOpenProject();
	void handleAddNode();
	void handleSaveStateProject();
	void handleLoadStateProject();


private:
	QWidget * mWidget;
	//DialogManager *mDialogManager;

	
};
