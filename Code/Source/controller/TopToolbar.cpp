#include "TopToolbar.h"

TopToolbar::TopToolbar(QWidget *parent)
	: QObject(parent)
{
	mWidget = parent;
}
TopToolbar::TopToolbar(QWidget *parent, QLayout *parent_layout)
	: QObject(parent)
{
	mWidget = parent;
	QPushButton *m_button1 = new QPushButton;
	m_button1->setIcon(QIcon("Resources/icon_new.png"));
	m_button1->setIconSize(QSize(20, 20));
	m_button1->setFixedSize(QSize(30, 30));
	connect(m_button1, SIGNAL(released()), this, SLOT(handleNewProject()));

	//QPushButton *m_button2 = new QPushButton;
	//m_button2->setIcon(QIcon("Resources/icon_save.png"));
	//m_button2->setIconSize(QSize(20, 20));
	//m_button2->setFixedSize(QSize(30, 30));
	//connect(m_button2, SIGNAL(released()), this, SLOT(handleSaveProject()));

	QPushButton *m_button3 = new QPushButton;
	m_button3->setIcon(QIcon("Resources/icon_open.png"));
	m_button3->setIconSize(QSize(20, 20));
	m_button3->setFixedSize(QSize(30, 30));
	connect(m_button3, SIGNAL(released()), this, SLOT(handleOpenProject()));

	QPushButton *m_button4 = new QPushButton;
	m_button4->setIcon(QIcon("Resources/icon_addnode.png"));
	m_button4->setIconSize(QSize(20, 20));
	m_button4->setFixedSize(QSize(30, 30));
	connect(m_button4, SIGNAL(released()), this, SLOT(handleAddNode()));



	QPushButton *m_button5 = new QPushButton;
//	m_button5->setIcon(QIcon("Resources/icon_saveState2.png"));
	m_button5->setIcon(QIcon("Resources/icon_save.png"));
	m_button5->setIconSize(QSize(20, 20));
	m_button5->setFixedSize(QSize(30, 30));
	connect(m_button5, SIGNAL(released()), this, SLOT(handleSaveStateProject()));


	QPushButton *m_button6 = new QPushButton;
//	m_button6->setIcon(QIcon("Resources/icon_loadState2.png"));
	m_button6->setIcon(QIcon("Resources/icon_open.png"));
	m_button6->setIconSize(QSize(20, 20));
	m_button6->setFixedSize(QSize(30, 30));
	connect(m_button6, SIGNAL(released()), this, SLOT(handleLoadStateProject()));



	QPushButton *m_button7 = new QPushButton;
	m_button7->setIcon(QIcon("Resources/group.png"));
	m_button7->setIconSize(QSize(40, 40));
	m_button7->setFixedSize(QSize(30, 30));
	connect(m_button7, SIGNAL(released()), this, SLOT(handleLoadStateProject()));
	//QPushButton *m_button5 = new QPushButton("M", mWidget);
	//m_button5->setFixedSize(QSize(30, 30));
	//connect(m_button5, SIGNAL(released()), mWidget, SLOT(handleButton()));

	//QPushButton *m_button6 = new QPushButton("M", mWidget);
	//m_button6->setFixedSize(QSize(30, 30));
	//connect(m_button6, SIGNAL(released()), mWidget, SLOT(handleButton()));


	curState = new QLabel;
	curState->setStyleSheet(QString("QLabel {font: bold; color : rgb(105,160,255);}"));
	curState->setFixedSize(200, 30);

	QSpacerItem *spacerItem = new QSpacerItem(50, 30, QSizePolicy::Fixed);
	
	parent_layout->addWidget(m_button1);
//	parent_layout->addWidget(m_button2);
	parent_layout->addWidget(m_button5);
	parent_layout->addWidget(m_button6);

//	parent_layout->addWidget(m_button3);
//	parent_layout->addWidget(m_button4);

	parent_layout->addItem(spacerItem);
//	parent_layout->addWidget(m_button3);

//	parent_layout->addWidget(m_button7);

	QSpacerItem *spacerItem2 = new QSpacerItem(50, 30, QSizePolicy::Fixed);


	parent_layout->addItem(spacerItem2);
	parent_layout->addWidget(curState);
}

TopToolbar::~TopToolbar()
{

}

//void TopToolbar::setDialogManager(DialogManager *ptr) {
//	mDialogManager = ptr;
//}


void TopToolbar::handleNewProject() {
	qDebug("New Project handle");
	mGlobals.CurrentProject->NewProject();

//	mGlobals.mDialogManager->mDialogNewProject->exec();

}

void TopToolbar::handleSaveProject() {
	qDebug("Save Project handle");
	if (mGlobals.CurrentProject->ProjectStatus) {
		mGlobals.CurrentProject->ProjectSave(mGlobals.CurrentProject->ProjectPath);
	}
	else {
		QMessageBox::critical(0, "Error", "Project save error");
	}
	/*DialogNewProject *md = new DialogNewProject(mWidget);
	md->exec();*/
}
void TopToolbar::handleOpenProject() {
	qDebug("Open Project handle");
	if (!mGlobals.CurrentProject->ProjectStatus) {

		QString filters("Background Layer file (*.aproj);;");
		QString dir = QFileDialog::getOpenFileName(mWidget, "Open Directory", QDir::currentPath(), filters);
		if (dir == "") {
			return;
		}
		qDebug() << "Open dir";
		qDebug() << dir;

		curState->setText("Loading Project...");
		curState->update();
		curState->repaint();

		mGlobals.CurrentProject->ProjectOpen(dir);
		//mGlobals.mainWindow->mContents->loadSubregionFeature();
		curState->setText("");

	}
	/*DialogNewProject *md = new DialogNewProject(mWidget);
	md->exec();*/
}
void TopToolbar::handleAddNode() {
	qDebug("Open Add node handle");
	mGlobals.mDialogManager->mDialogAddNode->exec();
}
void TopToolbar::handleSaveStateProject() {
	qDebug("Save As Project handle");

	if (!mGlobals.CurrentProject->ProjectStatus) {
		QMessageBox::critical(0, "Error", "Project save error");
		return;
	}
	QString dir = "";
	if (mGlobals.CurrentProject->ProjectPath != "") {
		dir = mGlobals.CurrentProject->ProjectPath;
	}
	else {

		QString filters("Project file (*.ZeVis);;");
		dir = QFileDialog::getSaveFileName(mWidget, "Open Directory", QDir::currentPath(), filters);
	}
	if (dir == "")return;
	qDebug() << dir;
//	ofstream f(dir.toStdString());
	f.open(dir.toStdString());
	if (!f.is_open())return;

	mGlobals.CurrentProject->ProjectPath = dir;
	mGlobals.mainWindow->mContents->contents_projectinfo_master->ProjectPath->setText(dir);


	if (mGlobals.CurrentProject->ProjectName == "") {
		f << "Blank" << endl;
	}
	else f << mGlobals.CurrentProject->ProjectName.toStdString() << endl;
	QString localPath = "";
	int slashCount = 0;
	//for (int i = mGlobals.CurrentProject->ProjectPath.size()-1; i >= 0; i--) {
	//	if (mGlobals.CurrentProject->ProjectPath.at(i) == QChar('/')) {
	//		slashCount++;
	//	}
	//	if (slashCount == 1)break;

	//	localPath.push_front(mGlobals.CurrentProject->ProjectPath.at(i));
	//}
	//f << localPath.toStdString()<< endl;

//	localPath = "";
//	slashCount = 0;
	//for (int i = mGlobals.CurrentProject->mLayerBack->BackgroundHeaderPath.size() - 1; i >= 0; i--) {
	//	if (mGlobals.CurrentProject->mLayerBack->BackgroundHeaderPath[i] == '/') {
	//		slashCount++;
	//	}
	//	if (slashCount == 2)break;

	//	localPath.push_front(mGlobals.CurrentProject->mLayerBack->BackgroundHeaderPath[i]);
	//}
//	f << localPath.toStdString() << endl;
	f << mGlobals.CurrentProject->mLayerBack->BackgroundHeaderPath << endl;

	//localPath = "";
	//slashCount = 0;
	//for (int i = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->CellHeaderPath.size() - 1; i >= 0; i--) {
	//	if (mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->CellHeaderPath[i] == '/') {
	//		slashCount++;
	//	}
	//	if (slashCount == 2)break;

	//	localPath.push_front(mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->CellHeaderPath[i]);
	//}
	//f << localPath.toStdString() << endl;

	f << mGlobals.CurrentProject->maxLayerCell << endl;
	for (int i = 0; i < mGlobals.CurrentProject->maxLayerCell; i++) {
		f << mGlobals.CurrentProject->mLayerCell[i]->CellHeaderPath << endl;
	}

	//f << mGlobals.CurrentProject->ResolutionX << endl;
	//f << mGlobals.CurrentProject->ResolutionY << endl;
	//f << mGlobals.CurrentProject->ResolutionZ << endl;

	//f << mGlobals.CurrentProject->ResolutionUnitX << endl;
	//f << mGlobals.CurrentProject->ResolutionUnitY << endl;
	//f << mGlobals.CurrentProject->ResolutionUnitZ << endl;

	f << mGlobals.mainWindow->mContents->contents_projectinfo_master->Resolution_Unit->currentIndex() << endl;

	f << mGlobals.CurrentProject->ViewPos_X << endl;
	f << mGlobals.CurrentProject->ViewPos_Y << endl;
	f << mGlobals.CurrentProject->ViewPos_Z << endl;
	f << 1.0/mGlobals.CurrentProject->ViewZoomLevel << endl;

	f << mGlobals.CurrentProject->VisualizeMethod_Index << endl;
	f << mGlobals.CurrentProject->CellOpacity << endl;


	f << mGlobals.CurrentProject->mSubregion.size() << endl;
	for (int k = 0; k < mGlobals.CurrentProject->mSubregion.size(); k++) {

		//localPath = "";
		//slashCount = 0;
		//std::string t_path = mGlobals.CurrentProject->mSubregion[k].SubregionPath + "header.srl";
		//for (int i = t_path.size() - 1; i >= 0; i--) {
		//	if (t_path[i] == '/') {
		//		slashCount++;
		//	}
		//	if (slashCount == 3)break;

		//	localPath.push_front(t_path[i]);
		//}
		//f << localPath.toStdString() << endl;

		f << mGlobals.CurrentProject->mSubregion[k].SubregionHeaderPath << endl;
		f << mGlobals.CurrentProject->mSubregion[k].SubregionColor.red() << endl;
		f << mGlobals.CurrentProject->mSubregion[k].SubregionColor.green() << endl;
		f << mGlobals.CurrentProject->mSubregion[k].SubregionColor.blue() << endl;
		f << mGlobals.CurrentProject->mSubregion[k].SubregionOpacity << endl;
	}

	//f << mGlobals.CurrentProject->mFeature.size() << endl;
	//for (int k = 0; k < mGlobals.CurrentProject->mFeature.size(); k++) {
	//	localPath = "";
	//	slashCount = 0;
	//	std::string t_path = mGlobals.CurrentProject->mFeature[k].FeaturePath;
	//	for (int i = t_path.size() - 1; i >= 0; i--) {
	//		if (t_path[i] == '/') {
	//			slashCount++;
	//		}
	//		if (slashCount == 3)break;

	//		localPath.push_front(t_path[i]);
	//	}
	//	f << localPath.toStdString() << endl;

	//	f << mGlobals.CurrentProject->mFeature[k].FeatureType << endl;
	//}

	foreach(QGraphicsItem *ptr, mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->items()) {
		if (ptr->type() == QNEBlock::Type) {
			QNEBlock *temp = (QNEBlock *)ptr;
			if (temp->mBlockFlags == QNEBlock::OriginBlock) {
				f << temp->mBlockFlags << endl;
				QString ttext = temp->mBlock->blockName_edit->text();
				for (int it = 0; it < ttext.size(); it++) {
					if (ttext[it] == ' ')ttext[it] = '_';
				}
				if (ttext == "")f << "Blank" << endl;
				else f << ttext.toStdString() << endl;
				f << temp->pos().x() << endl;
				f << temp->pos().y() << endl;
				f << temp->labelIndex << endl;

			}
			else if (temp->mBlockFlags == QNEBlock::DataBlock) {
				f << temp->mBlockFlags << endl;
				QString ttext = temp->mBlock->blockName_edit->text();
				for (int it = 0; it < ttext.size(); it++) {
					if (ttext[it] == ' ')ttext[it] = '_';
				}
				if (ttext == "")f << "Blank" << endl;
				else f << ttext.toStdString() << endl;
				f << temp->pos().x() << endl;
				f << temp->pos().y() << endl;
			}
			else if (temp->mBlockFlags == QNEBlock::SubregionBlock) {
				f << temp->mBlockFlags << endl;
				QString ttext = temp->mBlock->blockName_edit->text();
				for (int it = 0; it < ttext.size(); it++) {
					if (ttext[it] == ' ')ttext[it] = '_';
				}
				if (ttext == "")f << "Blank" << endl;
				else f << ttext.toStdString() << endl;
				f << temp->pos().x() << endl;
				f << temp->pos().y() << endl;
			}
			else if (temp->mBlockFlags == QNEBlock::CutPlaneBlock) {
				f << temp->mBlockFlags << endl;
				QString ttext = temp->mBlock->blockName_edit->text();
				for (int it = 0; it < ttext.size(); it++) {
					if (ttext[it] == ' ')ttext[it] = '_';
				}
				if (ttext == "")f << "Blank" << endl;
				else f << ttext.toStdString() << endl;
				f << temp->pos().x() << endl;
				f << temp->pos().y() << endl;
			}
			else if (temp->mBlockFlags == QNEBlock::FeatureBlock) {
				f << temp->mBlockFlags << endl;
				QString ttext = temp->mBlock->blockName_edit->text();
				for (int it = 0; it < ttext.size(); it++) {
					if (ttext[it] == ' ')ttext[it] = '_';
				}
				if (ttext == "")f << "Blank" << endl;
				else f << ttext.toStdString() << endl;
				f << temp->pos().x() << endl;
				f << temp->pos().y() << endl;
			}
			else if (temp->mBlockFlags == QNEBlock::SetBlock) {
				f << temp->mBlockFlags << endl;
				QString ttext = temp->mBlock->blockName_edit->text();
				for (int it = 0; it < ttext.size(); it++) {
					if (ttext[it] == ' ')ttext[it] = '_';
				}
				if (ttext == "")f << "Blank" << endl;
				else f << ttext.toStdString() << endl;
				f << temp->pos().x() << endl;
				f << temp->pos().y() << endl;
			}
			else if (temp->mBlockFlags == QNEBlock::StatisticBlock) {
				f << temp->mBlockFlags << endl;
				QString ttext = temp->mBlock->blockName_edit->text();
				for (int it = 0; it < ttext.size(); it++) {
					if (ttext[it] == ' ')ttext[it] = '_';
				}
				if (ttext == "")f << "Blank" << endl;
				else f << ttext.toStdString() << endl;
				f << temp->pos().x() << endl;
				f << temp->pos().y() << endl;
			}

		}
	}
	f << "Block_Info_Finish" << endl;


	foreach(QGraphicsItem *ptr, mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->items()) {
		
		if (ptr->type() == QNEConnection::Type) {
			QNEConnection *cur = (QNEConnection *)ptr;
			f << cur->pos1.x() << endl;
			f << cur->pos1.y() << endl;

			f << cur->pos2.x() << endl;
			f << cur->pos2.y() << endl;

		}
	}	
	f << "Connection_Info_Finish" << endl;


	int itemSize=mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->items().size();
	//for (int i = itemSize-1; i >=0; i--) {
	//	QGraphicsItem *ptr=mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->items().at(i);
	////}
	////foreach(QGraphicsItem *ptr, mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->items()) {
	//	if (ptr->type() == QNEBlock::Type) {
	//		QNEBlock *temp = (QNEBlock *)ptr;
	//		if (temp->mBlockFlags == QNEBlock::OriginBlock || temp->mBlockFlags == QNEBlock::DataBlock) {
	//			f << temp->mBlockFlags << endl;
	//			f << temp->pos().x() << endl;
	//			f << temp->pos().y() << endl;
	//			f << temp->mBlock->DataColor.red() << endl;
	//			f << temp->mBlock->DataColor.green() << endl;
	//			f << temp->mBlock->DataColor.blue() << endl;
	//		}
	//		else if (temp->mBlockFlags == QNEBlock::SubregionBlock) {
	//			f << temp->mBlockFlags << endl;
	//			f << temp->pos().x() << endl;
	//			f << temp->pos().y() << endl;
	//			f << temp->mBlock->SubregionDropdown->currentIndex() << endl;
	//			f << temp->mBlock->SubregionSelectMethod->currentIndex() << endl;
	//			__int64 t_volume = temp->mBlock->SubregionVolume;
	//			f << QString::number(temp->mBlock->SubregionVolume,'f',10).toStdString() << endl;
	//			std::list<unsigned int>::iterator iter = temp->mBlock->CellIndexListOutput.begin();
	//			for (; iter != temp->mBlock->CellIndexListOutput.end(); ++iter) {
	//				f << *iter <<endl;
	//			}
	//			f << "-1" << endl;
	//		}
	//		else if (temp->mBlockFlags == QNEBlock::CutPlaneBlock) {
	//			f << temp->mBlockFlags << endl;
	//			f << temp->pos().x() << endl;
	//			f << temp->pos().y() << endl;
	//			f << temp->mBlock->subregion_pos_x->text().toStdString() << endl;
	//			f << temp->mBlock->subregion_pos_y->text().toStdString() << endl;
	//			f << temp->mBlock->subregion_pos_z->text().toStdString() << endl;
	//			f << temp->mBlock->subregion_up_x->text().toStdString() << endl;
	//			f << temp->mBlock->subregion_up_y->text().toStdString() << endl;
	//			f << temp->mBlock->subregion_up_z->text().toStdString() << endl;
	//			f << temp->mBlock->SubregionSelectMethod->currentIndex() << endl;
	//			__int64 t_volume = temp->mBlock->SubregionVolume;
	//			f << QString::number(temp->mBlock->SubregionVolume, 'f', 10).toStdString() << endl;
	//			std::list<unsigned int>::iterator iter = temp->mBlock->CellIndexListOutput.begin();
	//			for (; iter != temp->mBlock->CellIndexListOutput.end(); ++iter) {
	//				f << *iter << endl;
	//			}
	//			f << "-1" << endl;
	//		}
	//		else if (temp->mBlockFlags == QNEBlock::FeatureBlock) {
	//			f << temp->mBlockFlags << endl;
	//			f << temp->pos().x() << endl;
	//			f << temp->pos().y() << endl;
	//			f << temp->mBlock->FeatureDropdown->currentIndex() << endl;
	//			f << temp->mBlock->histogram_start_value->text().toStdString() << endl;
	//			f << temp->mBlock->histogram_end_value->text().toStdString() << endl;

	//		}
	//		else if (temp->mBlockFlags == QNEBlock::SetBlock) {
	//			f << temp->mBlockFlags << endl;
	//			f << temp->pos().x() << endl;
	//			f << temp->pos().y() << endl;
	//			f << temp->mBlock->SetDropdown->currentIndex() << endl;

	//		}
	//		else if (temp->mBlockFlags == QNEBlock::StatisticBlock) {
	//			f << temp->mBlockFlags << endl;
	//			f << temp->pos().x() << endl;
	//			f << temp->pos().y() << endl;
	//			f << temp->mBlock->FeatureDropdown->currentIndex() << endl;

	//		}
	//	}
	//}

	for (int i = itemSize - 1; i >= 0; i--) {
		QGraphicsItem *ptr = mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->items().at(i);
		if (ptr->type() == QNEBlock::Type) {
			QNEBlock *temp = (QNEBlock *)ptr;
			if (temp->mBlockFlags == QNEBlock::OriginBlock) {
				graphTraverse(temp);
			}
		}
	}

	f << "Block_Setting_Finish" << endl;

	QVector<ImageAnnotation*> annotations=mGlobals.mainWindow->mContents->annotationWidget->annotations;
	f << annotations.size() <<endl;
	for (int i = 0; i < annotations.size(); i++) {
		f << annotations[i]->generatedTime->text().toStdString() 
			<< " ||" << annotations[i]->mention->text().toStdString()
			<< " ||color: " << annotations[i]->color.red() << " " << annotations[i]->color.green() << " " << annotations[i]->color.blue()
			<< "||min: " << annotations[i]->startPos.x() << " " << annotations[i]->startPos.y() << " " << annotations[i]->startPos.z()
			<< "||max: " << annotations[i]->endPos.x() << " " << annotations[i]->endPos.y() << " " << annotations[i]->endPos.z()
			<< "||axis: " << annotations[i]->axis << endl;
	}
	f << "Annotation_Finish" << endl;

	f.close();

	QMessageBox msg;
	msg.setText("Complete save project");
	msg.exec();

}

void TopToolbar::graphTraverse(QNEBlock *block) {
	if (block->mBlockFlags == QNEBlock::OriginBlock || block->mBlockFlags == QNEBlock::DataBlock) {
		f << block->mBlockFlags << endl;
		f << block->pos().x() << endl;
		f << block->pos().y() << endl;
		f << block->mBlock->DataColor.red() << endl;
		f << block->mBlock->DataColor.green() << endl;
		f << block->mBlock->DataColor.blue() << endl;
	}
	else if (block->mBlockFlags == QNEBlock::SubregionBlock) {
		f << block->mBlockFlags << endl;
		f << block->pos().x() << endl;
		f << block->pos().y() << endl;
		f << block->mBlock->SubregionDropdown->currentIndex() << endl;
		f << block->mBlock->SubregionSelectMethod->currentIndex() << endl;
		__int64 t_volume = block->mBlock->SubregionVolume;
		f << QString::number(block->mBlock->SubregionVolume, 'f', 10).toStdString() << endl;
		std::list<unsigned int>::iterator iter = block->mBlock->CellIndexListOutput.begin();
		for (; iter != block->mBlock->CellIndexListOutput.end(); ++iter) {
			f << *iter << endl;
		}
		f << "-1" << endl;
	}
	else if (block->mBlockFlags == QNEBlock::CutPlaneBlock) {
		f << block->mBlockFlags << endl;
		f << block->pos().x() << endl;
		f << block->pos().y() << endl;
		f << block->mBlock->subregion_pos_x->text().toStdString() << endl;
		f << block->mBlock->subregion_pos_y->text().toStdString() << endl;
		f << block->mBlock->subregion_pos_z->text().toStdString() << endl;
		f << block->mBlock->subregion_up_x->text().toStdString() << endl;
		f << block->mBlock->subregion_up_y->text().toStdString() << endl;
		f << block->mBlock->subregion_up_z->text().toStdString() << endl;
		f << block->mBlock->SubregionSelectMethod->currentIndex() << endl;
		__int64 t_volume = block->mBlock->SubregionVolume;
		f << QString::number(block->mBlock->SubregionVolume, 'f', 10).toStdString() << endl;
		std::list<unsigned int>::iterator iter = block->mBlock->CellIndexListOutput.begin();
		for (; iter != block->mBlock->CellIndexListOutput.end(); ++iter) {
			f << *iter << endl;
		}
		f << "-1" << endl;
	}
	else if (block->mBlockFlags == QNEBlock::FeatureBlock) {
		f << block->mBlockFlags << endl;
		f << block->pos().x() << endl;
		f << block->pos().y() << endl;
		f << block->mBlock->FeatureDropdown->currentIndex() << endl;
		f << block->mBlock->histogram_start_value->text().toStdString() << endl;
		f << block->mBlock->histogram_end_value->text().toStdString() << endl;

	}
	else if (block->mBlockFlags == QNEBlock::SetBlock) {
		f << block->mBlockFlags << endl;
		f << block->pos().x() << endl;
		f << block->pos().y() << endl;
		f << block->mBlock->SetDropdown->currentIndex() << endl;

	}
	else if (block->mBlockFlags == QNEBlock::StatisticBlock) {
		f << block->mBlockFlags << endl;
		f << block->pos().x() << endl;
		f << block->pos().y() << endl;
		f << block->mBlock->FeatureDropdown->currentIndex() << endl;

	}

	foreach(QNEPort *port, block->ports()) {
		if (port->portAlign() == QNEPort::Output) {
			foreach(QNEConnection *conn, port->connections()) {
				QNEBlock *next = conn->port2()->m_block;
				graphTraverse(next);
			}
		}
	}
}

void TopToolbar::handleLoadStateProject() {
	qDebug("Load Project handle");

	QString filters("Project file (*.ZeVis);;");

	QString dir = QFileDialog::getOpenFileName(mWidget, "Open Directory", QDir::currentPath(), filters);

	if (dir == "") {
		QMessageBox msg;
		msg.setText("Failed to load project");
		msg.exec();
		return;
	}


	ifstream f(dir.toStdString());
	if (!f.is_open()) {
		QMessageBox msg;
		msg.setText("Failed to load project");
		msg.exec();
		return;
	}



	//QString baseDir = QFileDialog::getExistingDirectory(mWidget, "Select Base Directory", QDir::currentPath());

	//
	//if (baseDir == "") {
	//	return;
	//}
	//if (baseDir[baseDir.length() - 1] != '/') {
	//	baseDir += '/';
	//}
	//qDebug() << "Base Directory: "<<baseDir;


	mGlobals.CurrentProject->ProjectPath = dir;
	mGlobals.mainWindow->mContents->contents_projectinfo_master->ProjectPath->setText(dir);


	curState->setText("Loading...");
	curState->update();
	curState->repaint();

	if (!mGlobals.mainWindow->project_started) {
		handleNewProject();
	}

	string i;
	getline(f,i);
	if (i == "Blank") {
		mGlobals.CurrentProject->ProjectName = "";
	}
	else {
		mGlobals.CurrentProject->ProjectName = QString(i.c_str());
	}
	mGlobals.mainWindow->mContents->contents_projectinfo_master->ProjectName->setText(QString(i.c_str()));
	qDebug() << mGlobals.CurrentProject->ProjectName;

//	getline(f, i);

//	mGlobals.CurrentProject->ProjectPath = baseDir + QString(i.c_str());

	getline(f, i);
	QFileInfo check_file(i.c_str());
	if (!check_file.exists()) {
		QString filters("EM header file (*.bgl);;");
		char tt[200];
		sprintf(tt, "Find EM file (%s)", i.c_str());
		i = QFileDialog::getOpenFileName(mWidget, tt, QDir::currentPath(), filters).toStdString();
	}
	QFileInfo check_file2(i.c_str());
	if (!check_file2.exists()) {
		QMessageBox msg;
		msg.setText("Failed to find EM file");
		msg.exec();
		curState->setText("");
		return;
	}
	qDebug() << i.c_str();
	qDebug() << mGlobals.CurrentProject->ProjectStatus;
	if (mGlobals.CurrentProject->ProjectStatus) {
		delete mGlobals.CurrentProject->mLayerBack;
	}
	mGlobals.mainWindow->mContents->contents_projectinfo_master->setLayerBackground(QString(i.c_str()));
//	mGlobals.CurrentProject->mLayerBack = new LayerBackground(i);
	qDebug() << "load em";


	int i_int;
	getline(f, i);
	i_int = QString(i.c_str()).toInt();
	qDebug() << i_int;

	for (int n = mGlobals.CurrentProject->maxLayerCell - 1; n >= 0; n--) {
		delete mGlobals.CurrentProject->mLayerCell[n];
	}
	mGlobals.mainWindow->mContents->labelList->clear();
	mGlobals.CurrentProject->maxLayerCell = 0;

	mGlobals.CurrentProject->mFeature.clear();


	for (int n=0; n < i_int; n++) {
		getline(f, i);
		QFileInfo check_file(i.c_str());
		if (!check_file.exists()) {
			char tt[200];
			sprintf(tt, "Find Label file (%s)", i.c_str());

			QString filters("Label header file (*.lbl);;");
			i = QFileDialog::getOpenFileName(mWidget, tt, QDir::currentPath(), filters).toStdString();
		}
		QFileInfo check_file2(i.c_str());
		if (!check_file2.exists()) {
			QMessageBox msg;
			msg.setText("Failed to find label file");
			msg.exec();
			curState->setText("");
			return;
		}

		qDebug() << i.c_str();
		mGlobals.mainWindow->mContents->setLabel(QString(i.c_str()));
		mGlobals.CurrentProject->curLayerCell = 0;

		//mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell] = new LayerCell(baseDir.toStdString() + i);

		//mGlobals.CurrentProject->ProjectSizeLoad();

		//mGlobals.CurrentProject->ProjectStatus = true;
	}

	qDebug() << "load label";

	double i_double;
	//f >> i_double;
	//mGlobals.CurrentProject->ResolutionX = i_double;
	//f >> i_double;
	//mGlobals.CurrentProject->ResolutionY = i_double;
	//f >> i_double;
	//mGlobals.CurrentProject->ResolutionZ = i_double;

	//f >> i_int;
	//mGlobals.CurrentProject->ResolutionUnitX = i_int;
	//f >> i_int;
	//mGlobals.CurrentProject->ResolutionUnitY = i_int;
	//f >> i_int;
	//mGlobals.CurrentProject->ResolutionUnitZ = i_int;

	//emit loadProjectInfo();

	f >> i_int;
	mGlobals.mainWindow->mContents->contents_projectinfo_master->Resolution_Unit->setCurrentIndex(i_int);
	mGlobals.mainWindow->mContents->contents_projectinfo_master->unit_change(i_int);


	f >> i;
	mGlobals.mainWindow->mTopInterface->x_value_edit->setText(QString(i.c_str()));
	f >> i;
	mGlobals.mainWindow->mTopInterface->y_value_edit->setText(QString(i.c_str()));
	f >> i;
	mGlobals.mainWindow->mTopInterface->z_value_edit->setText(QString(i.c_str()));
	f >> i;
	mGlobals.mainWindow->mTopInterface->level_value_edit->setText(QString(i.c_str()));

	mGlobals.CurrentProject->ViewPos_X = mGlobals.mainWindow->mTopInterface->x_value_edit->text().toFloat();
	mGlobals.CurrentProject->ViewPos_Y = mGlobals.mainWindow->mTopInterface->y_value_edit->text().toFloat();
	mGlobals.CurrentProject->ViewPos_Z = mGlobals.mainWindow->mTopInterface->z_value_edit->text().toFloat();

	mGlobals.CurrentProject->ViewZoomLevel = 2 - log2(mGlobals.mainWindow->mTopInterface->level_value_edit->text().toFloat());

	f >> i_int;
	mGlobals.mainWindow->mTopInterface->rendering_menu->setCurrentIndex(i_int);
	mGlobals.CurrentProject->VisualizeMethod_Index = i_int;

	f >> i_double;
	mGlobals.mainWindow->mTopInterface->cell_opacity->setValue(i_double * 100);
	mGlobals.CurrentProject->CellOpacity = i_double;

	emit mGlobals.mainWindow->mTopInterface->update_view(true);

	qDebug() << "load info";


	mGlobals.CurrentProject->mSubregion.clear();
	f >> i_int;
	for (int k = 0; k < i_int; k++) {
		f >> i;
		int i_r, i_g, i_b;
		int ind;
		f >> i_r;
		f >> i_g;
		f >> i_b;
		f >> i_double;

		QFileInfo check_file(i.c_str());
		if (!check_file.exists()) {
			char tt[200];
			sprintf(tt, "Find subregion file (%s)", i.c_str());

			QString filters("Subregion header file (*.srl);;");
			i = QFileDialog::getOpenFileName(mWidget, tt, QDir::currentPath(), filters).toStdString();
		}
		QFileInfo check_file2(i.c_str());
		if (!check_file2.exists()) {
			QMessageBox msg;
			msg.setText("Failed to find subregion file");
			msg.exec();
			curState->setText("");
			return;
		}


		ind=mGlobals.CurrentProject->AddSubregion(QString(i.c_str()));
		mGlobals.CurrentProject->mSubregion[ind].SubregionColor = QColor(i_r,i_g,i_b);
		mGlobals.CurrentProject->mSubregion[ind].SubregionOpacity = i_double;
		emit mGlobals.mDialogManager->mDialogAddSubregion->updatedSubregion();

		mGlobals.mainWindow->mContents->updateAllWidget(0,false);
	}
	qDebug() << "load subregion";

	//mGlobals.CurrentProject->mFeature.clear();
	//f >> i_int;
	//for (int k = 0; k < i_int; k++) {
	//	f >> i;
	//	int ind;
	//	f >> ind;
	//	mGlobals.CurrentProject->AddFeature(baseDir + QString(i.c_str()), ind);
	//	emit mGlobals.mDialogManager->mDialogAddFeature->updatedFeature();

	//}

	if (mGlobals.mainWindow->mContents->GraphEditorView != NULL) {
		mGlobals.mainWindow->mContents->graphEditorLayout->removeWidget(mGlobals.mainWindow->mContents->GraphEditorView->GraphView);
		mGlobals.mainWindow->mContents->GraphEditorView->GraphView->hide();
		delete mGlobals.mainWindow->mContents->GraphEditorView;
		mGlobals.mainWindow->mContents->GraphEditorView = new ViewGraphEditor(mGlobals.mainWindow->mContents->mWidget);
		mGlobals.mainWindow->mContents->GraphEditorView->initialize(mGlobals.mainWindow->mContents->graphEditorLayout);
		//mGlobals.mainWindow->mContents->overlapedLayout->addLayout(mGlobals.mainWindow->mContents->graphEditorLayout, 0, 0);
		//mGlobals.mainWindow->mContents->overlapedLayout->addLayout(mGlobals.mainWindow->mContents->buttonLayout, 0, 0);
		mGlobals.mainWindow->mContents->b_icon_add->raise();

		connect(mGlobals.mDialogManager->mDialogAddNode, SIGNAL(makenode(int, QString)), mGlobals.mainWindow->mContents->GraphEditorView, SLOT(addNode(int, QString)));

		connect(mGlobals.mDialogManager->mDialogAddSubregion, SIGNAL(updatedSubregion()), mGlobals.mainWindow->mContents->GraphEditorView, SLOT(updateSubregionList()));
		connect(mGlobals.mDialogManager->mDialogAddFeature, SIGNAL(updatedFeature()), mGlobals.mainWindow->mContents->GraphEditorView, SLOT(updateFeatureList()));


	}


//	mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->clear();

	//delete mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor;
	//mGlobals.mainWindow->mContents->GraphEditorView->GraphView->setScene(NULL);
	//delete mGlobals.mainWindow->mContents->GraphEditorView->GraphScene;
	//mGlobals.mainWindow->mContents->GraphEditorView->GraphScene = new QGraphicsScene();
	//mGlobals.mainWindow->mContents->GraphEditorView->GraphView->setScene(mGlobals.mainWindow->mContents->GraphEditorView->GraphScene);

	//mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor = new QNodesEditor(mGlobals.mainWindow->mContents->GraphEditorView);
	//mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->install(mGlobals.mainWindow->mContents->GraphEditorView->GraphScene, 
	//	mGlobals.mainWindow->mContents->GraphEditorView->GraphView);


	while (1) {
		f >> i;
		if (i == "Block_Info_Finish")break;
		string i_name;
		float i_x, i_y;
		int flag = stoi(i);
		f >> i_name;
		f >> i_x;
		f >> i_y;
		if (i_name == "Blank")i_name = "";

		if (flag == QNEBlock::OriginBlock) {
			int label_index;
			f >> label_index;
			mGlobals.CurrentProject->curLayerCell = label_index;
			mGlobals.mainWindow->mContents->GraphEditorView->addOrigin(i_name, i_x, i_y);

			//foreach(QGraphicsItem *ptr, mGlobals.mainWindow->mContents->GraphEditorView->GraphScene->items()) {
			//	if (ptr->type() == QNEBlock::Type) {
			//		QNEBlock *temp = (QNEBlock *)ptr;
			//		if (temp->mBlock->BlockName == "Origin") {
			//			temp->setPos(i_x, i_y);
			//			break;
			//		}
			//	}
			//}
		}
		else if (flag == QNEBlock::DataBlock) {
			mGlobals.mainWindow->mContents->GraphEditorView->addDataBucket(i_name,i_x,i_y);
		}
		else if (flag == QNEBlock::SubregionBlock) {
			mGlobals.mainWindow->mContents->GraphEditorView->addSubregion(i_name, i_x, i_y);
		}
		else if (flag == QNEBlock::CutPlaneBlock) {
			mGlobals.mainWindow->mContents->GraphEditorView->addCutPlane(i_name, i_x, i_y);
		}
		else if (flag == QNEBlock::FeatureBlock) {
			mGlobals.mainWindow->mContents->GraphEditorView->addFeature(i_name, i_x, i_y);

		}
		else if (flag == QNEBlock::SetBlock) {
			mGlobals.mainWindow->mContents->GraphEditorView->addSet(i_name, i_x, i_y);
		}
		else if (flag == QNEBlock::StatisticBlock) {
			mGlobals.mainWindow->mContents->GraphEditorView->addStatistic(i_name, i_x, i_y);
		}
	}
	qDebug() << "block info finish";

	while (1) {
		f >> i;
		if (i == "Connection_Info_Finish")break;

		float p1x, p1y, p2x, p2y;
		p1x = stof(i);
		f >> p1y;
		f >> p2x;
		f >> p2y;

		QGraphicsItem *first = mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->itemAt(QPointF(p1x, p1y));
		QGraphicsItem *second = mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->itemAt(QPointF(p2x, p2y));
		qDebug() << (first==NULL) << (second==NULL);
		if (first == NULL || second == NULL)continue;
		QNEPort *port1 = (QNEPort*)first;
		QNEPort *port2 = (QNEPort*)second;

		mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn = new QNEConnection(0);
		mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->scene->addItem(mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn);
		mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn->setPort1(port1);
		mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn->setPort2(port2);

		mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn->setPos1(first->scenePos());
		mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn->setPos2(second->scenePos());
		mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn->updatePath();



		//if (port1->block() != port2->block() && port1->isOutput() != port2->isOutput() && !port1->isConnected(port2))
		//{



		if (port1->isOutput()) {
			port2->clearConnection();
			if (port2->portFlags() == QNEPort::SetSubInputPort) {
				port2->m_block->labelIndex2 = port1->m_block->labelIndex;
				port2->m_block->updateInput_newconnect_sub(&port1->m_block->mBlock->CellIndexListOutput);
			}
			else {
				port2->m_block->labelIndex = port1->m_block->labelIndex;
				port2->m_block->updateInput_newconnect(&port1->m_block->mBlock->CellIndexListOutput);
			}

		}
		else {
			port1->clearConnection();
			if (port1->portFlags() == QNEPort::SetSubInputPort) {
				port1->m_block->labelIndex2 = port2->m_block->labelIndex;
				port1->m_block->updateInput_newconnect_sub(&port2->m_block->mBlock->CellIndexListOutput);
			}
			else {
				port1->m_block->labelIndex = port2->m_block->labelIndex;
				port1->m_block->updateInput_newconnect(&port2->m_block->mBlock->CellIndexListOutput);
			}

		}
		mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn->updatePath();
		mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn->connected = true;
		mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn = 0;
		//}
		//else {
		//	delete mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn;
		//	mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->conn = 0;
		//}




	}
	qDebug() << "connection finish";

	while (1) {
		f >> i;
		qDebug() << i.c_str();
		if (i == "Block_Setting_Finish")break;
		string i_name;
		float i_x, i_y, i_z;
		int flag = stoi(i);
		f >> i_x;
		f >> i_y;
		QNEBlock * temp = (QNEBlock *)mGlobals.mainWindow->mContents->GraphEditorView->GraphEditor->itemAt(QPointF(i_x, i_y));
		if (flag == QNEBlock::OriginBlock || flag == QNEBlock::DataBlock) {
			int r, g, b;
			f >> r;
			f >> g;
			f >> b;
			temp->mBlock->DataColor.setRgb(r, g, b);
			temp->mBlock->BucketColorBtn->setStyleSheet(QString("background-color: %1;foreground-color: %1; border-style: none;").arg(temp->mBlock->DataColor.name()));
//			temp->mBlock->DataHeatmap->updateDataColor(temp->mBlock->DataColor);
			temp->mBlock->DataHeatmap->DataColor = temp->mBlock->DataColor;
		}
		else if (flag == QNEBlock::SubregionBlock) {
			

			int setInd;
			f >> setInd;
			temp->mBlock->SubregionDropdown->setCurrentIndex(setInd);
			f >> setInd;
			temp->mBlock->SubregionSelectMethod->setCurrentIndex(setInd);
			double volume;
			f >> volume;
			qDebug() << volume;
			temp->mBlock->SubregionVolume = volume;
			temp->mBlock->SubregionPrecision = 4;

			temp->mBlock->CellIndexListOutput.clear();
			while(1){
				unsigned int i_output;
				f >> i_output;
				if (i_output == -1)break;
				temp->mBlock->CellIndexListOutput.push_back(i_output);
			}
			temp->mBlock->handleSubregionLoading();
		}
		else if (flag == QNEBlock::CutPlaneBlock) {

			f >> i_x;
			f >> i_y;
			f >> i_z;
			temp->mBlock->subregion_pos_x->setText(QString::number(i_x));
			temp->mBlock->subregion_pos_y->setText(QString::number(i_y));
			temp->mBlock->subregion_pos_z->setText(QString::number(i_z));
			f >> i_x;
			f >> i_y;
			f >> i_z;
			temp->mBlock->subregion_up_x->setText(QString::number(i_x));
			temp->mBlock->subregion_up_y->setText(QString::number(i_y));
			temp->mBlock->subregion_up_z->setText(QString::number(i_z));

			int setInd;
			f >> setInd;
			temp->mBlock->SubregionSelectMethod->setCurrentIndex(setInd);
			double volume;
			f >> volume;
			qDebug() << volume;
			temp->mBlock->SubregionVolume = volume;
			temp->mBlock->SubregionPrecision = 4;

			temp->mBlock->CellIndexListOutput.clear();
			while (1) {
				unsigned int i_output;
				f >> i_output;
				if (i_output == -1)break;
				temp->mBlock->CellIndexListOutput.push_back(i_output);
			}
			temp->mBlock->handleSubregionLoading();
		}
		else if (flag == QNEBlock::FeatureBlock) {			
			int setInd;
			f >> setInd;
			temp->mBlock->FeatureDropdown->setCurrentIndex(setInd);
			temp->mBlock->handleDropdownChange(setInd);
			string i_start, i_end;
			f >> i_start;
			f >> i_end;
			temp->mBlock->histogram_start_value->setText(QString(i_start.c_str()));
			temp->mBlock->histogram_end_value->setText(QString(i_end.c_str()));
			temp->mBlock->handleHistogramSetBtn_Value();

		}
		else if (flag == QNEBlock::SetBlock) {
			int setInd;
			f >> setInd;

			temp->mBlock->SetDropdown->setCurrentIndex(setInd);
			temp->mBlock->handleDropdownChange(setInd);
		}
		else if (flag == QNEBlock::StatisticBlock) {
			int StatisticInd;
			f >> StatisticInd;

			temp->mBlock->FeatureDropdown->setCurrentIndex(StatisticInd);
			temp->mBlock->handleDropdownChange2(StatisticInd);

		}
	}
	qDebug() << "load blocks";

	for (int i = mGlobals.mainWindow->mContents->annotationWidget->annotations.size()-1; i >=0; i--) {
		delete mGlobals.mainWindow->mContents->annotationWidget->annotations[i];
	}
	mGlobals.mainWindow->mContents->annotationWidget->annotations.clear();

	mGlobals.mainWindow->mContents->annotationWidget->list->clear();


	int len;
	f >> len;
	//	fscanf(f, "%d", &len);
	char line[500];
	f.getline(line, 500);

	Utils mUtil;
	for (int i = 0; i < len; i++) {
		f.getline(line, 500);
		std::vector<std::string> temp_array = mUtil.Split(line, "||");
		std::vector<std::string> temp_rgb = mUtil.Split(temp_array[2].c_str(), " ");
		std::vector<std::string> temp_min = mUtil.Split(temp_array[3].c_str(), " ");
		std::vector<std::string> temp_max = mUtil.Split(temp_array[4].c_str(), " ");
		std::vector<std::string> temp_axis = mUtil.Split(temp_array[5].c_str(), " ");

		mGlobals.mainWindow->mContents->annotationWidget->addAnnotation(QString(temp_array[1].c_str())
			, QColor(QString(temp_rgb[1].c_str()).toInt(), QString(temp_rgb[2].c_str()).toInt(), QString(temp_rgb[3].c_str()).toInt(), 100)
			, QVector3D(QString(temp_min[1].c_str()).toFloat(), QString(temp_min[2].c_str()).toFloat(), QString(temp_min[3].c_str()).toFloat())
			, QVector3D(QString(temp_max[1].c_str()).toFloat(), QString(temp_max[2].c_str()).toFloat(), QString(temp_max[3].c_str()).toFloat())
			, QString(temp_axis[1].c_str()).toInt());
		mGlobals.mainWindow->mContents->annotationWidget->annotations[mGlobals.mainWindow->mContents->annotationWidget->annotations.size() - 1]->generatedTime->setText(QString(temp_array[0].c_str()));

	}


	f.close();

	curState->setText("");


	QMessageBox msg;
	msg.setText("Complete load project");
	msg.exec();
}