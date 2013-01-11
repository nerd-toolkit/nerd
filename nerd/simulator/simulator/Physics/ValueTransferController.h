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
#ifndef NERD_ValueTransferController_H_
#define NERD_ValueTransferController_H_

#include "Physics/SimObject.h"
#include "Event/EventListener.h"
#include "Physics/SimSensor.h"
#include "Physics/SimActuator.h"
#include "Value/DoubleValue.h"
#include "Value/RangeValue.h"

namespace nerd {
	
	/**
	 * This SimObject provides an input InterfaceValue that regulates the transfer of "activation" between two arbitrary
	 * DoubleValues. These two DoubleValues (source, target) can be chosen by name via the ValueManager and thus can 
	 * be of any source. Activation of the control neuron removes a bit of the source DoubleValue and adds this bit to the
	 * target DoubleValue. Negative activations results in an opposite transfer. Parameters allow to specify the transfer
	 * mode (the way how activation is transfered depending on the state of source and target) and the cost per activation.
	 * If there is a cost, then part of the removed activation of the source is not added to the target and therefore gets
	 * lost. Parameters also allow to specify how much activation is transferred for a full activation of the control neuron.
	 * The SimObject also provides a sensor that reflects the amount of activation that was actually transferred.
	 **/
	
	class BoolValue;
	
	class ValueTransferController : public SimObject, public virtual SimSensor, public virtual SimActuator 
	{
		
	public:
		ValueTransferController(const QString &name, bool autoChangeTarget = false);
		ValueTransferController(const ValueTransferController &other);
		virtual ~ValueTransferController();
		
		virtual SimObject* createCopy() const;
		virtual QString getName() const;
		
		virtual void setup();
		virtual void clear();
		
		virtual void valueChanged(Value *value);
		
		virtual void updateActuators();
		virtual void updateSensorValues();
		
		NormalizedDoubleValue* getSource() const;
		NormalizedDoubleValue* getTarget() const;
		
		virtual bool transferActivations();
		
	protected:
		virtual bool transferModeSimple();
		virtual bool transferModeBothProportional();
		virtual bool transferModeEquilibrium(bool adaptTargetOnly);
		
	protected:		
		InterfaceValue *mTransferController;
		InterfaceValue *mTransferSensor;
		
		StringValue *mSourceValueName;
		StringValue *mTargetValueName;
		StringValue *mCustomNameOfControlNeuron;
		StringValue *mCustomNameOfTransferSensor;
		IntValue *mTransferMode;
		DoubleValue *mTransferRate;
		DoubleValue *mTransferCost;
		//RangeValue *mControllerRange;
		//TODO check ahy RangeValue leads to linker error???
		DoubleValue *mControllerRangeMin;
		DoubleValue *mControllerRangeMax;
		
		NormalizedDoubleValue *mSource;
		NormalizedDoubleValue *mTarget;
		NormalizedDoubleValue *mLocalSource;
		
		double mTransferredActivation;
		bool mAutoChangeTarget;
	};
}

#endif

