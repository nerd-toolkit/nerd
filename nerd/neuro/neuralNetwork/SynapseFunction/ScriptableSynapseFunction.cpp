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

#include "ScriptableSynapseFunction.h"
#include <QListIterator>
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include <iostream>
#include "Value/CodeValue.h"
#include "Core/Core.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"

using namespace std;

namespace nerd {

ScriptableSynapseFunction::ScriptableSynapseFunction()
	: ScriptingContext("Scripted"), NeuroModulatorSynapseFunction("Scripted"), mErrorState(0), mOwner(0),
		mFirstExecution(true), mDefaultSynapseFunction(0)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();
	
	mErrorState = new StringValue();
	
	mVar1 = new DoubleValue();
	mVar2 = new DoubleValue();
	mVar3 = new StringValue();
	mVar4 = new StringValue();
	
	addObserableOutput("Eta", mVar1);
	addObserableOutput("Xi", mVar2);
	
	
	
	mScriptFileName->removeValueChangedListener(this);
	
	//set default code (make sure that the change is NOT notified, otherwise script may be executed
	//before this object is fully constructed!
	mScriptCode->removeValueChangedListener(this);
	mScriptCode->setValueFromString("function reset() {/**/}/**//**/function calc() {/**/\treturn 0.5;/**/}");
	mScriptCode->addValueChangedListener(this);
	mScriptCode->setNotifyAllSetAttempts(true);
	
	mScriptCode->setErrorValue(mErrorState);
	
	addParameter("Code", mScriptCode);
	addParameter("Eta", mVar1);
	addParameter("Xi", mVar2);
	addParameter("Kappa", mVar3);
	addParameter("Lambda", mVar4);
	
	
	//allow script executions and reset also in the GUI thread (to react on changes immediately)
	mRestrictToMainExecutionThread = false;
}

ScriptableSynapseFunction::ScriptableSynapseFunction(const ScriptableSynapseFunction &other)
	: Object(), ValueChangedListener(), EventListener(), NeuroModulatorElement(other), ScriptingContext(other), 
		NeuroModulatorSynapseFunction(other), mErrorState(0), mOwner(0), mFirstExecution(true), mDefaultSynapseFunction(0)
{
	mNetworkManipulator = new ScriptedNetworkManipulator();
	
	mErrorState = new StringValue();
	
	mVar1 = dynamic_cast<DoubleValue*>(getParameter("Eta"));
	mVar2 = dynamic_cast<DoubleValue*>(getParameter("Xi"));
	mVar3 = dynamic_cast<StringValue*>(getParameter("Kappa"));
	mVar4 = dynamic_cast<StringValue*>(getParameter("Lambda"));
	
	addObserableOutput("Eta", mVar1);
	addObserableOutput("Xi", mVar2);
	
	mScriptFileName->removeValueChangedListener(this);
	
	//replace script code value with the one stored as parameter.
	
	delete mScriptCode;
	mScriptCode = dynamic_cast<CodeValue*>(getParameter("Code"));
	mScriptCode->setNotifyAllSetAttempts(true);
	
	mScriptCode->setErrorValue(mErrorState);
	
	//allow script executions and reset also in the GUI thread (to react on changes immediately)
	mRestrictToMainExecutionThread = false;
	
	if(other.mDefaultSynapseFunction != 0) {
		mDefaultSynapseFunction = other.mDefaultSynapseFunction->createCopy();
	}
}

ScriptableSynapseFunction::~ScriptableSynapseFunction() {
	mScriptCode = 0;
	if(mNetworkManipulator != 0) {
		delete mNetworkManipulator;
	}
}

SynapseFunction* ScriptableSynapseFunction::createCopy() const {
	return new ScriptableSynapseFunction(*this);
}

QString ScriptableSynapseFunction::getName() const {
	return NeuroModulatorSynapseFunction::getName();
}

void ScriptableSynapseFunction::valueChanged(Value *value) {
	ScriptingContext::valueChanged(value);
	NeuroModulatorSynapseFunction::valueChanged(value);
	
	if(value == mScriptCode) {
		//additionally call the reset function in the script 
		//(a resetScriptContext has already been triggered by the ScriptingContext)
		executeScriptFunction("reset();");
	}
}


void ScriptableSynapseFunction::resetScriptContext() {
	mErrorState->set("");
	ScriptingContext::resetScriptContext();
}


void ScriptableSynapseFunction::reset(Synapse *owner) {
	mOwner = owner;
	
	ModularNeuralNetwork *network = 0;
	if(owner != 0 && owner->getSource() != 0) {
		network = dynamic_cast<ModularNeuralNetwork*>(owner->getSource()->getOwnerNetwork());
	}
	if(mNetworkManipulator != 0) {
		mNetworkManipulator->setNeuralNetwork(network);
		mNetworkManipulator->setOwnerHint(owner);
	}

	resetScriptContext();
	executeScriptFunction("reset();");
}


double ScriptableSynapseFunction::calculate(Synapse *owner) {
	if(owner == 0) {
		return 0.0;
	}

	if(mFirstExecution) {
		reset(owner);
		mFirstExecution = false;
	}

	executeScriptFunction("call_calc();");

	return mOutput.get();;
}

bool ScriptableSynapseFunction::equals(SynapseFunction *synapseFunction) const {
	if(NeuroModulatorSynapseFunction::equals(synapseFunction) == false) {
		return false;
	}
	ScriptableSynapseFunction *af =
 			dynamic_cast<ScriptableSynapseFunction*>(synapseFunction);

	if(af == 0) {
		return false;
	}
	
	if(!mScriptCode->equals(af->mScriptCode)) {
		return false;
	}
	if(!mVar1->equals(af->mVar1)) {
		return false;
	}
	if(!mVar2->equals(af->mVar2)) {
		return false;
	}
	if(!mVar3->equals(af->mVar3)) {
		return false;
	}
	if(!mVar4->equals(af->mVar4)) {
		return false;
	}
	if(mDefaultSynapseFunction == 0 && af->mDefaultSynapseFunction != 0) {
		return false;
	}
	if(af->mDefaultSynapseFunction != 0 && mDefaultSynapseFunction != 0) {
		if(!mDefaultSynapseFunction->equals(af->mDefaultSynapseFunction)) {
			return false;
		}
	}
	
	return true;
}


double ScriptableSynapseFunction::useDefaultSynapseFunction() {
	if(mDefaultSynapseFunction == 0) {
		return 0.0;
	}
	return mDefaultSynapseFunction->calculate(mOwner);
}


bool ScriptableSynapseFunction::setDefaultSynapseFunction(const QString &name) {
	if(mDefaultSynapseFunction != 0) {
		if(mDefaultSynapseFunction->getName() == name) {
			//don't change anything
			return true;
		}
		delete mDefaultSynapseFunction;
		mDefaultSynapseFunction = 0;
	}
	if(name == "") {
		//indicates to remove the default activation function
		return true;
	}
	
	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();
	QList<SynapseFunction*> prototypes = nnm->getSynapseFunctionPrototypes();
	for(QListIterator<SynapseFunction*> i(prototypes); i.hasNext();) {
		SynapseFunction *sf = i.next();
		if(sf->getName() == name) {
			mDefaultSynapseFunction = sf->createCopy();
			return true;
		}
	}
	return false;
}


void ScriptableSynapseFunction::reportError(const QString &message) {
	QString msg = message;
	mErrorState->set(msg.replace("\n", " | "));
	Core::log(QString("ScriptableSynapseFunction [") + getName() + "]: " + message);
}


void ScriptableSynapseFunction::addCustomScriptContextStructures() {

	ScriptingContext::addCustomScriptContextStructures();
	
	if(mScript == 0) {
		reportError(QString("There was no script engine object!"));
		return;
	}

	QScriptValue networkManip = mScript->newQObject(mNetworkManipulator,
							QScriptEngine::QtOwnership,
			 				QScriptEngine::ExcludeSuperClassMethods
							  | QScriptEngine::ExcludeSuperClassProperties
							  | QScriptEngine::ExcludeChildObjects);
	mScript->globalObject().setProperty("net", networkManip);

	QScriptValue error = mScript->evaluate("var __returnValue__ = 0;");
	error = mScript->evaluate("function call_calc() {  __returnValue__ = this.calc(); };");
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add call_calc() function. ") 
						+ error.toString());
		return;
	}
	
	qulonglong ownerId = 0;
	if(mOwner != 0) {
		ownerId = mOwner->getId();
	}
	qulonglong sourceId = 0;
	if(mOwner != 0 && mOwner->getSource() != 0) {
		sourceId = mOwner->getSource()->getId();
	}
	qulonglong targetId = 0;
	if(mOwner != 0 && mOwner->getTarget() != 0) {
		targetId = mOwner->getTarget()->getId();
	}
	
	error = mScript->evaluate(QString("var weight = 0;"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable weight: " + error.toString()));
		//ignore error and go on.
	}
	error = mScript->evaluate(QString("var synapse = " + QString::number(ownerId) + ";"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable synapse: " + error.toString()));
		//ignore error and go on.
	}
	error = mScript->evaluate(QString("var source = " + QString::number(sourceId) + ";"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable source: " + error.toString()));
		//ignore error and go on.
	}
	error = mScript->evaluate(QString("var target = " + QString::number(targetId) + ";"));
	if(mScript->hasUncaughtException()) {
		reportError(QString("Could not add variable target: " + error.toString()));
		//ignore error and go on.
	}
	
	
	defineVariable("eta", mVar1);
	defineVariable("xi", mVar2);
	defineVariable("kappa", mVar3);
	defineVariable("lambda", mVar4);
}

/**
 * Imports the content of all Values aded with defineVariables to the scripting context.
 * IntValues and DoubleValues are set as native ints and doubles. 
 * BoolValues will be translated to real bools (true / false).
 * All other Value types will be set as Strings with enclosing parantheses.
 */
void ScriptableSynapseFunction::importVariables() {
	ScriptingContext::importVariables();
	
	if(mScript == 0) {
		return;
	}
	
	if(mOwner != 0) {
		
		qulonglong ownerId = mOwner->getId();
		qulonglong sourceId = 0;
		if(mOwner->getSource() != 0) {
			sourceId = mOwner->getSource()->getId();
		}
		qulonglong targetId = 0;
		if(mOwner->getTarget() != 0) {
			targetId = mOwner->getTarget()->getId();
		}
	
		mScript->evaluate("weight = " + mOwner->getStrengthValue().getValueAsString() + ";");
		mScript->evaluate("synapse = " + QString::number(ownerId) + ";");
		mScript->evaluate("source = " + QString::number(sourceId) + ";");
		mScript->evaluate("target = " + QString::number(targetId) + ";");
	}
}



void ScriptableSynapseFunction::exportVariables() {
	ScriptingContext::exportVariables();
	
	if(mScript != 0) {
		
		mScript->evaluate(mMainContextName + ".stringBuffer = __returnValue__.toString();");
		mOutput.setValueFromString(mVariableBuffer);

	}
}


}


