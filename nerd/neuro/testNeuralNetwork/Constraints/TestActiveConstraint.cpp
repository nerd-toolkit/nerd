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




#include "TestActiveConstraint.h"
#include <iostream>
#include "Core/Core.h"
#include "Constraints/ActiveConstraint.h"
#include "Network/Neuron.h"
#include "Network/Synapse.h"
#include "TransferFunction/TransferFunctionAdapter.h"
#include "ActivationFunction/ActivationFunctionAdapter.h"
#include "SynapseFunction/SynapseFunctionAdapter.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "NeuralNetworkConstants.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"

#define QV QVERIFY
#define QC QCOMPARE

using namespace std;
using namespace nerd;

void TestActiveConstraint::initTestCase() {
}

void TestActiveConstraint::cleanUpTestCase() {
}

//chris
void TestActiveConstraint::testConstruction() {
// 	ActiveConstraint *ac = new ActiveConstraint("ActiveConstraint");
// 	
// 	NeuronGroup *group = 0;
// 	CommandExecutor *executor = 0;
// 	QList<NeuralNetworkElement*> trashcan;
// 	
// 	//TODO do real tests here.
// 	ActiveConstraint *copy = dynamic_cast<ActiveConstraint*>(ac->createCopy());
// 	ac->getName();
// 
// 	//ac->eventOccured(0);
// 		
// 	ac->reset();
// 
// 	ac->isValid(0);
// 	ac->applyConstraint(group, executor, trashcan);
// 		
// 	ac->applyActiveConstraint(0, 0);
// 		
// 		
// 	ac->equals(copy);
// 		
// 	delete ac;
}



