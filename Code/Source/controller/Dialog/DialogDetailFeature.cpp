#include "DialogDetailFeature.h"



DialogDetailFeature::DialogDetailFeature(QWidget *parent) : QDialog(parent)
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


DialogDetailFeature::~DialogDetailFeature()
{
}



int DialogDetailFeature::exec()
{
	qDebug("BG");
	activateWindow();
	this->show();
	return 1;
}
void DialogDetailFeature::done(int val)
{
	qDebug("done.");

}

void DialogDetailFeature::accept()
{
	qDebug("Accept.");
	// here!!!!!
	this->hide();
}
void DialogDetailFeature::reject()
{
	emit thread_kill();

	qDebug("Reject.");
	this->hide();
}

void DialogDetailFeature::setIndexAndDraw(int index) {
	mIndex = index;
	
	int row_count = 0;
	int step = 0;
	std::vector<DataFeature>::iterator iter;
	for (iter = mGlobals.CurrentProject->mFeature.begin(); iter != mGlobals.CurrentProject->mFeature.end(); ++iter) {
		if (step == mIndex) {
			row_count = iter->FeatureMap.size();
			break;
		}
		step++;
	}

	step = 0;

	TableWidget = new QTableWidget(row_count, 2, this);
	TableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	TableWidget->setDragEnabled(true);
	TableWidget->setAcceptDrops(true);
	TableWidget->setDropIndicatorShown(true);

	QTableWidgetItem *indexHeaderItem = new QTableWidgetItem(tr("Index"));
	TableWidget->setHorizontalHeaderItem(0, indexHeaderItem);
	indexHeaderItem->setTextAlignment(Qt::AlignVCenter);
	
	QTableWidgetItem *valueHeaderItem = new QTableWidgetItem(tr("Value"));
	TableWidget->setHorizontalHeaderItem(1, valueHeaderItem);
	valueHeaderItem->setTextAlignment(Qt::AlignVCenter);
	
	TableWidget->setHorizontalHeaderItem(0, indexHeaderItem);
	TableWidget->setHorizontalHeaderItem(1, valueHeaderItem);



	for (iter = mGlobals.CurrentProject->mFeature.begin(); iter != mGlobals.CurrentProject->mFeature.end(); ++iter) {
		if (step == mIndex) {
			int row = 0;
			std::map<unsigned int, double>::iterator map_iter;
			for (map_iter = iter->FeatureMap.begin(); map_iter != iter->FeatureMap.end(); ++map_iter) {

				QTableWidgetItem *indexItem = new QTableWidgetItem(tr("%1").arg(map_iter->first));
				QTableWidgetItem *valueItem = new QTableWidgetItem(tr("%1").arg(map_iter->second));
				TableWidget->setItem(row, 0, indexItem);
				TableWidget->setItem(row, 1, valueItem);
				row++;
			}
			break;
		}
		step++;
	}


	QLayoutItem *item = main_layout->takeAt(0);
    delete item;

	main_layout->addWidget(TableWidget);

}
