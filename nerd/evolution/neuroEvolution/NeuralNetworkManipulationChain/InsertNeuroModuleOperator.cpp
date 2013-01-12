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



#include "InsertNeuroModuleOperator.h"
#include "Network/NeuralNetwork.h"
#include "Core/Core.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Math/Random.h"
#include <iostream>
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include "Network/Synapse.h"
#include "EvolutionConstants.h"
#include "Network/NeuroTagManager.h"
#include "ModularNeuralNetwork/NeuroModuleManager.h"
#include "Gui/NetworkEditorCommands/InsertSubnetworkCommand.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new InsertNeuroModuleOperator.
 */
InsertNeuroModuleOperator::InsertNeuroModuleOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mMaximalNumberOfNewModules(0), 
	  mInsertionProbability(0), mInsertSubModulesOnly(0)
{
	mMaximalNumberOfNewModules = new IntValue(2);
	mInsertionProbability = new NormalizedDoubleValue(0.0, 0.0, 1.0, 0.0, 1.0);
	mInsertSubModulesOnly = new BoolValue(true);

	addParameter("MaxNumberOfNewModules", mMaximalNumberOfNewModules);
	addParameter("InsertionProbability", mInsertionProbability);
	addParameter("OnlySubModules", mInsertSubModulesOnly);

}


/**
 * Copy constructor. 
 * 
 * @param other the InsertNeuroModuleOperator object to copy.
 */
InsertNeuroModuleOperator::InsertNeuroModuleOperator(const InsertNeuroModuleOperator &other) 
	: NeuralNetworkManipulationOperator(other)
{
	mMaximalNumberOfNewModules = dynamic_cast<IntValue*>(getParameter("MaxNumberOfNewModules"));
	mInsertionProbability = dynamic_cast<NormalizedDoubleValue*>(getParameter("InsertionProbability"));
	mInsertSubModulesOnly = dynamic_cast<BoolValue*>(getParameter("OnlySubModules"));
}

/**
 * Destructor.
 */
InsertNeuroModuleOperator::~InsertNeuroModuleOperator() {
}


NeuralNetworkManipulationOperator* InsertNeuroModuleOperator::createCopy() const {
	return new InsertNeuroModuleOperator(*this);
}

bool InsertNeuroModuleOperator::applyOperator(Individual *individual, CommandExecutor*)
{
	ModularNeuralNetwork *modularNetwork = dynamic_cast<ModularNeuralNetwork*>(individual->getGenome());

	if(modularNetwork == 0) {
		Core::log("InsertNeuronOperator: Could not apply operator because individual did not "
				  "provide a ModularNeuralNetwork as genome!");
		return true;
	}
		
	QString currentGenString = QString::number(Evolution::getEvolutionManager()
				->getCurrentGenerationValue()->get());


	int maxNumberOfNewModules = mMaximalNumberOfNewModules->get();
	double insertionProbability = mInsertionProbability->get();
	bool allowRootLevelModules = !mInsertSubModulesOnly->get();

	QList<NeuroModulePrototype*> availablePrototypes =
				NeuroModuleManager::getInstance()->getNeuroModulePrototpyes();

	if(maxNumberOfNewModules <= 0) {
		return true;
	}

	for(int i = 0; i < maxNumberOfNewModules; ++i) {

		if(Random::nextDouble() < insertionProbability) {

			QList<NeuroModule*> allModules = modularNetwork->getNeuroModules();

			//choose a module to insert the new module
			QList<NeuroModule*> targetCandidates = allModules;
			for(QListIterator<NeuroModule*> k(allModules); k.hasNext();) {
				NeuroModule *module = k.next();
				if(module->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)
					|| module->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE)) 
				{
					targetCandidates.removeAll(module);
				}
				else if(module->hasProperty(NeuralNetworkConstants::TAG_MAX_NUMBER_OF_SUBMODULES)) {
					int maxSubModules =  module->getProperty(
							NeuralNetworkConstants::TAG_MAX_NUMBER_OF_SUBMODULES)
							.toInt();
					if(module->getSubModules().size() >= maxSubModules) {
						targetCandidates.removeAll(module);
					}
				}
			}

			double random = Random::nextDouble();
			bool notASubModule = false;

			NeuroModule *target = 0;
			double fraction = 0.0;
			if(allowRootLevelModules) {
				fraction = (1.0 / (targetCandidates.size() + 1));

				if(random < fraction) {
					notASubModule = true;
				}
				random -= fraction;
			}
			if(!targetCandidates.empty() && !notASubModule) {
				if(!allowRootLevelModules) {
					fraction = (1.0 / targetCandidates.size());
				}
				for(int k = 0; k < targetCandidates.size(); ++k) {
					random -= fraction;
					if(random < 0.0) {
						target = targetCandidates.at(k);
						break;
					}
				}
			}

			if(!notASubModule && target == 0) {
				//could not find a suitable target module / network
				continue;
			}
			QStringList supportedModuleTypes;
			Properties *propertiesElement = 0;
			if(notASubModule) {
				propertiesElement = modularNetwork;
			}
			else {
				propertiesElement = target;
			}
			if(propertiesElement->hasProperty(NeuralNetworkConstants::TAG_MODULE_SUPPORTED_TYPES)) {
				QStringList typeList = propertiesElement->getProperty(
								NeuralNetworkConstants::TAG_MODULE_SUPPORTED_TYPES).split(",");
				if(typeList.empty()) {
					supportedModuleTypes.append("any");
				}
				else {
					for(QListIterator<QString> l(typeList); l.hasNext();) {
						supportedModuleTypes.append(l.next().trimmed().toLower());
					}
				}
			}
			else {
				supportedModuleTypes.append("any");
			}

			//select matching module from module pool
			QList<NeuroModulePrototype*> matchingPrototypes;
			if(supportedModuleTypes.contains("any")) {
				matchingPrototypes = availablePrototypes;
			}
			else {
				for(QListIterator<NeuroModulePrototype*> k(availablePrototypes); k.hasNext();) {
					NeuroModulePrototype *p = k.next();
					for(QListIterator<QString> l(supportedModuleTypes); l.hasNext();) {					
						if(p->mTypes.contains(l.next())) {
							matchingPrototypes.append(p);
						}
					}
				}
			}

			//if there are no matching prototypes, then skip this insertion step.
			if(matchingPrototypes.empty()) {
				continue;
			}

			NeuroModule *newModule = getRandomModule(matchingPrototypes);

			if(newModule == 0) {
				continue;
			}

			//set creation date
			newModule->setProperty(EvolutionConstants::TAG_EVO_CREATION_DATE, currentGenString);

			//mark as modified.
			newModule->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
			
			//mark as new
			newModule->setProperty(NeuralNetworkConstants::TAG_EVOLUTION_NEW_ELEMENT);

			//mark the individual as significantly modified
			individual->setProperty(EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE,
									currentGenString);
			individual->setProperty(EvolutionConstants::TAG_GENOME_CHANGE_SUMMARY,
						individual->getProperty(EvolutionConstants::TAG_GENOME_CHANGE_SUMMARY) 
							+ ",M:" + QString::number(newModule->getId()) + ":aM"); 
			
			
			QPointF pos;
			if(notASubModule) {
				pos = QPointF(-400 + Random::nextDouble() * 800.0,
						Random::nextDouble() * 800.0);
			}
			else {
				Vector3D targetPos = target->getPosition();
				QSizeF size = target->getSize();
				QSizeF newModuleSize = newModule->getSize();
				pos = QPointF(
						targetPos.getX() + (Random::nextDouble() * 
								(size.width() - newModuleSize.width())),
						targetPos.getY() + (Random::nextDouble() * 
								(size.height() - newModuleSize.height())));
			}

			ModularNeuralNetwork *tmpNet = new ModularNeuralNetwork();
			tmpNet->addNeuronGroup(newModule);

			//TODO add module to network
			InsertSubnetworkCommand *command = new InsertSubnetworkCommand(modularNetwork, 
						tmpNet, target, pos, true);

			command->doCommand();
			delete command;

			delete tmpNet;
		}
	}

	return true;
}


/**
 * Chooses one of the prototypes at random, extracts the first root module and returns
 * a copy of this module, including all neurons and synapses (deep copy).
 */
NeuroModule* InsertNeuroModuleOperator::getRandomModule(QList<NeuroModulePrototype*> prototypes) {
	if(prototypes.empty()) {
		return 0;
	}
	NeuroModulePrototype *prototype = prototypes.at(Random::nextInt(prototypes.size()));
	ModularNeuralNetwork *network = prototype->mNetwork;
	if(network == 0) {
		return 0;
	}
	if(network->getRootModules().empty()) {
		Core::log(QString("InsertNeuroModuleOperator: Could not extract a valid module "
						  "from prototype [").append(prototype->mName).append("]"), true);
		return 0;
	}
	NeuroModule *module = network->getRootModules().at(0);
	if(module == 0) {
		Core::log("InsertNeuroModuleOperator: First module of prototype network was 0", true);
		return 0;
	}
	return module->createDeepCopy();
}

}



