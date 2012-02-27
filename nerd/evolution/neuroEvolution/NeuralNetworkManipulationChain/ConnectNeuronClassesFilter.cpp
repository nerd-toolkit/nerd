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



#include "ConnectNeuronClassesFilter.h"
#include <iostream>
#include <QList>
#include "Network/NeuralNetwork.h"
#include "Core/Core.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Math/Random.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <iostream>

using namespace std;

namespace nerd {


/**
 * Constructs a new ConnectNeuronClassesFilter.
 */
ConnectNeuronClassesFilter::ConnectNeuronClassesFilter(const QString &name)
	: NeuralNetworkManipulationOperator(name)
{
	mSourceToTarget = new BoolValue(true);
	addParameter("SourceToTarget", mSourceToTarget);
}


/**
 * Copy constructor. 
 * 
 * @param other the ConnectNeuronClassesFilter object to copy.
 */
ConnectNeuronClassesFilter::ConnectNeuronClassesFilter(const ConnectNeuronClassesFilter &other) 
	: NeuralNetworkManipulationOperator(other)
{
	mSourceToTarget = dynamic_cast<BoolValue*>(getParameter("SourceToTarget"));
}

/**
 * Destructor.
 */
ConnectNeuronClassesFilter::~ConnectNeuronClassesFilter() {
}


NeuralNetworkManipulationOperator* ConnectNeuronClassesFilter::createCopy() const {
	return new ConnectNeuronClassesFilter(*this);
}

bool ConnectNeuronClassesFilter::applyOperator(Individual *individual, CommandExecutor*) {

	if(individual == 0 || dynamic_cast<NeuralNetwork*>(individual->getGenome()) == 0) {
		return false;
	}

	NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individual->getGenome());
	QList<Neuron*> neurons = net->getNeurons();

	QHash<QString, QList<Neuron*>*> mSources;
	QHash<QString, QList<Neuron*>*> mTargets;

	//collect neuron class information
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();

		if(neuron->hasProperty("ConnectionSourceClass")) {
			QString className = neuron->getProperty("ConnectionSourceClass");
		
			QList<Neuron*> *neuronList = mSources.value(className);
			if(neuronList == 0) {
				neuronList = new QList<Neuron*>();
				mSources.insert(className, neuronList);
			}
			neuronList->append(neuron);
		}
		if(neuron->hasProperty("ConnectionTargetClass")) {
			QString className = neuron->getProperty("ConnectionTargetClass");
		
			QList<Neuron*> *neuronList = mTargets.value(className);
			if(neuronList == 0) {
				neuronList = new QList<Neuron*>();
				mTargets.insert(className, neuronList);
			}
			neuronList->append(neuron);
		}
	}

	bool allNeuronsConnected = true;

	QList<QString> classNames = mSources.keys();
	
	for(QListIterator<QString> i(classNames); i.hasNext();) {
		QString className = i.next();
		QList<Neuron*> *sources = mSources.value(className);
		QList<Neuron*> *targets = mTargets.value(className);
		
		if(!mSourceToTarget->get()) {
			sources = mTargets.value(className);
			targets = mSources.value(className);
		}

		if(sources == 0 || targets == 0) {
			continue;
		}

		for(QListIterator<Neuron*> j(*sources); j.hasNext();) {
			Neuron *source = j.next();

			mVisitedNeurons.clear();
			QList<Neuron*> targetsToCheck(*targets);

			if(mSourceToTarget->get()) {
				if(!hasPathToNeuronClass(source, targetsToCheck)) {
					allNeuronsConnected = false;
					break;
				}
			}
			else {
				if(!hasPathFromNeuronClass(source, targetsToCheck)) {
					allNeuronsConnected = false;
					break;
				}
			}
		}

// 		for(QListIterator<Neuron*> j(*targets); j.hasNext();) {
// 			Neuron *target = j.next();
// 
// 			mVisitedNeurons.clear();
// 			QList<Neuron*> targetsToCheck(*sources) ;
// 			if(!hasPathToNeuronClass(target, targetsToCheck)) {
// 				allNeuronsConnected = false;
// 				break;
// 			}
// 		}

		if(!allNeuronsConnected) {
			break;
		}
	}


	//destroy all lists
	QList<QList<Neuron*>*> lists = mSources.values();
	while(!lists.empty()) {
		QList<Neuron*> *list = lists.at(0);
		lists.removeAll(list);
		delete list;
	}
	lists = mTargets.values();
	while(!lists.empty()) {
		QList<Neuron*> *list = lists.at(0);
		lists.removeAll(list);
		delete list;
	}

	return allNeuronsConnected;
}


bool ConnectNeuronClassesFilter::hasPathToNeuronClass(Neuron *targetNeuron, QList<Neuron*> &neuronsToCheck) {
	if(targetNeuron == 0) {
		return false;
	}

	if(neuronsToCheck.empty()) {
		return false;
	}
	QList<Neuron*> newNeuronList;
	for(QListIterator<Neuron*> i(neuronsToCheck); i.hasNext();) {
		Neuron *neuron = i.next();
		if(neuron == targetNeuron) {
			return true;
		}
		if(!mVisitedNeurons.contains(neuron)) {
			mVisitedNeurons.append(neuron);
			
			QList<Synapse*> synapses = neuron->getSynapses();
			for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
				Synapse *synapse = j.next();
				Neuron *target = synapse->getSource();
				if(target == 0) {
					continue;
				}
				if(!mVisitedNeurons.contains(target)) {
					newNeuronList.append(target);
				}
			}
		}
	}
	return hasPathToNeuronClass(targetNeuron, newNeuronList);
}

bool ConnectNeuronClassesFilter::hasPathFromNeuronClass(Neuron *targetNeuron, QList<Neuron*> &neuronsToCheck) {
	if(targetNeuron == 0) {
		return false;
	}

	if(neuronsToCheck.empty()) {
		return false;
	}
	QList<Neuron*> newNeuronList;
	for(QListIterator<Neuron*> i(neuronsToCheck); i.hasNext();) {
		Neuron *neuron = i.next();
		if(neuron == targetNeuron) {
			return true;
		}
		if(!mVisitedNeurons.contains(neuron)) {
			mVisitedNeurons.append(neuron);
			
			QList<Synapse*> synapses = neuron->getOutgoingSynapses();
			for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
				Synapse *synapse = j.next();
				Neuron *target = dynamic_cast<Neuron*>(synapse->getTarget());
				if(target == 0) {
					continue;
				}
				if(!mVisitedNeurons.contains(target)) {
					newNeuronList.append(target);
				}
			}
		}
	}
	return hasPathFromNeuronClass(targetNeuron, newNeuronList);

}


}



