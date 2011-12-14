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



#include "InsertSynapseModularOperator.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Core/Core.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Math/Random.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include "Core/Properties.h"
#include <iostream>
#include "Evolution/Evolution.h"
#include <limits>
#include "Network/NeuroTagManager.h"
#include "EvolutionConstants.h"
#include "Util/Util.h"
#include "Math/Math.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new InsertSynapseModularOperator.
 */
InsertSynapseModularOperator::InsertSynapseModularOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mInsertionProbability(0),
	  mMaximalNumberOfNewSynapses(0)
{
	mInsertionProbability = new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0);
	mInitialInsertionProbability = new NormalizedDoubleValue(0.9, 0.0, 1.0, 0.0, 1.0);
	mMaximalNumberOfNewSynapses = new IntValue(10);

	addParameter("InsertionProbability", mInsertionProbability);
	addParameter("MaxNumberOfNewSynapses", mMaximalNumberOfNewSynapses);
	addParameter("InitInsertionProbability", mInitialInsertionProbability);
	
	NeuroTagManager *ntm = NeuroTagManager::getInstance();
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_IN_SYNAPSES,
						NeuralNetworkConstants::TAG_TYPE_NEURON,
						"Specifies the maximal number of incomming synapses for this neuron."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_OUT_SYNAPSES,
						NeuralNetworkConstants::TAG_TYPE_NEURON,
						"Specifies the maximal number of outgoing synapses for this neuron."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NEURON_SOURCE_EXCLUDE_LIST,
						NeuralNetworkConstants::TAG_TYPE_NEURON,
						"A list of neuron ids from which no synaptic connections are allowed (unidirectional)."));
}


/**
 * Copy constructor. 
 * 
 * @param other the InsertSynapseModularOperator object to copy.
 */
InsertSynapseModularOperator::InsertSynapseModularOperator(const InsertSynapseModularOperator &other) 
	: NeuralNetworkManipulationOperator(other)
{
	mInsertionProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("InsertionProbability"));
	mInitialInsertionProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("InitInsertionProbability"));
	mMaximalNumberOfNewSynapses = dynamic_cast<IntValue*>(
			getParameter("MaxNumberOfNewSynapses"));
}

/**
 * Destructor.
 */
InsertSynapseModularOperator::~InsertSynapseModularOperator() {
}


NeuralNetworkManipulationOperator* InsertSynapseModularOperator::createCopy() const
{
	return new InsertSynapseModularOperator(*this);
}



bool InsertSynapseModularOperator::applyOperator(Individual *individual, CommandExecutor*) {

	ModularNeuralNetwork *net = dynamic_cast<ModularNeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("InsertSynapseOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	int currentNumberOfSynapses = net->getSynapses().size();

	//prepare the generation date as string.
	mGenerationDate = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());

	int maxNumberOfNewSynapses = mMaximalNumberOfNewSynapses->get();
	if(maxNumberOfNewSynapses <= 0) {
		return true;
	}

	QList<Neuron*> networkNeurons = net->getNeurons();
	QList<Neuron*> consideredNeurons(networkNeurons);
	for(QListIterator<Neuron*> i(networkNeurons); i.hasNext();) {
		//remove all neurons that can not be changed.
		Neuron *neuron = i.next();
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
			consideredNeurons.removeAll(neuron);
			continue;
		}
		//TODO add more
	}

	mConsideredSourceNeurons = consideredNeurons;
	for(QListIterator<Neuron*> i(consideredNeurons); i.hasNext();) {
		//remove all neurons that can not be a source neuron.
		Neuron *neuron = i.next();
		if(neuron->hasProperty("ProtectSynapses") //TODO remove this tag (replaced by NoSynapseSource)
			|| neuron->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_SOURCE))
		{
			mConsideredSourceNeurons.removeAll(neuron);
			continue;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_OUT_SYNAPSES)) {
			bool ok = true;
			int max = neuron->getProperty(NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_OUT_SYNAPSES)
										.toInt(&ok);
			if(ok && neuron->getOutgoingSynapses().size() >= max) {
				mConsideredSourceNeurons.removeAll(neuron);
				continue;
			}
		}
	}
	
	mConsideredTargetNeurons = consideredNeurons;
	for(QListIterator<Neuron*> i(consideredNeurons); i.hasNext();) {
		//remove all neurons that can not be a source neuron.
		Neuron *neuron = i.next();
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_TARGET)) {
			mConsideredTargetNeurons.removeAll(neuron);
			continue;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_INPUT_NEURON)) {
			mConsideredTargetNeurons.removeAll(neuron);
			continue;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_IN_SYNAPSES)) {
			bool ok = true;
			int max = neuron->getProperty(NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_IN_SYNAPSES)
										.toInt(&ok);
			if(ok && neuron->getSynapses().size() >= max) {
				mConsideredTargetNeurons.removeAll(neuron);
				continue;
			}
		}
	}
	
	if(mConsideredSourceNeurons.empty() || mConsideredTargetNeurons.empty()) {
		return true;
	}
	
	//cerr << "1s: " << mConsideredSourceNeurons.size() << "t: " << mConsideredTargetNeurons.size() << endl;

	updatePermittedPaths(net);
	
// 	cerr << "Permitted by source" << endl;
// 	for(QHashIterator<NeuroModule*, QList<NeuroModule*>*> i(mPermittedPathsBySource); i.hasNext();) {
// 		i.next();
// 		QList<NeuroModule*> *list = i.value();
// 		for(QListIterator<NeuroModule*> j(*list); j.hasNext();) {
// 			cerr << i.key()->getId() << " -> " << j.next()->getId() << endl;
// 		}
// 	}
// 	cerr << "Permitted from dest" << endl;
// 	for(QHashIterator<NeuroModule*, QList<NeuroModule*>*> i(mPermittedPathsFromDest); i.hasNext();) {
// 		i.next();
// 		QList<NeuroModule*> *list = i.value();
// 		for(QListIterator<NeuroModule*> j(*list); j.hasNext();) {
// 			cerr << i.key()->getId() << " -> " << j.next()->getId() << endl;
// 		}
// 	}
// 	cerr << "Forbidden by source" << endl;
// 	for(QHashIterator<NeuroModule*, QList<NeuroModule*>*> i(mForbiddenPathsBySource); i.hasNext();) {
// 		i.next();
// 		QList<NeuroModule*> *list = i.value();
// 		for(QListIterator<NeuroModule*> j(*list); j.hasNext();) {
// 			cerr << i.key()->getId() << " -> " << j.next()->getId() << endl;
// 		}
// 	}
	
	connectNewNeurons(net);
	
	randomlyConnect(net);
	
	//cerr << "4s: " << mConsideredSourceNeurons.size() << "t: " << mConsideredTargetNeurons.size() << endl;
	
	if(net->getSynapses().size() != currentNumberOfSynapses) {
		//mark the individual as significantly modified
		individual->setProperty(EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE,
									mGenerationDate);
	}

	return true;
}


bool InsertSynapseModularOperator::connectNewNeurons(ModularNeuralNetwork *net) {
	if(net == 0) {
		return false;
	}

	QList<Neuron*> allNeurons = net->getNeurons();
	for(QListIterator<Neuron*> i(allNeurons); i.hasNext();) {
		Neuron *neuron = i.next();

		if(!neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_INITIAL_CONNECTION_PROPORTION)) {
			continue;
		}

		double connectionProportion = Math::min(1.0, Math::max(0.0, neuron->getProperty(
					NeuralNetworkConstants::TAG_NEURON_INITIAL_CONNECTION_PROPORTION).toDouble()));
		
		neuron->removeProperty(NeuralNetworkConstants::TAG_NEURON_INITIAL_CONNECTION_PROPORTION);

		if(Math::compareDoubles(connectionProportion, 0.0, 0.0000001)) {
			continue;
		}

		int numberOfPossibleInputConnections = getValidSourceNeurons(neuron, net).size();
		int numberOfPossibleOutputConnections = getValidTargetNeurons(neuron, net).size();

		//calculate the desired number of synapses (rounded up).
		double cons = ((double) numberOfPossibleInputConnections) * connectionProportion;
		numberOfPossibleInputConnections = cons - ((int) cons) > 0.0 ? ((int) cons) + 1 : ((int) cons);
		
		cons = ((double) numberOfPossibleOutputConnections) * connectionProportion;
		numberOfPossibleOutputConnections = cons - ((int) cons) > 0.0 ? ((int) cons) + 1 : ((int) cons);

		numberOfPossibleInputConnections -= neuron->getSynapses().size();
		numberOfPossibleOutputConnections -= neuron->getOutgoingSynapses().size();

		double initInsertionProb = mInitialInsertionProbability->get();
		
		updateConsideredNeurons();

		if(mConsideredTargetNeurons.contains(neuron)) {
			for(int i = 0; i < numberOfPossibleInputConnections; ++i) {
				if(Random::nextDouble() <= initInsertionProb) {
					addSynapseToTargetNeuron(neuron, net);
				}
			}
		}
		if(mConsideredSourceNeurons.contains(neuron)) {
			for(int i = 0; i < numberOfPossibleOutputConnections; ++i) {
				if(Random::nextDouble() <= initInsertionProb) {
					addSynapseToSourceNeuron(neuron, net);
				}
			}
		}
		
	}

	return true;
}


bool InsertSynapseModularOperator::randomlyConnect(ModularNeuralNetwork *net) {


	if(net == 0) {
		return false;
	}

	double probability = mInsertionProbability->get();
	int maxNumberOfNewSynapses = mMaximalNumberOfNewSynapses->get();
	if(maxNumberOfNewSynapses <= 0) {
		return true;
	}
	
	updateConsideredNeurons();
	
	for(int i = 0; i < maxNumberOfNewSynapses && !mConsideredSourceNeurons.empty()
			&& !mConsideredTargetNeurons.empty(); ++i) 
	{

		if(Random::nextDouble() > probability) {
			continue;
		}

		//choose the smaller stack as heuristic
		//if(mConsideredSourceNeurons.size() <= mConsideredTargetNeurons.size()) {
		if(true) {
			//select source neuron
			Neuron *source = mConsideredSourceNeurons.value(
						Random::nextInt(mConsideredSourceNeurons.size()));
	
			addSynapseToSourceNeuron(source, net);
		}
		else { //TODO this branch seems to be faulty!
			//select target neuron
			Neuron *target = mConsideredTargetNeurons.value(
						Random::nextInt(mConsideredTargetNeurons.size()));
			
			addSynapseToTargetNeuron(target, net);
		}
		
		updateConsideredNeurons();
	}

	return true;
}


bool InsertSynapseModularOperator::addSynapseToSourceNeuron(Neuron *source, ModularNeuralNetwork *net) {

	if(source == 0) {
		return true;
	}

	QList<Neuron*> rootNeurons = net->getRootNeurons();

	//select target candidates
	QList<SynapseTarget*> targetCandidates;

	QList<Neuron*> allInputModuleNeurons = getValidTargetNeurons(source, net);

	//make sure that the target candidates contain only considerable neurons (not protected etc.)
	QList<Neuron*> validTargetCandidates;
	for(QListIterator<Neuron*> k(allInputModuleNeurons); k.hasNext();) {
		Neuron *neuron = k.next();

		if(!mConsideredTargetNeurons.contains(neuron)) {
			continue;
		}			

		//remove all targets that already are target of a synapse from neuron source
		QList<Synapse*> incommingSynapses = neuron->getSynapses();

		bool validTarget = true;

		for(QListIterator<Synapse*> m(incommingSynapses); m.hasNext();) {
			Synapse *inSyn = m.next();
			if(inSyn != 0 && inSyn->getSource() == source) {
				validTarget = false;
				break;
			}
		}	
		if(!validTarget) {
			continue;
		}

		
		validTargetCandidates.append(neuron);
	}

	bool foundValidSynapseConnection = false;
	while(!foundValidSynapseConnection) {

		if(validTargetCandidates.empty()) {
			//make sure that the same neuron is not selected again.
			mConsideredSourceNeurons.removeAll(source);
			return false;
		}
	
		Neuron *synapseTarget = validTargetCandidates.value(
					Random::nextInt(validTargetCandidates.size()));
	
		if(synapseTarget == 0 || source == 0) {
			return false;
		}

		//check if these neurons are allowed to be connected.
		if(!canBeConnected(source, synapseTarget, net)) {
			validTargetCandidates.removeAll(synapseTarget);
			continue;
		}
	

		foundValidSynapseConnection = true;
		//create synapse
		Synapse *synapse = Synapse::createSynapse(source, synapseTarget, 0.0, 
								*(net->getDefaultSynapseFunction()));
	
		synapse->setProperty(EvolutionConstants::TAG_EVO_CREATION_DATE, mGenerationDate);
		synapseTarget->addSynapse(synapse);
	
		//mark as modified.
		synapse->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
	}
	return true;
}

bool InsertSynapseModularOperator::addSynapseToTargetNeuron(Neuron *target, ModularNeuralNetwork *net) {

	if(target == 0) {
		return true;
	}

	QList<Neuron*> rootNeurons = net->getRootNeurons();

	//select source candidates
	QList<Neuron*> sourceCandidates;

	QList<Neuron*> allOutputModuleNeurons = 
				getValidSourceNeurons(target, net);

	//make sure that the target candidates contain only considerable neurons (not protected etc.)
	QList<Neuron*> validSourceCandidates;
	for(QListIterator<Neuron*> k(allOutputModuleNeurons); k.hasNext();) {
		Neuron *neuron = k.next();

		if(!mConsideredSourceNeurons.contains(neuron)) {
			continue;
		}			

		//remove all targets that already are target of a synapse from neuron source
		QList<Synapse*> outgoingSynapses = neuron->getOutgoingSynapses();

		bool validSource = true;

		for(QListIterator<Synapse*> m(outgoingSynapses); m.hasNext();) {
			Synapse *outSyn = m.next();
			if(outSyn != 0 && outSyn->getTarget() == target) {
				validSource = false;
				break;
			}
		}	
		if(!validSource) {
			continue;
		}

		validSourceCandidates.append(neuron);
	}

	bool foundValidSynapseConnection = false;

	while(!foundValidSynapseConnection) {

		if(validSourceCandidates.empty()) {
			//make sure that the same neuron is not selected again.
			mConsideredTargetNeurons.removeAll(target);
			return false;
		}
	
		Neuron *synapseSource = validSourceCandidates.value(
					Random::nextInt(validSourceCandidates.size()));
	
		if(synapseSource == 0 || target == 0) {
			return false;
		}

		//check if these neurons are allowed to be connected.
		if(!canBeConnected(synapseSource, target, net)) {
			validSourceCandidates.removeAll(synapseSource);
			continue;
		}
	
		foundValidSynapseConnection = true;

		//create synapse
		Synapse *synapse = Synapse::createSynapse(synapseSource, target, 0.0, 
								*(net->getDefaultSynapseFunction()));
	
		synapse->setProperty(EvolutionConstants::TAG_EVO_CREATION_DATE, mGenerationDate);
		target->addSynapse(synapse);
	
		//mark as modified.
		synapse->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
	}

	return true;
}


QList<Neuron*> InsertSynapseModularOperator::getValidSourceNeurons(Neuron *target, ModularNeuralNetwork *net)
{
	QList<Neuron*> allOutputModuleNeurons;

	if(target == 0 || net == 0) {
		return allOutputModuleNeurons;
	}

	int interfaceLevel = getInterfaceLevel(target, NeuralNetworkConstants::TAG_MODULE_INPUT);
	bool isInterface = interfaceLevel > 0 ? true : false;

	//crawl down into parent modules as deep as the interface level allows.
	NeuroModule *parentModule = net->getOwnerModule(target);
	NeuroModule *previousModule = parentModule;
	int k = 0;
	for(; k <= interfaceLevel; ++k) {

		if(parentModule != 0) {
			Util::addWithoutDuplicates(allOutputModuleNeurons, parentModule->getNeurons());

			for(QListIterator<NeuroModule*> j(parentModule->getSubModules()); j.hasNext();) {
				Util::addWithoutDuplicates(allOutputModuleNeurons, j.next()->getOutputNeurons());
			}
			if(isInterface) {
				previousModule = parentModule;
				parentModule = parentModule->getParentModule();
			}
			else {
				break;
			}
		}
		else {
			Util::addWithoutDuplicates(allOutputModuleNeurons, net->getRootNeurons());
			QList<NeuroModule*> rootModules = net->getRootModules();
			for(QListIterator<NeuroModule*> i(rootModules); i.hasNext();) {
				Util::addWithoutDuplicates(allOutputModuleNeurons, i.next()->getOutputNeurons());
			}
			break;
		}
	}

	return allOutputModuleNeurons;
}


QList<Neuron*> InsertSynapseModularOperator::getValidTargetNeurons(Neuron *source, ModularNeuralNetwork *net)
{
	QList<Neuron*> allInputModuleNeurons;

	if(source == 0 || net == 0) {
		return allInputModuleNeurons;
	}

	int interfaceLevel = getInterfaceLevel(source, NeuralNetworkConstants::TAG_MODULE_OUTPUT);
	bool isInterface = interfaceLevel > 0 ? true : false;

	//crawl down into parent modules as deep as the interface level allows.
	NeuroModule *parentModule = net->getOwnerModule(source);
	NeuroModule *previousModule = parentModule;
	int k = 0;
	for(; k <= interfaceLevel; ++k) {

		if(parentModule != 0) {
			Util::addWithoutDuplicates(allInputModuleNeurons, parentModule->getNeurons());

			for(QListIterator<NeuroModule*> j(parentModule->getSubModules()); j.hasNext();) {
				Util::addWithoutDuplicates(allInputModuleNeurons, j.next()->getInputNeurons());
			}
			if(isInterface) {
				previousModule = parentModule;
				parentModule = parentModule->getParentModule();
			}
			else {
				break;
			}
		}
		else {
			Util::addWithoutDuplicates(allInputModuleNeurons, net->getRootNeurons());
			QList<NeuroModule*> rootModules = net->getRootModules();
			for(QListIterator<NeuroModule*> i(rootModules); i.hasNext();) {
				Util::addWithoutDuplicates(allInputModuleNeurons, i.next()->getInputNeurons());
			}
			break;
		}
	}

	return allInputModuleNeurons;
}

int InsertSynapseModularOperator::getInterfaceLevel(Neuron *neuron, const QString &moduleInterfaceType) {
	
	if(moduleInterfaceType == "") {
		return 0;
	}
	bool neuronIsInterfaceNeuron = neuron->hasProperty(moduleInterfaceType);
	bool neuronIsExtendedInterfaceNeuron = 
			neuron->hasProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	QString levelString = neuron->getProperty(moduleInterfaceType);
	if(neuronIsExtendedInterfaceNeuron) {
		levelString = neuron->getProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE);
	}

	int interfaceLevel = 0;
	if(neuronIsInterfaceNeuron) {
		interfaceLevel = 1;

		if(neuronIsExtendedInterfaceNeuron && levelString.trimmed() == "") {
			interfaceLevel = numeric_limits<int>::max();
		}
		else {
			bool ok = true;
			int level = levelString.toInt(&ok);
			if(ok) {
				interfaceLevel =  level + 1; //TODO adapt neuroModule::getInput/Outputs
			}
		}
	}
	return interfaceLevel;
}


// QList<NeuroModule*> InsertSynapseModularOperator::getMatchingModules(
// 						const QString &matchingList, NeuroModule *owner) 
// {
// 	if(owner == 0 || owner->getOwnerNetwork() == 0) {
// 		return QList<NeuroModule*>();
// 	}
// 	QList<NeuroModule*> allModules = owner->getOwnerNetwork()->getNeuroModules();
// 	QList<NeuroModule*> matchingModules;
// 
// 	QString matchingListString = matchingList;
// 	QStringList entries = matchingList.split("|");
// 
// 	for(QListIterator<QString> i(entries); i.hasNext();) {
// 		QString entry = i.next();
// 		if(entry.contains(",")) {
// 			//get id
// 			QStringList ids = entry.split(",");
// 			if(ids.size() == 2) {
// 				bool ok1 = false;
// 				bool ok2 = false;
// 				qulonglong id1 = ids.at(0).toULongLong(&ok1);
// 				qulonglong id2 = ids.at(1).toULongLong(&ok2);
// 
// 				if(ok1 && ok2) {
// 					qulonglong targetId = id1;
// 					if(id1 == owner->getId()) {
// 						targetId = id2;
// 					}
// 					else if(id2 != owner->getId()) {
// 						//ignore this entry
// 						continue;
// 					}
// 					for(QListIterator<NeuroModule*> j(allModules); j.hasNext();) {
// 						NeuroModule *module = j.next();
// 						if(module->getId() == targetId) {
// 							if(!matchingModules.contains(module)) {
// 								matchingModules.append(module);
// 							}
// 							break;
// 						}
// 					}
// 				}
// 			}
// 		}
// 		else {
// 			for(QListIterator<NeuroModule*> j(allModules); j.hasNext();) {
// 				NeuroModule *module = j.next();
// 				QString type = module->getProperty(NeuralNetworkConstants::TAG_MODULE_TYPE).trimmed();
// 				entry = entry.trimmed();
// 				if(type != "" && (entry == type || entry == "*")) {
// 					if(!matchingModules.contains(module)) {
// 						matchingModules.append(module);
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return matchingModules;
// }


/**
 * Creates the internal lookup structure for all possible module-to-module relations.
 */
void InsertSynapseModularOperator::updatePermittedPaths(ModularNeuralNetwork *net) {
	
	//clear pathway hashtables.
	for(QHashIterator<NeuroModule*, QList<NeuroModule*>*> i(mPermittedPathsBySource); i.hasNext();) {
		i.next();
		QList<NeuroModule*> *list = i.value();
		if(list != 0) {
			delete list;
		}
	}
	for(QHashIterator<NeuroModule*, QList<NeuroModule*>*> i(mForbiddenPathsBySource); i.hasNext();) {
		i.next();
		QList<NeuroModule*> *list = i.value();
		if(list != 0) {
			delete list;
		}
	}
	for(QHashIterator<NeuroModule*, QList<NeuroModule*>*> i(mPermittedPathsFromDest); i.hasNext();) {
		i.next();
		QList<NeuroModule*> *list = i.value();
		if(list != 0) {
			delete list;
		}
	}
	mPermittedPathsBySource.clear();
	mForbiddenPathsBySource.clear();
	mModulesWithPermissionTag.clear();
	mPermittedPathsFromDest.clear();

	if(net == 0) {
		return;
	}

	QList<NeuroModule*> sources;
	QList<NeuroModule*> targets;

	//check if there are any pathway descriptions
	if(net->hasProperty(NeuralNetworkConstants::TAG_MODULE_PERMITTED_PATHWAYS)) {

		QList<NeuroModule*> allModules = net->getNeuroModules();
	
		QString pathways = net->getProperty(NeuralNetworkConstants::TAG_MODULE_PERMITTED_PATHWAYS);
		QStringList entries = pathways.split("|");
	
		for(QListIterator<QString> i(entries); i.hasNext();) {
			QString entry = i.next();
			//get id
			QStringList ids = entry.split(",");
			if(ids.size() == 2) {
				bool ok1 = false;
				bool ok2 = false;
				qulonglong id1 = ids.at(0).toULongLong(&ok1);
				qulonglong id2 = ids.at(1).toULongLong(&ok2);
	
				if(ok1 && ok2) {
					NeuroModule *source = ModularNeuralNetwork::selectNeuroModuleById(id1, allModules);
					NeuroModule *target = ModularNeuralNetwork::selectNeuroModuleById(id2, allModules);
	
					if(source != 0 && target != 0) {

						//collect the found modules for later usage and mark the source as restricted.
						sources.append(source);
						targets.append(target);
						if(!mModulesWithPermissionTag.contains(source)) {
							mModulesWithPermissionTag.append(source);
						}
					}
				}
			}
		}
		//Collect the connection relations for all modules and store them in the hashtables.
		QListIterator<NeuroModule*> s(sources);
		QListIterator<NeuroModule*> t(targets);
		for(; s.hasNext() && t.hasNext();) {
			NeuroModule *source = s.next();
			NeuroModule *target = t.next();

			QList<NeuroModule*> *permittedBySource = 0;
			QList<NeuroModule*> *forbiddenBySource = 0;
			if(mPermittedPathsBySource.keys().contains(source)) {
				permittedBySource = mPermittedPathsBySource.value(source);
				forbiddenBySource = mForbiddenPathsBySource.value(source);
			}
			if(permittedBySource == 0) {
				permittedBySource = new QList<NeuroModule*>();
				mPermittedPathsBySource.insert(source, permittedBySource);
			}
			if(forbiddenBySource == 0) {
				forbiddenBySource = new QList<NeuroModule*>();
				//mForbiddenPathsBySource.insert(target, forbiddenBySource);
				mForbiddenPathsBySource.insert(source, forbiddenBySource); 
			}
			if(!permittedBySource->contains(target)) {
				permittedBySource->append(target);

				QList<NeuroModule*> *permittedFrom = mPermittedPathsFromDest.value(target);
				if(permittedFrom == 0) {
					permittedFrom = new QList<NeuroModule*>();
					mPermittedPathsFromDest.insert(target, permittedFrom);
				}
				if(!permittedFrom->contains(source)) {
					permittedFrom->append(source);
				}
			}

			QList<NeuroModule*> permittedSubmodules;

			QList<NeuroModule*> sourceSubModules = source->getAllEnclosedModules();
			//TODO next ok?
			//sourceSubModules.prepend(source);
			for(QListIterator<NeuroModule*> k(sourceSubModules); k.hasNext();) {
				NeuroModule *module = k.next();
				if(mModulesWithPermissionTag.contains(module)) {
					continue;
				}
				QList<NeuroModule*> *permittedTo = mPermittedPathsBySource.value(module);
				if(permittedTo == 0) {
					permittedTo = new QList<NeuroModule*>();
					mPermittedPathsBySource.insert(module, permittedTo);
				}
				if(!permittedTo->contains(target)) {
					permittedTo->append(target);
				}
				permittedSubmodules.append(module);
			}

			QList<NeuroModule*> permittedTargetSubmodules;
			QList<NeuroModule*> submodules = target->getAllEnclosedModules();
			submodules.prepend(target);
			for(QListIterator<NeuroModule*> k(submodules); k.hasNext();) {
				NeuroModule *module = k.next();
				if(!permittedBySource->contains(module) && 
					(module == target || !mModulesWithPermissionTag.contains(module)))
				{
					permittedBySource->append(module);
				}
				QList<NeuroModule*> *permittedFrom = mPermittedPathsFromDest.value(module);
				if(permittedFrom == 0) {
					permittedFrom = new QList<NeuroModule*>();
					mPermittedPathsFromDest.insert(module, permittedFrom);
				}
				if(!permittedFrom->contains(source)) {
					permittedFrom->append(source);
				}
				for(QListIterator<NeuroModule*> l(permittedSubmodules); l.hasNext();) {
					NeuroModule *m = l.next();
					if(!permittedFrom->contains(m)) {
						permittedFrom->append(m);
					}
				}
				permittedTargetSubmodules.append(module);
			}

			for(QListIterator<NeuroModule*> k(sourceSubModules); k.hasNext();) {
				NeuroModule *module = k.next();
				if(mModulesWithPermissionTag.contains(module)) {
					continue;
				}
				QList<NeuroModule*> *permittedTo = mPermittedPathsBySource.value(module);
				if(permittedTo == 0) {
					permittedTo = new QList<NeuroModule*>();
					mPermittedPathsBySource.insert(module, permittedTo);
				}
				if(!permittedTo->contains(target)) {
					permittedTo->append(target);
				}
				for(QListIterator<NeuroModule*> l(permittedTargetSubmodules); l.hasNext();) {
					NeuroModule *m = l.next();
					if(!permittedTo->contains(m)) {
						permittedTo->append(m);
					}
				}
			}
			
		}
	}

// 	for(QHashIterator<NeuroModule*, QList<NeuroModule*>*> i(mPermittedPathsBySource); i.hasNext();) {
// 		i.next();
// 		QString message;
// 		for(QListIterator<NeuroModule*> j(*(i.value())); j.hasNext();) {
// 			message.append(QString::number(j.next()->getId())).append(",");
// 		}
// 		i.key()->setProperty("ToModules", message);
// 	}
// 	for(QHashIterator<NeuroModule*, QList<NeuroModule*>*> i(mPermittedPathsFromDest); i.hasNext();) {
// 		i.next();
// 		QString message;
// 		for(QListIterator<NeuroModule*> j(*(i.value())); j.hasNext();) {
// 			message.append(QString::number(j.next()->getId())).append(",");
// 		}
// 		i.key()->setProperty("FromModules", message);
// 	}
	
}


/**
 * Returns true if a synapse between source and target is valid. 
 */
bool InsertSynapseModularOperator::canBeConnected(Neuron *source, Neuron *target, ModularNeuralNetwork *net) {

	if(source == 0 || target == 0 || net == 0) {
		return false;
	}
	
	//support the explicit exclusion of source neurons
	if(target->hasProperty(NeuralNetworkConstants::TAG_NEURON_SOURCE_EXCLUDE_LIST)) {
		QString excludeList = target->getProperty(NeuralNetworkConstants::TAG_NEURON_SOURCE_EXCLUDE_LIST);
		excludeList.replace("|", ",");
		QStringList idStrings = excludeList.split(",");
		for(QListIterator<QString> idl(idStrings); idl.hasNext();) {
			bool ok = true;
			qulonglong id = idl.next().toULongLong(&ok);
			if(ok && id == source->getId()) {
				return false;
			}
		}
	}
	

	NeuroModule *sourceModule = net->getOwnerModule(source);
	NeuroModule *targetModule = net->getOwnerModule(target);

	//todo check root neurons
	if(sourceModule == 0 && targetModule == 0) {
		return true;
	}
	if(sourceModule == targetModule) {
		//connections in the same module are always possible.
		return true;
	}

	QList<NeuroModule*> *permittedFrom = mPermittedPathsFromDest.value(targetModule);

	//walk through the sourceModule and all of its parents until a restriction is found.
	NeuroModule *parentModule = sourceModule;
	while(parentModule != 0) {
		QList<NeuroModule*> *pathwaysBySource = mPermittedPathsBySource.value(parentModule);

		if(pathwaysBySource != 0) {
			if(pathwaysBySource->contains(targetModule)) {
				if(permittedFrom == 0) {
					return true;
				}
				else if(permittedFrom->contains(sourceModule)) {
					return true;
				}
				return false;
			}
			else {
				return false;
			}
		}

		parentModule = parentModule->getParentModule();
	}
	if(permittedFrom != 0) {
		return false;
	}
	//no restriction was found
	return true;
}


/**
 * Removes neurons that are not further considered because of dynamic properties,
 * whose conditions may change after each insertion (e.g. restrictions in number of
 * synapses.
 */
void InsertSynapseModularOperator::updateConsideredNeurons() {
	QList<Neuron*> consideredNeurons = mConsideredSourceNeurons;
	for(QListIterator<Neuron*> i(consideredNeurons); i.hasNext();) {
		//remove all neurons that can not be a source neuron.
		Neuron *neuron = i.next();
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_OUT_SYNAPSES)) {
			bool ok = true;
			int max = neuron->getProperty(NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_OUT_SYNAPSES)
										.toInt(&ok);
			if(ok && neuron->getOutgoingSynapses().size() >= max) {
				mConsideredSourceNeurons.removeAll(neuron);
				continue;
			}
		}
	}
	
	consideredNeurons = mConsideredTargetNeurons;
	for(QListIterator<Neuron*> i(consideredNeurons); i.hasNext();) {
		//remove all neurons that can not be a source neuron.
		Neuron *neuron = i.next();
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_IN_SYNAPSES)) {
			bool ok = true;
			int max = neuron->getProperty(NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_IN_SYNAPSES)
										.toInt(&ok);
			if(ok && neuron->getSynapses().size() >= max) {
				mConsideredTargetNeurons.removeAll(neuron);
				continue;
			}
		}
	}
}

}



