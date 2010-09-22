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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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

#include "ConstraintManager.h"
#include "Core/Core.h"
#include "NeuralNetworkConstants.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <iostream>

using namespace std;

namespace nerd {

ConstraintManager::ConstraintManager()
{
}

ConstraintManager::~ConstraintManager() {
	while(!mConstraintPrototypes.empty()) {
		GroupConstraint *c = mConstraintPrototypes.at(0);
		mConstraintPrototypes.removeAll(c);
		c->unpublishAllParameters();
		delete c;
	}
}

QString ConstraintManager::getName() const {
	return "ConstraintManager";
}

bool ConstraintManager::registerAsGlobalObject() {
	Core::getInstance()->addGlobalObject(
		NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER, this);

	return Core::getInstance()->getGlobalObject(
		NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER) == this;

}


bool ConstraintManager::init() {
	bool ok = true;

	return ok;
}


bool ConstraintManager::bind() {
	bool ok = true;

	return ok;
}


bool ConstraintManager::cleanUp() {
	return true;
}

bool ConstraintManager::addConstraintPrototype(GroupConstraint *constraint) {
	if(constraint == 0 || mConstraintPrototypes.contains(constraint)) {
		return false;
	}
	mConstraintPrototypes.append(constraint);

	return true;
}


bool ConstraintManager::removeConstrantPrototype(GroupConstraint *constraint) {
	if(constraint == 0 || !mConstraintPrototypes.contains(constraint)) {
		return false;
	}
	mConstraintPrototypes.removeAll(constraint);

	return true;
}

GroupConstraint* ConstraintManager::getConstraintPrototype(const QString &name) const {
	for(QListIterator<GroupConstraint*> i(mConstraintPrototypes); i.hasNext();) {
		GroupConstraint *constraint = i.next();
		if(constraint->getName() == name) {
			return constraint;
		}
	}
	return 0;
}


const QList<GroupConstraint*>& ConstraintManager::getConstraintPrototypes() const {
	return mConstraintPrototypes;
}



/**
 * Checks whether all constraints of a all NeuronGroups in the given network are valid.
 * Returns an empty QStringList if all constraints are valid, otherwise
 * the QStringList contains the error messages of this check.
 */
QStringList ConstraintManager::verifyAllConstraints(ModularNeuralNetwork *net) {

	QStringList errors;

	const QList<NeuronGroup*> &groups = net->getNeuronGroups();

	{
		for(QListIterator<NeuronGroup*> g(groups); g.hasNext();) {
			NeuronGroup *group = g.next();

			errors << ConstraintManager::verifyConstraints(group);
		}
	}

	return errors;
}


/**
 * Checks whether all constraints of a NeuronGroup are valid.
 * Returns an empty QStringList if all constraints are valid, otherwise
 * the QStringList contains the error messages of this check.
 */
QStringList ConstraintManager::verifyConstraints(NeuronGroup *group) {

	QStringList errors;

	const QList<GroupConstraint*> &constraints = group->getConstraints();

	for(QListIterator<GroupConstraint*> c(constraints); c.hasNext();) {
		GroupConstraint *constraint = c.next();

		if(!constraint->isValid(group)) {
			errors.append(QString("Constraint [") + constraint->getName()
				+ "] of NeuronGroup [" + group->getName() + "]["
				+ QString::number(group->getId()) + "] is invalid!");
			errors.append(constraint->getErrorMessage());
		}
	}

	return errors;
}



bool ConstraintManager::runConstraints(QList<NeuronGroup*> groups, int maxIterations,
						CommandExecutor *executor, QList<NeuralNetworkElement*> &trashcan,
						QStringList &errors)
{

	//check if all constraints are valid.
	for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
		errors << ConstraintManager::verifyConstraints(i.next());
	}

	if(!errors.empty()) {
		return false;
	}

	errors.clear();

	//resolve constraints
	for(int i = 0; i < maxIterations; ++i) {
		bool allOk = true;
		errors.append(QString("-- Resolver Run ").append(QString::number(i)).append(" --"));

		for(QListIterator<NeuronGroup*> g(groups); g.hasNext();) {
			NeuronGroup *group = g.next();

			QStringList resolverErrors;
			if(!ConstraintManager::runGroupConstraints(group, 1, executor, trashcan, resolverErrors)) {
				allOk = false;
			}

			if(!resolverErrors.empty()) {
				errors.append(QString("> ") + group->getName() + " (" + QString::number(group->getId()) 
								+ ") :");
				errors << resolverErrors;
			}
		}

		if(allOk) {
			return true;
		}
	}

	return false;
}


bool ConstraintManager::runGroupConstraints(NeuronGroup *group, 
					int maxIterations, CommandExecutor *executor, 
					QList<NeuralNetworkElement*> &trashcan,
					QStringList &errors) 
{
	const QList<GroupConstraint*> &constraints = group->getConstraints();

	for(int i = 0; i < maxIterations; ++i) {
		bool allOk = true;
		for(QListIterator<GroupConstraint*> c(constraints); c.hasNext();) {
			GroupConstraint *constraint = c.next();

			if(!constraint->applyConstraint(group, executor, trashcan)) {
				errors << constraint->getErrorMessage();
				allOk = false;
			}
		}
		if(allOk) {
			errors.clear();
			return true;
		}
	}

	return false;
}


}


