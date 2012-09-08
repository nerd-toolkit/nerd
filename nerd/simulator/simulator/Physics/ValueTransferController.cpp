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

#include "ValueTransferController.h"
#include "Value/BoolValue.h"
#include "Core/Core.h"
#include "SimulationConstants.h"
#include "NerdConstants.h"
#include "Math/Math.h"
#include <iostream>



using namespace std;

namespace nerd {
	
	ValueTransferController::ValueTransferController(const QString &name, bool autoChangeTarget) 
	: SimSensor(), SimActuator(), SimObject(name), mSource(0), mTarget(0), mLocalSource(0),
		mTransferredActivation(0.0), mAutoChangeTarget(autoChangeTarget)
	{
		mTransferController = new InterfaceValue(getName(), "Control", 0.0, -1.0, 1.0);
		mTransferSensor = new InterfaceValue(getName(), "Transfer", 0.0, -1.0, 1.0);
		
		mSourceValueName = new StringValue();
		mTargetValueName = new StringValue();
		mCustomNameOfControlNeuron = new StringValue("Control");
		mCustomNameOfTransferSensor = new StringValue("Transfer");
		mTransferMode = new IntValue(0);
		mTransferRate = new DoubleValue(0.01);
		mTransferCost = new DoubleValue(0.00001);
		mControllerRangeMax = new DoubleValue(1.0);
		mControllerRangeMin = new DoubleValue(-1.0);
		
		mLocalSource = new NormalizedDoubleValue(0.0, -9999999.0, 9999999.0, 0.0, 1.0);
		
		if(!mAutoChangeTarget) {
			addParameter("Control", mTransferController);
			addParameter("Sensor", mTransferSensor);
			addParameter("CustomControllerName", mCustomNameOfControlNeuron);
			addParameter("CustomSensorName", mCustomNameOfTransferSensor);
			addParameter("ControllerRangeMin", mControllerRangeMin);
			addParameter("ControllerRangeMax", mControllerRangeMax);
			
			mInputValues.append(mTransferController);
			mOutputValues.append(mTransferSensor);
		}
		else {
			mTransferController->set(1.0); //set to automatic full activation
			mTransferCost->set(0.0);
		}
		addParameter("SourceValueName", mSourceValueName);
		addParameter("TargetValueName", mTargetValueName);
		addParameter("TransferMode", mTransferMode);
		addParameter("TransferCost", mTransferCost);
		addParameter("TransferRate", mTransferRate);
		
		
		mTransferMode->setDescription("The mode in which transfers between a source and the target take place:\n"
									  "0: Costy Transfer: Activation is removed from the source (+ the cost) "
									    "and added to the target (without the cost)\n"
									  "1: Simultaneous Change: Both, the source and the target, are changed by "
									    "the same proportion. Rate is the change "
										"of the target, cost the change of the source. "
										"Both can be positive or negative.\n"
									  "2: Equilibrium: The target is changed in the direction of the source, "
									    "independent of the control neuron. So you should use the automatic version.\n"
									  "3: Mutual Equilibrium: The source and target are changed towards each other. "
									    "The change innetia is reflected by the difference of cost (source) "
										"and rate (target):");
		
	}
	
	ValueTransferController::ValueTransferController(const ValueTransferController &other) 
	: Object(), ValueChangedListener(), SimSensor(), SimActuator(), SimObject(other),
		mSource(0), mTarget(0), mLocalSource(0), mTransferredActivation(0.0), 
		mAutoChangeTarget(other.mAutoChangeTarget)
	{
		mLocalSource = new NormalizedDoubleValue(0.0, -9999999.0, 9999999.0, 0.0, 1.0);
		
		if(mAutoChangeTarget) {
			mTransferController = new InterfaceValue(getName(), "Control", 1.0, -1.0, 1.0); //set to automatic full activation
			mTransferSensor = new InterfaceValue(getName(), "Transfer", 0.0, -1.0, 1.0);
			mCustomNameOfControlNeuron = new StringValue("Control");
			mCustomNameOfTransferSensor = new StringValue("Transfer");
			mControllerRangeMax = new DoubleValue(1.0);
			mControllerRangeMin = new DoubleValue(-1.0);
		}
		else {
			mTransferController = dynamic_cast<InterfaceValue*>(getParameter("Control"));
			mTransferSensor = dynamic_cast<InterfaceValue*>(getParameter("Sensor"));
			mCustomNameOfControlNeuron = dynamic_cast<StringValue*>(getParameter("CustomControllerName"));
			mCustomNameOfTransferSensor = dynamic_cast<StringValue*>(getParameter("CustomSensorName"));
			mControllerRangeMin = dynamic_cast<DoubleValue*>(getParameter("ControllerRangeMin"));
			mControllerRangeMax = dynamic_cast<DoubleValue*>(getParameter("ControllerRangeMax"));
			
			mInputValues.append(mTransferController);
			mOutputValues.append(mTransferSensor);
		}
		mSourceValueName = dynamic_cast<StringValue*>(getParameter("SourceValueName"));	
		mTargetValueName = dynamic_cast<StringValue*>(getParameter("TargetValueName"));
		mTransferMode = dynamic_cast<IntValue*>(getParameter("TransferMode"));
		mTransferCost = dynamic_cast<DoubleValue*>(getParameter("TransferCost"));
		mTransferRate = dynamic_cast<DoubleValue*>(getParameter("TransferRate"));

	}
	
	ValueTransferController::~ValueTransferController() {
	}
	
	SimObject* ValueTransferController::createCopy() const {
		return new ValueTransferController(*this);
	}
	
	QString ValueTransferController::getName() const {
		return SimObject::getName();
	}

	void ValueTransferController::setup() {
		SimObject::setup();
		
		ValueManager *vm = Core::getInstance()->getValueManager();
		
		if(mSourceValueName->get() == "") {
			mSource = mLocalSource;
		}
		else {
			mSource = dynamic_cast<NormalizedDoubleValue*>(vm->getValue(mSourceValueName->get()));
			if(mSource == 0) {
				Core::log("ValueTransferController: Could not get the source value (NormalizedDoubleValue) with name ["
						+ mSourceValueName->get() + "]. The ValueTransferController will not work!", true);
			}
		}
		
		mTarget = dynamic_cast<NormalizedDoubleValue*>(vm->getValue(mTargetValueName->get()));
		if(mTarget == 0) {
			Core::log("ValueTransferController: Could not get the target value (NormalizedDoubleValue) with name ["
					  + mTargetValueName->get() + "]. The ValueTransferController will not work!", true);
		}
		
	}
	
	void ValueTransferController::clear() {
		SimObject::clear();
		
		mSource = 0;
		mTarget = 0;
	}
	
	void ValueTransferController::valueChanged(Value *value) {
		SimObject::valueChanged(value);
		if(value == 0) {
			return;
		}
		else if(value == mCustomNameOfControlNeuron) {
			mTransferController->setInterfaceName(mCustomNameOfControlNeuron->get());
		}
		else if(value == mCustomNameOfTransferSensor) {
			mTransferSensor->setInterfaceName(mCustomNameOfTransferSensor->get());
		}
		else if(value == mTransferRate || value == mTransferCost) {
			mTransferSensor->setMin(-(mTransferRate->get() + mTransferCost->get()));
			mTransferSensor->setMax(mTransferRate->get() + mTransferCost->get());
		}
		else if(value == mControllerRangeMin) {
			mTransferController->setMin(mControllerRangeMin->get());
		}
		else if(value == mControllerRangeMax) {
			mTransferController->setMax(mControllerRangeMax->get());
		}
	}
	
	void ValueTransferController::updateActuators() {
		
		transferActivations();
	}
	
	void ValueTransferController::updateSensorValues() {
		
		mTransferSensor->set(mTransferredActivation);
		
		if(mLocalSource != 0) {
			//reset local source
			mLocalSource->set(0.0);
		}
	}
	
	NormalizedDoubleValue* ValueTransferController::getSource() const {
		return mSource;
	}
	
	
	NormalizedDoubleValue* ValueTransferController::getTarget() const {
		return mTarget;
	}
	
	bool ValueTransferController::transferActivations() {
		switch(mTransferMode->get()) {
			case 0:
				return transferModeSimple();
				break;
			case 1:
				return transferModeBothProportional();
				break;
			case 2:
				return transferModeEquilibrium(true);
				break;
			case 3:
				return transferModeEquilibrium(false);
				break;
			default:
				//no transfer
				return false;
		}
		return false;
	}
	
	
	bool ValueTransferController::transferModeSimple() {
		
		if(mTarget == 0 || mSource == 0) {
			mTransferredActivation = 0.0;
			return false;
		}
		
		double activationToTransfer = mTransferRate->get() * mTransferController->get();

		NormalizedDoubleValue *source = activationToTransfer > 0.0 ? mSource : mTarget;
		NormalizedDoubleValue *target = activationToTransfer > 0.0 ? mTarget : mSource;
		
		double positiveTransfer = Math::abs(activationToTransfer);
		
		if(positiveTransfer > 0.0) {

			positiveTransfer = Math::min(positiveTransfer, Math::max(0.0, source->get() - source->getMin() - mTransferCost->get()));
			positiveTransfer = Math::min(positiveTransfer, target->getMax() - target->get());
			
			//only apply changes (especially the cost) if there is actually a transfer going on!
			if(positiveTransfer > 0.0) {
				source->set(source->get() - positiveTransfer - mTransferCost->get());
				target->set(target->get() + positiveTransfer);
			}
			
			mTransferredActivation = activationToTransfer > 0.0 ? positiveTransfer : -1 * positiveTransfer;
		}
		else {
			//do nothing.
			mTransferredActivation = 0.0;
		}

		return true;
	}
	
	bool ValueTransferController::transferModeBothProportional() {
		
		if(mTarget == 0 || mSource == 0 || mTransferController->get() <= 0.0) {
			mTransferredActivation = 0.0;
			return false;
		}
			
		double changeTarget = mTransferRate->get() * mTransferController->get();
		double changeSource = mTransferCost->get() * mTransferController->get();
		
		double changeFactor = mTransferController->get();
		
		//ignore negative changes.
		double change = changeTarget;
		if(changeTarget < 0.0) {
			change = Math::max(change, -1 * (mTarget->get() - mTarget->getMin()));
		}
		else if(changeTarget > 0.0) {
			change = Math::min(change, mTarget->getMax() - mTarget->get());
		}
		if(change != changeTarget || changeTarget == 0.0) {
			if(change == 0.0) {
				mTransferredActivation = 0.0;
				return true;
			}
			changeFactor = changeFactor * change / changeTarget;
			changeSource = changeSource * change / changeTarget;
			changeTarget = change;
		}
		change = changeSource;
		if(changeSource < 0.0) {
			change = Math::max(change, -1 * (mSource->get() - mSource->getMin()));
		}
		else if(changeSource > 0.0) {
			change = Math::min(change, mSource->getMax() - mSource->get());
		}
		if(change != changeSource || changeSource == 0.0) {
			if(change == 0.0) {
				mTransferredActivation = 0.0;
				return true;
			}
			changeFactor = changeFactor * change / changeSource;
			changeTarget = changeTarget * change / changeSource;
			changeSource = change;
		}
		mTransferredActivation = changeFactor;
		mSource->set(mSource->get() + changeSource);
		mTarget->set(mTarget->get() + changeTarget);
		
		return true;
	}
	
	bool ValueTransferController::transferModeEquilibrium(bool adaptTargetOnly) {
		if(mTarget == 0 || mSource == 0 || mTransferController->get() <= 0.0) {
			mTransferredActivation = 0.0;
			return false;
		}
		//no control neuron is used here to regulate anything. This is an autonomous process.
		
		double difference = mSource->get() - mTarget->get();
		
		if(adaptTargetOnly) {
			double change = difference * mTransferRate->get();
			mTarget->set(mTarget->get() + change);
			
			mTransferredActivation = change;
		}
		else {
			double factorSource = mTransferCost->get();
			double factorTarget = mTransferRate->get();
			double factorSum = factorSource + factorTarget;
			if(factorSum > 1.0) {
				factorSource = factorSource / factorSum;
				factorTarget = factorTarget / factorSum;
			}
			double changeTarget = difference * factorTarget;
			double changeSource = difference * factorSource;
			
			mTarget->set(mTarget->get() + changeTarget);
			mSource->set(mSource->get() - changeSource);

			mTransferredActivation = Math::abs(changeTarget) + Math::abs(changeSource);
		}
		
		return true;
	}
	
}
