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



#include "SimpleModuleItem.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Math/Math.h"
#include <QPainterPath>
#include "Gui/NetworkEditor/SynapseItem.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SimpleModuleItem.
 */
SimpleModuleItem::SimpleModuleItem(NetworkVisualization *owner)
	: ModuleItem(owner), mResizeMode(None), mHandleSize(10)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the SimpleModuleItem object to copy.
 */
SimpleModuleItem::SimpleModuleItem(const SimpleModuleItem &other) 
	: ModuleItem(other), mResizeMode(None), mHandleSize(other.mHandleSize)
{
}

/**
 * Destructor.
 */
SimpleModuleItem::~SimpleModuleItem() {
}


ModuleItem* SimpleModuleItem::createCopy() const {
	return new SimpleModuleItem(*this);
}


void SimpleModuleItem::setLocalPosition(const QPointF &position, bool force) {
	ModuleItem *parent = dynamic_cast<ModuleItem*>(getParent());
	if(parent == 0 || force) {
		ModuleItem::setLocalPosition(position, force);
	}
	else {
		QPointF newPosition = position;
		QSizeF parentSize = parent->getSize();
		if(newPosition.x() + mSize.width() > parentSize.width()) {
			newPosition += QPointF(parentSize.width() - (newPosition.x() + mSize.width()), 0.0);
		}
		if(newPosition.x() < 0.0) {
			newPosition += QPointF(-newPosition.x(), 0.0);
		}
		if(newPosition.y() + mSize.height() > parentSize.height()) {
			newPosition += QPointF(0.0, parentSize.height() - (newPosition.y() + mSize.height()));
		}
		if(newPosition.y() < 0.0) {
			newPosition += QPointF(0.0, -newPosition.y());
		}
		
		ModuleItem::setLocalPosition(newPosition);
	}
}

void SimpleModuleItem::setSize(QSizeF size) {
	ModuleItem::setSize(QSizeF(Math::max(50.0, size.width()), Math::max(50.0, size.height())));
}

QRectF SimpleModuleItem::getBoundingBox() {
	return QRectF(getGlobalPosition(), mSize);
}

QRectF SimpleModuleItem::getPaintingBox() {
	return QRectF(getGlobalPosition() - QPoint(25.0, 10.0), QSize(mSize.width() + 50, mSize.height() + 70));
}

bool SimpleModuleItem::isHit(const QPointF &point, Qt::MouseButtons, double scaling) {

	if(mIsInHiddenLayer) {
		return false;
	}

	QPointF pos = getGlobalPosition();

	double handleSize = mHandleSize;
	if(scaling < 1 && scaling != 0) {
		handleSize = mHandleSize / scaling;
	}

	QRectF lowerRightHandleRect(pos.x() + mSize.width() - Math::min(mSize.width(), handleSize), 
					  pos.y() + mSize.height() - Math::min(mSize.height(), handleSize),
					  handleSize, handleSize);

	if(lowerRightHandleRect.contains(point)) {
		mResizeMode = LowerRight;
		return true;
	}

	QRectF upperLeftHandleRect(pos.x(), pos.y(), handleSize, handleSize);

	if(upperLeftHandleRect.contains(point)) {
		mResizeMode = UpperLeft;
		return true;
	}

	mResizeMode = None;

	QRectF rect(pos.x(), pos.y(), mSize.width(), mSize.height());

	if(rect.contains(point)) {
		return true;
	}

	return false;
}


void SimpleModuleItem::mouseMoved(const QPointF &distance, Qt::MouseButtons mouseButton) {
	if(mouseButton & Qt::LeftButton) {

		if(mResizeMode != None) {
			QList<PaintItem*> items = mOwner->getSelectedItems();
			if(items.size() > 1 || !items.contains(this)) {
				mResizeMode = None;
			}
			else {
				QRectF memberBoundingBox = calculateMemberBoundingBox(false);

				QPointF pos = getGlobalPosition();

				if(mResizeMode == UpperLeft) {
					QPointF posMove = distance;

					if(pos.x() + posMove.x() > memberBoundingBox.x()) {
						posMove += QPointF(memberBoundingBox.x() - (pos.x() + posMove.x()), 0.0);
					}
					if(pos.y() + posMove.y() > memberBoundingBox.y()) {
						posMove += QPointF(0.0, memberBoundingBox.y() - (pos.y() + posMove.y()));
					}
					setLocalPosition(getLocalPosition() + posMove);
					setSize(mSize - QSizeF(posMove.x(), posMove.y()));
					for(QListIterator<PaintItem*> i(mMembers); i.hasNext();) {
						PaintItem *item = i.next();
						item->setLocalPosition(item->getLocalPosition() - posMove);
					}
					
				}

				if(mResizeMode == LowerRight) {
					QSizeF newSize = mSize + QSizeF(distance.x(), distance.y());

					if(pos.x() + newSize.width() < memberBoundingBox.x() + memberBoundingBox.width()) {
						newSize = QSizeF(memberBoundingBox.x() + memberBoundingBox.width() - pos.x(),
								newSize.height());
					}
					if(pos.y() + newSize.height() < memberBoundingBox.y() + memberBoundingBox.height()) {
						newSize = QSizeF(newSize.width(), 
								memberBoundingBox.y() + memberBoundingBox.height() - pos.y());
					}

					setSize(newSize);
				}
				
				
			}
		}
		if(mResizeMode == None) {
			setLocalPosition(getLocalPosition() + distance);
		}

		
	}
}



void SimpleModuleItem::paintSelf(QPainter *painter) {

	if(mHidden || !mActive || painter == 0 || mModule == 0 
		|| (mHideUnselectedElements && !mSelected) || mIsInHiddenLayer) 
	{
		return;
	}
	QPen oldPen = painter->pen();
	QPen newPen;
	newPen.setWidth(0);
	if(mUseCosmeticLines) {
		newPen.setCosmetic(true);
	}

	QPointF pos = getGlobalPosition();
	QRectF rect(pos.x(), pos.y(), mSize.width(), mSize.height());

	QColor color(100, 100, 100, 15);
	if(mUseCustomBackgroundColor) {
		color = mBackgroundColor;
	}
	QColor outlineColor(0, 0, 0);
	if(mSelected) {
		color = QColor(100, 100, 100, 100);
	}
// 	if(mShowSlaveState && mModule->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE)) {
// 		color.setRed(Math::min(255, color.red() + 100));
// 		outlineColor = QColor(255, 0, 0);
// 	}
// 	else if(mShowSlaveState && mModule->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
// 		color.setGreen(255);
// 		outlineColor = QColor(0, 255, 0);
// 	}
	if(mShowSlaveState) {
		QString rdof = mModule->getProperty(
				NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM);
		if(rdof.contains("C")) {
			if(mDOFHighlightColor != 0) {
				Color c = mDOFHighlightColor->get();
				outlineColor = QColor(c.red(), c.green(), c.blue(), c.alpha());
			}
			else {
				outlineColor = QColor(255, 0, 0, 255);
			}
		}
		else {
			if(mDOFNormalColor != 0 && mDOFHighlightWidth != 0) {
				Color c = mDOFNormalColor->get();
				outlineColor = QColor(c.red(), c.green(), c.blue(), c.alpha());
				newPen.setWidth(mDOFHighlightWidth->get());
			}
		}
	}

	if(!mUseCosmeticLines && mShowBackground) {
		painter->fillRect(rect, color);
	}
	newPen.setColor(outlineColor);
	painter->setPen(newPen);
	painter->drawRect(rect);

	if(mShowHandles) {
		QPainterPath upperLeftHandle(QPointF(pos.x(), pos.y()));
		upperLeftHandle.lineTo(QPoint(pos.x(), pos.y() + mHandleSize));
		upperLeftHandle.lineTo(QPoint(pos.x() + mHandleSize, pos.y()));
		painter->fillPath(upperLeftHandle, outlineColor);

		double x = pos.x() + mSize.width();
		double y = pos.y() + mSize.height();
		QPainterPath lowerRightHandle(QPointF(x, y));
		lowerRightHandle.lineTo(QPoint(x, y - mHandleSize));
		lowerRightHandle.lineTo(QPoint(x - mHandleSize, y));
		painter->fillPath(lowerRightHandle, outlineColor);
	}

	if(mShowName) {
		QFont font = painter->font();
		QFont oldFont = font;
		font.setPointSize(font.pointSize() + 2);
		painter->setFont(font);
		
		double scale = 1.0;
		//painter->scale(scale, scale);
		
		QRectF textRect((pos.x() - 0.0) / scale, (pos.y() + mSize.height()) / scale,
						 (mSize.width() + 0.0) / scale, 50.0 / scale);
		
		if(mIncreaseReadability) {
			QRectF textBoundingRect = painter->boundingRect(textRect, 
							Qt::AlignHCenter | Qt::AlignRight, 
							mModule->getName());
			painter->fillRect(QRectF(textBoundingRect.x(), textBoundingRect.y() + 2,
									 textBoundingRect.width(), textBoundingRect.height() - 4), 
							  QColor(255,255,255,180));
		}
		
		if(mDrawNamesSolid) {
			painter->setPen(QColor(0, 0, 0, 255));
		}
		
		painter->drawText(textRect, 
						 Qt::AlignHCenter | Qt::AlignRight, mModule->getName());
		//painter->scale(1.0 / scale, 1.0 / scale);
		
		painter->setFont(oldFont);
	}

	painter->setPen(oldPen);
}


void SimpleModuleItem::updateLayout() {

	if(mMembers.empty()) {
		mSize = QSize(50, 50);
		return;
	}

	for(int i = 1; i < mMembers.size(); ++i) {

		PaintItem *item = mMembers.at(i);
		item->invalidate();
		item->updateLayout();
	}

	QRectF memberBoundingBox = calculateMemberBoundingBox(true);

	mSize = QSizeF(memberBoundingBox.width() + 10, memberBoundingBox.height() + 10);
	QPointF globalOffset = getGlobalPosition() - mLocalPosition;

	QPointF newPosition(memberBoundingBox.x() - 5.0 - globalOffset.x(), 
					   memberBoundingBox.y() - 5.0 - globalOffset.y());
	QPointF shift(getLocalPosition().x() - newPosition.x(), 
				 getLocalPosition().y() - newPosition.y());
	
	for(int i = 0; i < mMembers.size(); ++i) {
		PaintItem *item = mMembers.at(i);
		item->setLocalPosition(item->getLocalPosition() + shift);
	}

	setLocalPosition(newPosition);
	invalidate();
}



QRectF SimpleModuleItem::calculateMemberBoundingBox(bool includeSynapses) {
	
	if(mMembers.empty()) {
		return QRectF(getGlobalPosition().x() + mSize.width() / 2.0, 
					  getGlobalPosition().y() + mSize.height() / 2.0, 
					  0.0, 0.0);
	}
	
	PaintItem *item = mMembers.at(0);
// 	item->invalidate();
// 	item->updateLayout();

	QRectF boundingBox = item->getBoundingBox();
	
	int minX = boundingBox.x();
	int maxX = boundingBox.x() + boundingBox.width();
	int minY = boundingBox.y();
	int maxY = boundingBox.y() + boundingBox.height();

	for(int i = 1; i < mMembers.size(); ++i) {

		item = mMembers.at(i);

		//check if synapses should be ignored.
		if(!includeSynapses && dynamic_cast<SynapseItem*>(item) != 0) {
			continue;
		}

		item->invalidate();

		QRectF boundingBox = item->getBoundingBox();
	
		if(minX > boundingBox.x()) {
			minX = boundingBox.x();
		}
		if(maxX < boundingBox.x() + boundingBox.width()) {
			maxX = boundingBox.x() + boundingBox.width();
		}
		if(minY > boundingBox.y()) {
			minY = boundingBox.y();
		}
		if(maxY < boundingBox.y() + boundingBox.height()) {
			maxY = boundingBox.y() + boundingBox.height();
		}
	}

	return QRectF(minX, minY, maxX - minX, maxY - minY);
}


}




