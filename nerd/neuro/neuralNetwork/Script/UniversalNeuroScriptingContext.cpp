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


#include "UniversalNeuroScriptingContext.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Script/ScriptedNetworkManipulator.h"
#include "Network/Neuro.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Control/ControlInterface.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new UniversalNeuroScriptingContext.
 */
UniversalNeuroScriptingContext::UniversalNeuroScriptingContext(const QString &name, 
			const QString &fileName, const QString &triggerEventName, 
			const QString &resetEventName, const QString &agentName)
	: UniversalScriptingContext(name, fileName, triggerEventName, resetEventName)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();

	mNameOfAgentInterface = new StringValue(agentName);
	
	Core::getInstance()->getValueManager()
		->addValue("/UniversalScripting/" + name + "/NetworkAgent", mNameOfAgentInterface);
}


/**
 * Destructor.
 */
UniversalNeuroScriptingContext::~UniversalNeuroScriptingContext() {
	if(mNetworkManipulator != 0) {
		delete mNetworkManipulator;
	}
}


void UniversalNeuroScriptingContext::addCustomScriptContextStructures() {
	UniversalScriptingContext::addCustomScriptContextStructures();

	QScriptValue networkManip = mScript->newQObject(mNetworkManipulator,
							QScriptEngine::QtOwnership,
			 				QScriptEngine::ExcludeSuperClassMethods
							  | QScriptEngine::ExcludeSuperClassProperties
							  | QScriptEngine::ExcludeChildObjects);
	mScript->globalObject().setProperty("net", networkManip);

	//mNetworkManipulator->setNeuralNetwork(0);

	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();
	QList<NeuralNetwork*> networks = nnm->getNeuralNetworks();

	for(QListIterator<NeuralNetwork*> i(networks); i.hasNext();) {
		ModularNeuralNetwork *modNet = dynamic_cast<ModularNeuralNetwork*>(i.next());
		ControlInterface *interface = modNet->getControlInterface();

		if(interface->getName() == mNameOfAgentInterface->get()) {
			mNetworkManipulator->setNeuralNetwork(modNet);
			
			//make sure that the assumed owner really is part of the current network.
			//if not, set the owner hint to 0
			if(mNetworkManipulator->getOwnerHint() != 0) {
				QList<NeuralNetworkElement*> elements;
				modNet->getNetworkElements(elements);
				if(!elements.contains(mNetworkManipulator->getOwnerHint())) {
					mNetworkManipulator->setOwnerHint(0);
				}
				   
			}
		}
	}
}



}



