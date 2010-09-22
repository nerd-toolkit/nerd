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

#ifndef NERD_HINGEMOTORADAPTER_H
#define NERD_HINGEMOTORADAPTER_H


#include "Physics/HingeJoint.h"
#include "Physics/SimSensor.h"
#include "Physics/SimActuator.h"

#include <QMap>

class QString;
namespace nerd { class SimObject; }
namespace nerd { class HingeMotor; }
namespace nerd { class DoubleValue; }
namespace nerd { class BoolValue; }
namespace nerd { class StringValue; }

namespace nerd {

/**
 * HingeMotorAdapter
 * 
 * Base class for all HingeMotorAdapters. A HingeMotorAdpater can hold different
 * HingeMotor objects. The user can configure which HingeMotor should be used.
 * The HingeMotorAdapter provides a motor interface and passes the set parameters to the
 * HingeMotors.  
 */  
class HingeMotorAdapter : public HingeJoint, public SimSensor, public SimActuator {
	public:
		
		HingeMotorAdapter(const QString &name, const QString &globalActiveMotorValue);
		HingeMotorAdapter(const HingeMotorAdapter &hingeMotorAdapter);
		
		virtual SimObject* createCopy() const = 0;
	
		virtual void valueChanged(Value *value);
		virtual void setup();
		virtual void clear();
	
		virtual void updateActuators();
		virtual void updateSensorValues();
	
	protected:
		virtual void updateMotorParameter(const QString &parameter, Value *value);
		virtual QString createMissingMotorErrorMsg();


	protected:
		DoubleValue *mMinAngleValue;
		DoubleValue *mMaxAngleValue;
    
		/**
		* Name of the HingeMotor which should be active. 
		*/
		StringValue *mActiveMotorName;
		
		/**
		* Defines if a change to the global ActiveMotor Value
		* changes also the current ActiveMotor of this HingeMotorAdapter.
		*/
		BoolValue *mUseGlobalActiveMotorChanges;
		
		/**
		* Name of the HingeMotor which should be globaly active for all HingeMotorAdapter
		* objects of the same type if their mUseGlobalActiveMotorChanges is true. 
		*/
		StringValue *mGlobalActiveMotorName;
		
		/**
		* Parameter which is used as Interface to change the mGlobalActiveMotorName directly
		* via the HingeMotorAdapter object.
		* 
		* mGlobalActiveMotorName is not a Parameter to avoid that it would be delete automatically
		* if one object gets deleted.
		*/
		StringValue *mGlobalActiveMotorNameInterace;
		
		/**
		* Map of all HingeMotors which can used for the adapter.
		* The Key is the name of the HingeMotor.
		*/
		QMap<QString, HingeMotor*> mHingeMotors;
		
		/**
		* Reference to the current active HingeMotor.
		*/
		HingeMotor *mActiveMotor;
};

} // namespace nerd
#endif
