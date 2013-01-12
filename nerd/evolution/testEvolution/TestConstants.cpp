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




#include "TestConstants.h"
#include <iostream>
#include "EvolutionConstants.h"
#include "NerdConstants.h"
#include "NeuralNetworkConstants.h"
#include "SimulationConstants.h"

using namespace std;
using namespace nerd;

void TestConstants::initTestCase() {
}

void TestConstants::cleanUpTestCase() {
}


//chris
void TestConstants::testConstants() {
	//check that constants of different libraries match.

	//NextGeneration Event
	QVERIFY(EvolutionConstants::EVENT_EVO_GENERATION_STARTED
		== NerdConstants::EVENT_EXECUTION_GENERATION_STARTED);
	
	//GenerationCompleted Event
	QVERIFY(EvolutionConstants::EVENT_EVO_GENERATION_COMPLETED
		== NerdConstants::EVENT_EXECUTION_GENERATION_COMPLETED);

	//NextTry
	QVERIFY(NerdConstants::EVENT_EXECUTION_NEXT_TRY
		== EvolutionConstants::EVENT_EXECUTION_NEXT_TRY);

	//TryCompleted
	QVERIFY(NerdConstants::EVENT_EXECUTION_TRY_COMPLETED
		== EvolutionConstants::EVENT_EXECUTION_TRY_COMPLETED);

	//NextIndividual
	QVERIFY(NerdConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL
		== EvolutionConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL);

	//IndividualCompleted
	QVERIFY(NerdConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED
		== EvolutionConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED);

	//GenerationCompleted
	QVERIFY(NerdConstants::EVENT_EXECUTION_GENERATION_COMPLETED
		== EvolutionConstants::EVENT_EXECUTION_GENERATION_COMPLETED);

	//Statis Mode
	QVERIFY(NeuralNetworkConstants::VALUE_EVO_STASIS_MODE
		== EvolutionConstants::VALUE_EVO_STASIS_MODE);

	//EvolutionManager
	QVERIFY(NeuralNetworkConstants::OBJECT_EVOLUTION_MANAGER
		== EvolutionConstants::OBJECT_EVOLUTION_MANAGER);

	QVERIFY(SimulationConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER
		== EvolutionConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER);

	QVERIFY(SimulationConstants::EVENT_PHYSICS_ENVIRONMENT_CHANGED
		== EvolutionConstants::EVENT_PHYSICS_ENVIRONMENT_CHANGED);

	QVERIFY(SimulationConstants::VALUE_RUN_SIMULATION_IN_REALTIME
		== EvolutionConstants::VALUE_RUN_SIMULATION_IN_REALTIME);

	QVERIFY(SimulationConstants::EVENT_EXECUTION_TERMINATE_TRY
		== EvolutionConstants::EVENT_EXECUTION_TERMINATE_TRY);
	
	//GenerationCompleted Event
	QVERIFY(SimulationConstants::VALUE_EXECUTION_PAUSE
		== EvolutionConstants::VALUE_EXECUTION_PAUSE);

	QVERIFY(SimulationConstants::VALUE_EXECUTION_CURRENT_TRY 
		== EvolutionConstants::VALUE_EXECUTION_CURRENT_TRY);

	//TerminateTryRule
	QVERIFY(SimulationConstants::OBJECT_TERMINATE_TRY_COLLISION_RULE
		== EvolutionConstants::OBJECT_TERMINATE_TRY_COLLISION_RULE);

}


