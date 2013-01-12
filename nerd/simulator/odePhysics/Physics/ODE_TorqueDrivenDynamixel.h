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


#ifndef NERDODE_TorqueDrivenDynamixel_H
#define NERDODE_TorqueDrivenDynamixel_H

#include "ODE_Joint.h"
#include "Physics/TorqueDrivenDynamixel.h"
#include "Event/EventListener.h"

namespace nerd {

/**
 * ODE_TorqueDrivenDynamixel represents a SimJoint object, that can represent a 
 * simple hinge joint which is simulated with the physics engine ODE.
 * ODE_TorqueDrivenDynamixel is an implementation of a torque controlled Dynamixel AX-12+ 
 * motor.
 * The parameters of the HingeJoint are controlled via its parameter values (see SimJoint, TorqueDrivenDynamixel).
 * Some values allow the global modification of the physical simulation of all TorqueDrivenDynamixel objects:
 * "/Dynamixel/ERP", "/Dynamixel/CFM", "/Dynamixel/CFM", "/Dynamixel/StopERP", "/Dynamixel/StopCFM". 
 * These values modify the according ode-settings which influence the ode-joint properties. 
 * For more information check the ode-manual or the nkg-wiki.
 */
class ODE_TorqueDrivenDynamixel : public TorqueDrivenDynamixel, public ODE_Joint{

	public:
		ODE_TorqueDrivenDynamixel(const QString &name);
		ODE_TorqueDrivenDynamixel(const ODE_TorqueDrivenDynamixel &joint);
		virtual ~ODE_TorqueDrivenDynamixel();
		
		virtual SimJoint* createCopy() const;

		virtual void setup();
		virtual void clear();
		
		virtual void valueChanged(Value *value);

	protected:
		virtual dJointID createJoint(dBodyID body1, dBodyID body2);

		virtual void updateMotors();
		virtual void updateSensors();

	private:		
	
		DoubleValue *mDynamixelERP;
		DoubleValue* mDynamixelCFM;
		DoubleValue *mDynamixelStopERP;
		DoubleValue* mDynamixelStopCFM;
		
		bool mIsStart;	

		// Parameters that are necessary for the motor control

		double angle;
		double startAngle;
		double torque;
		double friction;
		double max_torque;

		bool mUpdateMotor;
		double oldTorque;

		double mSimulationTime;
		double oldMotorSetting;
	};

}

#endif

