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

#include "ConstraintManager.h"
#include "Core/Core.h"
#include "NeuralNetworkConstants.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <iostream>
#include "Network/NeuroTagManager.h"
#include "Core/Properties.h"
#include "Network/NeuralNetworkElement.h"
#include "Event/EventManager.h"

using namespace std;

namespace nerd {
	
bool ConstraintManager::mMarkConstrainedElements = false;
ConstraintManager* ConstraintManager::sConstraintManager = 0;

ConstraintManager::ConstraintManager()
{
	NeuroTagManager *ntm = NeuroTagManager::getInstance();
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM, 
						 NeuralNetworkConstants::TAG_TYPE_NETWORK_ELEMENT,
						 "Contains a list of all restricted degrees of freedom of the "
						 "network element.\n"
						 "(B,W,TF,SF,AF,E)"));
	
	ConstraintManager::mMarkConstrainedElements = false;
	
	mConstraintsUpdatedEvent = Core::getInstance()->getEventManager()->getEvent(
			NeuralNetworkConstants::EVENT_CONSTRAINTS_UPDATED, true);

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
	sConstraintManager = this;
	
	Core::getInstance()->addGlobalObject(
		NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER, this);

	return Core::getInstance()->getGlobalObject(
		NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER) == this;

}

ConstraintManager* ConstraintManager::getInstance() {
	ConstraintManager *cm = dynamic_cast<ConstraintManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER));
	if(cm == 0) {
		cm = new ConstraintManager();
		if(!cm->registerAsGlobalObject()) {
			delete cm;
		}
		cm = dynamic_cast<ConstraintManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER));
	}
	return cm;
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


QList<GroupConstraint*> ConstraintManager::getConstraintPrototypes() const {
	return mConstraintPrototypes;
}

void ConstraintManager::notifyConstraintsUpdated() {
	if(mConstraintsUpdatedEvent != 0) {
		mConstraintsUpdatedEvent->trigger();
	}
}

/**
 * Checks whether all constraints of a all NeuronGroups in the given network are valid.
 * Returns an empty QStringList if all constraints are valid, otherwise
 * the QStringList contains the error messages of this check.
 */
QStringList ConstraintManager::verifyAllConstraints(ModularNeuralNetwork *net) {

	QStringList errors;

	QList<NeuronGroup*> groups = net->getNeuronGroups();

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

	QList<GroupConstraint*> constraints = group->getConstraints();

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
						QStringList &errors, bool collectOnlyErrorsOfLastResolverRun)
{
	ConstraintManager::mMarkConstrainedElements = true;
	
	ModularNeuralNetwork *net = 0;
	
	if(!groups.empty() && groups.at(0) != 0) {
		net = groups.at(0)->getOwnerNetwork();
		if(net != 0) {
			{
				QList<Neuron*> elements = net->getNeurons();
				for(int i = 0; i < elements.size(); ++i) {
					Neuron *neuron = elements.at(i);
					neuron->removeProperty(
						NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM);
					
					if(neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
						ConstraintManager::markElementAsConstrained(neuron, "BTA");
					}
					else {
						if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_PROTECT_BIAS))
						{
							ConstraintManager::markElementAsConstrained(neuron, "B");
						}
					}
				}
			}
			{
				QList<Synapse*> elements = net->getSynapses();
				for(int i = 0; i < elements.size(); ++i) {
					Synapse *synapse = elements.at(i);
					synapse->removeProperty(
						NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM);
					
					if(synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
						ConstraintManager::markElementAsConstrained(synapse, "WS");
					}
					else {
						if(synapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_PROTECT_STRENGTH))
						{
							ConstraintManager::markElementAsConstrained(synapse, "W");
						}
					}
				}
			}
			{
				QList<NeuronGroup*> elements = net->getNeuronGroups();
				for(int i = 0; i < elements.size(); ++i) {
					elements.at(i)->removeProperty(
						NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM);
				}
			}
		}
	}
	
	//check if all constraints are valid.
// 	for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
// 		errors << ConstraintManager::verifyConstraints(i.next());
// 	}

	if(!errors.empty()) {
		ConstraintManager::mMarkConstrainedElements = false;
		return false;
	}
	
	//reset all constraints
	for(QListIterator<NeuronGroup*> g(groups); g.hasNext();) {
		QList<GroupConstraint*> constraints = g.next()->getConstraints();
		for(QListIterator<GroupConstraint*> gc(constraints); gc.hasNext();) {
			gc.next()->reset();
		}
	}

	errors.clear();
	
	bool resolverSuccess = false;

	//resolve constraints
	for(int i = 0; i < maxIterations; ++i) {
		bool allOk = true;
		
		if(collectOnlyErrorsOfLastResolverRun) {
			errors.clear();
		}
		
		errors.append(QString("-- Resolver Run ").append(QString::number(i)).append(" --"));

		for(QListIterator<NeuronGroup*> g(groups); g.hasNext();) {
			NeuronGroup *group = g.next();

			QStringList resolverErrors;
			QStringList resolverWarnings;
			if(!ConstraintManager::runGroupConstraints(group, 1, executor, trashcan, 
													resolverErrors, resolverWarnings)) 
			{
				allOk = false;
			}

			if(!resolverErrors.empty()) {
				errors.append(QString("> ") + group->getName() + " (" + QString::number(group->getId()) 
								+ ") Errors:");
				errors << resolverErrors;
			}
			if(!resolverWarnings.empty()) {
				errors.append(QString("> ") + group->getName() + " (" + QString::number(group->getId()) 
								+ ") Warnings:");
				errors << resolverWarnings;
			}
		}

		if(allOk) {
// 			ConstraintManager::mMarkConstrainedElements = false;
// 			if(sConstraintManager != 0) {
// 				sConstraintManager->notifyConstraintsUpdated();
// 			}
// 			return true;
			resolverSuccess = true;
			break;
		}
	}
	ConstraintManager::mMarkConstrainedElements = false;
	if(sConstraintManager != 0) {
		sConstraintManager->notifyConstraintsUpdated();
	}
	
	//remove resolver working tags (starting with  _##_)
	if(net != 0) {
		QList<NeuralNetworkElement*> allElements;
		net->getNetworkElements(allElements);
		
		for(QListIterator<NeuralNetworkElement*> i(allElements); i.hasNext();) {
			NeuralNetworkElement *elem = i.next();
			
			elem->removePropertyByPattern(NeuralNetworkConstants::PROP_PREFIX_CONSTRAINT_TEMP + ".*");
		}
	}
	
	return resolverSuccess;
}


bool ConstraintManager::runGroupConstraints(NeuronGroup *group, 
					int maxIterations, CommandExecutor *executor, 
					QList<NeuralNetworkElement*> &trashcan,
					QStringList &errors, QStringList &warnings) 
{
	QList<GroupConstraint*> constraints = group->getConstraints();

	for(int i = 0; i < maxIterations; ++i) {
		bool allOk = true;
		
		warnings.clear();
		
		for(QListIterator<GroupConstraint*> c(constraints); c.hasNext();) {
			GroupConstraint *constraint = c.next();

			constraint->setErrorMessage("");
			if(!constraint->applyConstraint(group, executor, trashcan)) {
				errors << constraint->getName() + ": " + constraint->getErrorMessage();
				allOk = false;
			}
			if(constraint->getWarningMessage() != "") {
				warnings << constraint->getName() + ": " + constraint->getWarningMessage();
			}
		}
		if(allOk) {
			errors.clear();
			return true;
		}
	}

	return false;
}

void ConstraintManager::markElementAsConstrained(NeuralNetworkElement *elem, const QString &dof) {
	if(!ConstraintManager::mMarkConstrainedElements) {
		return;
	}
	Properties *prop = dynamic_cast<Properties*>(elem);
	if(prop == 0 || dof == "") {
		return;
	}
	QString rdofString = prop->getProperty(
			NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM);
	if(rdofString.contains(dof)) {
		return;
	}
	rdofString = rdofString.append(dof);
	prop->setProperty(NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM, rdofString);
}


}


