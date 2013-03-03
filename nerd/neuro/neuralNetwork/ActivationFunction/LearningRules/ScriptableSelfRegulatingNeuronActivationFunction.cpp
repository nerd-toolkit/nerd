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



#include "ScriptableSelfRegulatingNeuronActivationFunction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuron.h"
#include "Math/Math.h"
#include <math.h>
#include <ModularNeuralNetwork/ModularNeuralNetwork.h>
#include <NeuroModulation/NeuroModulator.h>


using namespace std;

namespace nerd {


/**
 * Constructs a new ScriptableSelfRegulatingNeuronActivationFunction.
 */
ScriptableSelfRegulatingNeuronActivationFunction::ScriptableSelfRegulatingNeuronActivationFunction()
	: QObject(), SelfRegulatingNeuronActivationFunction("Script_SRN_V1", false), mEquationScript(0)
{
	mTransmitterEquation = new StringValue("default");
	mReceptorEquation = new StringValue("default");
	mEtaEquation = new StringValue("default");
	mXiEquation = new StringValue("default");
	mThetaEquation = new StringValue("");
	mEquationTester = new StringValue("");
	mEquationTestOutput = new DoubleValue();
	
	addParameter("Trans-Eq (eta,h)", mTransmitterEquation);
	addParameter("Recept-Eq (xi,g)", mReceptorEquation);
	addParameter("Eta-Eq", mEtaEquation);
	addParameter("Xi-Eq", mXiEquation);
	addParameter("Theta-Eq", mThetaEquation);
	addParameter("Tester", mEquationTester);
	
	addObserableOutput("TestOutput", mEquationTestOutput);
	
	mTransmitterEquation->setDescription("2. Equation"
										 "\nVariables: alpha, beta, gamma, delta, theta, aStar, eta, xi, a, ap, x, y, z"
										 "\nFunctions: tf()"
										 "\n<default> recovers standard equation.");
	mReceptorEquation->setDescription("1. Equation"
										 "\nVariables: alpha, beta, gamma, delta, theta, aStar, eta, xi, a, ap, x, y, z"
										 "\nFunctions: tf()"
										 "\n<default> recovers standard equation.");
	mEtaEquation->setDescription("4. Equation"
										 "\nVariables: alpha, beta, gamma, delta, theta, aStar, eta, xi, a, ap, x, y, z, g, h"
										 "\nFunctions: tf()"
										 "\n<default> recovers standard equation.");
	mXiEquation->setDescription("3. Equation"
										 "\nVariables: alpha, beta, gamma, delta, theta, aStar, eta, xi, a, ap, x, y, z, g, h"
										 "\nFunctions: tf()"
										 "\n<default> recovers standard equation.");
	mThetaEquation->setDescription("An optional equation to update the neuron bias."
										"\nVariables: alpha, beta, gamma, delta, theta, aStar, eta, xi, a, ap, x, y, z, g, h"
										 "\nFunctions: tf()");
	
	mEquationTester->setDescription("Allows to test variables and equations on-the-fly. The result of this equation is passed "
									"to the observable 'TestOutput\n"
								    "The testequation is calculated AFTER all other equations.\n"
									"\nVariables: alpha, beta, gamma, delta, theta, aStar, eta, xi, a, ap, x, y, z, g, h"
									"\nFunctions: tf()");
}


/**
 * Copy constructor. 
 * 
 * @param other the ScriptableSelfRegulatingNeuronActivationFunction object to copy.
 */
ScriptableSelfRegulatingNeuronActivationFunction::ScriptableSelfRegulatingNeuronActivationFunction(
						const ScriptableSelfRegulatingNeuronActivationFunction &other) 
	: ObservableNetworkElement(other), QObject(), SelfRegulatingNeuronActivationFunction(other), mEquationScript(0)
{
	mTransmitterEquation = dynamic_cast<StringValue*>(getParameter("Trans-Eq (eta,h)"));
	mReceptorEquation = dynamic_cast<StringValue*>(getParameter("Recept-Eq (xi,g)"));
	mEtaEquation = dynamic_cast<StringValue*>(getParameter("Eta-Eq"));
	mXiEquation = dynamic_cast<StringValue*>(getParameter("Xi-Eq"));
	mThetaEquation = dynamic_cast<StringValue*>(getParameter("Theta-Eq"));
	mEquationTester = dynamic_cast<StringValue*>(getParameter("Tester"));
	
	mEquationTestOutput = new DoubleValue();
	addObserableOutput("TestOutput", mEquationTestOutput);
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
	
	if(mEtaEquation->get() == "default") {
		mEtaEquation->set("((1 - gamma) * eta) + (delta * h)");
	}
	if(mXiEquation->get() == "default") {
		mXiEquation->set("xi * (1 + (beta * g))");
	}
	if(mTransmitterEquation->get() == "default") {
		mTransmitterEquation->set("1 + tf(a)");
	}
	if(mReceptorEquation->get() == "default") {
		mReceptorEquation->set("Math.abs(tf(aStar)) - Math.abs(tf(a))");
	}
	
	double result = SelfRegulatingNeuronActivationFunction::calculateActivation(owner);
	
	
	//Execute equation tester
	if(mEquationTester->get().trimmed() != "") {
		mEquationScript->evaluate("nerd.stringBuffer = " + mEquationTester->get());
		if(mEquationScript->hasUncaughtException()) {
			Core::log("ScriptableSRNActivationFunction: Could not evaluate test equation ["
					+ mEquationTester->get() + ")", true);
		}
		else {
			mEquationTestOutput->set(mVariableBuffer.toDouble());
		}
	}
	
	return result;
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


/**
 * Helper function used to apply the transfer function to an activation. 
 * This function is used in the scriptin context with nerd.tf().
 */
double ScriptableSelfRegulatingNeuronActivationFunction::tf(double activation) {
	if(mOwner == 0 || mOwner->getTransferFunction() == 0) {
		return 0.0;
	}
	return mOwner->getTransferFunction()->transferActivation(activation, mOwner);
}

double ScriptableSelfRegulatingNeuronActivationFunction::nm(int type) {
	if(mOwner == 0 || mOwner->getOwnerNetwork() == 0) {
		return 0.0;
	}
	ModularNeuralNetwork *mnn = dynamic_cast<ModularNeuralNetwork*>(mOwner->getOwnerNetwork());
	if(mnn == 0) {
		return 0.0;
	}
	Vector3D position = mOwner->getPosition();
	
	return NeuroModulator::getConcentrationInNetworkAt(type, position, mnn);
}


/**
 * Setter of the string buffer used to transfer strings between the 
 * scripting context and the C++ programm.
 */
void ScriptableSelfRegulatingNeuronActivationFunction::setStringBuffer(const QString &string) {
	mVariableBuffer = string;
}

/**
 * Getter of the string buffer used to transfer strings between the 
 * scripting context and the C++ programm.
 */
QString ScriptableSelfRegulatingNeuronActivationFunction::getStringBuffer() const {
	return mVariableBuffer;
}



/**
 * Calculates the receptor strength g(x). If an equation is found in the
 * receptor field, then that equation is executed in the scripting context.
 * Otherwise, the g(x) of the C++ superclass is used.
 */
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
			mEquationScript->evaluate(QString("g = " + mVariableBuffer));
			return mVariableBuffer.toDouble();
		}
	}
	else {
		return SelfRegulatingNeuronActivationFunction::getReceptorStrengthUpdate(activation);
	}
	return 0.0;
}

/**
 * Calculates the transmitter strength h(x). If an equation is found in the
 * transmitter field, then that equation is executed in the scripting context.
 * Otherwise, the h(x) of the C++ superclass is used.
 */
double ScriptableSelfRegulatingNeuronActivationFunction::getTransmitterStrengthUpdate(double activation) {
	
	
	if(mTransmitterEquation->get() != "") {
		mEquationScript->evaluate("nerd.stringBuffer = " + mTransmitterEquation->get());
		if(mEquationScript->hasUncaughtException()) {
			Core::log("ScriptableSRNActivationFunction: Could not evaluate transmitter equation ["
						+ mTransmitterEquation->get() + ")", true);
		}
		else {
			mEquationScript->evaluate(QString("h = " + mVariableBuffer));
			return mVariableBuffer.toDouble();
		}
	}
	else {
		return SelfRegulatingNeuronActivationFunction::getTransmitterStrengthUpdate(activation);
	}
	return 0.0;
}

void ScriptableSelfRegulatingNeuronActivationFunction::updateXi(double activation) {
	if(mXiEquation->get() != "") {
		mEquationScript->evaluate("nerd.stringBuffer = " + mXiEquation->get());
		if(mEquationScript->hasUncaughtException()) {
			Core::log("ScriptableSRNActivationFunction: Could not evaluate xi equation ["
						+ mXiEquation->get() + ")", true);
		}
		else {
			mXi->set(Math::min(1000.0, Math::max(-1000.0, mVariableBuffer.toDouble())));
		}
	}
	else {
		SelfRegulatingNeuronActivationFunction::updateXi(activation);
	}
}


void ScriptableSelfRegulatingNeuronActivationFunction::updateEta(double activation) {
	if(mEtaEquation->get() != "") {
		mEquationScript->evaluate("nerd.stringBuffer = " + mEtaEquation->get());
		if(mEquationScript->hasUncaughtException()) {
			Core::log("ScriptableSRNActivationFunction: Could not evaluate eta equation ["
						+ mEtaEquation->get() + ")", true);
		}
		else {
			mEta->set(Math::min(1000.0, Math::max(-1000.0, mVariableBuffer.toDouble())));
		}
	}
	else {
		SelfRegulatingNeuronActivationFunction::updateEta(activation);
	}
}

void ScriptableSelfRegulatingNeuronActivationFunction::updateTheta(double) {
	if(mThetaEquation->get() != "") {
		mEquationScript->evaluate("nerd.stringBuffer = " + mThetaEquation->get());
		if(mEquationScript->hasUncaughtException()) {
			Core::log("ScriptableSRNActivationFunction: Could not evaluate theta equation ["
						+ mThetaEquation->get() + ")", true);
		}
		else {
			if(mOwner != 0) {
				mOwner->getBiasValue().set(mVariableBuffer.toDouble());
			}
		}
	}
}


/**
 * Called the first time, this function creates the scripting context and declares all
 * available variable names. This includes the parameters (a, b, g, d => alpha, beta, gamma, delta),
 * act (activation), actp (activation of previous step), xi, eta, aStar and the freely useable 
 * variables x, y and z (initialized to 0). Also the tf() function is declared, that can be
 * used to apply the transfer function of the corresponding neuron to a number.
 * 
 * At any call, the variables are set to their current value, so that they can be used
 * in the scripting context. 
 * 
 * Note, that the scripting context is not a fully program with functions, but instead a
 * an equation call of the form x = "Text in the Parameter Field", i.e. you do NOT
 * need to assign your calculated value to a variable or to return a value.
 * 
 * So, in the parameter field, you would just write 
 * 2 * eta + tf(Math.pow(a, 2))
 * to calculate the new g(x) or h(x).
 */
void ScriptableSelfRegulatingNeuronActivationFunction::setupScriptingContext(double activation) {
	
	if(mEquationScript == 0) {
		mEquationScript = new QScriptEngine();

		//make global objects available
		QScriptValue nerdScriptObj = mEquationScript->newQObject(this, 
								QScriptEngine::QtOwnership,
								QScriptEngine::ExcludeChildObjects);
		mEquationScript->globalObject().setProperty("nerd", nerdScriptObj);
		
		mEquationScript->evaluate(QString("function tf(act) { return nerd.tf(act); }"));
		mEquationScript->evaluate(QString("function nm(type) { return nerd.nm(type); }"));
		
		mEquationScript->evaluate("var alpha = 0;");
		mEquationScript->evaluate("var beta = 0;");
		mEquationScript->evaluate("var gamma = 0;");
		mEquationScript->evaluate("var delta = 0;");
		mEquationScript->evaluate("var aStar = 0;");
		mEquationScript->evaluate("var a = 0;");
		mEquationScript->evaluate("var xi = 0;");
		mEquationScript->evaluate("var eta = 0;");
		mEquationScript->evaluate("var ap = 0;");
		mEquationScript->evaluate("var g = 0;");
		mEquationScript->evaluate("var h = 0;");
		mEquationScript->evaluate("var x = 0; var y = 0; var z = 0;");
		mEquationScript->evaluate("var theta = 0;");
		
		mEquationScript->evaluate("function sign(value) { if(value > 0.0) { return 1.0; } else if(value < 0.0) { return -1.0; } else { return 0.0; } }");
	}
	
	
	//update parameters
	mEquationScript->evaluate(QString("alpha = " + mAlpha->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("beta = " + mBeta->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("gamma = " + mGamma->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("delta = " + mDelta->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("aStar = " + mAStar->getValueAsString() + ";"));
	
	if(mUseCurrentActivations) {
		mEquationScript->evaluate(QString("a = " + QString::number(activation) + ";"));
	}
	else {
		mEquationScript->evaluate(QString("a = " + QString::number(mOwner->getLastActivation()) + ";"));
	}
	
	mEquationScript->evaluate(QString("xi = " + mXi->getValueAsString() + ";"));
	mEquationScript->evaluate(QString("eta = " + mEta->getValueAsString() + ";"));
	
	if(mUseCurrentActivations) {
		mEquationScript->evaluate(QString("ap = " + QString::number(mOwner->getLastActivation()) + ";"));
	}
	else {
		mEquationScript->evaluate(QString("ap = " + QString::number(mActivationT2) + ";"));
	}
	
	mEquationScript->evaluate(QString("g = 0"));
	mEquationScript->evaluate(QString("h = 0"));
	if(mOwner != 0) {
		mEquationScript->evaluate(QString("theta = " + mOwner->getBiasValue().getValueAsString() + ";"));
	}
		
}


}



