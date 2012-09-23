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

#include "NeuroModulator.h"
#include <Math/Math.h>
#include "Network/Neuron.h"
#include <iostream>

using namespace std;

namespace nerd {

NeuroModulator::NeuroModulator() {
	
}


NeuroModulator::NeuroModulator(const NeuroModulator &other) {
	
}


NeuroModulator::~NeuroModulator() {
	
}



NeuroModulator* NeuroModulator::createCopy() {
	return new NeuroModulator(*this);
}

void NeuroModulator::reset(Neuron *owner) {
	mRadii.clear();
	mConcentrations.clear();
}

void NeuroModulator::update(Neuron *owner) {
	cerr << "Modulator: " << endl;
	for(int i = 0; i < mConcentrations.keys().size(); ++i) {
		int type = mConcentrations.keys().at(i);
		cerr << type << ": " << getConcentration(type, 0) << " % " << getRadius(type, 0) << endl;
	}
}

void NeuroModulator::setConcentration(int type, double concentration, Neuron *owner) {
	mConcentrations.insert(type, concentration);
}

double NeuroModulator::getConcentration(int type, Neuron *owner) {
	return mConcentrations.value(type, 0.0);
}


/**
 * Simple linear model.
 * Modi:
 * (0): off
 * (1): equal concentration
 * (2): linear concentration
 * (3): quadratic concentration
 */
double NeuroModulator::getConcentrationAt(int type, Vector3D position, Neuron *owner) {
	
	
	if(mModus == 0) {
		return 0.0;
	}
	
	//2D only here (on a plane)
	double concentration = mConcentrations.value(type, 0.0);
	if(concentration == 0.0) {
		return 0.0;
	}
	double radius = mRadii.value(type, 0.0);
	if(radius == 0.0) {
		return 0.0;
	}
	
	
	//equal concentration
	//the concentration at all points in the circle are similar.
	if(mModus == 1) {
		return concentration;
	}

	double distance = Math::distance(position, owner->getPosition());
	if(distance >= radius) {
		return 0.0;
	}
	
	//linear distribution
	//the concentration is linearly decaying from the center to the border of the circle.
	if(mModus == 2) {
		return (1.0 - (distance / radius)) * concentration;
	}
// 	if(mModus == 3) {
// 		
// 	}
	
	return 0.0;
}

QList<int> NeuroModulator::getModulatorTypes() const {
	return mConcentrations.keys();
}

void NeuroModulator::setRadius(int type, double radius, Neuron *owner) {
	mRadii.insert(type, radius);
}

double NeuroModulator::getRadius(int type, Neuron *owner) const {
	return mRadii.value(type, 0.0);
}


void NeuroModulator::setModus(int modus) {
	//TODO check for possible types?
	mModus = modus;
}


int NeuroModulator::getModus() const {
	return mModus;
}


bool NeuroModulator::equals(NeuroModulator *modulator) const {
	//TODO
	
	return true;
}



}


