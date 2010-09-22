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

#include "TestSignals.h"
#include "Core/Core.h"
#include "Signal/SignalSinus.h"
#include "NerdConstants.h"
#include "SimulationConstants.h"
#include "Math/Math.h"
#include <iostream>

using namespace std;

namespace nerd{

void TestSignals::testSignalSinus() {
	Core::resetCore();

	// Test SinusSignal

		// Create the TimeStepSizeValue
		DoubleValue *timeStepSizeValue = new DoubleValue(0.01); Core::getInstance()->getValueManager()->addValue(SimulationConstants::VALUE_TIME_STEP_SIZE, timeStepSizeValue);
		// Create the CurrentStepValue
		IntValue *currentStepValue = new IntValue(0);
		Core::getInstance()->getValueManager()->addValue(SimulationConstants::VALUE_EXECUTION_CURRENT_STEP, currentStepValue);
		// Create the NextStepEvent
		Event* nextStepEvent = Core::getInstance()->getEventManager()->createEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
		QVERIFY(nextStepEvent != 0);

	// Create the signal and initialize the core
	SignalSinus* mySignalSinus = new SignalSinus("MySinus", "/Signals");
	Core::getInstance()->init();

	// CurrentValue und IdleSteps Parameter should be there
	DoubleValue* currentValue = dynamic_cast<DoubleValue*>(mySignalSinus->getParameter("CurrentValue"));
	QVERIFY(currentValue != 0);
	IntValue* idleSteps = dynamic_cast<IntValue*>(mySignalSinus->getParameter("IdleSteps"));
	QVERIFY(idleSteps != 0);
	QVERIFY(currentValue->get() == 0.0);
	QVERIFY(idleSteps->get() == 0);

	// Current value should be 0.0
	QVERIFY(mySignalSinus->getCurrentValue() == 0.0);

	// Test getName()
	QVERIFY(mySignalSinus->getName() == "MySinus");

	// Period, Amplitude, Offset and AngularPhaseShift Paramaters should be there
	DoubleValue* period = dynamic_cast<DoubleValue*>(mySignalSinus->getParameter("Period"));
	QVERIFY(period != 0);
	QVERIFY(period->get() == 1.0);
	DoubleValue* amplitude = dynamic_cast<DoubleValue*>(mySignalSinus->getParameter("Amplitude"));
	QVERIFY(amplitude != 0);
	QVERIFY(amplitude->get() == 1.0);
	DoubleValue* offset = dynamic_cast<DoubleValue*>(mySignalSinus->getParameter("Offset"));
	QVERIFY(offset != 0);
	QVERIFY(offset->get() == 0.0);
	DoubleValue* phase = dynamic_cast<DoubleValue*>(mySignalSinus->getParameter("Phase"));
	QVERIFY(phase != 0);
	QVERIFY(phase->get() == 0.0);

	// Test computation
	double desiredValue;

	currentStepValue->set(1);
	nextStepEvent->trigger();
	desiredValue = Math::sin(1.0 / 100.0 * 2 * Math::PI);
	QVERIFY(Math::compareDoubles(mySignalSinus->getCurrentValue(), desiredValue, 10));

	currentStepValue->set(30);
	nextStepEvent->trigger();
	desiredValue = Math::sin(30.0 / 100.0 * 2 * Math::PI);
	QVERIFY(Math::compareDoubles(mySignalSinus->getCurrentValue(), desiredValue, 10));

	timeStepSizeValue->set(0.02);
	nextStepEvent->trigger();
	desiredValue = Math::sin(2.0 * 30.0 / 100.0 * 2 * Math::PI);
	QVERIFY(Math::compareDoubles(mySignalSinus->getCurrentValue(), desiredValue, 10));

	idleSteps->set(10);
	nextStepEvent->trigger();
	desiredValue = Math::sin(2.0 * 20.0 / 100.0 * 2 * Math::PI);
	QVERIFY(Math::compareDoubles(mySignalSinus->getCurrentValue(), desiredValue, 10));

	period->set(1.5);
	nextStepEvent->trigger();
	desiredValue = Math::sin((30 - 10) / (1.5 / 0.02) * 2 * Math::PI);
	QVERIFY(Math::compareDoubles(mySignalSinus->getCurrentValue(), desiredValue, 10));

	amplitude->set(3.3);
	nextStepEvent->trigger();
	desiredValue = 3.3 * Math::sin((30 - 10) / (1.5 / 0.02) * 2 * Math::PI);
	QVERIFY(Math::compareDoubles(mySignalSinus->getCurrentValue(), desiredValue, 10));

	offset->set(0.7);
	nextStepEvent->trigger();
	desiredValue = 0.7 + 3.3 * Math::sin((30 - 10) / (1.5 / 0.02) * 2 * Math::PI);
	QVERIFY(Math::compareDoubles(mySignalSinus->getCurrentValue(), desiredValue, 10));

	phase->set(0.12);
	nextStepEvent->trigger();
	desiredValue = 0.7 + 3.3 * Math::sin(0.12 + (30 - 10) / (1.5 / 0.02) * 2 * Math::PI);
	QVERIFY(Math::compareDoubles(mySignalSinus->getCurrentValue(), desiredValue, 10));

	// Test addDestinationValue mechanism
	DoubleValue *destValue1 = new DoubleValue(0.0);
	DoubleValue *destValue2 = new DoubleValue(0.0);
	DoubleValue *destValue3 = new DoubleValue(0.0);
	mySignalSinus->addDestinationValue(destValue1);
	mySignalSinus->addDestinationValue(destValue2);
	mySignalSinus->addDestinationValue(destValue3);
	period->set(1.0);
	amplitude->set(1.0);
	offset->set(0.0);
	phase->set(0.0);
	idleSteps->set(0);
	timeStepSizeValue->set(0.01);
	currentStepValue->set(1);
	nextStepEvent->trigger();
	desiredValue = Math::sin(1.0 / 100.0 * 2 * Math::PI);
	QVERIFY(Math::compareDoubles(mySignalSinus->getCurrentValue(), desiredValue, 10));
	QCOMPARE(destValue1->get(), mySignalSinus->getCurrentValue());
	QCOMPARE(destValue2->get(), mySignalSinus->getCurrentValue());
	QCOMPARE(destValue3->get(), mySignalSinus->getCurrentValue());

	// Test removeDestinationValue mechanism
	mySignalSinus->removeDestinationValue(destValue3);
	currentStepValue->set(2);
	nextStepEvent->trigger();
	desiredValue = Math::sin(2.0 / 100.0 * 2 * Math::PI);
	QVERIFY(Math::compareDoubles(mySignalSinus->getCurrentValue(), desiredValue, 10));
	QCOMPARE(destValue1->get(), mySignalSinus->getCurrentValue());
	QCOMPARE(destValue2->get(), mySignalSinus->getCurrentValue());
	QVERIFY(destValue3->get() != mySignalSinus->getCurrentValue());

	delete destValue1;
	delete destValue2;
	delete destValue3;

	Core::resetCore();
}

}
