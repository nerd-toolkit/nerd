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



#include "RemoveConstraintCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new RemoveConstraintCommand.
 */
RemoveConstraintCommand::RemoveConstraintCommand(NetworkVisualization *context, NeuronGroup *group, 
							 GroupConstraint *constraint)
	: Command("Remove Constraint"), mVisualizationContext(context), 
	  mObsoleteConstraint(constraint), mRemoveAll(false)
{
	mGroups.append(group);
}

RemoveConstraintCommand::RemoveConstraintCommand(NetworkVisualization *context, 	
							QList<NeuronGroup*> groups, GroupConstraint *constraint)
: Command("Remove Constraint"), mVisualizationContext(context), mGroups(groups), 
	  mObsoleteConstraint(constraint), mRemoveAll(true)
{

}


/**
 * Destructor.
 */
RemoveConstraintCommand::~RemoveConstraintCommand() {
}


bool RemoveConstraintCommand::isUndoable() const {
	return true;
}


bool RemoveConstraintCommand::doCommand() {
	if(mGroups.empty() || mObsoleteConstraint == 0 || mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	mRemovedConstraints.clear();

	QString constraintName = mObsoleteConstraint->getName();

	for(QListIterator<NeuronGroup*> i(mGroups); i.hasNext();) {
		NeuronGroup *group = i.next();

		QList<GroupConstraint*> removedConstraints;

		if(mRemoveAll) {
			QList<GroupConstraint*> constraints = group->getConstraints();
			for(QListIterator<GroupConstraint*> j(constraints); j.hasNext();) {
				GroupConstraint *constraint = j.next();
				if(constraint->getName() == constraintName) {
					group->removeConstraint(constraint);
					removedConstraints.append(constraint);
				}
			}
			
		}
		else {
			if(!group->removeConstraint(mObsoleteConstraint)) {
				return false;
			}
			else {
				removedConstraints.append(mObsoleteConstraint);
			}
		}

		mRemovedConstraints.insert(group, removedConstraints);
	}


	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


bool RemoveConstraintCommand::undoCommand() {
	if(mGroups.empty() || mObsoleteConstraint == 0 || mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	for(QListIterator<NeuronGroup*> i(mGroups); i.hasNext();) {
		NeuronGroup *group = i.next();

		QList<GroupConstraint*> removedConstraints = mRemovedConstraints.value(group);

		if(mRemoveAll) {
			for(QListIterator<GroupConstraint*> j(removedConstraints); j.hasNext();) {
				group->addConstraint(j.next());
			}
			
		}
		else {
			if(!group->addConstraint(mObsoleteConstraint)) {
				return false;
			}
		}
	}
		
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();
	return true;
}


}




