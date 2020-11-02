#include "ZebrafishAnalytics.h"
#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include "Source/view/theme/darkstyle.h"
#include "Source/view/theme/framelesswindow.h"
#include "Source/global.h"

#include <iostream>
#include <fstream>
#include <string>

#include "Source/model/DataCellAnalytics.h"

int main(int argc, char *argv[])
{

	DataCellAnalytics cal;
//	cal.featurePrint("Z:/150809_60nmpxS1_PSCalign150801_crop3072x3072x1024_TrakEM2/Labels1_3072_3072_1024_16.raw", "forLabel1_new_test1.csv");
	//cal.CellVolumeSurfaceSphericityCalculation("F:/Ganglia/XY/", "morp/Ganglia/new_", 3299);

	//cal.featurePrint("Z:/150809_60nmpxS1_PSCalign150801_crop3072x3072x1024_TrakEM2/Labels2_3072_3072_1024_16.raw", "forLabel2_new.csv");
	//cal.featurePrint("Z:/150809_60nmpxS1_PSCalign150801_crop3072x3072x1024_TrakEM2/Labels3_3072_3072_1024_16.raw", "forLabel3_new.csv");

	//cal.CellVolumeSurfaceSphericityCalculation("F:/Eye/XY/", "morp/Eye/new_", 438297);
	//cal.CellVolumeSurfaceSphericityCalculation("F:/Brain/XY/", "morp/Brain/new_", 358893);

	//cal.EssentricityCaculation("F:/Ganglia/XY/","morp/Ganglia/",1,3299,3299);
	//cal.EssentricityCaculation("F:/Eye/XY/", "morp/Eye/", 1,438297,438297);
	//cal.EssentricityCaculation("F:/Brain/XY/", "morp/Brain/", 1,200000,358893);
	//cal.EssentricityCaculation("F:/Brain/XY/", "morp/Brain/", 200001,358893,358893);
	//cal.CellFeatureCalculation("F:/EM/XY/", "F:/Ganglia/XY/", "morp/Ganglia/", 3299);
	//cal.CellFeatureCalculation("F:/EM/XY/", "F:/Eye/XY/", "morp/Eye/", 438297);
	//cal.CellFeatureCalculation("F:/EM/XY/", "F:/Brain/XY/", "morp/Brain/", 358893);
	//cal.CellVolumeSurfaceSphericityCalculation("F:/EM/XY/", "F:/Ganglia/XY/", "morp/Ganglia/",1,3299, 3299);
	//cal.CellVolumeSurfaceSphericityCalculation("F:/EM/XY/", "F:/Eye/XY/", "morp/Eye/", 1,438297,438297);

	//cal.CellVolumeSurfaceSphericityCalculation("F:/EM/XY/", "F:/Brain/XY/", "morp/Brain/", 1, 200000, 358893);
	//cal.CellVolumeSurfaceSphericityCalculation("F:/EM/XY/", "F:/Brain/XY/", "morp/Brain/", 200001, 358893, 358893);
	QApplication a(argc, argv);
	// style our application with custom dark style
	a.setStyle(new DarkStyle);

	mGlobals.Application_Path = a.applicationDirPath();
	mGlobals.loadSysinfo();



	// create frameless window (and set windowState or title)
	FramelessWindow framelessWindow;
	//framelessWindow.setWindowState(Qt::WindowMaximized);
	framelessWindow.setWindowTitle("Zebrafish Analytics");
//	framelessWindow.setWindowIcon(a.style()->standardIcon(QStyle::SP_DesktopIcon));

	framelessWindow.setWindowIcon(QIcon("Resources/zevis_icon.png"));
	
	// create our mainwindow instance
	ZebrafishAnalytics *mainWindow = new ZebrafishAnalytics;
	mGlobals.mainWindow = mainWindow;
	// add the mainwindow to our custom frameless window
	framelessWindow.setContent(mainWindow);
	framelessWindow.show();

	return a.exec();
}
