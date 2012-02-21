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



#include "ScriptableSelfRegulatingNeuronActivationFunction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuron.h"
#include "Math/Math.h"
#include <math.h>


using namespace std;

namespace nerd {


/**
 * Constructs a new ScriptableSelfRegulatingNeuronActivationFunction.
 */
ScriptableSelfRegulatingNeuronActivationFunction::ScriptableSelfRegulatingNeuronActivationFunction()
	: QObject(), SelfRegulatingNeuronActivationFunction("Script_SRN_V1"), mEquationScript(0)
{
	mTransmitterEquation = new StringValue();
	mReceptorEquation = new StringValue();
	
	addParameter("Trans-Eq", mTransmitterEquation);
	addParameter("Recept-Eq", mReceptorEquation);
}


/**
 * Copy constructor. 
 * 
 * @param other the ScriptableSelfRegulatingNeuronActivationFunction object to copy.
 */
ScriptableSelfRegulatingNeuronActivationFunction::ScriptableSelfRegulatingNeuronActivationFunction(
						const ScriptableSelfRegulatingNeuronActivationFunction &other) 
	: QObject(), SelfRegulatingNeuronActivationFunction(other), mEquationScript(0)
{
	mTransmitterEquation = dynamic_cast<StringValue*>(getParameter("Trans-Eq"));
	mReceptorEquation = dynamic_cast<StringValue*>(getParameter("Recept-Eq"));
}

/**
 * Destructor.
 */
ScriptableSelfRegulatingNeuronActivationFunction::~ScriptableSelfRegulatingNeuronActivationFunction() {
	if(mEquationScript != 0) {
		delete mEquationScript;
	}
}


ActivationFunction* ScriptableSelfRegulatingNeuronActivationFunction::createCopy() const {
	return new ScriptableSelfRegulatingNeuronActivationFunction(*this);
}

void ScriptableSelfRegulatingNeuronActivationFunction::reset(Neuron *owner) {
	SelfRegulatingNeuronActivationFunction::reset(owner);
}

double ScriptableSelfRegulatingNeuronActivationFunction::calculateActivation(Neuron *owner) {
	
	return SelfRegulatingNeuronActivationFunction::calculateActivation(owner);
}


bool ScriptableSelfRegulatingNeuronActivationFunction::equals(ActivationFunction *activationFunction) const {
	if(SelfRegulatingNeuronActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	ScriptableSelfRegulatingNeuronActivationFunction *af =
 			dynamic_cast<ScriptableSelfRegulatingNeuronActivationFunction*>(activationFunction);

	if(af == 0) {
		return false;
	}
	return true;
}


double ScriptableSelfRegulatingNeuronActivationFunction::tf(double activation) {
	if(mOwner == 0 || mOwner->getTransferFunction() == 0) {
		return 0.0;
	}
	return mOwner->getTransferFunction()->transferActivation(activation, mOwner);
}


void ScriptableSelfRegulatingNeuronActivationFunction::setStringBuffer(const QString &string) {
	mVariableBuffer = string;
}

QString ScriptableSelfRegulatingNeuronActivationFunction::getStringBuffer() const {
	return mVariableBuffer;
}




double ScriptableSelfRegulatingNeuronActivationFunction::getReceptorStrengthUpdate(double activation) {
	
	//is called first => here, we do the initialization of the scripting context if necessary.
	//do this only once to avoid a second, unnecessary function call.
	//keep in mind: xi, eta should not change during both receptor and transmitter update.
	
	if(mReceptorEquation->get() != "" || mTransmitterEquation->get() != "") {
		setupScriptingContext(activation);
	}
	
	if(mReceptorEquation->get() != "") {
		mEquationScript->evaluate("nerd.stringBuffer = " + mReceptorEquation->get());
		if(mEquationScript->hasUncaughtException()) {
			Core::log("ScriptableSRNActivationFunction: Could not evaluate receptor equation ["
						+ mReceptorEquation->get() + ")", true);
		}
		else {
			return mVariableBuffer.toDouble();
		}
	}
	else {
		return SelfRegulatingNeuronActivationFunction::getReceptorStrengthUpdate(activation);
	}
	return 0.0;
}


double ScriptableSelfRegulatingNeuronActivationFunction::getTransmitterStrengthUpdate(double activation) {
	
	if(mTransmitterEquation->get() != "") {
		mEquationScript->evaluate("nerd.stringBuffer = " + mTransmitterEquation->get());
		if(mEquationScript->hasUncaughtException()) {
			Core::log("ScriptableSRNActivationFunction: Could not evaluate transmitter equation ["
						+ mTransmitterEquation->get() + ")", true);
		}
		else {
			return mVariableBuffer.toDouble();
		}
	}
	else {
		return SelfRegulatingNeuronActivationFunction::getTransmitterStrengthUpdate(activation);
	}
	return 0.0;
}

void ScriptableSelfRegulatingNeuronActivationFunction::setupScriptingContext(double activation) {
	
	if(mEquationScript == 0) {
		mEquationScript = new QScriptEngine();

		//make global objects available
		QScriptValue nerdScriptObj = mEquationScript->newQObject(this, 
								QScriptEngine::QtOwnership,
								QScriptEngine::ExcludeChildObjects);
		mEquationScript->globalObject().setProperty("nerd", nerdScriptObj);
		
		mEquationScript->evaluate(QString("function tf(act) { return nerd.tf(act); }"));
		
		
		mEquationScript->evaluate("var a = 0;");
		mEquationScript->evaluate("var b = 0;");
		mEquationScript->evaluate("var g = 0;");
		mEquationScript->evaluate("var d = 0;");
		mEquationScript->evaluate("var aStar = 0;");
		mEquationScript->evaluate("var activation = 0;");
		mEquationScript->evaluate("var xi = 0;");
		mEquationScript->evaluate("var eta = 0;");
		mEquationScript->evaluate("var ap = 0;");
	}
	
	
	//update parameters
	mEquationScript->evaluate(QString("a = " + mAlpha->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("b = " + mBeta->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("g = " + mGamma->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("d = " + mDelta->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("aStar = " + mAStar->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("activation = " + QString::number(activation) + ";"));
	mEquationScript->evaluate(QString("xi = " + mXi->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("eta = " + mEta->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("ap = " + QString::number(mOwner->getLastActivation()) + ";"));
		
}


}



