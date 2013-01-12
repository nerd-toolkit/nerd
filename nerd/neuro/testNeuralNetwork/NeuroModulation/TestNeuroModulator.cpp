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




#include "TestNeuroModulator.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "SynapseFunction/ASeriesSynapseFunction.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "Value/DoubleValue.h"
#include "Network/Synapse.h"
#include "Math/ASeriesFunctions.h"
#include "NeuroModulation/NeuroModulator.h"


using namespace nerd;

void TestNeuroModulator::initTestCase() {
}

void TestNeuroModulator::cleanUpTestCase() {
}


//Chris
void TestNeuroModulator::testConstructionAndSettings() {
	
	NeuroModulator mod;
	
	//create modulator types (by setting concentration levels)
	QVERIFY(mod.getModulatorTypes().empty());
	mod.setConcentration(5, 100, 0);
	QCOMPARE(mod.getModulatorTypes().size(), 1);
	QVERIFY(mod.getModulatorTypes().contains(5));
	
	mod.setConcentration(12, 50, 0);
	QCOMPARE(mod.getModulatorTypes().size(), 2);
	QVERIFY(mod.getModulatorTypes().contains(12));
	
	//every type is only present once.
	mod.setConcentration(12, 11.5, 0);
	QCOMPARE(mod.getModulatorTypes().size(), 2);
	
	
	//set local areas
	QCOMPARE(mod.getLocalRect(88), QRectF(0.0, 0.0, 0.0, 0.0));
	mod.setLocalAreaRect(88, 150.0, 200.0, Vector3D(0.1, 0.2, 0.0), false);
	QCOMPARE(mod.getLocalRect(88), QRectF(0.1, 0.2, 150.0, 200.0));
	QVERIFY(mod.isCircularArea(88) == false);
	
	mod.setLocalAreaRect(88, 77.7, 99.9, Vector3D(1.1, 2.2, 5.0), true);
	QCOMPARE(mod.getLocalRect(88), QRectF(1.1, 2.2, 77.7, 77.7));
	QVERIFY(mod.isCircularArea(88) == true);
	
	
	
	QCOMPARE(2, mod.getDistributionModus(-1));
	mod.setDistributionModus(-1, 1);
	QCOMPARE(1, mod.getDistributionModus(-1)); //for all types
	
	mod.setDistributionModus(88, 4);
	QCOMPARE(1, mod.getDistributionModus(-1));
	QCOMPARE(1, mod.getDistributionModus(5));
	QCOMPARE(1, mod.getDistributionModus(12));
	QCOMPARE(4, mod.getDistributionModus(88)); 
	
	mod.setDistributionModus(12, 101);
	QCOMPARE(1, mod.getDistributionModus(-1));
	QCOMPARE(1, mod.getDistributionModus(5));
	QCOMPARE(101, mod.getDistributionModus(12));
	
	mod.setDistributionModus(-1, 222);
	QCOMPARE(222, mod.getDistributionModus(-1));
	QCOMPARE(222, mod.getDistributionModus(5));
	QCOMPARE(222, mod.getDistributionModus(12));
	
	mod.setDistributionModus(5, 11); 
	QCOMPARE(222, mod.getDistributionModus(-1));
	QCOMPARE(11, mod.getDistributionModus(5));
	QCOMPARE(222, mod.getDistributionModus(12));
	
	mod.setConcentration(909, 0.5, 0);
	QCOMPARE(222, mod.getDistributionModus(909));
	
	
}


//Chris
void TestNeuroModulator::testLocalConcentrationCalculation() {
	
}


//Chris
void TestNeuroModulator::testNetworkConcentrationCalculation() {
	
}


