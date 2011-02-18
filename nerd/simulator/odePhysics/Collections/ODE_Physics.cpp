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


#include "ODE_Physics.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Collision/CollisionManager.h"
#include "Collision/ODE_CollisionHandler.h"
#include "Physics/ODE_SimulationAlgorithm.h"
#include "Physics/Physics.h"
#include "Collections/SimulatorPrototypes.h"
#include "SimulationConstants.h"
#include "Physics/ODE_BoxBody.h"
#include "Physics/ODE_BoxInertiaBody.h"
#include "Physics/ODE_SphereBody.h"
#include "Physics/ODE_CylinderBody.h"
#include "Physics/ODE_Plane.h"
#include "Physics/ODE_FixedJoint.h"
#include "Physics/ODE_Dynamixel.h"
#include "Physics/ODE_ServoMotor.h"
#include "Physics/ODE_HingeJoint.h"
#include "Physics/ODE_SliderMotor.h"
#include "Physics/ODE_SliderJoint.h"
#include "MotorModels/ODE_DynamixelFrictionMotor.h"
#include "Collections/ModelPrototypes.h"
#include "Physics/ODE_ForceSensor.h"
#include "MotorModels/ODE_FrictionTorqueMotorModel.h"
#include "MotorModels/ODE_MSeriesTorqueSpringMotorModel.h"
#include "Physics/ODE_MSeriesSimpleDynamixel.h"
#include "Physics/SimpleToeMotor.h"
#include "Physics/ODE_HingeJoint.h"
#include "Physics/ODE_CapsuleBody.h"
#include "Physics/ODE_ForceHandle.h"
#include "MotorModels/ODE_LinearSpringModel.h"
#include "MotorModels/ODE_PID_PassiveActuatorModel.h"

namespace nerd {

ODE_Physics::ODE_Physics()
{
	PhysicsManager *pm = Physics::getPhysicsManager();
	pm->setPhysicalSimulationAlgorithm(new ODE_SimulationAlgorithm());

	CollisionManager *cm = Physics::getCollisionManager();
	cm->setCollisionHandler(new ODE_CollisionHandler());

	SimulatorPrototypes();

	pm->addPrototype(SimulationConstants::PROTOTYPE_BOX_BODY, 
		new ODE_BoxBody(SimulationConstants::PROTOTYPE_BOX_BODY));
	pm->addPrototype(SimulationConstants::PROTOTYPE_BOX_INERTIA_BODY, 
		new ODE_BoxInertiaBody(SimulationConstants::PROTOTYPE_BOX_INERTIA_BODY));
	pm->addPrototype(SimulationConstants::PROTOTYPE_SPHERE_BODY, 
		new ODE_SphereBody(SimulationConstants::PROTOTYPE_SPHERE_BODY));
	pm->addPrototype(SimulationConstants::PROTOTYPE_CYLINDER_BODY, 
		new ODE_CylinderBody(SimulationConstants::PROTOTYPE_CYLINDER_BODY));
	pm->addPrototype(SimulationConstants::PROTOTYPE_PLANE_BODY,
		new ODE_Plane(SimulationConstants::PROTOTYPE_PLANE_BODY));
	pm->addPrototype(SimulationConstants::PROTOTYPE_FIXED_JOINT, 
		new ODE_FixedJoint(SimulationConstants::PROTOTYPE_FIXED_JOINT));
	pm->addPrototype(SimulationConstants::PROTOTYPE_DYNAMIXEL_NO_TORQUE,
		new ODE_Dynamixel(SimulationConstants::PROTOTYPE_DYNAMIXEL_NO_TORQUE, true));
	pm->addPrototype(SimulationConstants::PROTOTYPE_DYNAMIXEL,
		new ODE_Dynamixel(SimulationConstants::PROTOTYPE_DYNAMIXEL));
	pm->addPrototype(SimulationConstants::PROTOTYPE_SERVO_MOTOR,
		new ODE_ServoMotor(SimulationConstants::PROTOTYPE_SERVO_MOTOR));
	pm->addPrototype(SimulationConstants::PROTOTYPE_HINGE_JOINT,
		new ODE_HingeJoint(SimulationConstants::PROTOTYPE_HINGE_JOINT));
	pm->addPrototype(SimulationConstants::PROTOTYPE_SLIDER_MOTOR,
		new ODE_SliderMotor(SimulationConstants::PROTOTYPE_SLIDER_MOTOR));
	pm->addPrototype(SimulationConstants::PROTOTYPE_SLIDER_JOINT,
		new ODE_SliderJoint(SimulationConstants::PROTOTYPE_SLIDER_JOINT));
	pm->addPrototype(SimulationConstants::PROTOTYPE_CAPSULE_BODY,
		new ODE_CapsuleBody(SimulationConstants::PROTOTYPE_CAPSULE_BODY));
	pm->addPrototype(SimulationConstants::PROTOTYPE_FORCE_HANDLE, 
		new ODE_ForceHandle(SimulationConstants::PROTOTYPE_FORCE_HANDLE));

	// add new prototypes for the motormodel
	pm->addPrototype("Prototypes/DynamixelFrictionMotor",
		new ODE_DynamixelFrictionMotor("Prototypes/DynamixelFrictionMotor"));

	pm->addPrototype("Prototypes/ODE_U_FrictionTorqueMotorModel",
		new ODE_U_FrictionTorqueMotorModel("Prototypes/ODE_U_FrictionTorqueMotorModel"));

	pm->addPrototype("Prototypes/ODE_MSeriesSimpleDynamixel1",
		new ODE_MSeriesSimpleDynamixel("ODE_MSeriesSimpleDynamixel", "/SimpleDynamixel1", 
										1, 1.04803939));

	pm->addPrototype("Prototypes/ODE_MSeriesSimpleDynamixel2",
		new ODE_MSeriesSimpleDynamixel("ODE_MSeriesSimpleDynamixel", "/SimpleDynamixel2", 
										2, 2 * 1.04803939));

	pm->addPrototype("Prototypes/ODE_MSeriesSimpleDynamixel3",
		new ODE_MSeriesSimpleDynamixel("ODE_MSeriesSimpleDynamixel", "/SimpleDynamixel3", 
										3, 3 * 1.04803939));

	pm->addPrototype("Prototypes/ODE_MSeriesSimpleDynamixel4",
		new ODE_MSeriesSimpleDynamixel("ODE_MSeriesSimpleDynamixel", "/SimpleDynamixel4", 
										4, 4 * 1.04803939));

	pm->addPrototype("Prototypes/ToeMotor",
		new SimpleToeMotor("ToeMotor"));

	pm->addPrototype(SimulationConstants::PROTOTYPE_FORCE_SENSOR,
		new ODE_ForceSensor("ForceSensor", 0.0, 500.0));

	pm->addPrototype("Prototypes/ODE_H_MSeriesTorqueSpringMotorModel",
		new ODE_H_MSeriesTorqueSpringMotorModel("Prototypes/ODE_H_MSeriesTorqueSpringMotorModel"));

	pm->addPrototype("Prototypes/LinearHingeSpringModel",
		new ODE_LinearSpringModel(MotorModel::HINGE_JOINT, "Prototypes/LinearSpringModel"));

	pm->addPrototype("Prototypes/LinearSliderSpringModel",
		new ODE_LinearSpringModel(MotorModel::SLIDER_JOINT, "Prototypes/LinearSpringModel"));

	pm->addPrototype("Prototypes/PID_PassiveActuatorModel",
		new ODE_PID_PassiveActuatorModel("Prototypes/PID_PassiveActuatorModel"));

	ModelPrototypes();
}

ODE_Physics::~ODE_Physics() {
}

}



