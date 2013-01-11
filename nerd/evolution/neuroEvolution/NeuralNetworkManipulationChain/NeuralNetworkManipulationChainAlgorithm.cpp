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

#include "NeuralNetworkManipulationChainAlgorithm.h"
#include "Evolution/Population.h"
#include "Evolution/World.h"
#include "Evolution/Individual.h"
#include "Network/NeuralNetwork.h"
#include <QListIterator>
#include "Core/Core.h"
#include <iostream>
#include "Core/SystemObject.h"
#include "Core/Properties.h"
#include "NeuralNetworkConstants.h"
#include "NeuralNetworkManipulationChain/OperatorSorter.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/Evolution.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "IO/NeuralNetworkIO.h"
#include "EvolutionConstants.h"

using namespace std;

//TODO make manipulationChain and operators independent of neuralnetwork.

namespace nerd {

NeuralNetworkManipulationChainAlgorithm::NeuralNetworkManipulationChainAlgorithm(const QString &name)
	: EvolutionAlgorithm(name)
{
	mCurrentStateValue = new StringValue();
	mVerboseState = new BoolValue(false);
	
	mEnableMutationHistory = new BoolValue(false);
	mMutationHistoryDoc = new StringValue("");
	mMutationHistoryHide = new BoolValue(true);
	mMutationHistoryCanBeDisabled = new BoolValue(true);
	mMutationHistoryIndex = new IntValue(-10);

	addParameter("CurrentState", mCurrentStateValue);
	addParameter("VerboseState", mVerboseState);
	addParameter("EnableMutationHistory", mEnableMutationHistory);
	
	addParameter("MutationHistory/Config/Enable", mEnableMutationHistory);
	addParameter("MutationHistory/Config/Doc", mMutationHistoryDoc);
	addParameter("MutationHistory/Config/Hidden", mMutationHistoryHide);
	addParameter("MutationHistory/Config/CanBeDisabled", mMutationHistoryCanBeDisabled);
	addParameter("MutationHistory/Config/OperatorIndex", mMutationHistoryIndex);
	
	EventManager *em = Core::getInstance()->getEventManager();
	mGenerateIndividualStartedEvent = em->getEvent(
				EvolutionConstants::EVENT_EVO_GENERATE_INDIVIDUAL_STARTED, true);
	mGenerateIndividualCompletedEvent = em->getEvent(
				EvolutionConstants::EVENT_EVO_GENERATE_INDIVIDUAL_COMPLETED, true);
	
}

NeuralNetworkManipulationChainAlgorithm::NeuralNetworkManipulationChainAlgorithm(
				const NeuralNetworkManipulationChainAlgorithm &other)
	: Object(), ValueChangedListener(), EvolutionAlgorithm(other)
{
	mOperators.clear();
	for(QListIterator<NeuralNetworkManipulationOperator*> i(other.mOperators); i.hasNext();) {
		addOperator(i.next()->createCopy());
	}
	
	mCurrentStateValue = dynamic_cast<StringValue*>(getParameter("CurrentState"));
	mVerboseState = dynamic_cast<BoolValue*>(getParameter("VerboseState"));
	mEnableMutationHistory = dynamic_cast<BoolValue*>(getParameter("EnableMutationHistory"));
	
	mMutationHistoryDoc = dynamic_cast<StringValue*>(getParameter("MutationHistory/Config/Doc"));
	mMutationHistoryHide = dynamic_cast<BoolValue*>(getParameter("MutationHistory/Config/Hidden"));
	mMutationHistoryCanBeDisabled = dynamic_cast<BoolValue*>(getParameter("MutationHistory/Config/CanBeDisabled"));
	mMutationHistoryIndex = dynamic_cast<IntValue*>(getParameter("MutationHistory/Config/OperatorIndex"));
	
	EventManager *em = Core::getInstance()->getEventManager();
	mGenerateIndividualStartedEvent = em->getEvent(
				EvolutionConstants::EVENT_EVO_GENERATE_INDIVIDUAL_STARTED, true);
	mGenerateIndividualCompletedEvent = em->getEvent(
				EvolutionConstants::EVENT_EVO_GENERATE_INDIVIDUAL_COMPLETED, true);
}

/**
 * Destroys the Algorithm and all contained ManipulationOperators.
 */
NeuralNetworkManipulationChainAlgorithm::~NeuralNetworkManipulationChainAlgorithm() {
	while(!mOperators.empty()) {
		NeuralNetworkManipulationOperator *op = mOperators.at(0);
		mOperators.removeAll(op);
		op->unpublishAllParameters();
		//delete only operators that are NOT SystemObjects. //TODO check if this is save
		if(dynamic_cast<SystemObject*>(op) == 0) {
			delete op;
		}
	}
}

EvolutionAlgorithm* NeuralNetworkManipulationChainAlgorithm::createCopy() {
	return new NeuralNetworkManipulationChainAlgorithm(*this);
}

bool NeuralNetworkManipulationChainAlgorithm::addOperator(
			NeuralNetworkManipulationOperator *netOperator)
{
	if(netOperator == 0 || mOperators.contains(netOperator)) {
		return false;
	}
	mOperators.append(netOperator);
	netOperator->setPrefix(getPrefix().append(netOperator->getName()).append("/"));
	netOperator->publishAllParameters();
	return true;
}


bool NeuralNetworkManipulationChainAlgorithm::removeOperator(
			NeuralNetworkManipulationOperator *netOperator)
{
	if(netOperator == 0 || !mOperators.contains(netOperator)) {
		return false;
	}
	mOperators.removeAll(netOperator);
	netOperator->unpublishAllParameters();
	return true;
}


QList<NeuralNetworkManipulationOperator*> 
			NeuralNetworkManipulationChainAlgorithm::getOperators() const
{
	return mOperators;
}


bool NeuralNetworkManipulationChainAlgorithm::createNextGeneration(QList<Individual*> &trashcan) {

	bool verbose = false;

	if(mOwnerWorld == 0) {
		return false;
	}

	EvolutionManager *em = Evolution::getEvolutionManager();

	if(verbose) {
		Core::log("NeuralNetworkManipulationChainAlgorithm: createNextGeneration()", true);
	}

	if(mVerboseState->get()) {
		mCurrentStateValue->set(QString("Starting generation with ")
						+ QString::number(mOperators.size()) + " Operators.");
	}

	//sort operators
	OperatorSorter::sortByIndex(mOperators);

	Core *core = Core::getInstance();

	QList<Population*> populations = mOwnerWorld->getPopulations();
	for(QListIterator<Population*> i(populations); i.hasNext() && !core->isShuttingDown();) {
		Population *pop = i.next();
		QList<Individual*> individuals = pop->getIndividuals();
		
		resetOperators();

		int individualCounter = 0;
		for(QListIterator<Individual*> j(individuals); j.hasNext();) {

			em->setCurrentNumberOfCompletedIndividualsDuringEvolution(individualCounter);

			++individualCounter;

			core->executePendingTasks();
			if(core->isShuttingDown()) {
				break;
			}
			
			Individual *ind = j.next();
			
			mGenerateIndividualStartedEvent->trigger();

			//set index property
			ind->setProperty("Index", QString::number(individualCounter));

			if(verbose) {
				Core::log(QString("NNChain: Manipulating neuron with id ")
							.append(QString::number(ind->getId())), true);
			}

			QString individualIdentifierString = QString("Pop ") + pop->getName() + " Ind: "
						+ QString::number(individualCounter) + "/"
						+ QString::number(individuals.size());

			if(mVerboseState->get()) {
				mCurrentStateValue->set(individualIdentifierString);
			}

			//ignore protected individuals
			if(ind->isGenomeProtected()) {
				mGenerateIndividualCompletedEvent->trigger();
				continue;
			}

			if(ind->getGenome() != 0 && dynamic_cast<NeuralNetwork*>(ind->getGenome()) == 0) {
				Core::log("NeuralNetworkManipulationChainAlgorithm: Could not handle genome.");
				//delete individual from the population, because the genome is not a NeuralNetwork
				pop->getIndividuals().removeAll(ind);

				trashcan.append(ind); //move to thrash
				mGenerateIndividualCompletedEvent->trigger();
				continue;
			}

			bool modificationValid = false;
			bool stillApplicable = true;
			int iterationCounter = 0;

			
			int runCounter = 0;
			while(!modificationValid && stillApplicable && !core->isShuttingDown()) {
				modificationValid = true;
				stillApplicable = false;
				++runCounter;

				core->executePendingTasks();

				if(core->isShuttingDown()) {
					break;
				}

				if(verbose) {
					Core::log(QString("NNChain: Running chain in iteration ")
								.append(QString::number(iterationCounter)), true);
				}

				for(QListIterator<NeuralNetworkManipulationOperator*> k(mOperators); k.hasNext();) {
					NeuralNetworkManipulationOperator *op = k.next();
					if(op->getEnableOperatorValue()->get()
						&& op->getMaximalNumberOfApplicationsValue()->get() > iterationCounter) 
					{
						stillApplicable = true;

						if(verbose) {
							Core::log(QString("NNChain: Executing operator ")
										.append(op->getName()), true);
						}

						if(ind->isGenomeProtected()) {
							break;
						}

						if(mVerboseState->get()) {
							mCurrentStateValue->set(QString("(") + QString::number(runCounter) 
									+ ") " + op->getName() + " - " 
									+ individualIdentifierString);
						}

						if(!op->runOperator(ind, 0)) { //TODO use command executor
							//if false the genome was rejected by the operator.
							modificationValid = false;

							if(verbose) {
								Core::log("NNChain: Failed executing operator [" + op->getName() + "]", true);
							}
						}

						if(mVerboseState->get()) {
							mCurrentStateValue->set(QString(" (") 
									+ op->getLastExecutionTimeValue()->getValueAsString() 
									+ " ms) - valid: " + QString::number((int) modificationValid));
						}
					}
					core->executePendingTasks();
					if(core->isShuttingDown()) {
						break;
					}
				}

				iterationCounter++;
			}

			if(core->isShuttingDown()) {
				return true;
			}

			if(!modificationValid || stillApplicable == false) {
				//Could not find a suitable mutation, Individual is removed.
				Core::log("NeuralNetworkManipulationChainAlgorithm: Could not find a suitable "
						  "mutation for network.");
				ModularNeuralNetwork *net = dynamic_cast<ModularNeuralNetwork*>(ind->getGenome());
				if(net != 0) {
					NeuralNetworkIO::createFileFromNetwork("test.onn", net);
				}
				pop->getIndividuals().removeAll(ind);

				if(verbose) {
					Core::log(QString("NNChain: Deleting invalid individual [")
								.append(QString::number(ind->getId())), true);
				}
				//move to thrash
				trashcan.append(ind);
			}
			else {
				//remove modification markers and temporary markers (starting with __ and ending __
				NeuralNetwork *network = dynamic_cast<NeuralNetwork*>(ind->getGenome());
				
				//update mutation history (if enabled)
				if(mEnableMutationHistory->get() == true) {
					Properties *p = dynamic_cast<Properties*>(network);
					if(p != 0 && ind != 0) {
						QString mutations = ind->getProperty(EvolutionConstants::TAG_GENOME_CHANGE_SUMMARY).trimmed();

						if(mutations != "") {
							QString generationDate = QString::number(Evolution::getEvolutionManager()
									->getCurrentGenerationValue()->get());
							QString currentString = p->getProperty(EvolutionConstants::TAG_NETWORK_MUTATION_HISTORY).trimmed();

							QString newString = QString("|") + generationDate + ":" + mutations;

							p->setProperty(EvolutionConstants::TAG_NETWORK_MUTATION_HISTORY, 
												currentString + newString);
						}
					}
				}

				if(network != 0) {
					QList<NeuralNetworkElement*> elements;
					network->getNetworkElements(elements);

					for(QListIterator<NeuralNetworkElement*> k(elements); k.hasNext();) {
						NeuralNetworkElement *elem = k.next();
						Properties *p = dynamic_cast<Properties*>(elem);
						if(p != 0) {
							p->removeProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
							p->removePropertyByPattern("__.*__");
						}
					}
				}
			}
			
			mGenerateIndividualCompletedEvent->trigger();
		}
		em->setCurrentNumberOfCompletedIndividualsDuringEvolution(individuals.size());

		resetOperators();
	}

	if(verbose) {
		Core::log("NNEvoChainAlg: Done with execution.", true);
	}

	return true;
}


bool NeuralNetworkManipulationChainAlgorithm::reset() {
	return true;
}

void NeuralNetworkManipulationChainAlgorithm::resetOperators() {
	for(QListIterator<NeuralNetworkManipulationOperator*> k(mOperators); k.hasNext();) {
		k.next()->resetOperator();
	}
}



}


