/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Project homepage: nerd.x-bot.org                                      *
 *                                                                         *
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work was funded (2008 - 2011) by EU-Project Number ICT 214856    *
 *                                                                         *
 *                                                                         *
 *   License Agreement:                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   Publications based on work using the NERD kit have to state this      *
 *   clearly by citing the nerd homepage and the nerd overview paper.      *
 ***************************************************************************/



#include "SimpleGroupItem.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Math/Math.h"
#include <QPainterPath>
#include "NeuralNetworkConstants.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new SimpleGroupItem.
 */
SimpleGroupItem::SimpleGroupItem(NetworkVisualization *owner)
	: GroupItem(owner), mSize(200.0, 30.0)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the SimpleGroupItem object to copy.
 */
SimpleGroupItem::SimpleGroupItem(const SimpleGroupItem &other) 
	: GroupItem(other), mSize(other.mSize)
{
}

/**
 * Destructor.
 */
SimpleGroupItem::~SimpleGroupItem() {
}

GroupItem* SimpleGroupItem::createCopy() const {
	return new SimpleGroupItem(*this);
}


QRectF SimpleGroupItem::getBoundingBox() {
	return QRectF(getGlobalPosition(), mSize);
}

QRectF SimpleGroupItem::getPaintingBox() {
	return QRectF(getGlobalPosition() - QPoint(10.0, 10.0), mSize + QSize(20.0, 20.0));
}

bool SimpleGroupItem::isHit(const QPointF &point, Qt::MouseButtons, double) {

	if(mIsInHiddenLayer) {
		return false;
	}
	
	QPointF pos = getGlobalPosition();
	QRectF rect(pos.x(), pos.y(), mSize.width(), mSize.height());

	if(rect.contains(point)) {
		return true;
	}

	return false;
}


void SimpleGroupItem::mouseMoved(const QPointF &distance, Qt::MouseButtons mouseButton) {
	if(mouseButton & Qt::LeftButton) {
		setLocalPosition(getLocalPosition() + distance);
	}
}



void SimpleGroupItem::paintSelf(QPainter *painter) {

	if(mHidden || !mActive || painter == 0 || mNeuronGroup == 0 
		|| (mHideUnselectedElements && !mSelected) || mIsInHiddenLayer) 
	{
		return;
	}

	QPointF pos = getGlobalPosition();
	QRectF rect(pos.x(), pos.y(), mSize.width(), mSize.height());

	if(mSelected) {
		painter->fillRect(QRectF(rect.x() - 4.0, rect.y() - 4.0, rect.width() + 8.0, rect.height() + 8.0),
						  QColor(255, 0, 0, 100));
	}

	QColor color(255, 255, 255);

	if(mShowSlaveState && mNeuronGroup->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE)) {
		color = QColor(255, 0, 0);
	}

	painter->fillRect(rect, color);
	painter->drawRect(rect);

	double scale = 1.6;
	painter->scale(scale, scale);
	painter->drawText(pos.x() / scale, (pos.y()) / scale,
						(mSize.width() + 0.0) / scale, mSize.height() / scale, 
						Qt::AlignHCenter | Qt::AlignVCenter, mNeuronGroup->getName());
	painter->scale(1.0 / scale, 1.0 / scale);
}




}




