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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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


#include "AddConstraintCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new AddConstraintCommand.
 */
AddConstraintCommand::AddConstraintCommand(NetworkVisualization *context, NeuronGroup *group, 
							 const GroupConstraint &constraint)
	: Command("Add Constraint"), mVisualizationContext(context)
{
	mGroups.append(group);
	mNewConstraints.append(constraint.createCopy());
}

AddConstraintCommand::AddConstraintCommand(NetworkVisualization *context, 	
			QList<NeuronGroup*> groups, const GroupConstraint &constraint)
	: Command("Add Constraint"), mVisualizationContext(context), mGroups(groups)
{
	for(int i = 0; i < mGroups.size(); ++i) {
		mNewConstraints.append(constraint.createCopy());
	}
}


/**
 * Destructor.
 */
AddConstraintCommand::~AddConstraintCommand() {
}


bool AddConstraintCommand::isUndoable() const {
	return true;
}


bool AddConstraintCommand::doCommand() {
	if(mGroups.empty() || mNewConstraints.size() != mGroups.size() 
		|| mVisualizationContext == 0) 
	{
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	bool ok = true;
	for(int i = 0; i < mGroups.size() && i < mNewConstraints.size(); ++i) {
		NeuronGroup *group = mGroups.at(i);
		GroupConstraint *constraint = mNewConstraints.at(i);

		if(!group->addConstraint(constraint)) {
			ok = false;
		}
	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return ok;
}


bool AddConstraintCommand::undoCommand() {
	if(mGroups.empty() || mNewConstraints.size() != mGroups.size() 
		|| mVisualizationContext == 0) 
	{
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}

	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	bool ok = true;
	for(int i = 0; i < mGroups.size() && i < mNewConstraints.size(); ++i) {
		NeuronGroup *group = mGroups.at(i);
		GroupConstraint *constraint = mNewConstraints.at(i);

		if(!group->removeConstraint(constraint)) {
			ok = false;
		}
	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	return ok;
}



}




