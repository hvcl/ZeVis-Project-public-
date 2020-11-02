#pragma once

#include <QObject>
#include <QPushButton>
#include <QtWidgets>
#include <QIcon>
#include <QVector>

#include "Dialog/DialogManager.h"
class DialogManager;

class ImageAnnotation{
public:
	QLabel *generatedTime=NULL;
	QLineEdit *mention=NULL;
	QPushButton *check=NULL;
	QPushButton *colorChange=NULL;
	QPushButton *remove=NULL;
	QColor color;
	QWidget *widget=NULL;
	QVector3D startPos;
	QVector3D endPos;
	int axis;
	int time;
	QHBoxLayout *layout;
	QDateTime dataTime;

//	ImageAnnotation();
	ImageAnnotation(QString m,QColor c,int index,QVector3D sPos,QVector3D ePos,int a) {
		QFont title_font("Arial", 11, QFont::Bold);
		generatedTime = new QLabel;
		dataTime = QDateTime::currentDateTime();
		generatedTime->setText(dataTime.toString());
		mention = new QLineEdit();
		mention->setText(m);
		mention->setFont(title_font);
		color = c;
		startPos = sPos;
		endPos = ePos;
		axis = a;

		check = new QPushButton;
		check->setObjectName(QString::number(index));
		check->setIcon(QIcon("Resources/icon_check.png"));
		check->setIconSize(QSize(15, 15));
		check->setFixedSize(QSize(20, 20));

		colorChange = new QPushButton;
		colorChange->setObjectName(QString::number(index));
		colorChange->setFixedSize(QSize(15, 15));
		colorChange->setFlat(true);
		colorChange->setAutoFillBackground(true);
		colorChange->setStyleSheet(QString("background-color: %1;foreground-color: %1; border-style: none;").arg(color.name()));
		//connect(&colorChange, SIGNAL(released()), this, SLOT(handleColorBtn()));

		remove=new QPushButton;
		remove->setObjectName(QString::number(index));
		remove->setIcon(QIcon("Resources/icon_trash.png"));
		remove->setIconSize(QSize(15, 15));
		remove->setFixedSize(QSize(20, 20));
		//connect(&remove, SIGNAL(clicked()), this, SLOT(handleDeleteBtn()));

		layout = new QHBoxLayout;
		layout->addWidget(generatedTime);
		layout->addWidget(mention);
		layout->addWidget(check);
		layout->addWidget(colorChange);
		layout->addWidget(remove);

		widget = new QWidget;
		widget->setLayout(layout);
	}

	void setIndex(int index) {
		check->setObjectName(QString::number(index));
		colorChange->setObjectName(QString::number(index));
		remove->setObjectName(QString::number(index));
	}
	void swap(ImageAnnotation *a) {
		QLabel *t_generatedTime = generatedTime;
		QLineEdit *t_mention = mention;
		QPushButton *t_check = check;
		QPushButton *t_colorChange = colorChange;
		QPushButton *t_remove = remove;
		QColor t_color=color;
		QVector3D t_startPos=startPos;
		QVector3D t_endPos=endPos;
		int t_axis=axis;
		int t_time=time;
		QDateTime t_dataTime = dataTime;

		generatedTime = a->generatedTime;
		mention = a->mention;
		check = a->check;
		colorChange = a->colorChange;
		remove = a->remove;
		color = a->color;
		startPos = a->startPos;
		endPos = a->endPos;
		axis = a->axis;
		time = a->time;
		dataTime = a->dataTime;

		a->generatedTime = t_generatedTime;
		a->mention = t_mention;
		a->check = t_check;
		a->colorChange = t_colorChange;
		a->remove = t_remove;
		a->color = t_color;
		a->startPos = t_startPos;
		a->endPos = t_endPos;
		a->axis = t_axis;
		a->time = t_time;
		a->dataTime = t_dataTime;

		layout->addWidget(generatedTime);
		layout->addWidget(mention);
		layout->addWidget(check);
		layout->addWidget(colorChange);
		layout->addWidget(remove);
		widget->setLayout(layout);

		a->layout->addWidget(a->generatedTime);
		a->layout->addWidget(a->mention);
		a->layout->addWidget(a->check);
		a->layout->addWidget(a->colorChange);
		a->layout->addWidget(a->remove);
		a->widget->setLayout(a->layout);

		//widget->setLayout(a->layout);
		//a->widget->setLayout(layout);

		int t_index = check->objectName().toInt();
		setIndex(a->check->objectName().toInt());
		a->setIndex(t_index);
	}
};


class Contents_Annotations : public QObject
{
	Q_OBJECT

public:
	Contents_Annotations(QWidget *parent);
	~Contents_Annotations();
	
//	QVector<annotation> annotations;
	QCheckBox *onoff;
	QListWidget *list;
	QVector<ImageAnnotation*> annotations;
	void updateList();
	QPushButton *sortByTime;
	QPushButton *sortByText;
	QPushButton *sortByColor;

	int globalTimeIndex = 0;

public slots:
	void handleColorBtn() {
		QColorDialogTester color_test;
		//color = QColor(color_test.onColor());
	}
	void removeAnnotation();
	void addAnnotation(QString m, QColor c,  QVector3D sPos, QVector3D ePos, int a);
	void checkAnnotation();
	void colorChangeAnnotation();
	void focusItem(QListWidgetItem* item);
	void focusItem(QString a);
	void focusItem();
	void focusItem(int index);

	void sortByTimeAction();
	void sortByTextAction();
	void sortByColorAction();
	void onoffAction(int a);

signals:
	void update_view_state(int x, int y, int z, float zoom);

public slots:
	void handleSaveAnnotationProject();
	void handleLoadAnnotationProject();
private:
	QWidget * mWidget;

protected:
	bool eventFilter(QObject* object, QEvent* event);

};
