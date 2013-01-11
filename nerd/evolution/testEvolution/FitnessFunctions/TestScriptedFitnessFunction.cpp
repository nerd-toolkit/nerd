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




#include "TestScriptedFitnessFunction.h"
#include <iostream>
#include "Core/Core.h"
#include "FitnessFunctions/ScriptedFitnessFunction.h"
#include "Value/BoolValue.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"

using namespace std;
using namespace nerd;

void TestScriptedFitnessFunction::initTestCase() {
}

void TestScriptedFitnessFunction::cleanUpTestCase() {
}

//chris
void TestScriptedFitnessFunction::testScripting() {

	Core::resetCore();

	BoolValue *boolean = new BoolValue(false);	
	Core::getInstance()->getValueManager()->addValue("/Enable", boolean);
	
	ScriptedFitnessFunction *sf = new ScriptedFitnessFunction("Fitness1");

	QVERIFY(sf->getName() == "Fitness1");

	QString code = "print('Hallo Welt'); ";

	code += "\ndefVar('myValue', '/ValueManager/RepositoryChangedCounter');";
	code += "\ndefVar('moreValue', '/Logger/RecentMessage');";
	code += "\ndefVar('enable', '/Enable');";
	code += "\nfunction reset() { print('RESET ', myValue); };";
	code += "\nfunction prepareTry() { print('Prepare Try ', " 	
			"nerd.getDoubleValue('/ValueManager/RepositoryChangedCounter')); };";
	code += "\nfunction calc() { print('CALC ', moreValue); if(enable == true) { print('Enabled'); };};";

	//Core::getInstance()->getValueManager()->printValues(".*"); 

	sf->setScriptCode(code);
	sf->reset();

	sf->reset();
	sf->prepareNextTry();
	sf->calculateCurrentFitness();

	Core::getInstance()->getValueManager()->notifyRepositoryChangedListeners();
	sf->calculateCurrentFitness();
	Core::getInstance()->getValueManager()->notifyRepositoryChangedListeners();
	sf->calculateCurrentFitness();

	boolean->set(true);

	Core::getInstance()->getValueManager()->notifyRepositoryChangedListeners();
	sf->calculateCurrentFitness();
	
	boolean->set(false);

	sf->reset();
	sf->prepareNextTry();
	sf->calculateCurrentFitness();

	delete sf;

	

	Core::resetCore();
}



