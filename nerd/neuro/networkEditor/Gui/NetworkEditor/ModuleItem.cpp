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
	  mShowName(true)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ModuleItem object to copy.
 */
ModuleItem::ModuleItem(const ModuleItem &other) 
	: PaintItem(other), mOwner(other.mOwner), mModule(0), mSize(other.mSize),
	  mShowName(other.mShowName)
{
}

/**
 * Destructor.
 */
ModuleItem::~ModuleItem() {
}

bool ModuleItem::setModule(NeuroModule *module) {
	if(mModule != module) {
		mMembers.clear();
	}
	mModule = module;
	if(mModule != 0) {
		NetworkEditorUtil::setPaintItemLocation(this, 
				mModule->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
		NetworkEditorUtil::setModuleItemSize(this, 
				mModule->getProperty(NeuralNetworkConstants::TAG_MODULE_SIZE));
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

}




