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



#include "SaveBestNetworksHandler.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "EvolutionConstants.h"
#include "Evolution/World.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/Population.h"
#include "Evolution/Individual.h"
#include "Evolution/Evolution.h"
#include "Math/IndividualSorter.h"
#include "Network/NeuralNetwork.h"
#include "IO/NeuralNetworkIO.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SaveBestNetworksHandler.
 */
SaveBestNetworksHandler::SaveBestNetworksHandler(int numberOfBestNetworksToStore)
	: mStoreNetworksEvent(0)
{
	mNumberOfBestNetworksToStore = new IntValue(numberOfBestNetworksToStore);

	Core::getInstance()->getValueManager()->addValue("/NetworkLogger/NumberOfBestNetworksToLog", 
					mNumberOfBestNetworksToStore);
	
	Core::getInstance()->addSystemObject(this);
}



/**
 * Destructor.
 */
SaveBestNetworksHandler::~SaveBestNetworksHandler() {
}


QString SaveBestNetworksHandler::getName() const {
	return "SaveBestNetworksHandler";
}


bool SaveBestNetworksHandler::init() {
	bool ok = true;


	return ok;
}


bool SaveBestNetworksHandler::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mStoreNetworksEvent = em->registerForEvent(EvolutionConstants::EVENT_EVO_SELECTION_STARTED, this, true);

	if(mStoreNetworksEvent == 0) {
		ok = false;
		Core::log("SaveBestNetworksHandler: Could not find selection started event! ");
	}

	return ok;
}


bool SaveBestNetworksHandler::cleanUp() {
	return true;
}



void SaveBestNetworksHandler::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mStoreNetworksEvent) {

		int numberOfBestNetworksToStore = mNumberOfBestNetworksToStore->get();

		int genNo = Evolution::getEvolutionManager()->getCurrentGenerationValue()->get() - 1;
		QString bestDir = Evolution::getEvolutionManager()->getEvolutionWorkingDirectory() 
							+ "gen" + QString::number(genNo) + "/best";

		//collect best networks
		QList<World*> worlds = Evolution::getEvolutionManager()->getEvolutionWorlds();
		for(QListIterator<World*> i_w(worlds); i_w.hasNext();) {
			World *world = i_w.next();
			QList<Population*> populations = world->getPopulations();

			for(QListIterator<Population*> i_p(populations); i_p.hasNext();) {
				Population *pop = i_p.next();
				QList<Individual*> individuals = pop->getIndividuals();
	
				if(individuals.empty()) {
					continue;
				}

				QList<FitnessFunction*> ffs = individuals.at(0)->getFitnessFunctions();

				for(QListIterator<FitnessFunction*> i_f(ffs); i_f.hasNext();) {
					FitnessFunction *fitnessFunction = i_f.next();
		
					QList<Individual*> generationCopy = individuals;
					IndividualSorter::sortByFitness(generationCopy, fitnessFunction);

					for(int i = 0; i < numberOfBestNetworksToStore && i < generationCopy.size(); ++i) {
						QString dir = bestDir;
						dir = dir + "/" + fitnessFunction->getName() + "/";
						Core::getInstance()->enforceDirectoryPath(dir);

						Individual *ind = generationCopy.at(i);

						NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(ind->getPhenotype());
						if(net != 0) {
							QString fileName = dir;
							fileName = fileName + "ind_" + QString::number(ind->getId()) + "_"
										+ QString::number(i) + "_"
										+ QString::number(ind->getFitness(fitnessFunction)) + ".onn";

							NeuralNetworkIO::createFileFromNetwork(fileName, net);

							ind->setProperty("Best File", fileName);
						}

					}

				}
			}
		}
		
	}
}

}



