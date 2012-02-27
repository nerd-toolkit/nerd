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

#include "NumberOfNeuronsConstraint.h"
#include "Math/Random.h"
#include "Network/Neuron.h"
#include "ActivationFunction/ActivationFunction.h"
#include "TransferFunction/TransferFunction.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <iostream>
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


NumberOfNeuronsConstraint::NumberOfNeuronsConstraint(int minNumberOfNeurons, int maxNumberOfNeurons) 
	: GroupConstraint("NumberOfNeurons"), mMinNumberOfNeurons(0), 
	  mMaxNumberOfNeurons(0), mAutoAdaptModule(0)
{
	mMinNumberOfNeurons = new IntValue(minNumberOfNeurons);
	mMaxNumberOfNeurons = new IntValue(maxNumberOfNeurons);
	mAutoAdaptModule = new BoolValue(false);

	addParameter("Min", mMinNumberOfNeurons);
	addParameter("Max", mMaxNumberOfNeurons);
	addParameter("AutoAdapt", mAutoAdaptModule);
}


NumberOfNeuronsConstraint::NumberOfNeuronsConstraint(const NumberOfNeuronsConstraint &other)
	: Object(), ValueChangedListener(), GroupConstraint(other), mMinNumberOfNeurons(0), 
	  mMaxNumberOfNeurons(0), mAutoAdaptModule(0)
{
	mMinNumberOfNeurons = dynamic_cast<IntValue*>(getParameter("Min"));
	mMaxNumberOfNeurons = dynamic_cast<IntValue*>(getParameter("Max"));
	mAutoAdaptModule = dynamic_cast<BoolValue*>(getParameter("AutoAdapt"));
}

NumberOfNeuronsConstraint::~NumberOfNeuronsConstraint() {
}

GroupConstraint* NumberOfNeuronsConstraint::createCopy() const {
	return new NumberOfNeuronsConstraint(*this);
}


bool NumberOfNeuronsConstraint::isValid(NeuronGroup*) {
	return true;
}


bool NumberOfNeuronsConstraint::applyConstraint(NeuronGroup *owner, CommandExecutor*,
												QList<NeuralNetworkElement*> &trashcan) 
{

	if(owner == 0 || owner->getOwnerNetwork() == 0) {
// 		cerr << "NumberOfneurons error: " << owner << " and " << owner->getOwnerNetwork() << " name " << owner->getName().toStdString().c_str() << endl;
		mErrorMessage = "Owner ModularNeuralNetwork or owner NeuronGroup has been NULL";
		return false;
	}

	ModularNeuralNetwork *net = owner->getOwnerNetwork();

	NeuroModule *ownerModule = dynamic_cast<NeuroModule*>(owner);

	QList<Neuron*> neurons = owner->getNeurons();
	if(ownerModule != 0) {
		neurons = ownerModule->getAllEnclosedNeurons();
	}

	int min = mMinNumberOfNeurons->get();
	int max = mMaxNumberOfNeurons->get();
	bool autoAdapt = mAutoAdaptModule->get();

	if(min < 0) {
		min = 0;
	}
	if(max < min) {
		max = min;
	}

	if(neurons.size() > max) {
		if(autoAdapt) {
			while(neurons.size() > max && max >= 0) {
				Neuron *newestNeuron = 0;
				int newestDate = -1;
				for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
					Neuron *neuron = i.next();
					if(newestNeuron == 0) {
						newestNeuron = neuron;
					}
					QString dateString = 
							neuron->getProperty(NeuralNetworkConstants::TAG_CREATION_DATE);
					if(dateString != "") {
						int date = dateString.toInt();
						if(date < newestDate) {
							continue;
						}
						newestDate = date;
						newestNeuron = neuron;
					}
					else if(newestDate == -1) {
						newestNeuron = neuron;
					}
				}
				if(newestNeuron == 0) {
					return false;
				}
				trashcan << net->savelyRemoveNetworkElement(newestNeuron);
				neurons.removeAll(newestNeuron);
			}
			return false;
		}
		else {
			mErrorMessage = "Too many neurons found in this group.";
			return false;
		}
	}

	if(neurons.size() < min) {
		if(autoAdapt) {

			ActivationFunction *af = net->getDefaultActivationFunction();
			TransferFunction *tf = net->getDefaultTransferFunction();

			while(neurons.size() < min) {
				Neuron *neuron = new Neuron("", *tf, *af);
				net->addNeuron(neuron);
				owner->addNeuron(neuron);
				neurons.append(neuron);
			}
			return true;
		}
		else {
			mErrorMessage = "Too few neurons found in this group.";
			return false;
		}
	}

	return true;
}


bool NumberOfNeuronsConstraint::equals(GroupConstraint *constraint) {
	if(GroupConstraint::equals(constraint) == false) {
		return false;
	}
	NumberOfNeuronsConstraint *c = dynamic_cast<NumberOfNeuronsConstraint*>(constraint);
	if(c == 0) {
		return false;
	}
	if(!mMinNumberOfNeurons->equals(c->mMinNumberOfNeurons)) {
		return false;
	}
	if(!mMaxNumberOfNeurons->equals(c->mMaxNumberOfNeurons)) {
		return false;
	}
	if(!mAutoAdaptModule->equals(c->mAutoAdaptModule)) {
		return false;
	}


	return true;
}


}


