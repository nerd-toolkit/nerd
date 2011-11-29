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


#include "PaintItem.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/Value.h"
#include "Value/ValueManager.h"
#include "NetworkEditorConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new PaintItem.
 */
PaintItem::PaintItem(int paintLevel)
	: mParent(0), mInvalidated(true), mPaintLevel(paintLevel), mActive(true), mHidden(false),
	  mSelected(false), mShowSlaveState(false), mHideUnselectedElements(false), mUseCosmeticLines(false),
	  mForcedHidden(false), mIsInHiddenLayer(false), mDOFNormalColor(0), mDOFHighlightColor(0)
{
}


PaintItem::PaintItem(const PaintItem *other) 
	: mParent(0), mInvalidated(true), mPaintLevel(other->mPaintLevel),
	  mActive(true), mHidden(false), mSelected(false), mShowSlaveState(false), 
	  mHideUnselectedElements(false), mUseCosmeticLines(false), mForcedHidden(false),
	  mIsInHiddenLayer(false), mDOFNormalColor(0), mDOFHighlightColor(0)
{

}


/**
 * Destructor.
 */
PaintItem::~PaintItem() {
}



void PaintItem::setParent(PaintItem *parent) {
	mParent = parent;
}


PaintItem* PaintItem::getParent() {
	return mParent;
}



void PaintItem::invalidate() {
	mInvalidated = true;
}


void PaintItem::setActive(bool active) {
	mActive = active;
}

void PaintItem::setHidden(bool hidden) {
	mHidden = hidden;
}


bool PaintItem::isHidden() const {
	return mHidden;
}



bool PaintItem::isActive() const {
	return mActive;
}

void PaintItem::setSelected(bool selected) {
	mSelected = selected;
}


bool PaintItem::isSelected() const {
	return mSelected;
}

void PaintItem::setToHiddenLayer(bool hidden) {
	mIsInHiddenLayer = hidden;
}


bool PaintItem::isInHiddenLayer() const {
	return mIsInHiddenLayer;
}


void PaintItem::setLocalPosition(const QPointF &position, bool) {
	mLocalPosition = position;
}


QPointF PaintItem::getLocalPosition() {
	return mLocalPosition;
}


QPointF PaintItem::getGlobalPosition() {
	if(mInvalidated) {
		if(mParent == 0) {
			mGlobalPosition = mLocalPosition;
		}
		else {
			mGlobalPosition = mLocalPosition + mParent->getGlobalPosition();
		}
	}
	return mGlobalPosition;
}


void PaintItem::setPaintLevel(int level) {
	mPaintLevel = level;
}

int PaintItem::getPaintLevel() const {
	return mPaintLevel;
}

void PaintItem::updateLayout() {
	//do nothing here
}


void PaintItem::setViewMode(int mode, bool state) {
	if(mode == SHOW_ELEMENT_SLAVE_STATUS) {
		if(state && (mDOFNormalColor == 0 || mDOFHighlightColor == 0)) {
			ValueManager *vm = Core::getInstance()->getValueManager();
			mDOFNormalColor = dynamic_cast<ColorValue*>(
						vm->getValue(NetworkEditorConstants::VALUE_DOF_MODE_COLOR_NORMAL));
			mDOFHighlightColor = dynamic_cast<ColorValue*>(
						vm->getValue(NetworkEditorConstants::VALUE_DOF_MODE_COLOR_HIGHLIGHT));
		}
		mShowSlaveState = state;
	}
	else if(mode == HIDE_UNSELECTED) {
		mHideUnselectedElements = state;
	}
	else if(mode == USE_COSMETIC_LINES) {
		mUseCosmeticLines = state;
	}
	else if(mode == FORCED_HIDDEN) {
		mForcedHidden = state;
	}
}

bool PaintItem::isViewModeEnabled(int) {
	return false;
}


Properties* PaintItem::getEncapsulatedProperties() const {
	return 0;
}



}




