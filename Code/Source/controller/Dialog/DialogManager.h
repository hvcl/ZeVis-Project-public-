#pragma once

#include "DialogAddNode.h"
#include "DialogGenBGLayer.h"
#include "DialogGenLBLayer.h"
#include "DialogGenSRLayer.h"
#include "DialogNewProject.h"
#include "DialogPreference.h"

#include "DialogAddFeature.h"
#include "DialogAddSubregion.h"
#include "DialogDetailFeature.h"
#include "DialogAnnotation.h"
#include "DialogStatistics.h"
#include "DialogInfoSubregion.h"
#include "DialogCellInfo.h"
#include <QWidget>



class DialogGenBGLayer;
class DialogAddNode;
class DialogGenLBLayer;
class DialogGenSRLayer;
class DialogNewProject;
class DialogPreference;
class DialogAddFeature;
class DialogAddSubregion;
class DialogDetailFeature;
class DialogAnnotation;
class DialogStatistics;
class DialogInfoSubregion;
class DialogCellInfo;

class DialogManager
{
public:
	DialogManager();
	DialogManager(QWidget *parent);
	~DialogManager();

	DialogGenBGLayer * mDialogGenBGLayer;
	DialogAddNode * mDialogAddNode;
	DialogGenLBLayer * mDialogGenLBLayer;
	DialogGenSRLayer * mDialogGenSRLayer;
	DialogNewProject * mDialogNewProject;
	DialogPreference * mDialogPreference;
	DialogAddFeature * mDialogAddFeature;
	DialogAddSubregion * mDialogAddSubregion;
	DialogDetailFeature * mDialogDetailFeature;
	DialogAnnotation * mDialogAnnotation;
	DialogStatistics * mDialogStatistics;
	DialogInfoSubregion *mDialogInfoSubregion;
	DialogCellInfo * mDialogCellInfo;
private:
	QWidget *mWidget;
	void initializeDialog();

};

