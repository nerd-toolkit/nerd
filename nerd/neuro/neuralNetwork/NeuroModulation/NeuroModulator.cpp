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
#include <Network/NeuralNetwork.h>
#include <ActivationFunction/NeuroModulatorActivationFunction.h>
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Math/Vector3D.h"

using namespace std;

namespace nerd {

NeuroModulator::NeuroModulator() 
	: mGeneralModus(2) 
{
	
}


NeuroModulator::NeuroModulator(const NeuroModulator &other) 
	: mGeneralModus(other.mGeneralModus) 
{
	
}


NeuroModulator::~NeuroModulator() {
	
}



NeuroModulator* NeuroModulator::createCopy() {
	return new NeuroModulator(*this);
}

void NeuroModulator::reset(NeuralNetworkElement *owner) {
	mConcentrations.clear();
}

void NeuroModulator::update(NeuralNetworkElement *owner) {
// 	for(int i = 0; i < mConcentrations.keys().size(); ++i) {
// 		int type = mConcentrations.keys().at(i);
// 		cerr << type << ": " << getConcentration(type, 0) << " % " << getRadius(type, 0) << endl;
// 	}
}

void NeuroModulator::setConcentration(int type, double concentration, NeuralNetworkElement *owner) {
	mConcentrations.insert(type, concentration);
}

double NeuroModulator::getConcentration(int type, NeuralNetworkElement *owner) {
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
double NeuroModulator::getConcentrationAt(int type, Vector3D position, NeuralNetworkElement *owner) {
	
	int modus = mGeneralModus;
	if(modus == -1) {
		modus = mModus.value(type, -1.0);
	}
	
	//2D only here (on a plane)
	double concentration = mConcentrations.value(type, 0.0);
	if(concentration == 0.0) {
		return 0.0;
	}
	QRectF area = getLocalRect(type);
	bool isCircle = isCircularArea(type);
	
	if(area.width() <= 0.0 || area.height() <= 0.0) {
		return 0.0;
	}
	
	//check if the point is in the area of influence
	double radius = 0.0;
	double distance = 0.0;
	if(isCircle) {
		radius = area.width() / 2.0;
		distance = Math::distance(position, owner->getPosition() + Vector3D(area.x(), area.y(), 0.0));
		
		if(distance >= radius) {
			return 0.0;
		}
	}
	else {
		Vector3D ownerPos = owner->getPosition();
		QRectF areaOfInfluence(ownerPos.getX() + area.x(), ownerPos.getY() + area.y(), area.width(), area.height());
		
		distance = Math::distance(position, ownerPos + Vector3D(area.x(), area.y(), 0.0));
		radius = Math::max(area.width(), area.height());
		
		if(!areaOfInfluence.contains(position.getX(), position.getY())) {
			return 0.0;
		}
	}
	
	//equal concentration
	//the concentration at all points in the circle are similar.
	if(modus == 1) {
		return concentration;
	}
	
	//linear distribution
	//the concentration is linearly decaying from the center to the border of the circle.
	if(modus == 2) {
		return (1.0 - (distance / radius)) * concentration;
	}
// 	if(mModus == 3) {
// 		
// 	}
	
	//return 0 if there is no supported modus.
	return 0.0;
}

QList<int> NeuroModulator::getModulatorTypes() const {
	return mConcentrations.keys();
}


void NeuroModulator::setLocalAreaRect(int type, double width, double height, 
									const Vector3D &offset, bool isCircle)
{
	mReferenceModules.remove(type);
	if(isCircle) {
		////TODO currently, only real circles are supported (no ellipses)
		height = width;
	}
	mAreas.insert(type, QRectF(offset.getX(), offset.getY(), width, height));
	mIsCircle.insert(type, isCircle);
}


void NeuroModulator::setAreaReferenceModule(int type, NeuroModule *module) {
	if(module == 0) {
		return;
	}
	mAreas.remove(type);
	mIsCircle.remove(type);
	mReferenceModules.insert(type, module);
}


QRectF NeuroModulator::getLocalRect(int type) {
	NeuroModule *refModule = mReferenceModules.value(type, 0);
	if(refModule == 0) {
		return mAreas.value(type, QRectF(0.0, 0.0, 0.0, 0.0));
	}
	
	//only if a reference module is given
	Vector3D refPos = refModule->getPosition();
	QSizeF size = refModule->getSize();
	return QRectF(refPos.getX(), refPos.getY(), size.width(), size.height());
}


bool NeuroModulator::isCircularArea(int type) {
	return mIsCircle.value(type, true);
}



void NeuroModulator::setModus(int type, int modus) {
	if(type == -1) {
		mModus.clear();
		mGeneralModus = modus;
		return;
	}
	QList<int> types = mConcentrations.keys();
	for(int i = 0; i < types.size(); ++i) {
		int t = types.at(i);
		if(t == type) {
			mModus.insert(t, modus);
		}
		else if(!mModus.keys().contains(t)) {
			mModus.insert(t, mGeneralModus);
		}
	}
	mGeneralModus = -1;
}


int NeuroModulator::getModus(int type) const {
	if(type == -1 || mGeneralModus != -1) {
		return mGeneralModus;
	}
	if(mModus.values().empty()) {
		return -1;
	}
	return mModus.value(type, -1.0);
}


bool NeuroModulator::equals(NeuroModulator *modulator) const {
	//TODO
	
	return true;
}


double NeuroModulator::getConcentrationInNetworkAt(int type, const Vector3D &position, NeuralNetwork *network) {
	if(network == 0) {
		return 0.0;
	}
	double concentration = 0.0;
	
	QList<Neuron*> neurons = network->getNeurons();
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		NeuroModulatorActivationFunction *nmaf = 
					dynamic_cast<NeuroModulatorActivationFunction*>(neuron->getActivationFunction());
		if(nmaf != 0) {
			NeuroModulator *mod = nmaf->getNeuroModulator();
			if(mod != 0) {
				concentration += mod->getConcentrationAt(type, position, neuron);
			}
		}
	}
	return concentration;
}

}


