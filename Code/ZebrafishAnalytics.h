#pragma once


#include <QMainWindow>
#include "Source/controller/TopToolbar.h"
#include "Source/controller/TopInterface.h"
#include "Source/controller/Contents.h"
#include "Source/controller/Dialog/DialogManager.h"

#include "Source/global.h"

class QAction;
class QActionGroup;
class QLabel;
class QMenu;

class TopToolbar;
class TopInterface;
class Contents;


class ZebrafishAnalytics : public QMainWindow
{
	Q_OBJECT

public:
	ZebrafishAnalytics();

	Contents *mContents = NULL;

	TopToolbar *mTopToolbar = NULL;

	TopInterface *mTopInterface = NULL;

	bool project_started = false;

	int timerID = 0;

protected:
	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

private slots:
	void handleButton();
	void handleNewProject();
	void handleOpenProject();
	void handleGenerateBGLayer();
	void handleGenerateLBLayer();
	void handleGenerateSRLayer();
	void handlePreference();
	void projectLoad();
	void handleProjectOn();


private:
	void connectAll();
	void createMenus();
	void createAction();
	QMenu * fileMenu = NULL;
	QAction *newProjectAction = NULL;
	QAction *generateBackgroundLayer = NULL;
	QAction *generateLabelLayer = NULL;
	QAction *generateSubregionLayer = NULL;
	QAction *preference = NULL;




};
