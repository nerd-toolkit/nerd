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



#include "NeuroModulatorManager.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {
	
	
	/**
	 * Constructs a new NeuroModulatorManager.
	 */
	NeuroModulatorManager::NeuroModulatorManager()
	{
		mEnableNeuroModulatorUpdate = new BoolValue(true);
		mEnableNeuroModulatorConcentrationLevels = new BoolValue(true);
		
		ValueManager *vm = Core::getInstance()->getValueManager();
		vm->addValue("/NeuralNetwork/NeuroModulators/EnableUpdate", mEnableNeuroModulatorUpdate);
		vm->addValue("/NeuralNetwork/NeuroModulators/EnableConcentrationLevels", mEnableNeuroModulatorConcentrationLevels);
	}
	
	
	/**
	 * Destructor.
	 */
	NeuroModulatorManager::~NeuroModulatorManager() {
	}
	
	/**
	 * This method returns the singleton instance of the NeuroModulatorManager.
	 * Note: calling this method in volves string comparisons and is computationally expensive.
	 * Try to minimize the use of this method.
	 *
	 * @return the global NeuroModulatorManager.
	 */
	NeuroModulatorManager* NeuroModulatorManager::getInstance() {
		NeuroModulatorManager *nm = dynamic_cast<NeuroModulatorManager*>(Core::getInstance()
		->getGlobalObject(NeuralNetworkConstants::OBJECT_NEURO_MODULATOR_MANAGER));
		if(nm == 0) {
			nm = new NeuroModulatorManager();
			if(!nm->registerAsGlobalObject()) {
				delete nm;
			}
			nm = dynamic_cast<NeuroModulatorManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_NEURO_MODULATOR_MANAGER));
		}
		return nm;
	}
	
	
	QString NeuroModulatorManager::getName() const {
		return "NeuroModulatorManager";
	}
	
	bool NeuroModulatorManager::registerAsGlobalObject() {
		Core::getInstance()->addGlobalObject(
			NeuralNetworkConstants::OBJECT_NEURO_MODULATOR_MANAGER, this);
		
		return Core::getInstance()->getGlobalObject(
			NeuralNetworkConstants::OBJECT_NEURO_MODULATOR_MANAGER) == this;
	}
	
	
	bool NeuroModulatorManager::init() {
		return true;
	}
	
	
	bool NeuroModulatorManager::bind() {
		return true;
	}
	
	
	bool NeuroModulatorManager::cleanUp() {
		return true;
	}
	
	BoolValue* NeuroModulatorManager::getEnableModulatorUpdateValue() const {
		return mEnableNeuroModulatorUpdate;
	}
	
	
	BoolValue* NeuroModulatorManager::getEnableModulatorConcentrationLevelsValue() const {
		return mEnableNeuroModulatorConcentrationLevels;
	}
	
}







