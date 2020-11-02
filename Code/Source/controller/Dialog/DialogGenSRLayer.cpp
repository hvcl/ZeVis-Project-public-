#include "DialogGenSRLayer.h"

DialogGenSRLayer::DialogGenSRLayer(QWidget *parent) :QDialog(parent)
{
	pWidget = parent;
	setModal(true);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
	int nWidth = 800;
	int nHeight = 400;
	if (parent != NULL)
		setGeometry(parent->x() + parent->width() / 2 - nWidth / 2,
			parent->y() + parent->height() / 2 - nHeight / 2,
			nWidth, nHeight);
	else
		resize(nWidth, nHeight);
}

DialogGenSRLayer::~DialogGenSRLayer()
{
}


void DialogGenSRLayer::find() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
}

int DialogGenSRLayer::exec()
{
	qDebug("SR");
	activateWindow();

	this->show();
	return 1;
}
void DialogGenSRLayer::done(int val)
{
	qDebug("done.");
}

void DialogGenSRLayer::accept()
{
	qDebug("Accept.");

	this->hide();
}
void DialogGenSRLayer::reject()
{
	qDebug("Reject.");
	this->hide();
}