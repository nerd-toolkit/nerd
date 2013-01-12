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



#include "SimpleScriptedConstraint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Random.h"
#include "Network/Neuron.h"
#include "Network/Synapse.h"
#include "ActivationFunction/ActivationFunction.h"
#include "TransferFunction/TransferFunction.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new SimpleScriptedConstraint.
 */
SimpleScriptedConstraint::SimpleScriptedConstraint()
	: GroupConstraint("MaxNumberOfNeurons"), mScriptCode(0)
{
	mScriptCode = new CodeValue("");
	
	addParameter("Script", mScriptCode);
}


/**
 * Copy constructor. 
 * 
 * @param other the SimpleScriptedConstraint object to copy.
 */
SimpleScriptedConstraint::SimpleScriptedConstraint(const SimpleScriptedConstraint &other)
	: Object(), ValueChangedListener(), GroupConstraint(other), mScriptCode(0)
{
	mScriptCode = dynamic_cast<CodeValue*>(getParameter("Script"));
}

/**
 * Destructor.
 */
SimpleScriptedConstraint::~SimpleScriptedConstraint() {
}

GroupConstraint* SimpleScriptedConstraint::createCopy() const {
	return new SimpleScriptedConstraint(*this);
}

bool SimpleScriptedConstraint::isValid(NeuronGroup*) {
	return true;
}


bool SimpleScriptedConstraint::applyConstraint(NeuronGroup *owner, CommandExecutor*,
												QList<NeuralNetworkElement*>&) 
{

	if(owner == 0 || owner->getOwnerNetwork() == 0) {
		mErrorMessage = "Owner ModularNeuralNetwork or owner NeuronGroup has been NULL";
		return false;
	}

	//ModularNeuralNetwork *net = owner->getOwnerNetwork();

	NeuroModule *ownerModule = dynamic_cast<NeuroModule*>(owner);

	QList<Neuron*> neurons = owner->getNeurons();
	if(ownerModule != 0) {
		neurons = ownerModule->getAllEnclosedNeurons();
	}

	mScript = new QScriptEngine();
	QString scriptCode = mScriptCode->get();
	
	

	return true;
}


bool SimpleScriptedConstraint::equals(GroupConstraint *constraint) const {
	if(GroupConstraint::equals(constraint) == false) {
		return false;
	}
	SimpleScriptedConstraint *c = dynamic_cast<SimpleScriptedConstraint*>(constraint);
	if(c == 0) {
		return false;
	}
	if(!mScriptCode->equals(c->mScriptCode)) {
		return false;
	}
	return true;
}








}



