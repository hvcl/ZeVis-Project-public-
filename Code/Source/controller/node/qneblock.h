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

#ifndef QNEBLOCK_H
#define QNEBLOCK_H

#include <QGraphicsPathItem>
#include <QObject>
#include <QPen>
#include <QGraphicsScene>
#include <QFontMetrics>
#include <QPainter>
#include <iostream>
#include <list>
#include <vector>
#include <QDebug>

#include "qneport.h"

#include "Source/global.h"
#include "Source/Utils.h"
#include "BlockWidget.h"

class QNEPort;
class BlockWidget;

class QNEBlock : public QGraphicsPathItem
{



public:
	enum { Type = QGraphicsItem::UserType + 3 };
	enum { OriginBlock = 1, DataBlock= 2, FeatureBlock = 3, SubregionBlock = 4,  SetBlock = 5, CutPlaneBlock = 6, StatisticBlock = 7, SubsetExplorer=8};
	enum { Input = 1, Left = 2, Center = 3, Right = 4, Output = 5};

    QNEBlock(QGraphicsItem *parent = 0);

	QNEPort* addPort(const QString &name, bool isInput, bool isOutput, int flags = 0, int ptr = 0, int align = 3);
	void addInputPort(const QString &name);
	void addSubInputPort(const QString &name);
	void addOutputPort(const QString &name);

	void addInputPorts(const QStringList &names);
	void addOutputPorts(const QStringList &names);
	void save(QDataStream&);
	void load(QDataStream&, QMap<quint64, QNEPort*> &portMap);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	QNEBlock* clone();
	QVector<QNEPort*> ports();

	int type() const { return Type; }
	int flag() const { return mBlockFlags; }

	void setInputDataOrigin(std::vector<cell> *data_ptr);
	void updateInput_newconnect(std::list<unsigned int> *data_ptr);
	void updateInput_newconnect_sub(std::list<unsigned int> *data_ptr);
	void setBlockFlagAndSize(std::string name, int aflags, int awidth, int aheight, QColor acolor, QWidget *parent);
	
		
	int horzMargin;
	int vertMargin;
	int width;
	int height;
	QColor node_color = QColor("#3A79A0");
	QColor node_selected_color = QColor("#e41a1c");
	QColor node_text_color = QColor("#e7e7e7");
	int mBlockFlags;
	BlockWidget *mBlock;
	QWidget *pWidget;

	QGraphicsScene *pScene;

	int labelIndex = -1;
	int labelIndex2 = -1;

	bool newSelected = false;

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
private:
	



	

};

#endif // QNEBLOCK_H
