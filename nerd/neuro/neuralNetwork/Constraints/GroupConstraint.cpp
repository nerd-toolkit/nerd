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

#include "GroupConstraint.h"
#include "Network/NeuralNetwork.h"
#include <iostream>

using namespace std;

namespace nerd {

GroupConstraint::GroupConstraint(const QString &name, qulonglong id)
	: ParameterizedObject(name), mId(id), mOwnerGroup(0)
{
	if(mId == 0) {
		mId = NeuralNetwork::generateNextId();
	}
}

GroupConstraint::GroupConstraint(const GroupConstraint &other) 
	: Object(), ValueChangedListener(), ParameterizedObject(other), mId(other.mId),
		mOwnerGroup(0)
{
}

GroupConstraint::~GroupConstraint() {
}


void GroupConstraint::setId(qulonglong id) {
	mId = id;
}


qulonglong GroupConstraint::getId() const {
	return mId;
}


bool GroupConstraint::attachToGroup(NeuronGroup *group) {
	mOwnerGroup = group;
	return true;
}


bool GroupConstraint::detachFromGroup(NeuronGroup*) {
	mOwnerGroup = 0;
	return true;
}


/** 
 * Can be overwritten to react on id changes of objects such as synapses, neurons or 
 * groups. This is suitable when an object is referenced by its id.
 */
void GroupConstraint::networkElementIdChanged(NeuronGroup *owner, qulonglong oldId, qulonglong newId) {
	//just to avoid warnings.
	oldId = oldId;
	newId = newId;
	owner = owner;
}


/**
 * Is called before the first iteration of constraint resolver attempts.
 * This can be used to count the current resolver iteration or to do
 * things stepwise.
 */
void GroupConstraint::reset() {
	mErrorMessage = "";
	mWarningMessage = "";
}

/**
 * Called to notify the constraint that some of the eventually controlled network elements
 * have changed ids now. The changedIds hash table contains all affected ids.
 * This method should be used to adapt id pointers to the new ids. 
 */
bool GroupConstraint::groupIdsChanged(QHash<qulonglong, qulonglong>) {
	return true;
}

/**
 * Returns the error message. This message contains a verbal description of reasons
 * why isValid(), setRequiredElements and applyConstraints() failed. 
 * Note: The message is only updated in case of an error, so in the meanwhile it will
 * keep the last message until the next error occurs.
 *
 * @return the error message of this Constraint.
 */
QString GroupConstraint::getErrorMessage() const {
	return  mErrorMessage;
}

/**
 * Can be used to set the error message. Usually this is done only by the Constraint
 * itself. Subclasses of GroupConstraints can also directly access the mErrorMessage
 * variable which is protected.
 */
void GroupConstraint::setErrorMessage(const QString &message) {
	mErrorMessage = message;
}

NeuronGroup* GroupConstraint::getOwnerGroup() const {
	return mOwnerGroup;
}


QString GroupConstraint::getWarningMessage() const {
	return mWarningMessage;
}


void GroupConstraint::setWarningMessage(const QString &message) {
	mWarningMessage = message;
}



bool GroupConstraint::equals(GroupConstraint *constraint) const {
	if(constraint == 0) {
		return false;
	}
	if(ParameterizedObject::equals(constraint) == false) {
		return false;
	}
	return true;
}



/**
 * Checks whether the Constraint is valid for the given ModularNeuralnetwork and 
 * the owner NeuronGroup. This method may check of all required elements are available
 * or if the group or network provide all required objects or attributes.
 *
 * Note: Networks with invalid constrains will usually be removed from any 
 * evolution population.
 */
// bool GroupConstraint::isValid(ModularNeuralNetwork *net, NeuronGroup *owner) {
// 	//just to avoid warnings
// 	net = net,
// 	owner = owner;
// 	return false;
// }


/**
 * Applies the constraint. This method will do checks and/or modifications to the 
 * NeuralNetwork. An important property of this method is that this method
 * returns true if and only if everything was ok without the need to do any changes
 * to the network or group. In all other cases (e.g. when a failed check was 
 * automatically corrected) the method has to return false.
 *
 * @return true iff the constraint is fulfilled and no modifications have been done.
 */
// bool GroupConstraint::applyConstraint(ModularNeuralNetwork *net, NeuronGroup *owner) {
// 	//just to avoid warnings
// 	net = net;
// 	owner = owner;
// 	return false;
// }

}


