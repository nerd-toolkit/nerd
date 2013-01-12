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




#include "TestDynamicsPlotManager.h"
#include <iostream>
#include "Core/Core.h"
#include "Network/NeuralNetwork.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "TransferFunction/TransferFunctionRamp.h"
#include "Adapters/TransferFunctionAdapter.h"
#include <Util/NeuralNetworkUtil.h>
#include <Util/DynamicsPlotterUtil.h>
#include <DynamicsPlot/DynamicsPlotManager.h>
#include <DynamicsPlotConstants.h>
#include "Adapters/DynamicsPlotterAdapter.h"

using namespace std;
using namespace nerd;

void TestDynamicsPlotManager::initTestCase() {
}

void TestDynamicsPlotManager::cleanUpTestCase() {
}


//Chris
void TestDynamicsPlotManager::testConstructorsAndGlobalRegistration() {
	Core::resetCore();
	
	DynamicsPlotManager *dpm = new DynamicsPlotManager();
	
	QVERIFY(dpm->getDynamicsPlotters().empty());
	QVERIFY(Core::getInstance()->getGlobalObjects().contains(dpm) == false);
	QVERIFY(Core::getInstance()->getGlobalObject(DynamicsPlotConstants::OBJECT_DYNAMICS_PLOT_MANAGER) == 0);
	
	int numberOfGlobalObjects = Core::getInstance()->getGlobalObjects().size();
	
	//register manager as global object.
	QVERIFY(dpm->registerAsGlobalObject() == true);
	
	QVERIFY(Core::getInstance()->getGlobalObjects().contains(dpm) == true);
	QVERIFY(Core::getInstance()->getGlobalObject(DynamicsPlotConstants::OBJECT_DYNAMICS_PLOT_MANAGER) == dpm);
	QCOMPARE(Core::getInstance()->getGlobalObjects().size(), numberOfGlobalObjects + 1);
	
	QVERIFY(dpm->registerAsGlobalObject() == true); //is ok, because this object is already registered.
	
	QVERIFY(Core::getInstance()->getGlobalObjects().contains(dpm) == true);
	QVERIFY(Core::getInstance()->getGlobalObject(DynamicsPlotConstants::OBJECT_DYNAMICS_PLOT_MANAGER) == dpm);
	QCOMPARE(Core::getInstance()->getGlobalObjects().size(), numberOfGlobalObjects + 1); //no change
	
	
	//try another one
	DynamicsPlotManager *other = new DynamicsPlotManager();
	QVERIFY(other->registerAsGlobalObject() == false); //there is already another manager registered.
	
	QVERIFY(Core::getInstance()->getGlobalObjects().contains(other) == false);
	QVERIFY(Core::getInstance()->getGlobalObject(DynamicsPlotConstants::OBJECT_DYNAMICS_PLOT_MANAGER) == dpm);
	QCOMPARE(Core::getInstance()->getGlobalObjects().size(), numberOfGlobalObjects + 1); //no change
	
	
	//Check if getInstance returns the correct object
	QVERIFY(DynamicsPlotManager::getInstance() == dpm);
	
	
	//try on-the-fly creation with getInstance();
	Core::resetCore();
	
	QVERIFY(Core::getInstance()->getGlobalObject(DynamicsPlotConstants::OBJECT_DYNAMICS_PLOT_MANAGER) == 0); //no dynamics plotter
	
	DynamicsPlotManager *dpm2 = DynamicsPlotManager::getInstance();
	QVERIFY(dpm2 != 0);
	QVERIFY(Core::getInstance()->getGlobalObject(DynamicsPlotConstants::OBJECT_DYNAMICS_PLOT_MANAGER) == dpm2);
	QVERIFY(dpm2->registerAsGlobalObject() == true);
	
	Core::resetCore();
}


//Chris
void TestDynamicsPlotManager::testPlotterManagement() {
	
	DynamicsPlotManager *dpm = DynamicsPlotManager::getInstance();
	
	QVERIFY(dpm->getDynamicsPlotters().size() == 0);
	
	//plotter is automatically added through constructor
	DynamicsPlotterAdapter *plotter1 = new DynamicsPlotterAdapter("Plotter1");
	
	QVERIFY(dpm->getDynamicsPlotters().size() == 1);
	QVERIFY(dpm->getDynamicsPlotters().contains(plotter1));
	
	//adding a second time fails.
	
	QVERIFY(dpm->addDynamicsPlotter(plotter1) == false);
	QVERIFY(dpm->getDynamicsPlotters().size() == 1);
	
	DynamicsPlotterAdapter *plotter2 = new DynamicsPlotterAdapter("Plotter2");
	DynamicsPlotterAdapter *plotter3 = new DynamicsPlotterAdapter("Plotter3");
	
	QVERIFY(dpm->getDynamicsPlotters().size() == 3);
	QVERIFY(dpm->getDynamicsPlotters().contains(plotter1));
	QVERIFY(dpm->getDynamicsPlotters().contains(plotter2));
	QVERIFY(dpm->getDynamicsPlotters().contains(plotter3));
	
	//adding NULL does not work
	QVERIFY(dpm->addDynamicsPlotter(0) == false);
	QVERIFY(dpm->getDynamicsPlotters().size() == 3);
	
	//removing NULL does not work
	QVERIFY(dpm->removeDynamicsPlotter(0) == false);
	QVERIFY(dpm->getDynamicsPlotters().size() == 3);
	
	//remove plotter
	QVERIFY(dpm->removeDynamicsPlotter(plotter2) == true);
	QVERIFY(dpm->getDynamicsPlotters().size() == 2);
	QVERIFY(dpm->getDynamicsPlotters().contains(plotter1));
	QVERIFY(dpm->getDynamicsPlotters().contains(plotter2) == false);
	QVERIFY(dpm->getDynamicsPlotters().contains(plotter3));
	
	//removing a second time fails
	QVERIFY(dpm->removeDynamicsPlotter(plotter2) == false);
	QVERIFY(dpm->getDynamicsPlotters().size() == 2);
	
	//TODO check deletion of objects
	
}



