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



#include "ModuleCrossOverOperator.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Evolution/Individual.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Math/Random.h"
#include "Util/ReplaceModuleHandler.h"
#include "Network/NeuroTagManager.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ModuleCrossOverOperator.
 */
ModuleCrossOverOperator::ModuleCrossOverOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mCrossoverProbability(0),
	  mProbabilityPerModule(0)
{
	mCrossoverProbability = new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0);
	mProbabilityPerModule = new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0);

	addParameter("CrossOverProbability", mCrossoverProbability);	
	addParameter("ProbabilityPerModule", mProbabilityPerModule);

}


/**
 * Copy constructor. 
 * 
 * @param other the ModuleCrossOverOperator object to copy.
 */
ModuleCrossOverOperator::ModuleCrossOverOperator(const ModuleCrossOverOperator &other) 
	: NeuralNetworkManipulationOperator(other), mCrossoverProbability(0),
	  mProbabilityPerModule(0)
{
	mCrossoverProbability = dynamic_cast<NormalizedDoubleValue*>(
				getParameter("CrossOverProbability"));
	mProbabilityPerModule = dynamic_cast<NormalizedDoubleValue*>(
				getParameter("ProbabilityPerModule"));
}

/**
 * Destructor.
 */
ModuleCrossOverOperator::~ModuleCrossOverOperator() {
}


NeuralNetworkManipulationOperator* ModuleCrossOverOperator::createCopy() const {
	return new ModuleCrossOverOperator(*this);
}


bool ModuleCrossOverOperator::applyOperator(Individual *individual, CommandExecutor*) {
	
	if(individual == 0) {
		return false;
	}

	if(individual->getGenome() != 0) {
		//do not change an already existing genome.
		return true;
	}

	QList<Individual*> &parents = individual->getParents();
	if(parents.size() < 2) {
		Core::log("ModuleCrossOverOperator: Can not perform operation because there are "
				  "too few parents: got " + QString::number(parents.size()) +
				  ", required 2", true);
		return true;
	}
	Individual *parent1 = parents.at(0);
	Individual *parent2 = parents.at(1);

	ModularNeuralNetwork *network1 = dynamic_cast<ModularNeuralNetwork*>(parent1->getGenome());
	ModularNeuralNetwork *network2 = dynamic_cast<ModularNeuralNetwork*>(parent2->getGenome());

	if(network1 == 0 || network2 == 0) {
		Core::log("ModuleCrossOverOperator: Parents did not provide a ModularNeuralNetwork!", true);
		return true;
	}

	if(Random::nextDouble() >= mCrossoverProbability->get()) {
		//don't execute crossover
		return true;
	}

	QList<NeuralNetworkElement*> trashcan;

	//select base parent at random
	if(Random::nextDouble() < 0.5) {
		ModularNeuralNetwork *tmp = network1;
		network1 = network2;
		network2 = tmp;
	}

	ModularNeuralNetwork *net = dynamic_cast<ModularNeuralNetwork*>(network1->createCopy());

	QList<NeuroModule*> ind1Modules = net->getNeuroModules();
	QList<NeuroModule*> ind2Modules = network2->getNeuroModules();


	//remove all modules that should not be replaced.
	QList<NeuroModule*> ind1ModulesCopy(ind1Modules);
	for(QListIterator<NeuroModule*> i(ind1ModulesCopy); i.hasNext();) {
		NeuroModule *module = i.next();
		if(module->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
			|| module->hasProperty(NeuralNetworkConstants::TAG_MODULE_PREVENT_REPLACEMENT)
			|| module->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE))
		{
			ind1Modules.removeAll(module);
		}
	}

	QList<NeuroModule*> ind2ModuleCopy(ind2Modules);
	for(QListIterator<NeuroModule*> i(ind2ModuleCopy); i.hasNext();) {
		NeuroModule *module = i.next();
		if(module->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
			|| module->hasProperty(NeuralNetworkConstants::TAG_MODULE_PREVENT_REPLACEMENT)
			|| module->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE))
		{
			ind2Modules.removeAll(module);
		}
	}

	while(!ind1Modules.empty()) {
		//randomly select a module.
		NeuroModule *module = ind1Modules.at(Random::nextInt(ind1Modules.size()));
		ind1Modules.removeAll(module);
		
		if(Random::nextDouble() >= mProbabilityPerModule->get()) {
			//keep this module
			continue;
		}

// 		QString moduleType = module->getProperty(NeuralNetworkConstants::TAG_MODULE_TYPE);
// 		int numberOfInputs = module->getInputNeurons().size();
// 		int numberOfOutputs = module->getOutputNeurons().size();

		//exchange module
		
		//find matching candidates
		QList<NeuroModule*> candidates;
		for(QListIterator<NeuroModule*> j(ind2Modules); j.hasNext();) {
			NeuroModule *candidate = j.next();
// 			if(candidate->getInputNeurons().size() != numberOfInputs
// 					|| candidate->getOutputNeurons().size() != numberOfOutputs)
// 			{
// 				//ignore modules with a different input/output interface.
// 				continue;
// 			}
// 			if(!candidate->hasProperty(NeuralNetworkConstants::TAG_MODULE_COMPATIBLE_TYPES)) {
// 				//if no restricting tag is available, then this is a candidate.
// 				candidates.append(candidate);
// 				continue;
// 			}
// 			QString compatibilityList = candidate->getProperty(NeuralNetworkConstants::TAG_MODULE_COMPATIBLE_TYPES);
// 			QStringList compatibleTypes = compatibilityList.split(",");
// 			for(QListIterator<QString> k(compatibleTypes); k.hasNext();) {
// 				if(k.next().trimmed() == moduleType) {
// 					candidates.append(candidate);
// 					break;
// 				}
// 			}
			if(ReplaceModuleHandler::canBeReplaced(module, candidate)) {
				candidates.append(candidate);
			}
		}
		
		if(candidates.empty()) {
			//no compatible module available
			continue;
		}

		NeuroModule *chosenCandidate = candidates.at(Random::nextInt(candidates.size()));

		QString errorMessage;
	
		ReplaceModuleHandler::replaceModule(module, chosenCandidate, trashcan, &errorMessage);
	}

	individual->setGenome(net);
	
	while(!trashcan.empty()) {
		NeuralNetworkElement *elem = trashcan.at(0);
		trashcan.removeAll(elem);
		delete elem;
	}

	return true;
}


void ModuleCrossOverOperator::removeParentModulesFromList(NeuroModule *module, QList<NeuroModule*> &list) {
	if(module == 0) {
		return;
	}
	list.removeAll(module);
	removeParentModulesFromList(module->getParentModule(), list);
}


}



