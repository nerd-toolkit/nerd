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
 
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Util/UnitTestMacros.h"
#include <iostream>

using namespace std;

#include "Value/TestValue.h"
#include "Value/TestValueManager.h"
#include "Event/TestEvent.h"
#include "Event/TestEventManager.h"
#include "Event/TestTriggerEventTask.h"
#include "Core/TestParameterizedObject.h"
#include "Core/TestCore.h"
#include "Math/TestVector3D.h"
#include "Math/TestQuaternion.h"
#include "Value/TestInterfaceValue.h"
#include "Value/TestNormalizedDoubleValue.h"
#include "Math/TestMath.h"
#include "Communication/TestUdpDatagram.h"
#include "Util/TestColor.h"
#include "Util/TestFileLocker.h"
#include "Math/TestMatrix.h"

TEST_START("TestNerd", 1, -1, 16); 

	TEST(TestMath);
	TEST(TestValue);
	TEST(TestValueManager); //test save and load values
	TEST(TestEvent);
	TEST(TestEventManager);
	TEST(TestTriggerEventTask);
	TEST(TestParameterizedObject);
	TEST(TestCore);
	TEST(TestQuaternion);
	TEST(TestVector3D);
	TEST(TestInterfaceValue);
	TEST(TestNormalizedDoubleValue);
	TEST(TestUdpDatagram);
	TEST(TestColor);
	TEST(TestFileLocker);
	TEST(TestMatrix);

TEST_END;


