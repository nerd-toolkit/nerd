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



#include "NeuroTagManager.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


NeuroTag::NeuroTag(const QString &name, const QString &type, const QString &description) 
	: mTagName(name), mType(type), mDescription(description)
{
}

/**
 * Constructs a new NeuroTagManager.
 */
NeuroTagManager::NeuroTagManager()
{
}


/**
 * Destructor.
 */
NeuroTagManager::~NeuroTagManager() {
}

/**
 * This method returns the singleton instance of the NeuroTagManager.
 * Note: calling this method in volves string comparisons and is computationally expensive.
 * Try to minimize the use of this method.
 *
 * @return the global NeuroTagManager.
 */
NeuroTagManager* NeuroTagManager::getInstance() {
	NeuroTagManager *nm = dynamic_cast<NeuroTagManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_NEURO_TAG_MANAGER));
	if(nm == 0) {
		nm = new NeuroTagManager();
		if(!nm->registerAsGlobalObject()) {
			delete nm;
		}
		nm = dynamic_cast<NeuroTagManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_NEURO_TAG_MANAGER));
	}
	return nm;
}


QString NeuroTagManager::getName() const {
	return "NeuroTagManager";
}

bool NeuroTagManager::registerAsGlobalObject() {
	Core::getInstance()->addGlobalObject(
		NeuralNetworkConstants::OBJECT_NEURO_TAG_MANAGER, this);

	return Core::getInstance()->getGlobalObject(
		NeuralNetworkConstants::OBJECT_NEURO_TAG_MANAGER) == this;
}


bool NeuroTagManager::init() {
	return true;
}


bool NeuroTagManager::bind() {
	return true;
}


bool NeuroTagManager::cleanUp() {
	return true;
}


bool NeuroTagManager::addTag(NeuroTag tag) {
	if(tag.mTagName.trimmed() == "" || tag.mType.trimmed() == "") {
		return false;
	}
	for(QList<NeuroTag>::iterator i = mNeuroTags.begin(); i != mNeuroTags.end(); ++i) {
		if(i->mType == tag.mType) {
			mNeuroTags.insert(i, tag);
			return true;
		}
	}
	mNeuroTags.append(tag);
	return true;
}


bool NeuroTagManager::hasTag(NeuroTag tag) const {
	for(QListIterator<NeuroTag> i(mNeuroTags); i.hasNext();) {
		NeuroTag current = i.next();
		if(current.mTagName == tag.mTagName && current.mType == tag.mType) {
			return true;
		}
	}
	return false;
}


QList<NeuroTag> NeuroTagManager::getTags() const {
	return mNeuroTags;
}



}







