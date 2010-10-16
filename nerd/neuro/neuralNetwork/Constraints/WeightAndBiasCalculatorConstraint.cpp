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



#include "WeightAndBiasCalculatorConstraint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Util/Util.h"
#include "NeuralNetworkConstants.h"
#include <QScriptEngine>
#include "Network/NeuroTagManager.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new WeightAndBiasCalculatorConstraint.
 */
WeightAndBiasCalculatorConstraint::WeightAndBiasCalculatorConstraint()
	: GroupConstraint("WeightAndBiasCalc")
{
	NeuroTagManager *ntm = NeuroTagManager::getInstance();
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_ELEMENT_VARIABLE_MASTER, 
						 NeuralNetworkConstants::TAG_TYPE_NETWORK_ELEMENT, 
						 "Defines a (master) variable for a bias or weight."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_ELEMENT_VARIABLE_SLAVE, 
						 NeuralNetworkConstants::TAG_TYPE_NETWORK_ELEMENT, 
						 "Defines a (slave) variable equation to calculate a bias "
						 "or weight."));
}


/**
 * Copy constructor. 
 * 
 * @param other the WeightAndBiasCalculatorConstraint object to copy.
 */
WeightAndBiasCalculatorConstraint::WeightAndBiasCalculatorConstraint(
						const WeightAndBiasCalculatorConstraint &other) 
	: Object(), ValueChangedListener(), GroupConstraint(other)
{
}

/**
 * Destructor.
 */
WeightAndBiasCalculatorConstraint::~WeightAndBiasCalculatorConstraint() {
}


GroupConstraint* WeightAndBiasCalculatorConstraint::createCopy() const {
	return new WeightAndBiasCalculatorConstraint(*this);
}

bool WeightAndBiasCalculatorConstraint::isValid(NeuronGroup*) {
	return true;
}

bool WeightAndBiasCalculatorConstraint::applyConstraint(NeuronGroup *owner, 
									CommandExecutor*, 
									QList<NeuralNetworkElement*>&)
{
	if(owner == 0) {
		//do not do anything.
		return true;
	}

	bool networkChanged = false;

	//collect variable equations, master variables, neurons and synapses
	QList<Neuron*> neurons = owner->getNeurons();
	QList<Synapse*> synapses;

	QHash<NeuralNetworkElement*, QString> equations;
	QHash<QString, QString> variables;

	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		Util::addWithoutDuplicates(synapses, neuron->getSynapses());
		Util::addWithoutDuplicates(synapses, neuron->getOutgoingSynapses());
		QString var = neuron->getProperty(NeuralNetworkConstants::TAG_ELEMENT_VARIABLE_MASTER);
		QString eq = neuron->getProperty(NeuralNetworkConstants::TAG_ELEMENT_VARIABLE_SLAVE);

		if(var.trimmed() != "") {
			//makes sure the variable carrier has a variable accuracy matching the string context.
			neuron->getBiasValue().setValueFromString(normalizeAccuracy(neuron->getBiasValue().get()));

			if(variables.keys().contains(var)) {
				QString message = QString("Variable [") 
							+ var + "] was declared multiple times!";
				mErrorMessage.append(message + "\n");
				Core::log("WeightAndBiasCalculatorConstraint: " + message);
				return false;
			}
			else {
				variables.insert(var, neuron->getBiasValue().getValueAsString());
			}
		}
		if(eq.trimmed() != "") {
			if(equations.keys().contains(neuron)) {
				QString message = QString("Neuron [") 
							+ QString::number(neuron->getId())
							+ "] has multiple equation lables!";
				mErrorMessage.append(message + "\n");
				Core::log("WeightAndBiasCalculatorConstraint: " + message);
				return false;
			}
			else {
				equations.insert(neuron, eq);
			}
		}
	}
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();
		QString var = synapse->getProperty(NeuralNetworkConstants::TAG_ELEMENT_VARIABLE_MASTER);
		QString eq = synapse->getProperty(NeuralNetworkConstants::TAG_ELEMENT_VARIABLE_SLAVE);

		if(var.trimmed() != "") {
			//makes sure the variable carrier has a variable accuracy matching the string context.
			synapse->getStrengthValue().setValueFromString(normalizeAccuracy(synapse->getStrengthValue().get()));


			if(variables.keys().contains(var)) {
				QString message = QString("Variable [") 
							+ var + "] was declared multiple times!";
				mErrorMessage.append(message + "\n");
				Core::log("WeightAndBiasCalculatorConstraint: " + message);
				return false;
			}
			else {
				variables.insert(var, synapse->getStrengthValue().getValueAsString());
			}
		}
		if(eq.trimmed() != "") {
			if(equations.keys().contains(synapse)) {
				QString message = QString("WeightAndBiasCalculatorConstraint: Synapse [") 
							+ QString::number(synapse->getId())
							+ "] has multiple equation lables!";
				mErrorMessage.append(message + "\n");
				Core::log("WeightAndBiasCalculatorConstraint: " + message);
				return false;
			}
			else {
				equations.insert(synapse, eq);
			}
		}
	}

	if(equations.empty() || variables.empty()) {
		return true;
	}

	QScriptEngine calc;

	for(QHashIterator<QString, QString> i(variables); i.hasNext();) {
		i.next();
		QScriptValue error = calc.evaluate(QString("var ") + i.key() + " = " + i.value() + ";");
		if(calc.hasUncaughtException()) {
			QString message = QString("Could not define variable ") + i.key() + " to value " + i.value();
			mErrorMessage.append(message).append("\n");
			Core::log("WeightAndBiasCalculatorConstraint: " + message);
			return false;
		}
	}
	for(QHashIterator<NeuralNetworkElement*, QString> i(equations); i.hasNext();) {
		i.next();
		QScriptValue result = calc.evaluate(i.value());
		if(calc.hasUncaughtException()) {
			QString message = QString("Could not evaluate equation ") + i.value() 
								 + " of element [" + QString::number(i.key()->getId()) + "]";
			mErrorMessage.append(message + "\n");
			Core::log("WeightAndBiasCalculatorConstraint: " + message);
			return false;
		}
		else {
			double value = result.toNumber();
			Neuron *neuron = dynamic_cast<Neuron*>(i.key());
			if(neuron != 0) {
				double oldVal = neuron->getBiasValue().get();
				neuron->getBiasValue().set(value);
				if(!Math::compareDoubles(oldVal, neuron->getBiasValue().get(), 0.00000001)) {

					//Core::log("BCC 1: " + QString::number(oldVal, 'f', 20) + " " + QString::number(value, 'f', 20), true);

					networkChanged = true;
				}
			}
			else {
				Synapse *synapse = dynamic_cast<Synapse*>(i.key());
				if(synapse != 0) {
					double oldVal = synapse->getStrengthValue().get();
					synapse->getStrengthValue().set(value);
					if(!Math::compareDoubles(oldVal, synapse->getStrengthValue().get(), 0.00000001)) {

						//Core::log("BCC 2: " + QString::number(oldVal, 'f', 20) + " " + QString::number(value, 'f', 20), true);

						networkChanged = true;
					}
				}
			}
		}
	}

	return !networkChanged;
}

QString WeightAndBiasCalculatorConstraint::normalizeAccuracy(double value) {

// 	QString reducedAccuracy = value;
// 	int indexOfDot = reducedAccuracy.indexOf(".");
// 	if(indexOfDot >= 0 &&  indexOfDot < reducedAccuracy.length() - 6) {
// 		reducedAccuracy = reducedAccuracy.mid(0, Math::min(reducedAccuracy.length(), indexOfDot + 6));
// 	}
// 	return reducedAccuracy;
	return QString::number(value, 'f', 6);
}
		
bool WeightAndBiasCalculatorConstraint::equals(GroupConstraint *constraint) {
	if(!GroupConstraint::equals(constraint)) {
		return false;
	}
	WeightAndBiasCalculatorConstraint *wcc = 
					dynamic_cast<WeightAndBiasCalculatorConstraint*>(constraint);

	if(wcc == 0) {
		return false;
	}
	return true;
}


}



