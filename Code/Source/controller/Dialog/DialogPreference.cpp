#include "DialogPreference.h"

DialogPreference::DialogPreference(QWidget *parent) :QDialog(parent)
{
	pWidget = parent;
	setModal(true);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
	int nWidth = parent->width() / 2;
	int nHeight = parent->height() / 2;
	if (parent != NULL)
		setGeometry(parent->x() + parent->width() / 2 - nWidth / 2,
			parent->y() + parent->height() / 2 - nHeight / 2,
			nWidth, nHeight);
	else
		resize(nWidth, nHeight);

	QHBoxLayout *main_layout = new QHBoxLayout;
	main_layout->setMargin(5);

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->setAlignment(Qt::AlignTop);
	left_layout->setMargin(5);
	left_layout->setContentsMargins(20, 20, 20, 20);
	QVBoxLayout *right_layout = new QVBoxLayout;
	right_layout->setMargin(5);
	right_layout->setContentsMargins(20, 20, 20, 20);
	right_layout->setAlignment(Qt::AlignTop);

	QLabel *vertical_line = new QLabel(this);
	vertical_line->setFrameStyle(QFrame::VLine | QFrame::Plain);



	QLabel *dialog_title = new QLabel(this);
	dialog_title->setMargin(5);
	dialog_title->setText("System Preference");
	dialog_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	dialog_title->setMinimumWidth(nWidth / 4);

	QFont title_font("Arial", 20, QFont::Bold);
	dialog_title->setFont(title_font);

	QLabel *dialog_contents = new QLabel(this);
	dialog_contents->setMargin(5);
	dialog_contents->setWordWrap(true);
	dialog_contents->setText("This page is used to set system environment settings. Check system information and choose cache location.");
	QFont contents_font("Arial", 11, QFont::Normal);
	dialog_contents->setFont(contents_font);

	left_layout->addWidget(dialog_title);
	left_layout->addWidget(dialog_contents);

	QHBoxLayout *cpu_name_layout = new QHBoxLayout;

	QFont right_title_font("Arial", 10, QFont::Bold);
	QFont right_font("Arial", 10, QFont::Normal);
	QLabel *cpu_name = new QLabel(this);
	cpu_name->setAlignment(Qt::AlignLeft);
	cpu_name->setText("CPU Name : ");
	cpu_name->setFont(right_title_font);

	QLabel *cpu_name_info = new QLabel(this);
	cpu_name_info->setText(mGlobals.CPU_Name);
	cpu_name_info->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	cpu_name_info->setFont(right_font);
	cpu_name_layout->addWidget(cpu_name);
	cpu_name_layout->addWidget(cpu_name_info);


	QHBoxLayout *gpu_name_layout = new QHBoxLayout;
	QLabel *gpu_name = new QLabel(this);
	gpu_name->setText("GPU Name : ");
	gpu_name->setFont(right_title_font);

	QLabel *gpu_name_info = new QLabel(this);
	gpu_name_info->setText(mGlobals.GPU_Name);
	gpu_name_info->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	gpu_name_info->setFont(right_font);
	gpu_name_layout->addWidget(gpu_name);
	gpu_name_layout->addWidget(gpu_name_info);


	QHBoxLayout *ram_size_layout = new QHBoxLayout;
	QLabel *ram_size = new QLabel(this);
	ram_size->setText("RAM Size : ");
	ram_size->setFont(right_title_font);

	QLabel *ram_size_info = new QLabel(this);
	ram_size_info->setText(QString::number(mGlobals.RAM_Size) + "MB");
	ram_size_info->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	ram_size_info->setFont(right_font);
	ram_size_layout->addWidget(ram_size);
	ram_size_layout->addWidget(ram_size_info);

	QHBoxLayout *os_name_layout = new QHBoxLayout;
	QLabel *os_name = new QLabel(this);
	os_name->setText("OS : ");
	os_name->setFont(right_title_font);

	QLabel *os_name_info = new QLabel(this);
	os_name_info->setText(mGlobals.OS_Name);
	os_name_info->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	os_name_info->setFont(right_font);
	os_name_layout->addWidget(os_name);
	os_name_layout->addWidget(os_name_info);

	QHBoxLayout *save_location_layout = new QHBoxLayout;

	QLabel *save_location = new QLabel(this);
	save_location->setText("Cache path");
	save_location->setFont(right_font);

	savepath = new QLineEdit(this);
	savepath->setText(mGlobals.Cache_Path);
	savepath->setFocus();

	QPushButton * save_location_find_btn = new QPushButton(this);
	save_location_find_btn->setText("Find");
	connect(save_location_find_btn, SIGNAL(released()), this, SLOT(find_save_dir()));

	save_location_layout->addWidget(save_location);
	save_location_layout->addWidget(savepath);
	save_location_layout->addWidget(save_location_find_btn);



	QHBoxLayout *filler_layout = new QHBoxLayout;
	QWidget *filler = new QWidget;
	filler->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	filler_layout->addWidget(filler);


	QHBoxLayout *btn_layout = new QHBoxLayout;
	QPushButton * done_btn = new QPushButton(this);
	done_btn->setText("Done");
	connect(done_btn, SIGNAL(released()), this, SLOT(accept()));

	QPushButton * reject_btn = new QPushButton(this);
	reject_btn->setText("Cancel");
	connect(reject_btn, SIGNAL(released()), this, SLOT(reject()));

	btn_layout->addWidget(done_btn);
	btn_layout->addWidget(reject_btn);

	cpu_name_layout->setMargin(10);
	right_layout->addLayout(cpu_name_layout);
	gpu_name_layout->setMargin(10);
	right_layout->addLayout(gpu_name_layout);
	ram_size_layout->setMargin(10);
	right_layout->addLayout(ram_size_layout);
	os_name_layout->setMargin(10);
	right_layout->addLayout(os_name_layout);
	save_location_layout->setMargin(10);
	right_layout->addLayout(save_location_layout);
	right_layout->addLayout(filler_layout);
	right_layout->addLayout(btn_layout);

	this->setLayout(main_layout);

	main_layout->addLayout(left_layout);
	main_layout->addWidget(vertical_line);
	main_layout->addLayout(right_layout);

}

DialogPreference::~DialogPreference()
{
}

void DialogPreference::find_save_dir() {
	//QString filters("Analytics Projects (*.aproj);;");
	//QString dir = QFileDialog::getSaveFileName(this, "Save file", QDir::currentPath(), filters);
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open image sequence directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	savepath->setText(dir);
}


int DialogPreference::exec()
{
	activateWindow();

	this->show();
	return 1;
}
void DialogPreference::done(int val)
{
	printf("done()\n");
}

void DialogPreference::accept()
{
	qDebug("Accept.");
	mGlobals.Cache_Path = savepath->text();
	
	QDir dir(mGlobals.Cache_Path);
	dir.mkpath(mGlobals.Cache_Path);

	mGlobals.saveSysinfo();

	this->hide();
}
void DialogPreference::reject()
{
	qDebug("Reject.");
	this->hide();
}