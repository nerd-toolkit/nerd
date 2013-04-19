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

#include "NeuroModulator.h"
#include <Math/Math.h>
#include "Network/Neuron.h"
#include <iostream>
#include <Network/NeuralNetwork.h>
#include <Network/Neuro.h>
#include <ActivationFunction/NeuroModulatorActivationFunction.h>
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Math/Vector3D.h"
#include "Core/Core.h"
#include "Value/BoolValue.h"
#include "NeuroModulatorManager.h"
#include <sstream>

using namespace std;

namespace nerd {

NeuroModulator::NeuroModulator() 
	: mDefaultDistributionModus(2), mDefaultUpdateModus(0), mResetPending(true),
		mEnableUpdate(0), mEnableConcentrationCalculation(0)
{
	mEnableUpdate = NeuroModulatorManager::getInstance()->getEnableModulatorUpdateValue();
	mEnableConcentrationCalculation = NeuroModulatorManager::getInstance()->getEnableModulatorConcentrationLevelsValue();
	
	mNetworkManager = Neuro::getNeuralNetworkManager();
	
	//build up documentation
	mUpdateModiDocumentations.insert(1, getModulatorDefaultDoc());
}


NeuroModulator::NeuroModulator(const NeuroModulator &other) 
	: mUpdateModiDocumentations(other.mUpdateModiDocumentations), mDefaultDistributionModus(other.mDefaultDistributionModus),
		mDefaultUpdateModus(other.mDefaultUpdateModus), mResetPending(true), mEnableUpdate(0), mEnableConcentrationCalculation(0)
{
	mEnableUpdate = NeuroModulatorManager::getInstance()->getEnableModulatorUpdateValue();
	mEnableConcentrationCalculation = NeuroModulatorManager::getInstance()->getEnableModulatorConcentrationLevelsValue();
	
	mNetworkManager = Neuro::getNeuralNetworkManager();
}


NeuroModulator::~NeuroModulator() {
	
}



NeuroModulator* NeuroModulator::createCopy() {
	return new NeuroModulator(*this);
}

void NeuroModulator::reset(NeuralNetworkElement*) {
// 	QList<int> types = getModulatorTypes();
// 	for(int i = 0; i < types.size(); ++i) {
// 		int type = types.at(i);
// 		mConcentrations.insert(type, 0.0);
// 	}
	mConcentrations.clear();
	mAreas.clear();
	mReferenceModules.clear();
	mDistributionModi.clear();
	mUpdateModi.clear();
	mUpdateModiParameters.clear();
	mUpdateModiVariables.clear();
	
	mResetPending = true;
	
}

void NeuroModulator::update(NeuralNetworkElement *owner) {
	
	if(mNetworkManager->getDisablePlasticityValue()->get()) {
		//disable neuro modulators as long as this hint value is true.
		return;
	}

	
	QList<int> types = getModulatorTypes();
	for(int i = 0; i < types.size(); ++i) {
		int type = types.at(i);
		
		if(mEnableUpdate->get() || mResetPending) {
			updateType(type, owner, mResetPending);
		}
	}
	if(!types.empty()) {
		mResetPending = false;
	}
}

void NeuroModulator::updateType(int type, NeuralNetworkElement *owner, bool reset) {
	int modus = getUpdateModus(type);
	
	if(modus == 1) {
		updateModulatorDefault(type, owner, reset);
	}
}

void NeuroModulator::setConcentration(int type, double concentration, NeuralNetworkElement *owner) {
	bool containsType = mConcentrations.keys().contains(type);
	mConcentrations.insert(type, concentration);
	if(!containsType) {
		//initialize the update operator.
		updateType(type, owner, true);
	}
}

double NeuroModulator::getConcentration(int type, NeuralNetworkElement*) {
	return mConcentrations.value(type, 0.0);
}


/**
 * Simple linear model.
 * Modi:
 * (0): off
 * (1): equal concentration
 * (2): linear concentration
 * (3): quadratic concentration
 */
double NeuroModulator::getConcentrationAt(int type, Vector3D position, NeuralNetworkElement *owner) {
	
	if(!mEnableConcentrationCalculation->get()) {
		return 0.0;
	}
	
	if(!mDistributionModi.keys().contains(type)) {
		mDistributionModi.insert(type, mDefaultDistributionModus);
	}
	
	//TODO check what to do with default stuff (is that necessary?)
//  	int modus = mDefaultDistributionModus;
//  	if(modus == -1) {
		int modus = mDistributionModi.value(type, -1);
//  	}
	
	//2D only here (on a plane)
	double concentration = mConcentrations.value(type, 0.0);
	if(concentration == 0.0) {
		return 0.0;
	}
	QRectF area = getLocalRect(type);
	bool isCircle = isCircularArea(type);
	
	if(area.width() <= 0.0 || area.height() <= 0.0) {
		return 0.0;
	}
	
	//check if the point is in the area of influence
	double radius = 0.0;
	double distance = 0.0;
	if(isCircle) {
		radius = area.width() / 2.0;
		distance = Math::distance(position, owner->getPosition() + Vector3D(area.x(), area.y(), 0.0));
		
		if(distance >= radius) {
			return 0.0;
		}
	}
	else {
		Vector3D ownerPos = owner->getPosition();
		QRectF areaOfInfluence(ownerPos.getX() + area.x(), ownerPos.getY() + area.y(), area.width(), area.height());
		
		distance = Math::distance(position, ownerPos + Vector3D(area.x(), area.y(), 0.0));
		radius = Math::max(area.width(), area.height());
		
		if(!areaOfInfluence.contains(position.getX(), position.getY())) {
			return 0.0;
		}
	}
	
	//equal concentration
	//the concentration at all points in the circle are similar.
	if(modus == 1) {
		return concentration;
	}
	
	//linear distribution
	//the concentration is linearly decaying from the center to the border of the circle.
	if(modus == 2) {
		return (1.0 - (distance / radius)) * concentration;
	}
// 	if(mDistributionModi == 3) {
// 		
// 	}

	Core::log("NeuroModulator: (" + QString::number(owner->getId()) + ") Unsupported disribution modus: " + QString(modus), true);
	
	//return 0 if there is no supported modus.
	return 0.0;
}

QList<int> NeuroModulator::getModulatorTypes() const {
	return mConcentrations.keys();
}

/**
 * Sets the maximal concentration of the specified modulator type.
 * that can be produced by this modulator cell.
 */
void NeuroModulator::setMaxConcentration(int type, double concentration) {
	mMaxConcentrations.insert(type, concentration);
}


/**
 * Returns the maximal possible concentration of the specified modulator type
 * that can be produced by this modulator cell.
 * If no maximum is given for the specified type, the 1.0 is assigned as default.
 */
double NeuroModulator::getMaxConcentration(int type) {
	if(mMaxConcentrations.keys().contains(type)) {
		return mMaxConcentrations.value(type);
	}
	mMaxConcentrations.insert(type, 1.0);
	return 1.0;
}


void NeuroModulator::setLocalAreaRect(int type, double width, double height, 
									const Vector3D &offset, bool isCircle)
{
	mReferenceModules.remove(type);
	if(isCircle) {
		////TODO currently, only real circles are supported (no ellipses)
		height = width;
	}
	mAreas.insert(type, QRectF(offset.getX(), offset.getY(), width, height));
	mIsCircle.insert(type, isCircle);
}


void NeuroModulator::setAreaReferenceModule(int type, NeuroModule *module) {
	if(module == 0) {
		return;
	}
	mAreas.remove(type);
	mIsCircle.remove(type);
	mReferenceModules.insert(type, module);
}


QRectF NeuroModulator::getLocalRect(int type) {
	NeuroModule *refModule = mReferenceModules.value(type, 0);
	if(refModule == 0) {
		return mAreas.value(type, QRectF(0.0, 0.0, 0.0, 0.0));
	}
	
	//only if a reference module is given
	Vector3D refPos = refModule->getPosition();
	QSizeF size = refModule->getSize();
	return QRectF(refPos.getX(), refPos.getY(), size.width(), size.height());
}


bool NeuroModulator::isCircularArea(int type) {
	return mIsCircle.value(type, true);
}


/**
 * Sets the modus by which the distribution of the modulator concentration is
 * calculated relative to the center position of the owner object.
 * 
 * If type == -1, then the default modus is changed AND all currently available
 * modulator types are set to use this default modus.
 * If the type is a specific type, then only that type's modus is changed.
 */
void NeuroModulator::setDistributionModus(int type, int modus) {
	modus = modus;
	if(type == -1) {
		mDistributionModi.clear();
		mDefaultDistributionModus = modus;
	}
	if(!mConcentrations.keys().contains(type)) {
		mConcentrations.insert(type, 0.0);
	}
	QList<int> types = mConcentrations.keys();
	for(int i = 0; i < types.size(); ++i) {
		int t = types.at(i);
		if(t == type) {
			mDistributionModi.insert(t, modus);
		}
		else if(type == -1) {
			mDistributionModi.insert(t, mDefaultDistributionModus);
		}
		else if(!mDistributionModi.keys().contains(t)) {
			mDistributionModi.insert(t, mDefaultDistributionModus);
		}
	}
}

/**
 * Returns the currently used distribution mode of a modulator type.
 * If type == -1, then the default modus is returned.
 */
int NeuroModulator::getDistributionModus(int type) const {
	if(type == -1) {
		return mDefaultDistributionModus;
	}
	return mDistributionModi.value(type, mDefaultDistributionModus);
}

//check if this should be rewritten more clean...
void NeuroModulator::setUpdateModus(int type, int modus) {
	if(type == -1) {
		mUpdateModi.clear();
		mDefaultUpdateModus = modus;
	}
	QList<int> types = mConcentrations.keys();
	if(!types.contains(type)) {
		mConcentrations.insert(type, 0.0);
		types = mConcentrations.keys();
	}
	for(int i = 0; i < types.size(); ++i) {
		int t = types.at(i);
		int currentModus = mUpdateModi.value(t);
		
		if(t == type) {
			mUpdateModi.insert(t, modus);
		}
		else if(type == -1) {
			mUpdateModi.insert(t, mDefaultUpdateModus);
		}
		else if(!mUpdateModi.keys().contains(t)) {
			mUpdateModi.insert(t, mDefaultUpdateModus);
		}
		if(currentModus != mUpdateModi.value(t)) {
			mUpdateModiParameters.insert(t, QList<double>());
			mUpdateModiVariables.insert(t, QList<double>());
			updateType(t, 0, true);
		}
	}
}


int NeuroModulator::getUpdateModus(int type) const {
	if(type == -1) {
		return mDefaultUpdateModus;
	}
	return mUpdateModi.value(type, mDefaultUpdateModus);
}

bool NeuroModulator::setUpdateModusParameters(int modus, const QList<double> &parameters) {
	QList<double> oldParams = mUpdateModiParameters.value(modus, QList<double>());
	if(oldParams.size() != 0 && parameters.size() != oldParams.size()) { 
		//TODO this is a temp solution: detection of a parameter set before an update call should be solved in different way.
		//length has to match!
		Core::log(QString("NeuroModulator: Number of modus parameters do not match. Required are ") 
				+ QString::number(oldParams.size()) + " parameters, but got only " 
				+ QString::number(parameters.size()), true);
		return false;
	}
	mUpdateModiParameters.insert(modus, parameters);
	return true;
}


QList<double> NeuroModulator::getUpdateModusParameters(int type) const {
	return mUpdateModiParameters.value(type, QList<double>());
}


QList<QString> NeuroModulator::getUpdateModusParameterNames(int type) const {
	return mUpdateModiParameterNames.value(getUpdateModus(type), QList<QString>());
}


QList<double> NeuroModulator::getUpdateModusVariables(int type) const {
	return mUpdateModiVariables.value(type, QList<double>());
}


QList<QString> NeuroModulator::getUpdateModusVariableNames(int type) const {
	return mUpdateModiVariableNames.value(getUpdateModus(type), QList<QString>());
}

QHash<int, QString> NeuroModulator::getUpdateModusDocumentations() const {
	return mUpdateModiDocumentations;
}

bool NeuroModulator::equals(NeuroModulator *modulator) const {
	if(modulator == 0) {
		return false;
	}
	if(mDefaultDistributionModus != modulator->mDefaultDistributionModus) {
		return false;
	}
	if(mDefaultUpdateModus != modulator->mDefaultUpdateModus) {
		return false;
	}
	if(mEnableUpdate != 0 && modulator->mEnableUpdate !=0 ) {
		if(mEnableUpdate->get() != modulator->mEnableUpdate->get()) {
			return false;
		}
	}
	if(mEnableConcentrationCalculation != 0 && modulator->mEnableConcentrationCalculation != 0) {
		if(mEnableConcentrationCalculation->get() != modulator->mEnableConcentrationCalculation->get()) {
			return false;
		}
	}
	//TODO compare all elements of the modulator settings here!
	
	return true;
}



double NeuroModulator::getConcentrationInNetworkAt(int type, const Vector3D &position, NeuralNetwork *network) {

	if(network == 0) {
		return 0.0;
	}
	double concentration = 0.0;
	
	QList<Neuron*> neurons = network->getNeurons();
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		NeuroModulatorActivationFunction *nmaf = 
					dynamic_cast<NeuroModulatorActivationFunction*>(neuron->getActivationFunction());
		if(nmaf != 0) {
			NeuroModulator *mod = nmaf->getNeuroModulator();
			if(mod != 0) {
				concentration += mod->getConcentrationAt(type, position, neuron);
			}
		}
	}
	return concentration;
}


QString NeuroModulator::getModulatorDefaultDoc() {
	stringstream doc;
	doc << "Simple model of a modulator cell. Modulator production starts only if the cell was stimulated "
		<< "beyond an internal activation threshold (range [0,1]) and stopps production when this activation "
		<< "drops beyond that threshold again. Gains and Drops are both given with positive numbers.\n"
		<< "Params:\n"
		<< "StimGain: Increment for a single stimulation to increase the internal activation.\n"
		<< "StimDrop: Decrement for a single step without stimulation to decrease the internal activation\n"
		<< "StimThreshold: The threshold that separates the modulator production or reduction modes\n"
		<< "ConGain:  Increment of the modulator concentration during each update step in production mode\n"
		<< "ConDrop:  Decrement of the modulator concentration during each update step in reductio mode\n"
		<< "AreaGain: Increment of the modulator area during each update step in production mode\n"
		<< "AreaDrop: Decrement of the modulator area during each update step in reduction mode\n"
		<< "LowerTriggerBound: Lower boundary of the neuron activation range leading to a cell stimulation\n"
		<< "UpperTriggerBound: Upper boundary of the neuron activation range leading to a cell stimulation\n"
		<< "MaxWidth: Maximal width (or radius) of the modulator area\n"
		<< "MaxHeight: Maximal height of the modulator area\n";
		//TODO add modus when useful
	
	return QString(doc.str().c_str());
}

/**
 * Requires the following parameters: UpdateMode 1
 *  activationGain: [0, 1] the duration the activation threshold has to be violated before reaction.
 *  activationDrop: [0, 1] the desensibilization time after a violation.
 *  activationThresholds: [0, 1] the thresholds at which increase or decrease are triggered.
 *  concentrationGain: the increment rate for the modulator concentration
 *  concentrationDrop: the rate at which the modulator is depleted
 *  areaGain: the increment rate for the area
 *  areaDrop: the rate at which the area is reduced again.
 *  lowerTrigger: the lower part of the activation range
 *  upperTrigger: the upper part of the activation range
 *  maxWidth: the maximal width of the area (or the radius)
 *  maxHeight: the maximal height of the area
 *  modus: internal modus to determine what input is used to trigger the modulator 
 *         (e.g. neuron activity, synapse weight, mean activity in module, modulator concentration, etc.)
 */
void NeuroModulator::updateModulatorDefault(int type, NeuralNetworkElement *owner, bool reset) {
	
	QList<double> parameters = mUpdateModiParameters.value(type); 
	if(parameters.size() != 13) {
		QList<QString> paramNames;
		//provide default values;
		paramNames.append("StimGain");
		parameters.append(1.0); //activation state
		paramNames.append("StimDrop");
		parameters.append(1.0);
		paramNames.append("StimProductionThreshold");
		parameters.append(0.5);
		paramNames.append("StimReductionThreshold");
		parameters.append(0.5);
		paramNames.append("ConGain");
		parameters.append(0.001); //concentration 
		paramNames.append("ConDrop");
		parameters.append(0.005);
		paramNames.append("AreaGain");
		parameters.append(0.01); //area
		paramNames.append("AreaDrop");
		parameters.append(0.01);
		paramNames.append("LowerTriggerBound");
		parameters.append(0.9); //triggers
		paramNames.append("UpperTriggerBound");
		parameters.append(1.0);
		paramNames.append("MaxWidth");
		parameters.append(200); //max area
		paramNames.append("MaxHeight");
		parameters.append(200);
		paramNames.append("Modus");
		parameters.append(0); //modus
	
		mUpdateModiParameters.insert(1, parameters);
		mUpdateModiParameterNames.insert(1, paramNames);
	}
	

	QList<double> variables = mUpdateModiVariables.value(type); //here use the 
	if(variables.empty() || reset) {
		variables.clear();
		QList<QString> variableNames;
		//initialize with default variable values
		variableNames.append("ActivationState");
		variables.append(0.0);
		
		mUpdateModiVariableNames.insert(1, variableNames);
		mUpdateModiVariables.insert(1, variables);
	}
	
	if(reset) {
		return;
	}
	
	double activationGain = parameters.at(0);
	double activationDrop = parameters.at(1);
	double activationProductionThreshold = parameters.at(2);
	double activationReductionThreshold = parameters.at(3);
	double concentrationGain = parameters.at(4);
	double concentrationDrop = parameters.at(5);
	double areaGain = parameters.at(6);
	double areaDrop = parameters.at(7);
	double lowerTrigger = parameters.at(8);
	double upperTrigger = parameters.at(9);
	double maxWidth = parameters.at(10);
	double maxHeight = parameters.at(11);
	double innerModus = parameters.at(12);
	
	double maxConcentration = 1.0;
	if(!mMaxConcentrations.keys().contains(type)) {
		mMaxConcentrations.insert(type, 1.0);
	}
	else {
		maxConcentration = mMaxConcentrations.value(type, 1.0);
	}
	
	
	double currentActivation = 0.0;
	
	if(lowerTrigger >= upperTrigger) {
		Core::log("Warning: NeuroModulator lower and upper trigger are the same or lower > upper! [Disabling Update]", true);
		return;
	}
	
	Neuron *neuron = dynamic_cast<Neuron*>(owner);
	Synapse *synapse = dynamic_cast<Synapse*>(owner);
	NeuroModule *module = dynamic_cast<NeuroModule*>(owner);
	
	if(neuron != 0) {
		currentActivation = neuron->getLastOutputActivation();
	}
	else if(synapse != 0) {
		currentActivation = synapse->getStrengthValue().get();
	}
	else if(module != 0) {
		//currentActivation = 
	}
	else {
		Core::log("Warning: NeuroModulator could not find a suitable neuron, synapse or neuromodule!", true);
		return;
	}
	
	double triggerActivation =  (currentActivation - lowerTrigger) / (upperTrigger - lowerTrigger);
	
	if(triggerActivation < 0.0 || triggerActivation > 1.0) {
		//not triggered
		
		double modActivationState = variables.at(0);
		modActivationState = Math::max(0.0, modActivationState - activationDrop);
		variables.replace(0, modActivationState);
		
		if(modActivationState <= activationReductionThreshold) {
			
			if(innerModus == 0) {
				//change concentration
				double concentration = getConcentration(type, owner);
				concentration = Math::max(0.0, concentration - concentrationDrop);
				setConcentration(type, concentration, owner);
				
				//change area
				QRectF area = getLocalRect(type);
				if(concentration > 0.0) {
					area.setWidth(Math::max(0.0, area.width() - (maxWidth * areaDrop)));
					area.setHeight(Math::max(0.0, area.height() - (maxHeight * areaDrop)));
				}
				else {
					//reset area if there is no modulator level left.
					area.setWidth(0.0);
					area.setHeight(0.0);
				}
				
				mAreas.insert(type, QRectF(area.x(), area.y(), area.width(), area.height()));
			}
		}
	}
	else {
		//triggered
		
		double concentration = getConcentration(type, owner);
		
		double modActivationState = variables.at(0);
		
		modActivationState = Math::min(1.0, modActivationState + activationGain);

		variables.replace(0, modActivationState);
		
		if(modActivationState >= activationProductionThreshold) {
			
			if(innerModus == 0) {
				//change concentration
				concentration = Math::min(maxConcentration, concentration + concentrationGain);
				setConcentration(type, concentration, owner);
				
				//change area
				QRectF area = getLocalRect(type);
				area.setWidth(Math::min(maxWidth, area.width() + (maxWidth * areaGain)));
				area.setHeight(Math::min(maxHeight, area.height() + (maxHeight * areaGain)));
				
				mAreas.insert(type, QRectF(area.x(), area.y(), area.width(), area.height()));
			}
		}
	}
	
	mUpdateModiVariables.insert(type, variables);
}

}


