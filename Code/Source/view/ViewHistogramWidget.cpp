#include "ViewHistogramWidget.h"
#include <algorithm>

ViewHistogramWidget::ViewHistogramWidget(QWidget *parent)
	: QWidget(parent)
{
	mWidget = parent;
	HistBoxLayout = new QVBoxLayout;
	HistBoxLayout->setMargin(0);
	mWidget->setLayout(HistBoxLayout);
}


ViewHistogramWidget::~ViewHistogramWidget()
{
}
void ViewHistogramWidget::setInterface_percent(QLineEdit *s, QLineEdit *e) {
	start_edit_percent = s;
	end_edit_percent = e;
}
void ViewHistogramWidget::setInterface_value(QLineEdit *s, QLineEdit *e) {
	start_edit_value = s;
	end_edit_value = e;
}
void ViewHistogramWidget::setInterface_unit(QLabel *s, QLabel *e) {
	start_edit_value_unit = s;
	end_edit_value_unit = e;
}


void ViewHistogramWidget::setRenderingSize(int width, int height) {
	mWidth = width;
	mHeight = height;
	HistogramData.resize(mWidth);
	this->setFixedHeight(mHeight);
	this->setFixedWidth(mWidth);
}
void ViewHistogramWidget::setData(int feature_index, std::list<unsigned int> *in_ptr, std::list<unsigned int> *out_ptr)
{
	mInput = in_ptr;
	mOutput = out_ptr;
	mDataMaximum = -999999999;
	mDataMinimum = 999999999;
	mFeatureIndex = feature_index;

	std::list<unsigned int>::iterator iter = mInput->begin();
	for (iter = mInput->begin(); iter != mInput->end(); ++iter) {
		if (mDataMaximum <= mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[*iter]) {
			mDataMaximum = mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[*iter];
		}
		if (mDataMinimum >= mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[*iter]) {
			mDataMinimum = mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[*iter];
		}
	}
	clearHistogramData();
	if (mDataMaximum == mDataMinimum) {
		mDataStep = 1;
	}
	else mDataStep = (mDataMaximum - mDataMinimum) / (mWidth-1);
	for (iter = mInput->begin(); iter != mInput->end(); ++iter) {
		double value = mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[*iter] - mDataMinimum;
		int index = value / mDataStep;
		//qDebug() << index;
		HistogramData[index].push_back(uuint(*iter,mFeatureIndex));
	}
	for (int i = 0; i < HistogramData.size(); i++) {
		sortCellbyFeature(i);
	}

	//qDebug() << "feature data setting finish";

	redraw();
}
void ViewHistogramWidget::sortCellbyFeature(int data_index) {
	std::sort(HistogramData[data_index].begin(), HistogramData[data_index].end());
}
bool uuint::operator<(uuint t) const {
	return mGlobals.CurrentProject->mFeature[index].FeatureMap[v] < mGlobals.CurrentProject->mFeature[index].FeatureMap[t.v];
}

void ViewHistogramWidget::clearHistogramData() {
	std::vector<std::vector<uuint>>::iterator temp = HistogramData.begin();
	for (temp = HistogramData.begin(); temp != HistogramData.end(); ++temp) {
		temp->clear();
	}
}
void ViewHistogramWidget::redraw() {
	mBarsets.clear();
	QBarSeries *series = new QBarSeries();
	for (int i = 0; i < mWidth; i++) {
		QBarSet *set = new QBarSet("");
		*set << log(HistogramData[i].size() + 1);
		set->setPen(Qt::NoPen);
		set->setColor(Qt::black);
		series->append(set);
		mBarsets.append(set);
	}
	
	series->setBarWidth(1);
	series->setUseOpenGL(true);

	Histogram *chart = new Histogram();
	chart->addSeries(series);

	chart->setMargins(QMargins(0, 0, 0, 0));
	chart->layout()->setContentsMargins(0, 0, 0, 0);
	chart->setBackgroundRoundness(0);
	chart->setBackgroundBrush(QBrush(QColor("#363636")));

	chart->setAnimationOptions(QChart::SeriesAnimations);

	chart->legend()->hide();
	//chart->createDefaultAxes(); 
	HistogramViewer *chartView = new HistogramViewer(chart);
	connect(chartView, SIGNAL(update_move_pos(double, double, int)), this, SLOT(update_move_box(double, double, int)));
	connect(chartView, SIGNAL(update_release_pos(double, double, int)), this, SLOT(update_release_box(double, double, int)));
	connect(chartView, SIGNAL(clear_mouse()), this, SLOT(clear_box()));

	chartView->setRenderHint(QPainter::Antialiasing);
	chartView->setFixedHeight(mHeight);
	chartView->setFixedWidth(mWidth);

	QLayoutItem *item = HistBoxLayout->takeAt(0);
	delete item;

	HistBoxLayout->addWidget(chartView);
}


void ViewHistogramWidget::clear_box() {
	foreach(QBarSet *item, mBarsets) {
		item->setColor(QColor("black"));
	}
	//emit focusedHist(mIndex);
}

void ViewHistogramWidget::update_release_box(double start, double end, int flag) {
	qDebug() << "update_release_box";



	int type = mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureType;
	QString unit_str;
	if (type == 0 || mGlobals.CurrentProject->ResolutionUnitX == 3) {
		unit_str = "v";
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



	if (flag == 0) {
		if (start < 0) start = 0;
		if (end > 1) end = 1.0;
		int total_cell_number=mInput->size();

		int start_cell_index = total_cell_number * start;
		int end_cell_index = total_cell_number * end;

		start_cell_value = 0;
		end_cell_value = 0;

		int start_idx = 0;
		int end_idx = 0;
		
		mOutput->clear();

		
		//for (std::vector<uuint>::iterator iter = HistogramData[10].begin(); iter != HistogramData[10].end(); ++iter) {
		//	qDebug()<<mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[(*iter).v];

		//}

		int sum = 0;
		bool start_flag = false;
		bool end_flag = false;
		for (int i = 0; i < HistogramData.size(); i++) {
			if (HistogramData[i].size() == 0) {
				end_idx++;
				if (start_flag == false)start_idx++;
				continue;
			}
			if (end_cell_index > sum + HistogramData[i].size()) {
				end_idx++;
			}
			else {
				std::vector<uuint>::iterator iter = HistogramData[i].begin();
				int tsum = sum;
				for (; iter != HistogramData[i].end(); ++iter) {
					if (start_flag == false) {
						if (tsum == start_cell_index) {
							start_cell_value = mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[(*iter).v];
							start_flag = true;
							mOutput->push_back((*iter).v);
						}

					}
					else if (tsum < end_cell_index)mOutput->push_back((*iter).v);
					tsum++;
					if (tsum == end_cell_index) {
						end_cell_value = mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[(*iter).v];
						break;
					}

				}
				end_flag = true;
				break;
			}

			if (start_cell_index >= sum + HistogramData[i].size()) {
				start_idx++;
			}
			else {
				if (start_flag == false) {
					start_flag = true;
					std::vector<uuint>::iterator iter = HistogramData[i].begin();
					int tsum = sum;
					for (; iter != HistogramData[i].end(); ++iter) {
						if(tsum>=start_cell_index)mOutput->push_back((*iter).v);
						if (tsum == start_cell_index)start_cell_value = mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[(*iter).v];
						tsum++;
					}
				}
				else{
					std::vector<uuint>::iterator iter = HistogramData[i].begin();
					for (; iter != HistogramData[i].end(); ++iter) {
						mOutput->push_back((*iter).v);
					}
				}
			}
			sum += HistogramData[i].size();
		}
	

		HistStartIdx = start_idx;
		HistEndIdx = end_idx;
		qDebug() << start_idx << end_idx;

		Utils mUtil;
		for (int i = 0; i < start_idx; ++i) {
			mBarsets.at(i)->setColor(QColor("black"));
		}
		for (int i = start_idx; i < end_idx; ++i) {
//			mBarsets.at(i)->setColor(QColor("#3A79A0"));
			mBarsets.at(i)->setColor(QColor("white"));
		}


		for (int i = end_idx; i < mBarsets.length(); ++i) {
			mBarsets.at(i)->setColor(QColor("black"));
		}



		//updateCellColor();
		//start_edit_percent->setText(QString::number((int)(start * 100)));
		//end_edit_percent->setText(QString::number((int)(end * 100)));


		start_edit_value_unit->setText(QString("Start(") + unit_str + QString(")"));
		end_edit_value_unit->setText(QString("End(") + unit_str + QString(")"));

		start_edit_value->setText(QString::number(start_cell_value*multi_value, 'f', 4));
		end_edit_value->setText(QString::number(end_cell_value*multi_value, 'f', 4));

		//double start_data = (double) *HistogramData[start_idx].begin();
		//double end_data = (double) *HistogramData[end_idx-1].end();

		//start_edit_value->setText(QString::number(start_data,'g', 4));
		//end_edit_value->setText(QString::number(end_data, 'g', 4));

		emit OutputUpdated();
	}
	else if (flag == 1) {
		start /= multi_value;
		end /= multi_value;
		if (start < mDataMinimum) start = mDataMinimum;
		if (end > mDataMaximum) end = mDataMaximum;

		int total_cell_number = mInput->size();

		int start_cell_index = 0;
		int end_cell_index = 0;

		start_cell_value = start-0.000001;
		end_cell_value = end + 0.000001;
		qDebug() << start_cell_value << end_cell_value;

		int start_idx = 0;
		int end_idx = 0;

		bool end_idx_flag = false;

		mOutput->clear();

		bool start_flag = false;

		int sum = 0;

		for (int i = 0; i < HistogramData.size(); i++) {
			if (HistogramData[i].size() == 0) {
				end_idx++;
				if (start_flag == false)start_idx++;
				continue;
			}
			std::vector<uuint>::iterator iter0 = HistogramData[i].end();
			--iter0;
			if (end_cell_value > mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[(*iter0).v]) {
				end_idx++;
			}
			else {
				int tsum = sum;
				std::vector<uuint>::iterator iter = HistogramData[i].begin();
				for (; iter != HistogramData[i].end(); ++iter) {
					if (start_flag == false) {
						if (start_cell_value < mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[(*iter).v]) {
							start_cell_index = tsum;
							start_flag = true;
							mOutput->push_back((*iter).v);
						}
					}
					else if (end_cell_value > mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[(*iter).v])mOutput->push_back((*iter).v);
					else {
						break;
					}
					tsum++;
				}
				end_cell_index = tsum;
				end_idx_flag = true;
				break;
			}
			if (start_cell_value > mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[(*iter0).v]) {
				start_idx++;
			}
			else {
				if (start_flag == false) {
					
					std::vector<uuint>::iterator iter = HistogramData[i].begin();
					int tsum = sum;
					for (; iter != HistogramData[i].end(); ++iter) {
						if (start_cell_value < mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[(*iter).v]) {
							if (start_flag == false) {
								start_cell_index = tsum;
								start_flag = true;
							}
							mOutput->push_back((*iter).v);
						}
						tsum++;
					}
				}
				else{
					std::vector<uuint>::iterator iter = HistogramData[i].begin();
					for (; iter != HistogramData[i].end(); ++iter) {
						mOutput->push_back((*iter).v);
					}
				}
			}
			sum += HistogramData[i].size();
		}
		if (end_idx_flag == false) {
			end_cell_index = total_cell_number;
		}

		qDebug() << start_idx << end_idx;

		HistStartIdx = start_idx;
		HistEndIdx = end_idx;

		Utils mUtil;
		for (int i = 0; i < start_idx; ++i) {
			mBarsets.at(i)->setColor(QColor("black"));
		}
		for (int i = start_idx; i < end_idx; ++i) {
//			mBarsets.at(i)->setColor(QColor("#3A79A0"));
			mBarsets.at(i)->setColor(QColor("white"));
		}


		for (int i = end_idx; i < mBarsets.length(); ++i) {
			mBarsets.at(i)->setColor(QColor("black"));
		}

		start_edit_percent->setText(QString::number(double(start_cell_index)/total_cell_number*100,'f',4));
		end_edit_percent->setText(QString::number(double(end_cell_index) / total_cell_number*100, 'f', 4));

		//start_edit_value->setText(QString::number(start, 'g', 4));
		//end_edit_value->setText(QString::number(end, 'g', 4));


		emit OutputUpdated();
	}
	else if (flag == 2) {
		if (start < 0) start = 0;
		if (end > 1) end = 1.0;
		int total_cell_number = mInput->size();

		int start_cell_index = 0;
		int end_cell_index = 0;

		start_cell_value = 0;
		end_cell_value = 0;

		int start_idx = (mBarsets.length()-1) * start;
		int end_idx = (mBarsets.length()-1) * end;

		mOutput->clear();

		while (HistogramData[start_idx].size() == 0) {
			start_idx++;
			if (start_idx >= mBarsets.length())break;
		}

		while (HistogramData[end_idx].size() == 0) {
			end_idx--;
			if (end_idx <0)break;
		}
		if (start_idx < mBarsets.length() && end_idx >=0) {
			int sum = 0;
			bool start_flag = false;
			for (int i = 0; i < HistogramData.size(); i++) {

				if (i == start_idx) {
					start_cell_index = sum + 1;
					start_cell_value = mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[HistogramData[i][0].v];
				}

				if (i == end_idx) {
						end_cell_index = sum + HistogramData[i].size();
						end_cell_value = mGlobals.CurrentProject->mFeature[mFeatureIndex].FeatureMap[HistogramData[i][HistogramData[i].size() - 1].v];
				}
				sum += HistogramData[i].size();
			}
			for (int i = 0; i < HistogramData.size(); i++) {
				if (i >= start_idx && i <= end_idx) {
					std::vector<uuint>::iterator iter = HistogramData[i].begin();
					for (; iter != HistogramData[i].end(); ++iter) {
						mOutput->push_back((*iter).v);
					}
				}
			}


			HistStartIdx = start_idx;
			HistEndIdx = end_idx;
			qDebug() << start_idx << end_idx;

			Utils mUtil;
			for (int i = 0; i < start_idx; ++i) {
				mBarsets.at(i)->setColor(QColor("black"));
			}
			for (int i = start_idx; i <= end_idx; ++i) {
//				mBarsets.at(i)->setColor(QColor("#3A79A0"));
				mBarsets.at(i)->setColor(QColor("white"));
			}


			for (int i = end_idx+1; i < mBarsets.length(); ++i) {
				mBarsets.at(i)->setColor(QColor("black"));
			}
		}
		else {
			for (int i = 0; i < mBarsets.length(); ++i) {
				mBarsets.at(i)->setColor(QColor("black"));
			}
		}




		start_edit_percent->setText(QString::number(double(start_cell_index) / total_cell_number * 100, 'f', 4));
		end_edit_percent->setText(QString::number(double(end_cell_index) / total_cell_number * 100, 'f', 4));

		start_edit_value_unit->setText(QString("Start(") + unit_str + QString(")"));
		end_edit_value_unit->setText(QString("End(") + unit_str + QString(")"));

		start_edit_value->setText(QString::number(start_cell_value*multi_value, 'f', 4));
		end_edit_value->setText(QString::number(end_cell_value*multi_value, 'f', 4));

		emit OutputUpdated();

	}

}


void ViewHistogramWidget::update_move_box(double start, double end, int flag) {
	qDebug() << "update_move_box";
	if (flag == 0) {
		if (start < 0) start = 0;
		if (end > 1) end = 1.0;

		int start_idx = (mBarsets.length()-1) * start;
		int end_idx = (mBarsets.length()-1) * end;

		Utils mUtil;
		for (int i = start_idx; i <= end_idx; ++i) {
			mBarsets.at(i)->setColor(QColor("#3A79A0"));
		}
	}
	else if (flag == 1) {
		if (start < mDataMinimum) start = mDataMinimum;
		if (end > mDataMaximum) end = mDataMaximum;

		int start_idx = (start - mDataMinimum) / mDataStep;
		int end_idx = (end - mDataMinimum) / mDataStep;

		HistStartIdx = start_idx;
		HistEndIdx = end_idx;
		
		Utils mUtil;
		for (int i = start_idx; i < end_idx; ++i) {
			mBarsets.at(i)->setColor(QColor("#3A79A0"));
		}
	}
	

}
//
