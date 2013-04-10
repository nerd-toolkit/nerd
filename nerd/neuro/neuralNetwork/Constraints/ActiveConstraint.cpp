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

#include "ActiveConstraint.h"
#include <QListIterator>
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <iostream>
#include "Value/CodeValue.h"
#include "Core/Core.h"
#include <NerdConstants.h>

using namespace std;

namespace nerd {

ActiveConstraint::ActiveConstraint(const QString &name)
	: GroupConstraint(name), mOwner(0), mNextStepEvent(0)
{
}

ActiveConstraint::ActiveConstraint(const ActiveConstraint &other)
	:  Object(), EventListener(), ValueChangedListener(), GroupConstraint(other),
	  mOwner(0), mNextStepEvent(0)
{
}


ActiveConstraint::~ActiveConstraint() {
	if(mNextStepEvent != 0) {
		mNextStepEvent->removeEventListener(this);
		mNextStepEvent = 0;
	}
}

QString ActiveConstraint::getName() const {
	return GroupConstraint::getName();
}



void ActiveConstraint::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(event == mNextStepEvent) {
		if(mOwner == 0) {
			Core::log("ActiveConstraint: Please run Constraint Resolver to initialize the active constraint!");
		}
		else {
			applyActiveConstraint(mOwner, mOwner->getOwnerNetwork());
		}
	}
}


void ActiveConstraint::reset() {
}



bool ActiveConstraint::isValid(NeuronGroup *owner) {
	
	mOwner = owner;
	if(mOwner == 0) {
		mErrorMessage = "ActiveConstraint: Did not find an owner group!";
		return false;
	}
	return true;
}


bool ActiveConstraint::applyConstraint(NeuronGroup *owner, CommandExecutor*,
									 QList<NeuralNetworkElement*>&)
{
	mOwner = owner;
	if(mOwner == 0) {
		return false;
	}
	
	return true;
}



		
bool ActiveConstraint::equals(GroupConstraint *constraint) const {
	if(GroupConstraint::equals(constraint) == false) {
		return false;
	}
	ActiveConstraint *af = dynamic_cast<ActiveConstraint*>(constraint);
	if(af == 0) {
		return false;
	}

	return true;
}






}


