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

#include "SimulatorPrototypes.h"
#include "Physics/AccelSensor.h"
#include "Physics/Physics.h"
#include "SimulationConstants.h"
#include "Models/PlaneGround.h"
#include "Physics/DistanceSensor.h"
#include "Physics/LightSource.h"
#include "Physics/LightSensor.h"
#include "Physics/BoxBody.h"
#include "Physics/SphereBody.h"
#include "Physics/CylinderBody.h"
#include "Physics/PlaneBody.h"
#include "Physics/WavefrontBody.h"
#include "MotorModels/DynamixelMotorAdapter.h"
#include "Physics/CurrentConsumptionSensor.h"
#include "MotorModels/TorqueDynamixelMotorAdapter.h"
#include "Physics/UniversalJointMotorAdapter.h"
#include "MotorModels/SpringAdapter.h"
#include <iostream>
#include "PlugIns/PrototypeOverviewPrinter.h"
#include "Physics/SinkBox.h"


using namespace std;

namespace nerd{

  SimulatorPrototypes::SimulatorPrototypes() {

	new PrototypeOverviewPrinter();

    PhysicsManager *pm = Physics::getPhysicsManager();

    pm->addPrototype(SimulationConstants::PROTOTYPE_ACCELSENSOR, 
        new AccelSensor(SimulationConstants::PROTOTYPE_ACCELSENSOR, 2));

	pm->addPrototype(SimulationConstants::PROTOTYPE_ACCELSENSOR_3D, 
        new AccelSensor(SimulationConstants::PROTOTYPE_ACCELSENSOR_3D, 3));

    pm->addPrototype(SimulationConstants::PROTOTYPE_DISTANCE_SENSOR,
        new DistanceSensor(SimulationConstants::PROTOTYPE_DISTANCE_SENSOR));

    pm->addPrototype(SimulationConstants::PROTOTYPE_LIGHT_SOURCE,
        new LightSource(SimulationConstants::PROTOTYPE_LIGHT_SOURCE, 0.8, 0.5, 0));

    pm->addPrototype(SimulationConstants::PROTOTYPE_LIGHT_SENSOR,
        new LightSensor(SimulationConstants::PROTOTYPE_LIGHT_SENSOR));

    pm->addPrototype(SimulationConstants::PROTOTYPE_BOX_BODY_NO_PHYSICS, 
        new BoxBody(SimulationConstants::PROTOTYPE_BOX_BODY_NO_PHYSICS)); 
    pm->addPrototype(SimulationConstants::PROTOTYPE_SPHERE_BODY_NO_PHYSICS, 
        new SphereBody(SimulationConstants::PROTOTYPE_SPHERE_BODY_NO_PHYSICS)); 
    pm->addPrototype(SimulationConstants::PROTOTYPE_CYLINDER_BODY_NO_PHYSICS, 
        new CylinderBody(SimulationConstants::PROTOTYPE_CYLINDER_BODY_NO_PHYSICS)); 
    pm->addPrototype(SimulationConstants::PROTOTYPE_PLANE_BODY_NO_PHYSICS, 
        new PlaneBody(SimulationConstants::PROTOTYPE_PLANE_BODY_NO_PHYSICS)); 

    pm->addPrototype(SimulationConstants::PROTOTYPE_WAVEFRONT_BODY_NO_PHYSICS,
        new WavefrontBody(SimulationConstants::PROTOTYPE_WAVEFRONT_BODY_NO_PHYSICS));

	pm->addPrototype(SimulationConstants::PROTOTYPE_CURRENT_CONSUMPTION_SENSOR,
		new CurrentConsumptionSensor(SimulationConstants::PROTOTYPE_CURRENT_CONSUMPTION_SENSOR));

	pm->addPrototype(SimulationConstants::PROTOTYPE_UNIVERSAL_JOINT_MSERIES_MOTOR_ADAPTER,
		new UniversalJointMotorAdapter(
				SimulationConstants::PROTOTYPE_UNIVERSAL_JOINT_MSERIES_MOTOR_ADAPTER));

	//Torque motors

	// Joint Motor with 1 Dynamixel, JointAngleSensor, MotorAngleSensor, no CurrentConsumptionSensor, no SpringCoupling and
	// springToJointTransmissionRatio 1:1.
	pm->addPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_TEST,
		new TorqueDynamixelMotorAdapter(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_TEST, 
										1, true, true, true, false, true));

	pm->addPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_MULTIPLE_TEST,
		new TorqueDynamixelMotorAdapter(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_MULTIPLE_TEST, 
										2, true, true, true, false, true));

	// Joint Motors for M-Series
	pm->addPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_0,
		new TorqueDynamixelMotorAdapter(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_0,
										0, true, false, false, false, true));
	pm->addPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_1,
		new TorqueDynamixelMotorAdapter(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_1,
										1, true, true, true, true, true));
	pm->addPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_2,
		new TorqueDynamixelMotorAdapter(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_2,
										2, true, true, true, true, true));
	pm->addPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_3,
		new TorqueDynamixelMotorAdapter(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_3,
										3, true, true, true, true, true));
	pm->addPrototype(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_4,
		new TorqueDynamixelMotorAdapter(SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_4,
										4, true, true, true, true, true));

    pm->addPrototype("Prototypes/DynamixelMotorAdapter",
        new DynamixelMotorAdapter("Prototypes/DynamixelMotorAdapter", 
                                  "/DynamixelMotorAdapter/ActiveModel"));

	pm->addPrototype("Prototypes/SinkBox", new SinkBox("Prototypes/SinkBox"));

	//TODO change into constants
	pm->addPrototype(SimulationConstants::PROTOTYPE_HINGE_SPRING_ADAPTER, 
		new SpringAdapter(MotorModel::HINGE_JOINT, SimulationConstants::PROTOTYPE_HINGE_SPRING_ADAPTER));
	pm->addPrototype(SimulationConstants::PROTOTYPE_SLIDER_SPRING_ADAPTER, 
		new SpringAdapter(MotorModel::SLIDER_JOINT, SimulationConstants::PROTOTYPE_SLIDER_SPRING_ADAPTER));
  }

}
