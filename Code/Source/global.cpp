#include "global.h"

Globals mGlobals;


Globals::Globals()
{
	
	CurrentProject = new DataProject;
	if (projectType == 60) {
		CurrentProject->ResolutionX = 56.4;
		CurrentProject->ResolutionY = 56.4;
		CurrentProject->ResolutionZ = 60;
	}
	QString a;
	gUnitList.append("(Constant)");
	gUnitList.append("nm");
	a = "nm";
	a += QChar(0x00B2);
	gUnitList.append(a);
	
	a = "nm";
	a += QChar(0x00B3);
	gUnitList.append(a);

	a = QChar(0x00B5);
	a += "m";
	gUnitList.append(a);
	
	a = QChar(0x00B5);
	a += "m";
	a += QChar(0x00B2);
	gUnitList.append(a);

	a = QChar(0x00B5);
	a += "m";
	a += QChar(0x00B3);
	gUnitList.append(a);
	
	gUnitList.append("mm");
	a = "mm";
	a += QChar(0x00B2);
	gUnitList.append(a);

	a = "mm";
	a += QChar(0x00B3);
	gUnitList.append(a);


	//gUnitList.append("(Constant)");
}


void Globals::saveSysinfo() {
	std::string sys_conf_path = mGlobals.Application_Path.toLocal8Bit().constData();
	sys_conf_path += "/application_sys.conf";

	std::ofstream ofs(sys_conf_path);

	ofs << OS_Name.toLocal8Bit().constData() << std::endl;
	ofs << CPU_Name.toLocal8Bit().constData() << std::endl;
	ofs << GPU_Name.toLocal8Bit().constData() << std::endl;
	ofs << Cache_Path.toLocal8Bit().constData() << std::endl;
	ofs << RAM_Size << std::endl;

	ofs.close();
}

void Globals::loadSysinfo() {
	std::string sys_conf_path = Application_Path.toLocal8Bit().constData();
	sys_conf_path += "/application_sys.conf";

	std::ifstream ifs(sys_conf_path);

	if (ifs.is_open()) {

		std::string std_OS_Name;
		std::string std_CPU_Name;
		std::string std_GPU_Name;
		std::string std_Cache_Path;

		std::getline(ifs, std_OS_Name);
		std::getline(ifs, std_CPU_Name);
		std::getline(ifs, std_GPU_Name);
		std::getline(ifs, std_Cache_Path);

		ifs >> RAM_Size;

		OS_Name = QString::fromStdString(std_OS_Name);
		CPU_Name = QString::fromStdString(std_CPU_Name);
		GPU_Name = QString::fromStdString(std_GPU_Name);
		Cache_Path = QString::fromStdString(std_Cache_Path);

	}
	else {
		// show message:
		qDebug() << "Error opening file";
		getSysinfo();
		saveSysinfo();
	}


	ifs.close();
}

void Globals::getSysinfo() {
	//First make a delegate object that handles the cases where the computer has multiple CPUs
	std::unique_ptr<CPUInfoDelegate> cpuInfo = std::make_unique<CPUInfoDelegate>();

	//Then extract the separate CPUs into a vector (of CPUInfo objects)
	std::vector<CPUInfo> cpuInfoVector{ cpuInfo->cpuInfoVector() };

	//Print out the number of CPUs, directory from the delegate object
	std::cout << "This computer has " << cpuInfo->numberOfCPUInfoItems() << " CPU(s) installed" << std::endl;

	for (std::vector<CPUInfo>::const_iterator iter = cpuInfoVector.begin(); iter != cpuInfoVector.end(); iter++) {
		CPU_Name = QString::fromStdString(iter->name());
	}

	std::unique_ptr<RAMInfoDelegate> ramInfo = std::make_unique<RAMInfoDelegate>();
	std::vector<RAMInfo> ramInfoVector = ramInfo->ramInfoVector();
	std::cout << "This computer has " << ramInfo->numberOfRAMInfoItems() << " RAM stick(s) installed" << std::endl;

	for (std::vector<RAMInfo>::const_iterator iter = ramInfoVector.begin(); iter != ramInfoVector.end(); iter++) {
		qDebug() << QString::fromStdString(iter->capacity());
		QStringList temp = QString::fromStdString(iter->capacity()).split("MB");
		
		RAM_Size += temp[0].toInt();
	}

	std::unique_ptr<GPUInfoDelegate> gpuInfo = std::make_unique<GPUInfoDelegate>();
	std::vector<GPUInfo> gpuInfoVector = gpuInfo->gpuInfoVector();
	std::cout << "This computer has " << gpuInfo->numberOfGPUInfoItems() << " GPU(s) installed" << std::endl;
	for (std::vector<GPUInfo>::const_iterator iter = gpuInfoVector.begin(); iter != gpuInfoVector.end(); iter++) {
		GPU_Name = QString::fromStdString(iter->name());
	}

	std::unique_ptr<OSInfoDelegate> osInfo = std::make_unique<OSInfoDelegate>();
	std::vector<OSInfo> osInfoVector = osInfo->osInfoVector();
	std::cout << "This computer has " << osInfo->numberOfOSInfoItems() << " OS(s) installed" << std::endl;
	for (std::vector<OSInfo>::const_iterator iter = osInfoVector.begin(); iter != osInfoVector.end(); iter++) {
		OS_Name = QString::fromStdString(iter->name()).split("|")[0];
	}

	Cache_Path = Application_Path;
}

Globals::~Globals()
{

}
