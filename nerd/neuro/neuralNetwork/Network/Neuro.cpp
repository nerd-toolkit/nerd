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

#include "Neuro.h"
#include "Core/Core.h"
#include "Network/NeuralNetworkManager.h"
#include "NeuralNetworkConstants.h"

namespace nerd {

NeuralNetworkManager *Neuro::mGlobalManager = 0;


bool Neuro::install() {
	bool ok = true;
	Core *core = Core::getInstance();

	NeuralNetworkManager *nm = dynamic_cast<NeuralNetworkManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_NEURAL_NETWORK_MANAGER));
	if(nm == 0) {
		nm = new NeuralNetworkManager();
		if(!core->addGlobalObject(NeuralNetworkConstants::OBJECT_NEURAL_NETWORK_MANAGER, nm)) {
			ok = false;
		}
	}
	mGlobalManager = nm;
	return ok;
}

void Neuro::reset() {
	mGlobalManager = 0;
}


NeuralNetworkManager* Neuro::getNeuralNetworkManager() {
	if(mGlobalManager == 0) {
		NeuralNetworkManager *nm = dynamic_cast<NeuralNetworkManager*>(Core::getInstance()
				->getGlobalObject(NeuralNetworkConstants::OBJECT_NEURAL_NETWORK_MANAGER));
		if(nm == 0) {
			Neuro::install();
		}
	}
	return mGlobalManager;
}


}


