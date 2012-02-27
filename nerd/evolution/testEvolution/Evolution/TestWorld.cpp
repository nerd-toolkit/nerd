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




#include "TestWorld.h"
#include <iostream>

#include "Evolution/PopulationAdapter.h"
#include "Evolution/World.h"
#include "Evolution/EvolutionAlgorithmAdapter.h"
#include "Evolution/EvaluationMethodAdapter.h"
#include "Value/IntValue.h"
#include "Core/Core.h"

using namespace std;
using namespace nerd;

void TestWorld::initTestCase() {
}

void TestWorld::cleanUpTestCase() {
}


//chris
void TestWorld::testConstructor() { 
	 
	bool destroyedEvolution = false;
	bool destroyedEvaluation = false;
	{
		World world("World1");
		
		QVERIFY(world.getName() == "World1");
		QVERIFY(world.getPopulations().size() == 0);
		QVERIFY(world.getEvolutionAlgorithm() == 0);
		QVERIFY(world.getEvaluationMethod() == 0);

		EvolutionAlgorithmAdapter *evo = new EvolutionAlgorithmAdapter("Evo1");
		world.setEvolutionAlgorithm(evo);
		evo->mDestroyFlag = &destroyedEvolution;
		QVERIFY(world.getEvolutionAlgorithm() == evo);

		EvaluationMethodAdapter *eval = new EvaluationMethodAdapter("Eval1");
		world.setEvaluationMethod(eval);
		eval->mDestroyFlag = &destroyedEvaluation;
		QVERIFY(world.getEvaluationMethod() == eval);
	}
	QVERIFY(destroyedEvolution == true);
	QVERIFY(destroyedEvaluation == true);
}


//chris
void TestWorld::testAddAndRemovePopulations() {
	
	bool destroyPopulation = false;
	PopulationAdapter *pop1 = new PopulationAdapter("Pop1");
	PopulationAdapter *pop2 = new PopulationAdapter("Pop2");
	pop2->mDestroyFlag = &destroyPopulation;
	
	{
		World world("World1");

		QVERIFY(world.addPopulation(0) == false);
		QVERIFY(world.getPopulations().size() == 0);

		QVERIFY(world.addPopulation(pop1) == true);
		QVERIFY(world.getPopulations().size() == 1);
		QVERIFY(world.getPopulations().at(0) == pop1);

		QVERIFY(world.addPopulation(pop1) == false); //can not add a second time
		QVERIFY(world.getPopulations().size() == 1);

		QVERIFY(world.addPopulation(pop2) == true);
		QVERIFY(world.getPopulations().size() == 2);
		QVERIFY(world.getPopulations().contains(pop1));
		QVERIFY(world.getPopulations().contains(pop2));

		QVERIFY(world.removePopulation(0) == false); //cannot remove NULL
		QVERIFY(world.getPopulations().size() == 2);

		QVERIFY(world.removePopulation(pop1) == true); 
		QVERIFY(world.getPopulations().size() == 1);
		QVERIFY(world.getPopulations().contains(pop2));
	}
	//pop2 was destroyed by world.
	QVERIFY(destroyPopulation == true);
	delete pop1;
}


//chris
void TestWorld::testPublishingBehavior() {

	Core::resetCore();

	ValueManager *vm = Core::getInstance()->getValueManager();
	World world("World1");

	EvaluationMethodAdapter *eval1 = new EvaluationMethodAdapter("Eval1");
	EvaluationMethodAdapter *eval2 = new EvaluationMethodAdapter("Eval2");
	eval1->addParameter("Param1", new IntValue());
	eval2->addParameter("Param2", new IntValue());

	EvolutionAlgorithmAdapter *evo1 = new EvolutionAlgorithmAdapter("Evo1");
	EvolutionAlgorithmAdapter *evo2 = new EvolutionAlgorithmAdapter("Evo2");
	evo1->addParameter("Param3", new IntValue());
	evo2->addParameter("Param4", new IntValue());

	//EvaluationMethod
	QVERIFY(world.getEvaluationMethod() == 0);
	QVERIFY(vm->getValue("/Evolution/World1/Evaluation/Eval1/Param1") == 0);
	QVERIFY(vm->getValue("/Evolution/World1/Evaluation/Eval2/Param2") == 0);

	world.setEvaluationMethod(eval1);

	QVERIFY(world.getEvaluationMethod() == eval1);
	QVERIFY(vm->getValue("/Evo/Evaluation/World1/Eval1/Param1") != 0);
	QVERIFY(vm->getValue("/Evo/Evaluation/World1/Eval2/Param2") == 0);

	world.setEvaluationMethod(eval2);
	QVERIFY(world.getEvaluationMethod() == eval2);
	QVERIFY(vm->getValue("/Evo/Evaluation/World1/Eval1/Param1") == 0);
	QVERIFY(vm->getValue("/Evo/Evaluation/World1/Eval2/Param2") != 0);

	world.setEvaluationMethod(0);
	QVERIFY(world.getEvaluationMethod() == 0);
	QVERIFY(vm->getValue("/Evo/Evaluation/World1/Eval1/Param1") == 0);
	QVERIFY(vm->getValue("/Evo/Evaluation/World1/Eval2/Param2") == 0);

	//EvolutionAlgorithm
	QVERIFY(world.getEvolutionAlgorithm() == 0);
	QVERIFY(vm->getValue("/Evo/World1/Algorithm/Evo1/Param3") == 0);
	QVERIFY(vm->getValue("/Evo/World1/Algorithm/Evo1/Param4") == 0);

	world.setEvolutionAlgorithm(evo1);

	vm->saveValues("valuedump.txt", vm->getValueNamesMatchingPattern(".*"), "");


	QVERIFY(world.getEvolutionAlgorithm() == evo1);
	QVERIFY(vm->getValue("/Evo/World1/Algorithm/Evo1/Param3") != 0);
	QVERIFY(vm->getValue("/Evo/World1/Algorithm/Evo2/Param4") == 0);

	world.setEvolutionAlgorithm(evo2);

	QVERIFY(world.getEvolutionAlgorithm() == evo2);
	QVERIFY(vm->getValue("/Evo/World1/Algorithm/Evo1/Param3") == 0);
	QVERIFY(vm->getValue("/Evo/World1/Algorithm/Evo2/Param4") != 0);
	
	world.setEvolutionAlgorithm(0);

	QVERIFY(vm->getValue("/Evo/World1/Algorithm/Evo1/Param3") == 0);
	QVERIFY(vm->getValue("/Evo/World1/Algorithm/Evo2/Param4") == 0);

	delete eval1;
	delete eval2;
	delete evo1;
	delete evo2;


}



