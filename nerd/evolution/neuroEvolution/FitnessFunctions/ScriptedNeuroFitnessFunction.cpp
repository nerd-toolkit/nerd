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



#include "ScriptedNeuroFitnessFunction.h"
#include <QList>
#include "Core/Core.h"
#include <QScriptEngine>
#include <QStringList>
#include <QHashIterator>
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "Value/BoolValue.h"
#include "Value/StringValue.h"
#include "PlugIns/CommandLineArgument.h"
#include "PlugIns/PlugInManager.h"
#include <QFile>
#include <QTextStream>
#include "EvolutionConstants.h"
#include "Value/MultiPartValue.h"
#include "Util/Tracer.h"
#include <QDir>
#include <QTextStream>
#include "Network/NeuralNetwork.h"
#include "Script/ScriptedNetworkManipulator.h" 
#include <iostream>

using namespace std;

namespace nerd {


/**
 * Constructs a new ScriptedNeuroFitnessFunction.
 */
ScriptedNeuroFitnessFunction::ScriptedNeuroFitnessFunction(const QString &name)
	: ScriptedFitnessFunction(name), mNetworkManipulator(0)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();
}


/**
 * Copy constructor. 
 * 
 * @param other the ScriptedNeuroFitnessFunction object to copy.
 */
ScriptedNeuroFitnessFunction::ScriptedNeuroFitnessFunction(const ScriptedNeuroFitnessFunction &other) 
	: ScriptedFitnessFunction(other)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();
}

/**
 * Destructor.
 */
ScriptedNeuroFitnessFunction::~ScriptedNeuroFitnessFunction() {
	if(mNetworkManipulator != 0) {
		delete mNetworkManipulator;
	}
}


/**
 * Creates a new instance as copy of this.
 *
 * @return a copy of this FitnessFunction.
 */
FitnessFunction* ScriptedNeuroFitnessFunction::createCopy() const {
	return new ScriptedNeuroFitnessFunction(*this);
}


void ScriptedNeuroFitnessFunction::setControlInterface(ControlInterface *controlInterface) {
	ScriptedFitnessFunction::setControlInterface(controlInterface);

	ModularNeuralNetwork *network = 0;
	if(controlInterface != 0) {
		network = dynamic_cast<ModularNeuralNetwork*>(controlInterface->getController());
	}
	if(mNetworkManipulator != 0) {
		mNetworkManipulator->setNeuralNetwork(network);
	}
}

void ScriptedNeuroFitnessFunction::resetScriptContext() {
	mNetworkManipulator->setNeuralNetwork(0);
	ScriptedFitnessFunction::resetScriptContext();
}

void ScriptedNeuroFitnessFunction::prepareNextTry() {
	ScriptedFitnessFunction::prepareNextTry();
	
	if(mControlInterface != 0) {
		ModularNeuralNetwork *network = dynamic_cast<ModularNeuralNetwork*>(mControlInterface->getController());
		mNetworkManipulator->setNeuralNetwork(network);
	}
}

void ScriptedNeuroFitnessFunction::defineNeuron(const QString &name, const QString &neuronName) {
	if(mObservedNeurons.keys().contains(name) || mObservedNeurons.values().contains(neuronName)) {
		return;
	}
	mObservedNeurons.insert(name, neuronName);
}

/**
* Convenience method to access the activity of a neuron. 
* @param neuronName The name of the neuron.
* @return The current output activity of the neuron.
*/
double ScriptedNeuroFitnessFunction::getNeuronActivity(const QString &neuronName) const {


	if(mControlInterface == 0 
		|| dynamic_cast<NeuralNetwork*>(mControlInterface->getController()) == 0)
	{
		return 0.0;
	}

	QList<Neuron*> neurons = dynamic_cast<NeuralNetwork*>(
		mControlInterface->getController())->getNeurons();
	for(int i = 0; i < neurons.size(); i++) {
		Neuron *neuron = neurons.at(i);
		if(neuron->getNameValue().get() == neuronName) {
			return neuron->getOutputActivationValue().get();
		}
	}

	Core::log("ScriptedNeuroFitnessFunction: Could not find neuron " + neuronName);

	return 0;
}

void ScriptedNeuroFitnessFunction::addCustomScriptContextStructures() {

	ScriptedFitnessFunction::addCustomScriptContextStructures();

	QScriptValue networkManip = mScript->newQObject(mNetworkManipulator,
							QScriptEngine::QtOwnership,
			 				QScriptEngine::ExcludeSuperClassMethods
							  | QScriptEngine::ExcludeSuperClassProperties
							  | QScriptEngine::ExcludeChildObjects);
	mScript->globalObject().setProperty("net", networkManip);

	QScriptValue error = mScript->evaluate(QString("function defNeuron(varName, neuronName) {")
					  + "  if(arguments.length != 2"
					  + "    || (typeof arguments[0] != 'string')"
					  + "    || (typeof arguments[1] != 'string')) "
					  + "  { "
					  + "    print('defNeuron: Invalid use of parameters!');"
					  + "    throw TypeError('defVar: requires two string parameters!');"
					  + "  }"
					  + "  nerd.defineNeuron(varName, neuronName);"
					  + "}");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add defNeuron function. ") 
						+ error.toString());
		return;
	}

	if(mControlInterface != 0) {
		ModularNeuralNetwork *network = dynamic_cast<ModularNeuralNetwork*>(mControlInterface->getController());
		mNetworkManipulator->setNeuralNetwork(network);
	}

	for(QHash<QString, QString>::iterator i = mObservedNeurons.begin(); 
		i != mObservedNeurons.end(); i++) 
	{
		error = mScript->evaluate(QString("var ") + i.key() + " = 0;");
		if(mScript->hasUncaughtException()) {
			reportError(QString("Could not add variable with name [") 
					+ i.key() + "]: " + error.toString());
			//ignore error and go on.
		}
	}
}

/**
 * Imports the content of all Values aded with defineVariables to the scripting context.
 * IntValues and DoubleValues are set as native ints and doubles. 
 * BoolValues will be translated to real bools (true / false).
 * All other Value types will be set as Strings with enclosing parantheses.
 */
void ScriptedNeuroFitnessFunction::importVariables() {
	ScriptingContext::importVariables();

	if(mScript != 0) {
		//update neuron variables
		for(QHash<QString, QString>::iterator i = mObservedNeurons.begin(); 
			i != mObservedNeurons.end(); i++) 
		{
			//mScript->evaluate(i.key() + " = " + QString::number(getNeuronActivity(i.value())) + ";");
		}
	}
}

}



