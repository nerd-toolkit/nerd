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


#include "ModuleItem.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Util/NetworkEditorUtil.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ModuleItem.
 */
ModuleItem::ModuleItem(NetworkVisualization *owner)
	: PaintItem(100), mOwner(owner), mModule(0), mSize(50, 50), 
	  mShowName(true), mShowHandles(true), mShowBackground(true), mUseCustomBackgroundColor(false)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ModuleItem object to copy.
 */
ModuleItem::ModuleItem(const ModuleItem &other) 
	: PaintItem(other), mOwner(other.mOwner), mModule(0), mSize(other.mSize),
	  mShowName(other.mShowName), mShowHandles(other.mShowHandles), 
	  mShowBackground(other.mShowBackground), mBackgroundColor(other.mBackgroundColor),
	  mUseCustomBackgroundColor(other.mUseCustomBackgroundColor)
{
}

/**
 * Destructor.
 */
ModuleItem::~ModuleItem() {
}


NeuralNetworkElement* ModuleItem::getNetworkElement() const {
	return mModule;
}

bool ModuleItem::setModule(NeuroModule *module) {
	if(mModule != module) {
		mMembers.clear();
	}
	if(mModule != 0) {
		mModule->removePropertyChangedListener(this);
	}
	mModule = module;
	mUseCustomBackgroundColor = false;
	
	if(mModule != 0) {
		NetworkEditorUtil::setPaintItemLocation(this, 
				mModule->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
		NetworkEditorUtil::setModuleItemSize(this, 
				mModule->getProperty(NeuralNetworkConstants::TAG_MODULE_SIZE));
		
		mModule->addPropertyChangedListener(this);
		propertyChanged(mModule, NeuralNetworkConstants::TAG_MODULE_CUSTOM_BACKGROUND_COLOR);
	}
	return true;
}


NeuroModule* ModuleItem::getNeuroModule() const {
	return mModule;
}



void ModuleItem::setSize(QSizeF size) {
	mSize = size;
}


QSizeF ModuleItem::getSize() const {
	return mSize;
}

void ModuleItem::showName(bool show) {
	mShowName = show;
}

bool ModuleItem::isShowingName() const {
	return mShowName;
}


bool ModuleItem::addMemberItem(PaintItem *member) {
	if(member == 0 || mMembers.contains(member)) {
		return false;
	}
	mMembers.append(member);
	member->setParent(this);
	setPaintLevel(mPaintLevel); //update paint level of members
	return true;
}


bool ModuleItem::removeMemberItem(PaintItem *member) {
	if(member == 0 || !mMembers.contains(member)) {
		return false;
	}
	mMembers.removeAll(member);
	member->setParent(0);
	return true;
}




QList<PaintItem*> ModuleItem::getMemberItems() const {
	return mMembers;
}


void ModuleItem::setPaintLevel(int level) {
	PaintItem::setPaintLevel(level);
	//ensure that submodules are located higher than the parent modules.
	for(QListIterator<PaintItem*> i(mMembers); i.hasNext();) {
		ModuleItem *module = dynamic_cast<ModuleItem*>(i.next());
		if(module != 0) {
			module->setPaintLevel(level + 1);
		}
	}
}

Properties* ModuleItem::getEncapsulatedProperties() const {
	return mModule;
}

void ModuleItem::propertyChanged(Properties *owner, const QString &property) {
	if(owner == 0 || owner != mModule) {
		return;
	}
	if(property == NeuralNetworkConstants::TAG_MODULE_CUSTOM_BACKGROUND_COLOR) {
		bool colorSpecified = false;
		if(mModule->hasProperty(NeuralNetworkConstants::TAG_MODULE_CUSTOM_BACKGROUND_COLOR)) {
			//Check if there is a custom background color. If so, parse it.
			
			QStringList colorParts = mModule->getProperty(
					NeuralNetworkConstants::TAG_MODULE_CUSTOM_BACKGROUND_COLOR).split(",");
			if(colorParts.size() == 4) {
				bool okr = true;
				bool okg = true;
				bool okb = true;
				bool okh = true;
				double r = colorParts.at(0).toDouble(&okr);
				double g = colorParts.at(1).toDouble(&okg);
				double b = colorParts.at(2).toDouble(&okb);
				double h = colorParts.at(3).toDouble(&okh);
				if(okr && okg && okb && okh) {
					mBackgroundColor = QColor(r, g, b, h);
					mUseCustomBackgroundColor = true;
					colorSpecified = true;
				}
			}
		}
		if(!colorSpecified) {
			mUseCustomBackgroundColor = false;
		}
	}
}

void ModuleItem::setViewMode(int mode, bool enabled) {
	if(mode == PaintItem::SHOW_MODULE_NAMES) {
		mShowName = enabled;
	}
	if(mode == PaintItem::SHOW_MODULE_HANDLES) {
		mShowHandles = enabled;
	}
	if(mode == PaintItem::SHOW_MODULE_BACKGROUND) {
		mShowBackground = enabled;
	}
	else {
		PaintItem::setViewMode(mode, enabled);
	}
}

bool ModuleItem::isViewModeEnabled(int mode) {
	if(mode == PaintItem::SHOW_MODULE_NAMES) {
		return mShowName;
	}
	else if(mode == PaintItem::SHOW_MODULE_HANDLES) {
		return mShowHandles;
	}
	else if(mode == PaintItem::SHOW_MODULE_BACKGROUND) {
		return mShowBackground;
	}
	else {
		return PaintItem::isViewModeEnabled(mode);
	}
}

}




