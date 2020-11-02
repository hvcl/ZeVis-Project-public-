#include "DialogStatistics.h"

DialogStatistics::DialogStatistics(QWidget *parent) : QDialog(parent)
{
	pWidget = parent;
	setModal(true);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
	int nWidth = 1920 / 2;
	int nHeight = 1080 / 2;
	if (parent != NULL) {
		//setGeometry(parent->x() + parent->width() / 2 - nWidth / 2, parent->y() + parent->height() / 2 - nHeight / 2, nWidth, nHeight);
		setGeometry(0 + 1920 / 2 - nWidth / 2, 0 + 1080 / 2 - nHeight / 2, nWidth, nHeight);
	}
	else {
		resize(nWidth, nHeight);
	}
	main_layout = new QHBoxLayout;
	main_layout->setMargin(5);
	this->setLayout(main_layout);



}

DialogStatistics::~DialogStatistics()
{
}

int DialogStatistics::exec()
{
	qDebug("BG");
	activateWindow();
	this->show();
	return 1;
}
void DialogStatistics::done(int val)
{
	qDebug("done.");

}

void DialogStatistics::accept()
{
	qDebug("Accept.");
	this->hide();
}
void DialogStatistics::reject()
{
	emit thread_kill();

	qDebug("Reject.");
	this->hide();
}


void DialogStatistics::generateStatistics(int labelIndex,QString name, std::list<unsigned int> *data_ptr) {
	TitleName = name;
	CellIndexList = data_ptr;
	
	QWidget *widget = new QWidget;
	QVBoxLayout * layout = new QVBoxLayout;
	widget->setLayout(layout);
	QFont title_font("Arial", 15, QFont::Bold);
	QFont feature_title_font("Arial", 13, QFont::Bold);
	QFont small_font("Arial", 10, QFont::Bold);
	QFont small_contents_font("Arial", 10, QFont::Normal);


	QLabel *titlelabel = new QLabel;
	titlelabel->setText(TitleName);
	titlelabel->setFont(title_font);
	layout->addWidget(titlelabel);

	QHBoxLayout * count_layout = new QHBoxLayout;
	QLabel *count_label_t = new QLabel;
	count_label_t->setText("Cell Count : ");
	count_label_t->setFont(small_font);
	QLineEdit *count_label_c = new QLineEdit;
	count_label_c->setText(QString::fromStdString(std::to_string(CellIndexList->size())));
	count_label_c->setFont(small_contents_font);

	QLabel *count_label_u = new QLabel;
	count_label_u->setText("cells");
	count_label_u->setAlignment(Qt::AlignCenter);
	count_label_u->setFixedSize(50, 30);
	count_label_u->setFont(small_font);

	count_layout->addWidget(count_label_t);
	count_layout->addWidget(count_label_c);
	count_layout->addWidget(count_label_u);
	layout->addLayout(count_layout);

	if (CellIndexList->size() > 0) {
		int startIndex = mGlobals.CurrentProject->mLayerCell[labelIndex]->featureStartIndex;
		int endIndex = mGlobals.CurrentProject->mLayerCell[labelIndex]->featureEndIndex;

		int index = 0;
//		std::vector<DataFeature>::iterator iter = mGlobals.CurrentProject->mFeature.begin();
//		for (iter; iter != mGlobals.CurrentProject->mFeature.end(); ++iter) {
		for (int i = startIndex; i < endIndex; i++) {
			statistic stat = getBasicStatistic(i);
			int type = mGlobals.CurrentProject->mFeature[i].FeatureType;
			QString unit_str;
			if (type == 0) {
				unit_str = "constant";
			}
			else if (mGlobals.CurrentProject->ResolutionUnitX == 3) {
				unit_str = "voxels";

			}
			else if (type == 1) {
				if (mGlobals.CurrentProject->ResolutionUnitX == 0) {
					unit_str = mGlobals.gUnitList[1];
				}
				else if (mGlobals.CurrentProject->ResolutionUnitX == 1) {
					unit_str = mGlobals.gUnitList[4];
				}
				else if (mGlobals.CurrentProject->ResolutionUnitX == 2) {
					unit_str = mGlobals.gUnitList[7];
				}				
			}
			else if (type == 2) {
				if (mGlobals.CurrentProject->ResolutionUnitX == 0) {
					unit_str = mGlobals.gUnitList[2];
				}
				else if (mGlobals.CurrentProject->ResolutionUnitX == 1) {
					unit_str = mGlobals.gUnitList[5];
				}
				else if (mGlobals.CurrentProject->ResolutionUnitX == 2) {
					unit_str = mGlobals.gUnitList[8];
				}
			}
			else if (type == 3) {
				if (mGlobals.CurrentProject->ResolutionUnitX == 0) {
					unit_str = mGlobals.gUnitList[3];
				}
				else if (mGlobals.CurrentProject->ResolutionUnitX == 1) {
					unit_str = mGlobals.gUnitList[6];
				}
				else if (mGlobals.CurrentProject->ResolutionUnitX == 2) {
					unit_str = mGlobals.gUnitList[9];
				}
			}

			QLabel *hline = new QLabel(this);
			hline->setFrameStyle(QFrame::HLine | QFrame::Plain);

			QVBoxLayout * feature_layout = new QVBoxLayout;
			QLabel *featurename_label = new QLabel;
			featurename_label->setText(QString::fromStdString(mGlobals.CurrentProject->mFeature[i].FeatureName));
			featurename_label->setFont(feature_title_font);

			QHBoxLayout * feature_min_layout = new QHBoxLayout;
			QLabel *feature_min_label_t = new QLabel;
			feature_min_label_t->setText("Minimum : ");
			feature_min_label_t->setFont(small_font);
			QLineEdit *feature_min_label_c = new QLineEdit;
			feature_min_label_c->setText(QString::fromStdString(std::to_string(stat.min)));
			feature_min_label_c->setFont(small_contents_font);
			QLabel *feature_min_label_u = new QLabel;
			feature_min_label_u->setText(unit_str);
			feature_min_label_u->setAlignment(Qt::AlignCenter);
			feature_min_label_u->setFixedSize(50, 30);
			feature_min_label_u->setFont(small_font);
			feature_min_layout->addWidget(feature_min_label_t);
			feature_min_layout->addWidget(feature_min_label_c);
			feature_min_layout->addWidget(feature_min_label_u);

			QHBoxLayout * feature_max_layout = new QHBoxLayout;
			QLabel *feature_max_label_t = new QLabel;
			feature_max_label_t->setText("Maximum : ");
			feature_max_label_t->setFont(small_font);
			QLineEdit *feature_max_label_c = new QLineEdit;
			feature_max_label_c->setText(QString::fromStdString(std::to_string(stat.max)));
			feature_max_label_c->setFont(small_contents_font);
			QLabel *feature_max_label_u = new QLabel;
			feature_max_label_u->setText(unit_str);
			feature_max_label_u->setAlignment(Qt::AlignCenter);
			feature_max_label_u->setFixedSize(50, 30);
			feature_max_label_u->setFont(small_font);
			feature_max_layout->addWidget(feature_max_label_t);
			feature_max_layout->addWidget(feature_max_label_c);
			feature_max_layout->addWidget(feature_max_label_u);

			QHBoxLayout * feature_mean_layout = new QHBoxLayout;
			QLabel *feature_mean_label_t = new QLabel;
			feature_mean_label_t->setText("Mean : ");
			feature_mean_label_t->setFont(small_font);
			QLineEdit *feature_mean_label_c = new QLineEdit;
			feature_mean_label_c->setText(QString::fromStdString(std::to_string(stat.mean)));
			feature_mean_label_c->setFont(small_contents_font);
			QLabel *feature_mean_label_u = new QLabel;
			feature_mean_label_u->setText(unit_str);
			feature_mean_label_u->setAlignment(Qt::AlignCenter);
			feature_mean_label_u->setFixedSize(50, 30);
			feature_mean_label_u->setFont(small_font);
			feature_mean_layout->addWidget(feature_mean_label_t);
			feature_mean_layout->addWidget(feature_mean_label_c);
			feature_mean_layout->addWidget(feature_mean_label_u);

			QHBoxLayout * feature_var_layout = new QHBoxLayout;
			QLabel *feature_var_label_t = new QLabel;
			feature_var_label_t->setText("Variance : ");
			feature_var_label_t->setFont(small_font);
			QLineEdit *feature_var_label_c = new QLineEdit;
			feature_var_label_c->setText(QString::fromStdString(std::to_string(stat.variance)));
			feature_var_label_c->setFont(small_contents_font);
			QLabel *feature_var_label_u = new QLabel;
			feature_var_label_u->setText("");
			feature_var_label_u->setAlignment(Qt::AlignCenter);
			feature_var_label_u->setFixedSize(50, 30);
			feature_var_label_u->setFont(small_font);
			feature_var_layout->addWidget(feature_var_label_t);
			feature_var_layout->addWidget(feature_var_label_c);
			feature_var_layout->addWidget(feature_var_label_u);

			QHBoxLayout * feature_sum_layout = new QHBoxLayout;
			QLabel *feature_sum_label_t = new QLabel;
			feature_sum_label_t->setText("Sum : ");
			feature_sum_label_t->setFont(small_font);
			QLineEdit *feature_sum_label_c = new QLineEdit;
			feature_sum_label_c->setText(QString::fromStdString(std::to_string(stat.sum)));
			feature_sum_label_c->setFont(small_contents_font);
			QLabel *feature_sum_label_u = new QLabel;
			feature_sum_label_u->setText(unit_str);
			feature_sum_label_u->setAlignment(Qt::AlignCenter);
			feature_sum_label_u->setFixedSize(50, 30);
			feature_sum_label_u ->setFont(small_font);
			feature_sum_layout->addWidget(feature_sum_label_t);
			feature_sum_layout->addWidget(feature_sum_label_c);
			feature_sum_layout->addWidget(feature_sum_label_u);

			QHBoxLayout * feature_five_layout = new QHBoxLayout;
			QLabel *feature_five_label_t = new QLabel;
			feature_five_label_t->setText("Five Number Summary (" + unit_str +")  : ");
			feature_five_label_t->setFont(small_font);
			feature_five_layout->addWidget(feature_five_label_t);
			for (int i = 0; i < 5; ++i) {
				QVBoxLayout * _1_layout = new QVBoxLayout;
				
				QLabel * _1_title = new QLabel;
				_1_title->setAlignment(Qt::AlignCenter);
				_1_title->setText(QString::number(i*25)+QString("%"));
				_1_title->setFont(small_font);
				QLineEdit * _1_data = new QLineEdit;
				_1_data->setText(QString::fromStdString(std::to_string(stat.five[i])));
				_1_data->setFont(small_contents_font);
				_1_layout->addWidget(_1_title);
				_1_layout->addWidget(_1_data);
				feature_five_layout->addLayout(_1_layout);
			}
			
			feature_layout->addWidget(hline);
			feature_layout->addWidget(featurename_label);
			feature_layout->addLayout(feature_min_layout);
			feature_layout->addLayout(feature_max_layout);
			feature_layout->addLayout(feature_mean_layout);
			feature_layout->addLayout(feature_var_layout);
			feature_layout->addLayout(feature_sum_layout);
			feature_layout->addLayout(feature_five_layout);
			layout->addLayout(feature_layout);
			index++;
		}
	}
	QLayoutItem* item;
	while ((item = main_layout->takeAt(0)) != NULL)
	{
		delete item->widget();
		delete item;
	}
	QScrollArea* scroll = new QScrollArea();
	scroll->setWidget(widget);

	main_layout->addWidget(scroll);
}

statistic DialogStatistics::getBasicStatistic(int findex) {
	statistic result;
	std::vector<double> for_five;
	std::list<unsigned int>::iterator iter = CellIndexList->begin();
	result.max = -999999999;
	result.min = 999999999;
	result.sum = 0;

	int type = mGlobals.CurrentProject->mFeature[findex].FeatureType;
	
	double multi_value;
	if (type == 0 || mGlobals.CurrentProject->ResolutionUnitX == 3) {
		multi_value = 1;
	}
	else if (type == 1) {
		multi_value = (mGlobals.CurrentProject->ResolutionX * mGlobals.CurrentProject->ResolutionY * mGlobals.CurrentProject->ResolutionZ) / 3;
	}
	else if (type == 2) {
		multi_value = (mGlobals.CurrentProject->ResolutionX * mGlobals.CurrentProject->ResolutionY);
	}
	else if (type == 3) {
		multi_value = (mGlobals.CurrentProject->ResolutionX * mGlobals.CurrentProject->ResolutionY * mGlobals.CurrentProject->ResolutionZ);
	}



	for (iter = CellIndexList->begin(); iter != CellIndexList->end(); ++iter) {
		double value = mGlobals.CurrentProject->mFeature[findex].FeatureMap[*iter] * multi_value;
		for_five.push_back(value);
		if (result.max <= value) { result.max = value; }
		if (result.min >= value) { result.min = value; }
		result.sum += value;
	}
	
	result.mean = result.sum / for_five.size();
	std::sort(for_five.begin(), for_five.end());
	result.five[0] = for_five[0];
	result.five[1] = for_five[for_five.size() / 4];
	result.five[2] = for_five[for_five.size()/2];
	result.five[3] = for_five[for_five.size() / 4 * 3];
	result.five[4] = for_five[for_five.size()-1];

	long double temp_sum = 0.0;
	for (iter = CellIndexList->begin(); iter != CellIndexList->end(); ++iter) {
		double value = mGlobals.CurrentProject->mFeature[findex].FeatureMap[*iter] * multi_value - result.mean;
		temp_sum += value * value;
	}
	result.variance = temp_sum / for_five.size();
	result.count = for_five.size();
	return result;
}
