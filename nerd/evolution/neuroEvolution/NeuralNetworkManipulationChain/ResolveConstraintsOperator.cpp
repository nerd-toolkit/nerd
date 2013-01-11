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

#include "ResolveConstraintsOperator.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Core/Core.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Math/Random.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <iostream>
#include <QListIterator>
#include "Constraints/GroupConstraint.h"
#include <QTime>

//#define OUT(messag)
#define OUT(message) cerr << message

using namespace std;

namespace nerd {

ResolveConstraintsOperator::ResolveConstraintsOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name), mMaxNumberOfResolverIterations(0)
{
	mMaxNumberOfResolverIterations = new IntValue(25);
	addParameter("MaxNumberOfIterations", mMaxNumberOfResolverIterations);
}

ResolveConstraintsOperator::ResolveConstraintsOperator(const ResolveConstraintsOperator &other)
	: NeuralNetworkManipulationOperator(other)
{
	mMaxNumberOfResolverIterations = dynamic_cast<IntValue*>(
			getParameter("MaxNumberOfIterations"));
}

ResolveConstraintsOperator::~ResolveConstraintsOperator() {
}

NeuralNetworkManipulationOperator* ResolveConstraintsOperator::createCopy() const
{
	return new ResolveConstraintsOperator(*this);
}


bool ResolveConstraintsOperator::applyOperator(Individual *individual, CommandExecutor*) 
{

	bool verbose = false;

	ModularNeuralNetwork *net = dynamic_cast<ModularNeuralNetwork*>(individual->getGenome());
	QList<NeuralNetworkElement*> trashcan;

	if(net == 0) {
		Core::log("ResolveConstraintsOperator: Could not apply operator because individual did not "
				  "provide a ModularNeuralNetwork as genome!");
		return true;
	}

	QList<NeuronGroup*> groups = net->getNeuronGroups();

	if(verbose) {
		Core::log("ConstraintResolver: Checking for validity", true);
	}

	{
		for(QListIterator<NeuronGroup*> g(groups); g.hasNext();) {
			NeuronGroup *group = g.next();

			QList<GroupConstraint*> constraints = group->getConstraints();

			for(QListIterator<GroupConstraint*> c(constraints); c.hasNext();) {
				GroupConstraint *constraint = c.next();

				if(!constraint->isValid(group)) {
					//remove all networks that have invalid constraints.
					return false;
				}
			}
		}
	}

	bool constraintsResolved = true;

	QTime stopWatch;
	stopWatch.start();

	Core *core = Core::getInstance();

	int i = 0;
	for(; i < mMaxNumberOfResolverIterations->get(); ++i) {

		if(verbose) {
			Core::log("ConstraintResolver: Starting Resolver Iteration " + QString::number(i), true);
		}

		constraintsResolved = true;
		
		for(QListIterator<NeuronGroup*> g(groups); g.hasNext();) {
			NeuronGroup *group = g.next();

			QList<GroupConstraint*> constraints = group->getConstraints();

			for(QListIterator<GroupConstraint*> c(constraints); c.hasNext();) {
				GroupConstraint *constraint = c.next();

				if(!constraint->applyConstraint(group, 0, trashcan)) { //TODO add executor
					constraintsResolved = false;
					
					if(verbose) {
						Core::log("ConstraintResolver: Failed resolving [" + constraint->getName() + "]", true);
					}
				}
			}
		}

		if(constraintsResolved || stopWatch.elapsed() > 10000) {
			break;
		}
		core->executePendingTasks();
		if(core->isShuttingDown()) {
			return false;
		}
	}
// 	cerr << "Resolver ran " << i << " times for individual " << individual->getId() << endl;

	while(!trashcan.empty()) {
		NeuralNetworkElement *elem = trashcan.at(0);
		trashcan.removeAll(elem);
		delete elem;
	}

	if(constraintsResolved) {
		return true;
	}
	return false;
}


}


