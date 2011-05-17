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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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



#include "SimpleSynapseItem.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/PaintItem.h"
#include <QPainterPath>
#include <math.h>
#include "Math/Math.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SimpleSynapseItem.
 */
SimpleSynapseItem::SimpleSynapseItem(NetworkVisualization *owner)
	: SynapseItem(owner), mSize(60, 20)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the SimpleSynapseItem object to copy.
 */
SimpleSynapseItem::SimpleSynapseItem(const SimpleSynapseItem &other) 
	: SynapseItem(other), mSize(other.mSize)
{
}

/**
 * Destructor.
 */
SimpleSynapseItem::~SimpleSynapseItem() {
}


SynapseItem* SimpleSynapseItem::createCopy() const {
	return new SimpleSynapseItem(*this);
}


bool SimpleSynapseItem::setSynapse(Synapse *synapse) {
	bool ok = SynapseItem::setSynapse(synapse);

	return ok;
}


QRectF SimpleSynapseItem::getBoundingBox() {
	return QRectF(getGlobalPosition() - QPointF(mSize.width() / 2.0, mSize.height() / 2.0), mSize);
}

QRectF SimpleSynapseItem::getPaintingBox() {

	if(mForcedHidden) {
		return QRectF();
	}

	QPointF pos = getGlobalPosition();
	double minX = pos.x() - (mSize.width() / 2.0) - 30.0;
	double maxX = pos.x() + (mSize.width() / 2.0) + 30.0;
	double minY = pos.y() - (mSize.height() / 2.0) - 30.0;
	double maxY = pos.y() + (mSize.height() / 2.0) + 30.0;
	
	if(mTarget != 0) {
		QPointF targetPos = mTarget->getGlobalPosition();
		if(targetPos.x() - 10.0 < minX) {
			minX = targetPos.x() - 10.0;
		}
		if(targetPos.x() + 10.0 > maxX) {
			maxX = targetPos.x() + 10.0;
		}
		if(targetPos.y() - 10.0 < minY) {
			minY = targetPos.y() - 10.0;
		}
		if(targetPos.y() + 10.0 > maxY) {
			maxY = targetPos.y() + 10.0;
		}
	}
	if(mSourceNeuron != 0) {
		QPointF targetPos = mSourceNeuron->getGlobalPosition();
		if(targetPos.x() - 10.0 < minX) {
			minX = targetPos.x() - 10.0;
		}
		if(targetPos.x() + 10.0 > maxX) {
			maxX = targetPos.x() + 10.0;
		}
		if(targetPos.y() - 10.0 < minY) {
			minY = targetPos.y() - 10.0;
		}
		if(targetPos.y() + 10.0 > maxY) {
			maxY = targetPos.y() + 10.0;
		}
	}
	return QRectF(minX, minY, maxX - minX, maxY - minY);
}


bool SimpleSynapseItem::isHit(const QPointF &point, Qt::MouseButtons, double) {

	if(mForcedHidden || mIsInHiddenLayer) {
		return false;
	}

	QRectF boundingBox(getGlobalPosition() - QPoint(mSize.width() / 2.0, mSize.height() / 2.0), mSize);
	if(boundingBox.contains(point)) {
		return true;
	}
	return false;
}


void SimpleSynapseItem::mouseMoved(const QPointF &distance, Qt::MouseButtons mouseButton) {
	if(mouseButton & Qt::LeftButton) {
		setLocalPosition(getLocalPosition() + distance);
	}
}


void SimpleSynapseItem::paintSelf(QPainter *painter) {
	if(mHidden || !mActive || painter == 0 || mSynapse == 0 
		|| (mHideUnselectedElements && !mSelected) || mForcedHidden || mIsInHiddenLayer) 
	{
		return;
	}
	QPen oldPen = painter->pen();
	QPen newPen(QColor(0,0,0,255));
	if(mUseCosmeticLines) {
		newPen.setCosmetic(true);
	}

	newPen.setColor(QColor(0, 0, 0, 255));
	if(mShowSlaveState && mSynapse->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE)) {
		newPen.setColor(QColor(255, 0, 0, 255));
	}
	else if(mShowSlaveState && mSynapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
		newPen.setColor(QColor(0, 255, 0, 255));
	}
	painter->setPen(newPen);

	QPointF pos = getGlobalPosition();
	QRectF rect(pos - QPointF(mSize.width() / 2.0, mSize.height() / 2.0), mSize);
	

	if(mSourceNeuron != 0) {
		if(mSourceNeuron == mTarget) {
			painter->drawLine(mSourceNeuron->getGlobalPosition(), pos - QPointF(20.0, 0.0));
		}
		else {
			painter->drawLine(mSourceNeuron->getGlobalPosition(), pos);
		}
	}
	if(mTarget != 0) {
		QPointF targetPos = mTarget->getGlobalPosition();
		QPointF ownPos = pos;

		if(mSourceNeuron == mTarget) {
			ownPos = pos + QPointF(20.0, 0.0);
			painter->drawLine(pos - QPointF(20.0, 0.0), ownPos);
			painter->drawLine(ownPos, targetPos);
		}
		else {
			painter->drawLine(ownPos, targetPos);
		}
		
		
		//draw synapse endpoint
		QPainterPath arrow;
		if(!mUseSynapseTypeSymbols || mSynapse->getStrengthValue().get() >= 0.0) {
			//draw arrow
			arrow = QPainterPath(QPointF(0, 0));
			arrow.lineTo(-15, -7);
			arrow.lineTo(-15, 7);
			arrow.lineTo(0, 0);
		}
		else {
			//draw circle
			arrow.addEllipse(-15,-7, 15, 15);
		}

		QMatrix transformation = painter->worldMatrix();
		
		double angle = atan2(targetPos.x() - ownPos.x(), targetPos.y() - ownPos.y());
		double distance = Math::distance(ownPos, targetPos);

		painter->translate(targetPos);
		painter->rotate(90 + (-angle / Math::PI * 180.0));
		painter->translate(-1 * Math::min(15.0, distance - 17), 0);
		painter->fillPath(arrow, QColor(0, 0, 0, 255));

		painter->setWorldMatrix(transformation);
	}

	QRectF boundingBox = getBoundingBox();
	if(mSelected) {
		painter->fillRect(boundingBox, QColor(255, 0, 0, 50));
	}

	if(!mHideWeight && !mLocalHideWeight) {
		QString strength = mSynapse->getStrengthValue().getValueAsString();
		if(strength.size() > 7) {
			strength = strength.mid(0, 7);
		}
		QFont oldFont = painter->font();
		QFont newFont(oldFont);
		newFont.setPointSizeF(oldFont.pointSizeF() + 0.5);
		painter->setFont(newFont);
		
		QRectF textRect = rect;
		textRect.moveTo(textRect.x() + mPositionOffset.getX(), textRect.y() + mPositionOffset.getY());
		
		painter->drawText(textRect, Qt::AlignCenter | Qt::AlignHCenter | Qt::TextSingleLine, 
				strength);
		painter->setFont(oldFont);
	}

	if(!mSynapse->getEnabledValue().get()) {
		painter->drawLine(boundingBox.x(), 
						  boundingBox.y(), 
						  boundingBox.x() + boundingBox.width(), 
						  boundingBox.y() + boundingBox.height());

		painter->drawLine(boundingBox.x() + boundingBox.width(), 
						  boundingBox.y(), 
						  boundingBox.x(), 
						  boundingBox.y() + boundingBox.height());
	}

	painter->setPen(oldPen);
	
}



}




