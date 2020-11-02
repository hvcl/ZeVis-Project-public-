#include "Contents_Annotations.h"



Contents_Annotations::Contents_Annotations(QWidget *parent)
	: QObject(parent)
{
	mWidget = parent;
	QVBoxLayout *layout = new QVBoxLayout;
	QFont title_font("Arial", 11, QFont::Bold);
	QLabel *title_label = new QLabel(mWidget);
	title_label->setFont(title_font);
	title_label->setText("Annotations");
	QWidget *filler1 = new QWidget;
	filler1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	onoff = new QCheckBox;
	onoff->setChecked(true);
	connect(onoff, SIGNAL(stateChanged(int)), this, SLOT(onoffAction(int)));
	QLabel *onoffLabel = new QLabel("on/off");

	QPushButton *m_button2 = new QPushButton;
	m_button2->setIcon(QIcon("Resources/icon_save.png"));
	m_button2->setIconSize(QSize(20, 20));
	m_button2->setFixedSize(QSize(30, 30));
	connect(m_button2, SIGNAL(released()), this, SLOT(handleSaveAnnotationProject()));

	QPushButton *m_button3 = new QPushButton;
	m_button3->setIcon(QIcon("Resources/icon_open.png"));
	m_button3->setIconSize(QSize(20, 20));
	m_button3->setFixedSize(QSize(30, 30));
	connect(m_button3, SIGNAL(released()), this, SLOT(handleLoadAnnotationProject()));


	QHBoxLayout *topLayout = new QHBoxLayout;
	topLayout->addWidget(title_label);
	topLayout->addWidget(filler1);
	topLayout->addWidget(onoff);
	topLayout->addWidget(onoffLabel);
	topLayout->addWidget(m_button2);
	topLayout->addWidget(m_button3);


	sortByTime = new QPushButton(QString("Time ")+QChar(31));
	sortByTime->setAutoFillBackground(true);
	QPalette palette = sortByTime->palette();
	palette.setColor(QPalette::Window, QColor(Qt::blue));
	sortByTime->setPalette(palette);

	sortByText = new QPushButton(QString("Text ")+QChar(31));
	sortByText->setAutoFillBackground(true);
	palette = sortByText->palette();
	palette.setColor(QPalette::Window, QColor(Qt::gray));
	sortByText->setPalette(palette);

	sortByColor = new QPushButton(QString("Color ")+QChar(31));
	sortByColor->setAutoFillBackground(true);
	palette = sortByColor->palette();
	palette.setColor(QPalette::Window, QColor(Qt::gray));
	sortByColor->setPalette(palette);

	connect(sortByTime, SIGNAL(pressed()), this, SLOT(sortByTimeAction()));
	connect(sortByText, SIGNAL(pressed()), this, SLOT(sortByTextAction()));
	connect(sortByColor, SIGNAL(pressed()), this, SLOT(sortByColorAction()));


	QHBoxLayout *sortLayout = new QHBoxLayout;

	sortLayout->addWidget(sortByTime);
	sortLayout->addWidget(sortByText);
	sortLayout->addWidget(sortByColor);

	list = new QListWidget;       
	list->setMouseTracking(true);
	list->setFocusPolicy(Qt::ClickFocus);
	connect(list, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(focusItem(QListWidgetItem*)));

	layout->addLayout(topLayout);
	layout->addLayout(sortLayout);
	layout->addWidget(list);

	mWidget->setLayout(layout);

}

Contents_Annotations::~Contents_Annotations()
{
}
void Contents_Annotations::handleSaveAnnotationProject() {
	QString filters("Annotation file (*.annotation);;");
	QString dir = QFileDialog::getSaveFileName(mWidget, "Save Annotation", QDir::currentPath(), filters);
	if (dir == "")return;
	FILE *f = fopen(dir.toStdString().c_str(), "w");
	fprintf(f, "%d\n", annotations.size());
	for (int i = 0; i < annotations.size(); i++) {
		fprintf(f, "%s ||%s ||color: %d %d %d||min: %f %f %f||max: %f %f %f||axis: %d\n",
			annotations[i]->generatedTime->text().toStdString().c_str(), annotations[i]->mention->text().toStdString().c_str(),
			annotations[i]->color.red(), annotations[i]->color.green(), annotations[i]->color.blue(),
			annotations[i]->startPos.x(), annotations[i]->startPos.y(), annotations[i]->startPos.z(),
			annotations[i]->endPos.x(), annotations[i]->endPos.y(), annotations[i]->endPos.z(),
			annotations[i]->axis);
	}
	fclose(f);
}
void Contents_Annotations::handleLoadAnnotationProject() {
	QString filters("Annotation file (*.annotation);;");
	QString dir = QFileDialog::getOpenFileName(mWidget, "Load Annotation", QDir::currentPath(), filters);
	if (dir == "")return;
	ifstream inf(dir.toStdString().c_str());
//	FILE *f = fopen(dir.toStdString().c_str(), "r");
	int len;
	inf >> len;
//	fscanf(f, "%d", &len);
	char line[500];
	inf.getline(line, 500);

	Utils mUtil;
	for (int i = 0; i < len; i++) {
		inf.getline(line, 500);
		std::vector<std::string> temp_array = mUtil.Split(line, "||");
		std::vector<std::string> temp_rgb = mUtil.Split(temp_array[2].c_str(), " ");
		std::vector<std::string> temp_min = mUtil.Split(temp_array[3].c_str(), " ");
		std::vector<std::string> temp_max = mUtil.Split(temp_array[4].c_str(), " ");
		std::vector<std::string> temp_axis = mUtil.Split(temp_array[5].c_str(), " ");

		addAnnotation(QString(temp_array[1].c_str())
			, QColor(QString(temp_rgb[1].c_str()).toInt(), QString(temp_rgb[2].c_str()).toInt(), QString(temp_rgb[3].c_str()).toInt(),100)
			, QVector3D(QString(temp_min[1].c_str()).toFloat(), QString(temp_min[2].c_str()).toFloat(), QString(temp_min[3].c_str()).toFloat())
			, QVector3D(QString(temp_max[1].c_str()).toFloat(), QString(temp_max[2].c_str()).toFloat(), QString(temp_max[3].c_str()).toFloat())
			, QString(temp_axis[1].c_str()).toInt());
		annotations[annotations.size() - 1]->generatedTime->setText(QString(temp_array[0].c_str()));

	}
	inf.close();
		//&t_time, &t_mention, &t1, &t_r, &t_g, &t_b, &t2, &t_minx, &t_miny, &t_minz, &t3, &t_maxx, &t_maxy, &t_maxz, &t4, &t_axis);
	
	mWidget->update();


}
void Contents_Annotations::onoffAction(int a) {
	mGlobals.mainWindow->mContents->updateAllWidget(0, 0);
}
void Contents_Annotations::sortByTimeAction() {
	QString t=sortByTime->text();
	if (t[t.length() - 1] == QChar(31)) {
		sortByTime->setText(QString("Time ") + QChar(30));
		for (int i = 0; i < annotations.size(); i++) {
			for (int j = i + 1; j < annotations.size(); j++) {
				if (annotations[i]->time < annotations[j]->time) {
					annotations[i]->swap(annotations[j]);
				}
			}
		}
	}
	else {
		sortByTime->setText(QString("Time ") + QChar(31));
		for (int i = 0; i < annotations.size(); i++) {
			for (int j = i+1; j < annotations.size(); j++) {
				if (annotations[i]->time > annotations[j]->time) {
					annotations[i]->swap(annotations[j]);
				}
			}
		}
	}



	QPalette palette = sortByTime->palette();
	palette.setColor(QPalette::Window, QColor(Qt::blue));
	sortByTime->setPalette(palette);

	palette = sortByText->palette();
	palette.setColor(QPalette::Window, QColor(Qt::gray));
	sortByText->setPalette(palette);

	palette = sortByColor->palette();
	palette.setColor(QPalette::Window, QColor(Qt::gray));
	sortByColor->setPalette(palette);
	mWidget->update();

}
void Contents_Annotations::sortByTextAction() {
	QString t = sortByText->text();
	if (t[t.length() - 1] == QChar(31)) {
		sortByText->setText(QString("Text ") + QChar(30));
		for (int i = 0; i < annotations.size(); i++) {
			for (int j = i + 1; j < annotations.size(); j++) {
				if (annotations[i]->mention->text() < annotations[j]->mention->text()) {
					annotations[i]->swap(annotations[j]);
				}
			}
		}
	}
	else {
		sortByText->setText(QString("Text ") + QChar(31));
		for (int i = 0; i < annotations.size(); i++) {
			for (int j = i + 1; j < annotations.size(); j++) {
				if (annotations[i]->mention->text() > annotations[j]->mention->text()) {
					annotations[i]->swap(annotations[j]);
				}
			}
		}
	}



	QPalette palette = sortByTime->palette();
	palette.setColor(QPalette::Window, QColor(Qt::gray));
	sortByTime->setPalette(palette);

	palette = sortByText->palette();
	palette.setColor(QPalette::Window, QColor(Qt::blue));
	sortByText->setPalette(palette);

	palette = sortByColor->palette();
	palette.setColor(QPalette::Window, QColor(Qt::gray));
	sortByColor->setPalette(palette);
	mWidget->update();
}

void Contents_Annotations::sortByColorAction() {
	QString t = sortByColor->text();
	if (t[t.length() - 1] == QChar(31)) {
		sortByColor->setText(QString("Color ") + QChar(30));
		for (int i = 0; i < annotations.size(); i++) {
			for (int j = i + 1; j < annotations.size(); j++) {
				if (annotations[i]->color.name() < annotations[j]->color.name()) {
					annotations[i]->swap(annotations[j]);
				}
			}
		}
	}
	else {
		sortByColor->setText(QString("Color ") + QChar(31));
		for (int i = 0; i < annotations.size(); i++) {
			for (int j = i + 1; j < annotations.size(); j++) {
				if (annotations[i]->color.name() > annotations[j]->color.name()) {
					annotations[i]->swap(annotations[j]);
				}
			}
		}
	}



	QPalette palette = sortByTime->palette();
	palette.setColor(QPalette::Window, QColor(Qt::gray));
	sortByTime->setPalette(palette);

	palette = sortByText->palette();
	palette.setColor(QPalette::Window, QColor(Qt::gray));
	sortByText->setPalette(palette);

	palette = sortByColor->palette();
	palette.setColor(QPalette::Window, QColor(Qt::blue));
	sortByColor->setPalette(palette);
	mWidget->update();
}

void Contents_Annotations::addAnnotation(QString m, QColor c, QVector3D sPos, QVector3D ePos, int a) {
	int index=annotations.size();
	ImageAnnotation *newAnnotation=new ImageAnnotation(m, c, index, sPos, ePos, a);
	
	newAnnotation->mention->installEventFilter(this);
	newAnnotation->mention->setMouseTracking(true);
	newAnnotation->time = globalTimeIndex++;
	connect(newAnnotation->remove, SIGNAL(pressed()), this,SLOT( removeAnnotation()));
	connect(newAnnotation->check, SIGNAL(pressed()), this, SLOT(checkAnnotation()));
	connect(newAnnotation->colorChange, SIGNAL(pressed()), this, SLOT(colorChangeAnnotation()));
	connect(this, SIGNAL(update_view_state(int, int, int, float)), mGlobals.mainWindow->mTopInterface, SLOT(update_value(int, int, int, float)));

	connect(newAnnotation->mention, SIGNAL(textEdit(QString)), this, SLOT(focusItem(QString)));
	connect(newAnnotation->check, SIGNAL(pressed()), this, SLOT(focusItem()));
	connect(newAnnotation->colorChange, SIGNAL(pressed()), this, SLOT(focusItem()));


	QListWidgetItem *item = new QListWidgetItem();
	annotations.push_back(newAnnotation);
	item->setSizeHint(annotations[annotations.size()-1]->widget->sizeHint());
	list->addItem(item);
	list->setItemWidget(item, annotations[annotations.size() - 1]->widget);
	
	focusItem(item);
	mGlobals.mainWindow->mContents->contents_left_tabwidget->setCurrentIndex(2);
	mWidget->update();


}
bool Contents_Annotations::eventFilter(QObject* object, QEvent* event) {
	for (int i = 0; i < annotations.size(); i++) {
		if (annotations[i]->mention == object) {
			if (event->type() == QEvent::FocusIn) {
				QListWidgetItem* item = list->item(i);
				item->setSelected(true);
				list->setCurrentItem(item, QItemSelectionModel::Select);
				list->repaint();
				break;
			}
		}
	}
	return false;
}
void Contents_Annotations::focusItem(QListWidgetItem* item) {
	qDebug() << "Item entered";
	item->setSelected(true);
	list->setCurrentItem(item, QItemSelectionModel::Select);
	list->repaint();
}
void Contents_Annotations::focusItem(QString a) {
	QObject *senderObj = sender(); // This will give Sender object
	int index = senderObj->objectName().toInt();
	QListWidgetItem* item = list->item(index);
	item->setSelected(true);
	list->setCurrentItem(item, QItemSelectionModel::Select);
	list->repaint();
}
void Contents_Annotations::focusItem(int index) {
	QListWidgetItem* item = list->item(index);
	item->setSelected(true);
	list->setCurrentItem(item, QItemSelectionModel::Select);
	list->repaint();
}
void Contents_Annotations::focusItem() {
	QObject *senderObj = sender(); // This will give Sender object
	int index = senderObj->objectName().toInt();
	QListWidgetItem* item = list->item(index);
	item->setSelected(true);
	list->setCurrentItem(item, QItemSelectionModel::Select);
	list->repaint();
}
void Contents_Annotations::checkAnnotation() {
	QObject *senderObj = sender(); // This will give Sender object
	int index = senderObj->objectName().toInt();

	mGlobals.CurrentProject->ViewPos_X = (annotations[index]->startPos.x() + annotations[index]->endPos.x())*0.5;
	mGlobals.CurrentProject->ViewPos_Y = (annotations[index]->startPos.y() + annotations[index]->endPos.y())*0.5;
	mGlobals.CurrentProject->ViewPos_Z = (annotations[index]->startPos.z() + annotations[index]->endPos.z())*0.5;
	emit update_view_state(mGlobals.CurrentProject->ViewPos_X, mGlobals.CurrentProject->ViewPos_Y, mGlobals.CurrentProject->ViewPos_Z, mGlobals.CurrentProject->ViewZoomLevel);
	mGlobals.mainWindow->mContents->contents_center_tabwidget->setCurrentIndex(annotations[index]->axis);
	mGlobals.mainWindow->mContents->updateAllWidget(0, 0);

}
void Contents_Annotations::colorChangeAnnotation() {
	QObject *senderObj = sender(); // This will give Sender object
	int index = senderObj->objectName().toInt();

	QColorDialogTester color_test;
	QString color_str = color_test.onColor();
	qDebug() << color_str;

	if (color_str.length() > 2) {
		annotations[index]->color = QColor(color_str);
		annotations[index]->color.setAlpha(100);
		annotations[index]->colorChange->setStyleSheet(QString("background-color: %1;foreground-color: %1; border-style: none;").arg(annotations[index]->color.name()));

		mGlobals.mainWindow->mContents->updateAllWidget(0, 0);
	}


}
void Contents_Annotations::removeAnnotation() {
	QObject *senderObj = sender(); // This will give Sender object
	int index = senderObj->objectName().toInt();
	QListWidgetItem *item = list->item(index);
	list->removeItemWidget(item);
	delete item;

	for (int i = index+1; i < annotations.size(); i++) {
		annotations[i]->setIndex(i - 1);
	}
	annotations.remove(index);
	mGlobals.mainWindow->mContents->updateAllWidget(0, 0);

}

void Contents_Annotations::updateList() {

}