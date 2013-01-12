/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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
 *   The NERD Toolkit is part of the EU project ALEAR                      *
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
 *   clearly by citing the NERD homepage and the NERD overview paper.      *  
 ***************************************************************************/

#include "NumberOfReadNeuronsConstraint.h"
#include "Math/Random.h"
#include "Network/Neuron.h"
#include "Network/Synapse.h"
#include "ActivationFunction/ActivationFunction.h"
#include "TransferFunction/TransferFunction.h"
#include "SynapseFunction/SynapseFunction.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include <QListIterator>


namespace nerd {


NumberOfReadNeuronsConstraint::NumberOfReadNeuronsConstraint(int maxNumberOfReadNeurons) 
	: GroupConstraint("NumberOfReadNeurons"), mMaxNumberOfReadNeurons(0)
{
	mMaxNumberOfReadNeurons = new IntValue(maxNumberOfReadNeurons);

	addParameter("MaxReadNeurons", mMaxNumberOfReadNeurons);
}


NumberOfReadNeuronsConstraint::NumberOfReadNeuronsConstraint(const NumberOfReadNeuronsConstraint &other)
	: Object(), ValueChangedListener(), GroupConstraint(other)
{
	mMaxNumberOfReadNeurons = dynamic_cast<IntValue*>(getParameter("MaxReadNeurons"));
}

NumberOfReadNeuronsConstraint::~NumberOfReadNeuronsConstraint() {
}

GroupConstraint* NumberOfReadNeuronsConstraint::createCopy() const {
	return new NumberOfReadNeuronsConstraint(*this);
}


bool NumberOfReadNeuronsConstraint::isValid(NeuronGroup*) {
	return true;
}


bool NumberOfReadNeuronsConstraint::applyConstraint(NeuronGroup *owner, CommandExecutor*,
													QList<NeuralNetworkElement*>&) 
{

	if(owner == 0 || owner->getOwnerNetwork() == 0) {
		mErrorMessage = "Owner ModularNeuralNetwork or owner NeuronGroup has been NULL";
		return false;
	}

	ModularNeuralNetwork *net = owner->getOwnerNetwork();
	NeuroModule *module = dynamic_cast<NeuroModule*>(owner);

	QList<Neuron*> groupNeurons = owner->getNeurons();
	
	if(module != 0) {
		groupNeurons = module->getAllEnclosedNeurons();
	}
	

	QList<Synapse*> synapses = net->getSynapses();
	QList<Neuron*> readNeurons;

	int maxNumberOfReadNeurons = mMaxNumberOfReadNeurons->get();

	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();

		SynapseTarget *target = synapse->getTarget();
		Neuron *targetNeuron = dynamic_cast<Neuron*>(target);

		if(targetNeuron != 0 && groupNeurons.contains(targetNeuron)) {
			//skip neurons that are read from internal neurons.
			continue;
		}

		Neuron *neuron = synapse->getSource();
		if(groupNeurons.contains(neuron) && !readNeurons.contains(neuron)) {
			readNeurons.append(neuron);
			if(readNeurons.size() > maxNumberOfReadNeurons) {
				mErrorMessage = "Too many neurons were read from external sources.";
				return false;
			}
		}
	}

	return true;
}

bool NumberOfReadNeuronsConstraint::equals(GroupConstraint *constraint) const {
	if(GroupConstraint::equals(constraint) == false) {
		return false;
	}
	NumberOfReadNeuronsConstraint *c = dynamic_cast<NumberOfReadNeuronsConstraint*>(constraint);
	if(c == 0) {
		return false;
	}
	if(mMaxNumberOfReadNeurons->equals(c->mMaxNumberOfReadNeurons) == false) {
		return false;
	}
	return true;
}

}



