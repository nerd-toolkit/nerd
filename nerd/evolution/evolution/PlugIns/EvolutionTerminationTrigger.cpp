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



#include "EvolutionTerminationTrigger.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Event/EventManager.h"
#include <limits>
#include "EvolutionConstants.h"
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/World.h"
#include "Evolution/Population.h"
#include "Fitness/FitnessFunction.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new EvolutionTerminationTrigger.
 */
EvolutionTerminationTrigger::EvolutionTerminationTrigger()
	: mTriggerEvent(0), mEvolutionRestartedEvent(0), mNumberOfGenerationsWithoutImprovement(0),
	  mCurrentBestFitness(numeric_limits<double>::min())
{

	Core::getInstance()->addSystemObject(this);

	mEnableTerminationTrigger = new BoolValue(false);
	mQuitApplicationAtTermination = new BoolValue(false);
	mMaxNumberOfGenerationsValue = new IntValue(numeric_limits<int>::max());
	mMaxNumberOfGenerationsWithoutFitnessImprovement = new IntValue(numeric_limits<int>::max());

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/EvolutionTerminationTrigger/EnableTrigger", mEnableTerminationTrigger);
	vm->addValue("/EvolutionTerminationTrigger/QuitAtTermination", mQuitApplicationAtTermination);
	vm->addValue("/EvolutionTerminationTrigger/MaxNumberOfGenerations", mMaxNumberOfGenerationsValue);
	vm->addValue("/EvolutionTerminationTrigger/MaxNumberOfGenerationsWithoutImprovement",
 						mMaxNumberOfGenerationsWithoutFitnessImprovement);
}


/**
 * Destructor.
 */
EvolutionTerminationTrigger::~EvolutionTerminationTrigger() {
}


QString EvolutionTerminationTrigger::getName() const {
	return "EvolutionTerminationTrigger";
}


bool EvolutionTerminationTrigger::init() {

	mNumberOfGenerationsWithoutImprovement = 0;
	mCurrentBestFitness = numeric_limits<double>::min();

	return true;
}


bool EvolutionTerminationTrigger::bind() {

	mTriggerEvent = Core::getInstance()->getEventManager()->registerForEvent(
						EvolutionConstants::EVENT_EVO_GENERATION_STARTED, this);
	
	mEvolutionRestartedEvent = Core::getInstance()->getEventManager()->registerForEvent(
						EvolutionConstants::EVENT_EVO_EVOLUTION_RESTARTED, this);

	//do not return false if this fails, this just disables the function of this plug-in.
	return true;
}


bool EvolutionTerminationTrigger::cleanUp() {
	return true;
}


void EvolutionTerminationTrigger::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(event == mTriggerEvent) {

		if(!mEnableTerminationTrigger->get()) {
			return;
		}

		EvolutionManager *em = Evolution::getEvolutionManager();

		bool quitApplication = false;
		QString message;

		if(em->getCurrentGenerationValue()->get() > mMaxNumberOfGenerationsValue->get()) {
			message.append("Max number of generations (")
						.append(QString::number(mMaxNumberOfGenerationsValue->get()))
						.append(") exceeded! ");
			quitApplication = true;
		}

		++mNumberOfGenerationsWithoutImprovement;

		QList<World*> worlds = em->getEvolutionWorlds();
		for(QListIterator<World*> i(worlds); i.hasNext();) {
			World *world = i.next();
			QList<Population*> populations = world->getPopulations();
			for(QListIterator<Population*> j(populations); j.hasNext();) {
				Population *pop = j.next();
				QList<FitnessFunction*> fitnessFunctions = pop->getFitnessFunctions();
				for(QListIterator<FitnessFunction*> k(fitnessFunctions); k.hasNext();) {
					FitnessFunction *fitness = k.next();

					double max = fitness->getMaxFitnessValue()->get();
					if(max > mCurrentBestFitness) {
						mCurrentBestFitness = max;
						mNumberOfGenerationsWithoutImprovement = 0;
					}
				}
			}
		}

		if(mNumberOfGenerationsWithoutImprovement > mMaxNumberOfGenerationsWithoutFitnessImprovement->get()) {
			message.append("Max number of generations without improvement (")
					.append(QString::number(mMaxNumberOfGenerationsWithoutFitnessImprovement->get()))
						.append(") exceeded!");
			quitApplication = true;
		}

		//TODO add other criteria


		if(quitApplication) {
			QFile overviewFile("evolutionSummary.txt");
			if(overviewFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
				QTextStream output(&overviewFile);
				output << "Evolution in directory: " << endl;
				output << em->getEvolutionWorkingDirectory() << endl;
				output << "Terminated Evolution at generation " 
						<< QString::number(em->getCurrentGenerationValue()->get()) << endl;
				output << "Best Fitness: " << QString::number(mCurrentBestFitness) << endl << endl;
			}
			overviewFile.close();

			message += QString("\nMax Fitness: ") + QString::number(mCurrentBestFitness) + QString("\n\n");
			Core::log(QString("EvolutionTerminationTrigger in generation ")
					.append(QString::number(em->getCurrentGenerationValue()->get()))
					.append(": ").append(message), true);
			if(mQuitApplicationAtTermination->get()) {
				Core::getInstance()->scheduleTask(new ShutDownTask());
			}
			else {
				restartEvolution();
			}
		}
	}
	else if(event == mEvolutionRestartedEvent) {
		//reset the local variables.
		init();
	}
}



void EvolutionTerminationTrigger::restartEvolution() {
	EvolutionManager *em = Evolution::getEvolutionManager();
	em->restartEvolution();
}

}



