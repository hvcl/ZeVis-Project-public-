#include "DialogManager.h"



DialogManager::DialogManager()
{
}

DialogManager::DialogManager(QWidget *parent)
{
	mWidget = parent;
	initializeDialog(); 
}


DialogManager::~DialogManager()
{
}


void DialogManager::initializeDialog() {
	mDialogAddNode = new DialogAddNode(mWidget);
	mDialogGenBGLayer = new DialogGenBGLayer(mWidget);
	mDialogGenLBLayer = new DialogGenLBLayer(mWidget);
	mDialogGenSRLayer = new DialogGenSRLayer(mWidget);
	mDialogNewProject = new DialogNewProject(mWidget);
	mDialogPreference = new DialogPreference(mWidget);
	mDialogAddFeature = new DialogAddFeature(mWidget);
	mDialogAddSubregion = new DialogAddSubregion(mWidget);
	mDialogDetailFeature = new DialogDetailFeature(mWidget);
	mDialogAnnotation = new DialogAnnotation(mWidget);
	mDialogStatistics = new DialogStatistics(mWidget);
	mDialogInfoSubregion = new DialogInfoSubregion(mWidget);
	mDialogCellInfo = new DialogCellInfo(mWidget);
}

