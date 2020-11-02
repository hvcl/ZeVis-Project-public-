#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

#include <qglobal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <QDebug>
#include <QDialog>
#include <vector>
#include <memory>
#include <QStringList>
#include <QOpenGLWidget>

#include "Source/model/DataProject.h"
#include "Source/model/system/systeminfo.h"
#include "Source/controller/Dialog/DialogManager.h"
#include "ZebrafishAnalytics.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>

class DialogManager;
class ZebrafishAnalytics;
class Globals
{

public:
	QStringList mImageFormat = { "Format_Invalid", "Format_Mono", "Format_MonoLSB", "Format_Indexed8", "Format_RGB32", "Format_ARGB32", "Format_ARGB32_Premultiplied", "Format_RGB16", "Format_ARGB8565_Premultiplied", "Format_RGB666", "Format_ARGB6666_Premultiplied", "Format_RGB555", "Format_ARGB8555_Premultiplied", "Format_RGB888", "Format_RGB444", "Format_ARGB4444_Premultiplied", "Format_RGBX8888", "Format_RGBA8888", "Format_RGBA8888_Premultiplied", "Format_BGR30", "Format_A2BGR30_Premultiplied", "Format_RGB30", "Format_A2RGB30_Premultiplied", "Format_Alpha8", "Format_Grayscale8" };
	int colorset[12][3] = { { 166, 206, 227 },{ 31, 120, 180 },{ 178, 223, 138 },{ 51, 160, 44 },{ 251, 154, 153 },{ 227, 26, 28 },{ 253, 191, 111 },{ 255, 127, 0 },{ 202, 178, 214 },{ 106, 61, 154 },{ 255, 255, 153 },{ 177, 89, 40 } };
	DataProject * CurrentProject;
	
	QString OS_Name;
	QString CPU_Name;
	QString GPU_Name;
	QString Cache_Path;
	QString Application_Path;
	int RAM_Size = 0;

	void getSysinfo();
	void saveSysinfo();
	void loadSysinfo();

	DialogManager *mDialogManager;

	ZebrafishAnalytics *mainWindow;

	int projectType = 60;

	QStringList gUnitList;
	Globals();
	~Globals();

};

extern Globals mGlobals;

#endif // GLOBALS_H
