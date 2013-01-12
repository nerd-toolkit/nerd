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

#include "TestColor.h"
#include "Core/Core.h"
#include "Util/Color.h"
#include <iostream>

using namespace std;

namespace nerd{


//chris
void TestColor::testConstructor() {
	
	Color c1;
	QVERIFY(c1.red() == 255);
	QVERIFY(c1.green() == 255);
	QVERIFY(c1.blue() == 255);
	QVERIFY(c1.alpha() == 255);

	Color c2(1, 2, 3, 4);
	QVERIFY(c2.red() == 1);
	QVERIFY(c2.green() == 2);
	QVERIFY(c2.blue() == 3);
	QVERIFY(c2.alpha() == 4);

	Color c3(5, 6, 7);
	QVERIFY(c3.red() == 5);
	QVERIFY(c3.green() == 6);
	QVERIFY(c3.blue() == 7);
	QVERIFY(c3.alpha() == 255);

	// c4 has to get the same color values as c3 and c3 has to stay unchanged
	Color c4(c3);
	QVERIFY(c4.red() == 5);
	QVERIFY(c4.green() == 6);
	QVERIFY(c4.blue() == 7);
	QVERIFY(c4.alpha() == 255);
	QVERIFY(c3.red() == 5);
	QVERIFY(c3.green() == 6);
	QVERIFY(c3.blue() == 7);
	QVERIFY(c3.alpha() == 255);

	Color c5;
	c5.setRed(17);
	QCOMPARE(c5.red(), 17);

	c5.setGreen(18);
	QCOMPARE(c5.green(), 18);

	c5.setBlue(19);
	QCOMPARE(c5.blue(), 19);

	c5.setAlpha(20);
	QCOMPARE(c5.alpha(), 20);

	Color c6(-1, 756, -34, 1200);
	QVERIFY(c6.red() == 0);
	QVERIFY(c6.green() == 255);
	QVERIFY(c6.blue() == 0);
	QVERIFY(c6.alpha() == 255);

	Color c7(256, 300, -300);
	QVERIFY(c7.red() == 255);
	QVERIFY(c7.green() == 255);
	QVERIFY(c7.blue() == 0);
	QVERIFY(c7.alpha() == 255);

	c7.setRed(-20);
	QVERIFY(c7.red() == 0);

	c7.setGreen(700);
	QVERIFY(c7.green() == 255);

	c7.setBlue(-20);
	QVERIFY(c7.blue() == 0);

	c7.setAlpha(700);
	QVERIFY(c7.alpha() == 255);

}

}
