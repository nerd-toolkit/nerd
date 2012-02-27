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



#include "EvolvableParameter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "Physics/SimObjectGroup.h"
#include "Physics/Physics.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/NeuralNetwork.h"
#include "NerdConstants.h"
#include "EvolutionConstants.h"
#include <QStringList>
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new EvolvableParameter.
 */
EvolvableParameter::EvolvableParameter(const QString &name)
	: SimObject(name), mTargetGroup(0)
{	
	mTargetObjectGroup = new StringValue("");
	mControlledParameterNames = new StringValue("");
	mMinValue = new DoubleValue(0.0);
	mMaxValue = new DoubleValue(100.0);
	mInterfaceValue = new InterfaceValue(name, "Gene", 0.0, mMinValue->get(), mMaxValue->get());

	mInputValues.append(mInterfaceValue);

	addParameter("Target", mTargetObjectGroup);
	addParameter("ControlledParameters", mControlledParameterNames);
	addParameter("Min", mMinValue);
	addParameter("Max", mMaxValue);

	EventManager *em = Core::getInstance()->getEventManager();
	if(em->getEvent(EvolutionConstants::EVENT_SET_EVOLVED_OBJECT_PARAMETERS, false) == 0) {
		Event *e = em->createEvent(EvolutionConstants::EVENT_SET_EVOLVED_OBJECT_PARAMETERS);
		Event *resetSettingsEvent = 
				em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_SETTINGS_COMPLETED, false);
		if(resetSettingsEvent != 0) {
			resetSettingsEvent->addUpstreamEvent(e);
		}
	} 
	mSetEvolvedParametersEvent = 
			em->getEvent(EvolutionConstants::EVENT_SET_EVOLVED_OBJECT_PARAMETERS, false);
	mSetEvolvedParametersEvent->addEventListener(this);

	mNameValue->addValueChangedListener(this);
}


/**
 * Copy constructor. 
 * 
 * @param other the EvolvableParameter object to copy.
 */
EvolvableParameter::EvolvableParameter(const EvolvableParameter &other) 
	: ValueChangedListener(), EventListener(), SimObject(other), 
	  mTargetGroup(0)
{
	mTargetObjectGroup = dynamic_cast<StringValue*>(getParameter(
			"Target"));
	mControlledParameterNames = dynamic_cast<StringValue*>(getParameter(
			"ControlledParameters"));
	mMinValue = dynamic_cast<DoubleValue*>(getParameter("Min"));
	mMaxValue = dynamic_cast<DoubleValue*>(getParameter("Max"));
	
	mInterfaceValue = dynamic_cast<InterfaceValue*>(other.mInterfaceValue->createCopy());
	mInputValues.append(mInterfaceValue);

	EventManager *em = Core::getInstance()->getEventManager();
	mSetEvolvedParametersEvent = 
			em->getEvent(EvolutionConstants::EVENT_SET_EVOLVED_OBJECT_PARAMETERS, true);
	mSetEvolvedParametersEvent->addEventListener(this);

	mNameValue->addValueChangedListener(this);
}

/**
 * Destructor.
 */
EvolvableParameter::~EvolvableParameter() {
	
	if(mSetEvolvedParametersEvent != 0) {
		mSetEvolvedParametersEvent->removeEventListener(this);
	}
	//TODO destroy InterfaceValue
	delete mInterfaceValue;
}


SimObject* EvolvableParameter::createCopy() const {
	return new EvolvableParameter(*this);
}


QString EvolvableParameter::getName() const {
	return SimObject::getName();
}



void EvolvableParameter::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm*) {
}


void EvolvableParameter::setup() {
}


void EvolvableParameter::clear() {
}


void EvolvableParameter::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mSetEvolvedParametersEvent) {

		//set the bias of the corresponding control neuron as value for the controlled paramters.

		//get corresponding neural network
		if(mTargetGroup == 0) {
			return;
		}

		NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(mTargetGroup->getController());

		if(net == 0) {
			return;
		}

		//get corresponding neuron
		Neuron *targetNeuron = 0;
		QList<Neuron*> neurons = net->getNeurons();
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *n = i.next();
			if(n->getNameValue().get() == mInterfaceValue->getName()) {
				targetNeuron = n;
			}
		}

		if(targetNeuron == 0) {
			return;
		}

		double tfMin = targetNeuron->getTransferFunction()->getLowerBound();
		double tfMax = targetNeuron->getTransferFunction()->getUpperBound();

		//may also add a search space restriction property to the neuron (min / max)
		targetNeuron->setProperty(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_TARGET);
		targetNeuron->setProperty(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_SOURCE);
		targetNeuron->setProperty(NeuralNetworkConstants::TAG_NEURON_MIN_BIAS, QString::number(tfMin));
		targetNeuron->setProperty(NeuralNetworkConstants::TAG_NEURON_MAX_BIAS, QString::number(tfMax));
		
		NormalizedDoubleValue v(0.0, mMinValue->get(), mMaxValue->get(), tfMin, tfMax);
		v.setNormalized(targetNeuron->getBiasValue().get());

		double parameterValue = v.get();

		//apply parameter settings
		for(QListIterator<DoubleValue*> i(mControlledParameters); i.hasNext();) {
			i.next()->set(parameterValue);
		}

		
	}
}


void EvolvableParameter::valueChanged(Value *value) {
	SimObject::valueChanged(value);
	if(value == 0) {
		return;
	}
	else if(value == mMinValue || value == mMaxValue) {
		//TODO this should rather change a property at the current neuron => force interval.
		mInterfaceValue->setMin(mMinValue->get());
		mInterfaceValue->setMax(mMaxValue->get());
	}
	else if(value == mTargetObjectGroup) {
		//add this EvolvableParameter to the SimObjectGroup

		if(mTargetGroup != 0) {
			mTargetGroup->removeObject(this);
		}
	
		PhysicsManager *pm = Physics::getPhysicsManager();
		mTargetGroup = pm->getSimObjectGroup(mTargetObjectGroup->get());

		if(mTargetGroup == 0) {
			Core::log(QString("EvolvableParameter: Could not add parameter object "
						"to SimObjectGroup [")
						.append(mTargetObjectGroup->get()).append("]"));
		}
		else {
			mTargetGroup->addObject(this);
		}
	}
	else if(value == mControlledParameterNames) {
		//Collect all parameters that fit the patterns in mControlledParameterNames.
		//Note: there can be more than a single pattern, separated by commas.

		mControlledParameters.clear();
		ValueManager *vm = Core::getInstance()->getValueManager();

		QStringList parameterNames = mControlledParameterNames->get().split(",");
		for(QListIterator<QString> i(parameterNames); i.hasNext();) {
			QString pattern = i.next();
			QList<Value*> params = vm->getValuesMatchingPattern(pattern);
			for(QListIterator<Value*> j(params); j.hasNext();) {
				DoubleValue *param = dynamic_cast<DoubleValue*>(j.next());
				if(param != 0) {
					mControlledParameters.append(param);
				}
			}
		}
	}
}

}



