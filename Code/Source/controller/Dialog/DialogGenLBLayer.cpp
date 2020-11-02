#include "DialogGenLBLayer.h"

DialogGenLBLayer::DialogGenLBLayer(QWidget *parent) :QDialog(parent)
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
	dialog_title->setAlignment(Qt::AlignTop);
	dialog_title->setMargin(5);
	dialog_title->setText("Generate \nLabeled Layer");
	dialog_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	dialog_title->setMinimumWidth(nWidth / 4);

	QFont title_font("Arial", 20, QFont::Bold);
	dialog_title->setFont(title_font);

	QLabel *dialog_contents = new QLabel(this);
	dialog_contents->setMargin(5);
	dialog_contents->setWordWrap(true);
	dialog_contents->setText("Load the label of the cell. The data can be loaded with an unsigned int 32 bit RAW type.");
	QFont contents_font("Arial", 11, QFont::Normal);
	dialog_contents->setFont(contents_font);

	left_layout->addWidget(dialog_title);
	left_layout->addWidget(dialog_contents);


	QFont right_font("Arial", 10, QFont::Normal);
	QFont right_small_font("Arial", 9, QFont::Bold);
	QFont right_small_contents_font("Arial", 9, QFont::Normal);

	QHBoxLayout *label_size_layout = new QHBoxLayout;

	QLabel *label_size = new QLabel(this);
	label_size->setText("Labeled Sequence Size   ");
	label_size->setFont(right_font);

	QLabel *label_size_x_title = new QLabel(this);
	label_size_x_title->setText("X :");
	label_size_x_title->setFont(right_small_contents_font);

	label_size_x = new QLineEdit(this);
	label_size_x->setPlaceholderText("0");
	label_size_x->setFocus();

	QLabel *label_size_y_title = new QLabel(this);
	label_size_y_title->setText("Y :");
	label_size_y_title->setFont(right_small_contents_font);

	label_size_y = new QLineEdit(this);
	label_size_y->setPlaceholderText("0");
	label_size_y->setFocus();

	QLabel *label_size_z_title = new QLabel(this);
	label_size_z_title->setText("Z :");
	label_size_z_title->setFont(right_small_contents_font);

	label_size_z = new QLineEdit(this);
	label_size_z->setPlaceholderText("0");
	label_size_z->setFocus();


	label_size_layout->addWidget(label_size);
	label_size_layout->addWidget(label_size_x_title);
	label_size_layout->addWidget(label_size_x);
	label_size_layout->addWidget(label_size_y_title);
	label_size_layout->addWidget(label_size_y);
	label_size_layout->addWidget(label_size_z_title);
	label_size_layout->addWidget(label_size_z);

	QHBoxLayout *location_layout = new QHBoxLayout;

	QLabel *location = new QLabel(this);
	location->setText("Labeled Sequence Location");
	location->setFont(right_font);

	seedImagePath = new QLineEdit(this);
	seedImagePath->setPlaceholderText("Type your labeled sequence location");
	seedImagePath->setFocus();

	QPushButton * location_find_btn = new QPushButton(this);
	location_find_btn->setText("Find");
	connect(location_find_btn, SIGNAL(released()), this, SLOT(find()));

	location_layout->addWidget(location);
	location_layout->addWidget(seedImagePath);
	location_layout->addWidget(location_find_btn);

	QHBoxLayout *image_info_layout = new QHBoxLayout;
	exampleImage = new QLabel;
	exampleImage->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	exampleImage->setAlignment(Qt::AlignCenter);

	exampleImage->setText("No Image");
	exampleImage->setFixedWidth(nHeight / 4);
	exampleImage->setFixedHeight(nHeight / 4);
	exampleImage->setFrameStyle(QFrame::Box | QFrame::Plain);
	exampleImage->setVisible(false);
	QVBoxLayout *image_info_right_layout = new QVBoxLayout;
	image_info_right_layout->setAlignment(Qt::AlignTop);

	QHBoxLayout *image_type_layout = new QHBoxLayout;
	image_type_layout->setAlignment(Qt::AlignLeft);
	image_type_title = new QLabel(this);
	image_type_title->setText("labeled sequence type : ");
	image_type_title->setFont(right_small_font);
	image_type_title->setVisible(false);
	image_type = new QLabel(this);
	image_type->setVisible(false);
	image_type->setFont(right_small_contents_font);
	image_type_layout->addWidget(image_type_title);
	image_type_layout->addWidget(image_type);

	QHBoxLayout *image_size_layout = new QHBoxLayout;
	image_size_layout->setAlignment(Qt::AlignLeft);
	image_size_title = new QLabel(this);
	image_size_title->setText("labeled sequence size : ");
	image_size_title->setFont(right_small_font);
	image_size_title->setVisible(false);
	image_size = new QLabel(this);
	image_size->setVisible(false);
	image_size->setFont(right_small_contents_font);
	image_size_layout->addWidget(image_size_title);
	image_size_layout->addWidget(image_size);


	image_info_right_layout->addLayout(image_type_layout);
	image_info_right_layout->addLayout(image_size_layout);

	image_info_layout->addWidget(exampleImage);
	image_info_layout->addLayout(image_info_right_layout);


	QHBoxLayout *save_location_layout = new QHBoxLayout;

	QLabel *save_location = new QLabel(this);
	save_location->setText("Labeled Layer Save Path");
	save_location->setFont(right_font);

	savepath = new QLineEdit(this);
	savepath->setPlaceholderText("Type your labeled layer location");
	savepath->setFocus();

	QPushButton * save_location_find_btn = new QPushButton(this);
	save_location_find_btn->setText("Find");
	connect(save_location_find_btn, SIGNAL(released()), this, SLOT(find_save()));

	save_location_layout->addWidget(save_location);
	save_location_layout->addWidget(savepath);
	save_location_layout->addWidget(save_location_find_btn);

	mProgressLog = new QLabel(this);
	mProgressLog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	mProgressLog->setFont(right_font);
	mProgressLog->setVisible(false);


	mProgressbar = new QProgressBar;
	mProgressbar->setVisible(false);

	QHBoxLayout *filler_layout = new QHBoxLayout;
	QWidget *filler = new QWidget;
	filler->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	filler_layout->addWidget(filler);



	QHBoxLayout *btn_layout = new QHBoxLayout;

	QPushButton * done_btn = new QPushButton(this);
	done_btn->setText("Generate");
	connect(done_btn, SIGNAL(released()), this, SLOT(generate()));

	QPushButton * reject_btn = new QPushButton(this);
	reject_btn->setText("Cancel");
	connect(reject_btn, SIGNAL(released()), this, SLOT(reject()));

	btn_layout->addWidget(done_btn);
	btn_layout->addWidget(reject_btn);
	
	label_size_layout->setMargin(10);
	right_layout->addLayout(label_size_layout);
	location_layout->setMargin(10);
	right_layout->addLayout(location_layout);
	image_info_layout->setMargin(10);
	right_layout->addLayout(image_info_layout);
	save_location_layout->setMargin(10);
	right_layout->addLayout(save_location_layout);

	right_layout->addLayout(filler_layout);
	right_layout->addWidget(mProgressLog);
	right_layout->addWidget(mProgressbar);
	right_layout->addLayout(btn_layout);

	this->setLayout(main_layout);

	main_layout->addLayout(left_layout);
	main_layout->addWidget(vertical_line);
	main_layout->addLayout(right_layout);

}

DialogGenLBLayer::~DialogGenLBLayer()
{
}


void DialogGenLBLayer::find() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open labeled sequence directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty() && !dir.isNull()) {
		seedImagePath->setText(dir);

		QDir imageSeqDir(dir);
		imageSeqDir.setNameFilters(QStringList() << "*.raw" );
		mImageList = imageSeqDir.entryList();

		
		if (mImageList.size() > 0) {
			image_path_check = true;
			QString exampleImagePath = dir + "/" + mImageList.at(0);

			int input_width, input_height;
			input_width = label_size_x->text().toInt();
			input_height = label_size_y->text().toInt();
			unsigned int *raw_slice = new unsigned int[input_width*input_height];
			ifstream raw_ifs(exampleImagePath.toStdString(), std::ifstream::binary);
			raw_ifs.read((char *)&raw_slice[0], input_width*input_height*sizeof(unsigned int));


			QImage *Img = new QImage(input_width, input_height, QImage::Format_RGB32);
			Img->fill(Qt::black);
			int rawDataCount = 0;
			
			qDebug() << input_width;
			qDebug() << input_height;
			for (int y = 0; y<input_height; y++)
			{
				for (int x = 0; x<input_width; x++)
				{
					unsigned int temp = raw_slice[rawDataCount++];
					if (temp != 0) {
						Img->setPixel(x,y, qRgba(mGlobals.colorset[temp%12][0], mGlobals.colorset[temp % 12][1], mGlobals.colorset[temp % 12][2], 100));
					}
					else {
						Img->setPixel(x,y, qRgba(0, 0, 0, 100));
					}
				}
			}
			int width = exampleImage->width();
			int height = exampleImage->height();
			QPixmap *buffer = new QPixmap();
			
			*buffer = QPixmap::fromImage(*Img);
			float scaledFactor = (float)Img->height() / (float)height;

			*buffer = buffer->scaled(Img->width() / scaledFactor, Img->height() / scaledFactor);
			width = Img->width() / scaledFactor;
			height = Img->height() / scaledFactor;
			exampleImage->setFixedWidth(width);
			exampleImage->setFixedHeight(height);
			exampleImage->setPixmap(*buffer);
			exampleImage->setVisible(true);


			image_type_title->setVisible(true);
			image_size_title->setVisible(true);
			whole_volume_x = Img->width();
			whole_volume_y = Img->height();
			whole_volume_z = mImageList.size();

			QString sizetext = "X: " + QString::number(Img->width()) + " Y: " + QString::number(Img->height()) + " Z: " + QString::number(mImageList.size());
			qDebug() << mGlobals.mImageFormat[3];
			image_type->setText("raw(unsigned int)");
			image_size->setText(sizetext);

			image_type->setVisible(true);
			image_size->setVisible(true);
		}
		else {
			QMessageBox::about(0, QString::fromStdString("Image load Error"), QString::fromStdString("Image load Error"));
		}
	}
	qDebug() << "done";
}

void DialogGenLBLayer::find_save() {
	
	QString dir = QFileDialog::getExistingDirectory(this, tr("Labeled Layer directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	//QString dir = QFileDialog::getExistingDirectory(this, tr("Open image sequence directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty() && !dir.isNull()) {
		savepath->setText(dir);
		save_path_check = true;
	}
}

int DialogGenLBLayer::exec()
{
	qDebug("BG");
	activateWindow();

	this->show();
	return 1;
}
void DialogGenLBLayer::done(int val)
{
	qDebug("done.");
}

void DialogGenLBLayer::generate() {
	mProgressbar->setVisible(true);
	mProgressLog->setVisible(true);

	ThreadGenLB *mThread = new ThreadGenLB(this);

	mThread->setVolumeSize(whole_volume_x, whole_volume_y, whole_volume_z);
	mThread->setPathInfo(savepath->text().toLocal8Bit().constData(), image_type->text().toLocal8Bit().constData(), seedImagePath->text().toLocal8Bit().constData());
	mThread->start();

	connect(mThread, SIGNAL(progress_generate_lb(float)), this, SLOT(update_progress(float)));
	connect(mThread, SIGNAL(progress_generate_lb_log(QString)), this, SLOT(update_progress_log(QString)));
	connect(mThread, SIGNAL(finish_generate_lb()), this, SLOT(accept()));

}

void DialogGenLBLayer::update_progress_log(QString str) {
	mProgressLog->setText(str);
}


void DialogGenLBLayer::update_progress(float v) {
	//qDebug("update value");
	//qDebug() << v;
	mProgressbar->setValue((int)v);
}

void DialogGenLBLayer::accept()
{
	this->hide();
	qDebug("Accept.");
}
void DialogGenLBLayer::reject()
{
	emit thread_kill();

	qDebug("Reject.");
	this->hide();
}
