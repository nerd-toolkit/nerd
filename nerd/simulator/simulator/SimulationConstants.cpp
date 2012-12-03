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

#include "SimulationConstants.h"

using namespace nerd;

//**************************************************************************
//Value names
//**************************************************************************

const QString SimulationConstants::VALUE_EXECUTION_PAUSE
		= "/Control/Pause";

const QString SimulationConstants::VALUE_RANDOMIZATION_SIMULATION_SEED 
		= "/Simulation/Seed";

const QString SimulationConstants::VALUE_RANDOMIZE_SEED_AT_RESET
		= "/Simulation/RandomizeSeedAtReset";

const QString SimulationConstants::VALUE_EXECUTION_CURRENT_TRY
		= "/Control/CurrentTry";

const QString SimulationConstants::VALUE_TIME_STEP_SIZE
		= "/Simulation/TimeStepSize";

const QString SimulationConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER
		= "/Evolution/CurrentGeneration";

const QString SimulationConstants::VALUE_EXECUTION_CURRENT_STEP
		= "/Control/CurrentStep";

const QString SimulationConstants::VALUE_NNM_BYPASS_NETWORKS
		= "/NeuralNetwork/BypassNetworks";

const QString SimulationConstants::VALUE_RUN_SIMULATION_IN_REALTIME
		= "/Simulation/RunAtRealtime";

const QString SimulationConstants::VALUE_RUN_REAL_TIME_RECORDER
		= "/ScreenRecorder/Control/Run";
		
const QString SimulationConstants::VALUE_SWITCH_YZ_AXES
		= "/Simulation/Switch-Y-Z-Axes";
		
const QString SimulationConstants::VALUE_DISABLE_PHYSICS
		= "/Simulation/DisablePhysics";

//**************************************************************************
//Event names
//**************************************************************************


const QString SimulationConstants::EVENT_SIMULATION_UPDATE_SENSORS
		= "/Simulation/UpdateSensors";

const QString SimulationConstants::EVENT_SIMULATION_UPDATE_MOTORS 
		= "/Simulation/UpdateMotors";

const QString SimulationConstants::EVENT_EXECUTION_TERMINATE_TRY
		 = "/Control/TerminateTry";

const QString SimulationConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL
		= "/Control/NextIndividual";

const QString SimulationConstants::EVENT_PHYSICS_ENVIRONMENT_CHANGED
		= "/Simulation/PhysicsEnvironmentChanged";

const QString SimulationConstants::EVENT_RANDOMIZE_ENVIRONMENT
		= "/Simulation/RandomizeEnvironment";

//**************************************************************************
//Object Names
//**************************************************************************

const QString SimulationConstants::OBJECT_TERMINATE_TRY_COLLISION_RULE 
		= "TerminateTry";

//**************************************************************************
//Prototype Names
//**************************************************************************

const QString SimulationConstants::PROTOTYPE_ACCELSENSOR = "Prototypes/AccelSensor";
const QString SimulationConstants::PROTOTYPE_ACCELSENSOR_3D = "Prototypes/AccelSensor3D";
const QString SimulationConstants::PROTOTYPE_BOX_BODY = "Prototypes/Box";
const QString SimulationConstants::PROTOTYPE_BOX_INERTIA_BODY = "Prototypes/BoxInertia";
const QString SimulationConstants::PROTOTYPE_SPHERE_BODY = "Prototypes/Sphere";
const QString SimulationConstants::PROTOTYPE_CYLINDER_BODY = "Prototypes/Cylinder";
const QString SimulationConstants::PROTOTYPE_PLANE_BODY = "Prototypes/Plane";
const QString SimulationConstants::PROTOTYPE_CAPSULE_BODY = "Prototypes/Capsule";
const QString SimulationConstants::PROTOTYPE_FIXED_JOINT = "Prototypes/FixedJoint";
const QString SimulationConstants::PROTOTYPE_HINGE_JOINT = "Prototypes/HingeJoint";
const QString SimulationConstants::PROTOTYPE_DYNAMIXEL = "Prototypes/Dynamixel";
const QString SimulationConstants::PROTOTYPE_DYNAMIXEL_NO_TORQUE = "Prototypes/DynamixelNoTorque";
const QString SimulationConstants::PROTOTYPE_A_SERIES = "Prototypes/ASeries";
const QString SimulationConstants::PROTOTYPE_DISTANCE_SENSOR = "Prototypes/DistanceSensor";
const QString SimulationConstants::PROTOTYPE_ACTIVE_DISTANCE_SENSOR = "Prototypes/ActiveDistanceSensor";
const QString SimulationConstants::PROTOTYPE_KHEPERA = "Prototypes/Khepera";
const QString SimulationConstants::PROTOTYPE_SLIDER_MOTOR = "Prototypes/SliderMotor";
const QString SimulationConstants::PROTOTYPE_SHAKER_BOARD = "Prototypes/ShakerBoard";
const QString SimulationConstants::PROTOTYPE_SLIDER_JOINT = "Prototypes/SliderJoint";
const QString SimulationConstants::PROTOTYPE_SERVO_MOTOR = "Prototypes/ServoMotor";
const QString SimulationConstants::PROTOTYPE_SERVO_MOTOR_WITH_TEMPERATURE = "Prototypes/ServoMotorTemp";
const QString SimulationConstants::PROTOTYPE_LIGHT_SOURCE = "Prototypes/LightSource";
const QString SimulationConstants::PROTOTYPE_SPHERIC_LIGHT_SOURCE = "Prototypes/SphericLightSource";
const QString SimulationConstants::PROTOTYPE_LIGHT_SENSOR = "Prototypes/LightSensor";
const QString SimulationConstants::PROTOTYPE_FORCE_SENSOR = "Prototypes/ForceSensor";
const QString SimulationConstants::PROTOTYPE_CURRENT_CONSUMPTION_SENSOR = 
													"Prototypes/CurrentConsumptionSensor";

const QString SimulationConstants::PROTOTYPE_UNIVERSAL_JOINT_MSERIES_MOTOR_ADAPTER = 
													"Prototypes/UniversalJointAdapterMSeries";

const QString SimulationConstants::PROTOTYPE_FORCE_HANDLE = "Prototypes/ForceHandle";
const QString SimulationConstants::PROTOTYPE_HINGE_SPRING_ADAPTER = "Prototypes/HingeSpringAdapter";
const QString SimulationConstants::PROTOTYPE_SLIDER_SPRING_ADAPTER = "Prototypes/SliderSpringAdapter";

const QString SimulationConstants::PROTOTYPE_GYROSCOPE = "Prototypes/Gyroscope";
const QString SimulationConstants::PROTOTYPE_VALUE_TRANSFER_CONTROLLER = "Prototypes/ValueTransferController";
const QString SimulationConstants::PROTOTYPE_AUTO_VALUE_TRANSFER_CONTROLLER = "Prototypes/AutomaticValueChanger";
const QString SimulationConstants::PROTOTYPE_INTERNAL_STATE_VALUE = "Prototypes/InternalStateValue";
const QString SimulationConstants::PROTOTYPE_EXTERNAL_SENSOR_ADAPTER = "Prototypes/ExternalSensor";

const QString SimulationConstants::PROTOTYPE_TORQUE_DYNAMIXEL_M1 = "Prototypes/TorqueDynamixelM1";
const QString SimulationConstants::PROTOTYPE_TORQUE_DYNAMIXEL_M2 = "Prototypes/TorqueDynamixelM2";
const QString SimulationConstants::PROTOTYPE_TORQUE_DYNAMIXEL_M4 = "Prototypes/TorqueDynamixelM4";
const QString SimulationConstants::PROTOTYPE_TORQUE_DYNAMIXEL_M6 = "Prototypes/TorqueDynamixelM6";

const QString SimulationConstants::PROTOTYPE_BOX_BODY_NO_PHYSICS = "Prototypes/BoxV";
const QString SimulationConstants::PROTOTYPE_SPHERE_BODY_NO_PHYSICS = "Prototypes/SphereV";
const QString SimulationConstants::PROTOTYPE_CYLINDER_BODY_NO_PHYSICS = "Prototypes/CylinderV";
const QString SimulationConstants::PROTOTYPE_PLANE_BODY_NO_PHYSICS = "Prototypes/PlaneV";
const QString SimulationConstants::PROTOTYPE_WAVEFRONT_BODY_NO_PHYSICS = "Prototypes/WavefrontV";

const QString SimulationConstants::PROTOTYPE_M_SERIES_V1 = "Prototypes/MSeriesV1";

const QString SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_TEST = "Prototypes/MSeriesTorqueDynamixelMotorTest";
const QString SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_MULTIPLE_TEST = "Prototypes/MSeriesTorqueDynamixelMotorMultipleTest";
const QString SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_0 = "Prototypes/MSeriesTorqueDynamixelMotor0";
const QString SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_1 = "Prototypes/MSeriesTorqueDynamixelMotor1";
const QString SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_2 = "Prototypes/MSeriesTorqueDynamixelMotor2";
const QString SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_3 = "Prototypes/MSeriesTorqueDynamixelMotor3";
const QString SimulationConstants::PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_4 = "Prototypes/MSeriesTorqueDynamixelMotor4";

const QString SimulationConstants::PROTOTYPE_DIFFERENTIAL_DRIVE = "Prototypes/DifferentialDrive";

const QString SimulationConstants::PROTOTYPE_PASSIVE_HINGE_ACTUATOR_ADAPTER 
		= "Prototypes/PassiveHingeActuatorAdapter";


