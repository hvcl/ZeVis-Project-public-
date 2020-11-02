/* Copyright (c) 2012, STANISLAW ADASZEWSKI
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of STANISLAW ADASZEWSKI nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL STANISLAW ADASZEWSKI BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "qneblock.h"


QNEBlock::QNEBlock(QGraphicsItem *parent) : QGraphicsPathItem(parent)
{
	QPainterPath p;
	p.addRoundedRect(-50, -15, 80, 50, 5, 5);
	setPath(p);
	setPen(QPen(node_color));
	setBrush(node_color);
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	horzMargin = 0;
	vertMargin = 0;
	width = horzMargin;
	height = vertMargin;

	
}

QNEPort* QNEBlock::addPort(const QString &name, bool isInput, bool isOutput, int flags, int ptr, int align)
{
	QNEPort *port = new QNEPort(this);
	port->setName(name);
	if (flags == QNEPort::NamePort) {
		port->label->setPlainText("");
		mBlock->blockName_edit = port->nameItem;
	}
	port->setIsInput(isInput);
	port->setIsOutput(isOutput);
	port->setNEBlock(this);
	//qDebug() << "jj1";
	port->setPortFlags(flags);
	//qDebug() << "jj2";

	port->setAlign(align);
	port->setPtr(ptr);

	QFontMetrics fm(scene()->font());
	int w = fm.width(name);
	int h = fm.height();
	//port->setPos(0, height + h/2);
	if (w > width - horzMargin)
		width = w + horzMargin;
	//height = h + 5;

	QPainterPath p;
	p.addRoundedRect(-width/2, -height/2, width, height, 5, 5);
	setPath(p);

	int y = -height / 2 + 15;//-height / 2 + vertMargin + port->radius();

	int inputport_count = 0;
	foreach(QGraphicsItem *port_, childItems()) {
		QNEPort *port = (QNEPort*)port_;
		if (port->portAlign() == QNEPort::Input) {
			++inputport_count;
		}
	}
	int inputport_y_step;
	int inputport_y;
	if (port->portAlign() == QNEPort::Input) {
		inputport_y_step = height / (inputport_count);
		inputport_y = inputport_y_step;
	}

	//qDebug() << width << "/////" << height;


	bool firstflag = false;
    foreach(QGraphicsItem *port_, childItems()) {
		
		if (port_->type() != QNEPort::Type)
			continue;

		QNEPort *port = (QNEPort*) port_;

		if (port->portAlign() == QNEPort::Input) {
			//qDebug() << inputport_count << " " << inputport_y << " " << inputport_y_step << " " << height;
			if (port->m_block->mBlockFlags == QNEBlock::SetBlock) {
				if (firstflag == false) {
					port->setPos(-width / 2, -height/6);
					firstflag = true;
				}
				else {
					port->setPos(-width / 2, height/6);
				}
			}
			else {
				port->setPos(-width / 2, 0);
			}
		}
		else if (port->portAlign() == QNEPort::Left) {
			port->setPos(-width/2 , y);
			y += port->portHeight() + 5;
		}
		else if (port->portAlign() == QNEPort::Center) {
			port->setPos(0 - port->portWidth()/2, y);
			y += port->portHeight();
		}
		else if (port->portAlign() == QNEPort::Right) {
			
			if (port->portFlags() == QNEPort::ToolBoxPort) {
				port->setPos(width / 2 - port->portWidth(), -height / 2);
			}
			else {
				port->setPos(width / 2, y);
				y += port->portHeight() + 5;
			}
		}
		else if (port->portAlign() == QNEPort::Output) {
			port->setPos(width / 2, 0);
		}	
		else if (port->portAlign() == QNEPort::Icon) {
			port->setPos(-width / 2, -height / 2);

		}
	}

	return port;
}
void QNEBlock::setBlockFlagAndSize(std::string name, int aflags, int awidth, int aheight, QColor acolor, QWidget *parent) {
	pWidget = parent;
	pScene = ((ViewGraphEditor*)parent)->GraphScene;
	mBlockFlags = aflags;
	width = awidth;
	height = aheight;
	node_color = acolor;
	mBlock = new BlockWidget(pWidget);
	//if (name == "") {
	//	name = "Blank";
	//}
	mBlock->initialize(name, mBlockFlags, width, height, node_color, this);
}

void QNEBlock::setInputDataOrigin(std::vector<cell> *data_ptr) {
	std::vector<cell>::iterator iter = data_ptr->begin();
	for (iter = data_ptr->begin(); iter != data_ptr->end(); ++iter) {
		mBlock->CellIndexListInput.push_back(iter->index);
	}
//	mBlock->updated();
	mBlock->updatedInputList();
}
void QNEBlock::updateInput_newconnect(std::list<unsigned int> *data_ptr) {
	qDebug() << "updateinput size : " <<  data_ptr->size();
	mBlock->CellIndexListInput.clear();
	std::list<unsigned int>::iterator iter = data_ptr->begin();
	for (iter = data_ptr->begin(); iter != data_ptr->end(); ++iter) {
		mBlock->CellIndexListInput.push_back(*iter);
	}

	mBlock->updatedInputList();
	//mBlock->DataHeatmap->updateHeatmap();
}
void QNEBlock::updateInput_newconnect_sub(std::list<unsigned int> *data_ptr) {
	mBlock->CellIndexListInput_Sub.clear();
	std::list<unsigned int>::iterator iter = data_ptr->begin();
	for (iter = data_ptr->begin(); iter != data_ptr->end(); ++iter) {
		mBlock->CellIndexListInput_Sub.push_back(*iter);
	}
	mBlock->updatedInputList();
}

void QNEBlock::addInputPort(const QString &name)
{
	addPort(name, true, false, 0,0, Input);
}
void QNEBlock::addSubInputPort(const QString &name)
{
	addPort(name, true, false, QNEPort::SetSubInputPort, 0, Input);
}

void QNEBlock::addOutputPort(const QString &name)
{
	addPort(name, false, true, 0, 0, Output);
}

void QNEBlock::addInputPorts(const QStringList &names)
{
	foreach(QString n, names)
		addInputPort(n);
}

void QNEBlock::addOutputPorts(const QStringList &names)
{
	foreach(QString n, names)
		addOutputPort(n);
}

void QNEBlock::save(QDataStream &ds)
{
	ds << pos();

	int count(0);

    foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() != QNEPort::Type)
			continue;

		count++;
	}

	ds << count;

    foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() != QNEPort::Type)
			continue;

		QNEPort *port = (QNEPort*) port_;
		ds << (quint64) port;
		ds << port->portName();
		ds << port->isOutput();
		ds << port->portFlags();
	}
}

void QNEBlock::load(QDataStream &ds, QMap<quint64, QNEPort*> &portMap)
{
	QPointF p;
	ds >> p;
	setPos(p);
	int count;
	ds >> count;
	for (int i = 0; i < count; i++)
	{
		QString name;
		bool output;
		int flags;
		quint64 ptr;
		int align;

		ds >> ptr;
		ds >> name;
		ds >> output;
		ds >> flags;
		ds >> align;
		portMap[ptr] = addPort(name, output, flags, ptr, align);
	}
}

#include <QStyleOptionGraphicsItem>

void QNEBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)
	if (isSelected()) {
		painter->setPen(QPen(node_selected_color));
		painter->setBrush(node_color);
		mBlock->updatedCellColor();
//		mGlobals.mainWindow->mContents->Cell_Explorer->IsNewJob = true;
//		emit mGlobals.mainWindow->mContents->Cell_Explorer->cell_Update(&mBlock->CellIndexListOutput);

		if (mBlockFlags == QNEBlock::CutPlaneBlock) {
			bool ok1=false, ok2 = false, ok3 = false, ok4 = false, ok5 = false, ok6 = false, ok7 = false, ok8 = false;
			
			mBlock->plane_pos_x = mBlock->subregion_pos_x->text().toFloat(&ok1);
			mBlock->plane_pos_y = mBlock->subregion_pos_y->text().toFloat(&ok2);
			mBlock->plane_pos_z = mBlock->subregion_pos_z->text().toFloat(&ok3);
			mBlock->plane_up_x = mBlock->subregion_up_x->text().toFloat(&ok4);
			mBlock->plane_up_y = mBlock->subregion_up_y->text().toFloat(&ok5);
			mBlock->plane_up_z = mBlock->subregion_up_z->text().toFloat(&ok6);

			if (ok1 && ok2 && ok3 && ok4 && ok5 && ok6) {
				mGlobals.CurrentProject->planeNormal[0] = (float)mBlock->plane_up_x;
				mGlobals.CurrentProject->planeNormal[1] = (float)mBlock->plane_up_y;
				mGlobals.CurrentProject->planeNormal[2] = (float)mBlock->plane_up_z;
				mGlobals.CurrentProject->planePoint[0] = (float)mBlock->plane_pos_x / (float)mGlobals.CurrentProject->DataSizeX;
				mGlobals.CurrentProject->planePoint[1] = (float)mBlock->plane_pos_y / (float)mGlobals.CurrentProject->DataSizeY;
				mGlobals.CurrentProject->planePoint[2] = (float)mBlock->plane_pos_z / (float)mGlobals.CurrentProject->DataSizeZ;
			}
		}

	} else {
		painter->setPen(QPen(node_color));
		painter->setBrush(node_color);
	}

	painter->drawPath(path());
	qDebug() << pos();
}

void QNEBlock::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
	if (labelIndex != -1) {
		mGlobals.mainWindow->mContents->labelList->setCurrentRow(labelIndex);
		mGlobals.mainWindow->mContents->handleLabelChange(labelIndex);
		//			mGlobals.CurrentProject->curLayerCell = labelIndex;
		mBlock->updatedCellColor();

//		mGlobals.mainWindow->mContents->Cell_Explorer->IsNewJob = true;
//		emit mGlobals.mainWindow->mContents->Cell_Explorer->cell_Update(&mBlock->CellIndexListOutput);
	}
}
QNEBlock* QNEBlock::clone()
{
    QNEBlock *b = new QNEBlock(0);
    this->scene()->addItem(b);

	foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() == QNEPort::Type)
		{
			QNEPort *port = (QNEPort*) port_;
			b->addPort(port->portName(), port->isOutput(), port->portFlags(), port->ptr(), port->portAlign());
		}
	}

	return b;
}

QVector<QNEPort*> QNEBlock::ports()
{
	QVector<QNEPort*> res;
	foreach(QGraphicsItem *port_, childItems())
	{
		if (port_->type() == QNEPort::Type)
			res.append((QNEPort*) port_);
	}
	return res;
}

QVariant QNEBlock::itemChange(GraphicsItemChange change, const QVariant &value)
{

    Q_UNUSED(change);

	return value;
}
