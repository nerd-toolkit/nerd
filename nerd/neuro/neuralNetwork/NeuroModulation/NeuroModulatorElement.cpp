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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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

#include "NeuroModulatorElement.h"
#include "Network/NeuralNetworkElement.h"
#include <iostream>

using namespace std;

namespace nerd {
	
	NeuroModulatorElement::NeuroModulatorElement() 
		: mNeuroModulator(0)
	{
	}
	
	NeuroModulatorElement::NeuroModulatorElement(const NeuroModulatorElement &other)
		: mNeuroModulator(0)
	{
		if(other.mNeuroModulator != 0) {
			mNeuroModulator = other.mNeuroModulator->createCopy();
		}
	}
	
	NeuroModulatorElement::~NeuroModulatorElement() {
	}
	
	
	void NeuroModulatorElement::resetNeuroModulators(NeuralNetworkElement *owner) {
		if(mNeuroModulator != 0) {
			mNeuroModulator->reset(owner);
		}
	}
	
	
	/**
	 * Returns 0.0, but executes the NeuroModulator if available!
	 */
	double NeuroModulatorElement::updateNeuroModulators(NeuralNetworkElement *owner) {
		
		if(mNeuroModulator != 0) {
			mNeuroModulator->update(owner);
		}
		return 0.0;
	}
	
	
	void NeuroModulatorElement::setNeuroModulator(NeuroModulator *modulator) {
		cerr << "Adding modulator" << endl;
		if(mNeuroModulator != 0) {
			delete mNeuroModulator;
		}
		mNeuroModulator = modulator;
	}
	
	NeuroModulator* NeuroModulatorElement::getNeuroModulator() const {
		return mNeuroModulator;
	}
	

	
	
}


