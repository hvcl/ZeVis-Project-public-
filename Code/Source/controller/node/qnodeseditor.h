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

#ifndef QNODESEDITOR_H
#define QNODESEDITOR_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>

#include "qneport.h"
#include "qneconnection.h"
#include "qneblock.h"

class QGraphicsScene;
class QGraphicsView;
class QNEConnection;
class QGraphicsItem;
class QPointF;
class QNEBlock;

class QNodesEditor : public QObject
{
	Q_OBJECT
public:
	explicit QNodesEditor(QObject *parent = 0);
	void removeThis(QGraphicsItem * ptr);

	void install(QGraphicsScene *scene, QGraphicsView *view);

	bool eventFilter(QObject *, QEvent *);

	void save(QDataStream &ds);
	void load(QDataStream &ds);
	QGraphicsItem *itemAt(const QPointF&);

	void set_modifiers(Qt::KeyboardModifiers modifiers);
	void set_zoom_factor_base(double value);

	Qt::KeyboardModifiers _modifiers;
	double _zoom_factor_base;
	QPointF target_scene_pos, target_viewport_pos;
	QGraphicsScene *scene;
	QGraphicsView *view;
	QNEConnection *conn;
private:

private:

	// QNEBlock *selBlock;
};

#endif // QNODESEDITOR_H
