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


#ifndef NERDSimulationConstants_H
#define NERDSimulationConstants_H

#include <QString>

namespace nerd {

	/**
	 * SimulationConstants.
	 */
	class SimulationConstants {
	public:

	//**************************************************************************
	//Directory Names
	//**************************************************************************

	//**************************************************************************
	//Value Names
	//**************************************************************************

		static const QString VALUE_EXECUTION_PAUSE;
		static const QString VALUE_RANDOMIZATION_SIMULATION_SEED;
		static const QString VALUE_EXECUTION_CURRENT_TRY;
		static const QString VALUE_EVO_CURRENT_GENERATION_NUMBER;
		static const QString VALUE_TIME_STEP_SIZE;
		static const QString VALUE_EXECUTION_CURRENT_STEP;
		static const QString VALUE_NNM_BYPASS_NETWORKS;
		static const QString VALUE_RUN_SIMULATION_IN_REALTIME;

	//**************************************************************************
	//Event Names
	//**************************************************************************

		static const QString EVENT_SIMULATION_UPDATE_SENSORS;
		static const QString EVENT_SIMULATION_UPDATE_MOTORS;
		static const QString EVENT_EXECUTION_TERMINATE_TRY;
		static const QString EVENT_EXECUTION_NEXT_INDIVIDUAL;
		static const QString EVENT_PHYSICS_ENVIRONMENT_CHANGED;
		static const QString EVENT_RANDOMIZE_ENVIRONMENT;

	//**************************************************************************
	//Object Names
	//**************************************************************************

		static const QString OBJECT_TERMINATE_TRY_COLLISION_RULE;

	//**************************************************************************
	//Prototype Names
	//**************************************************************************

		static const QString PROTOTYPE_ACCELSENSOR;
		static const QString PROTOTYPE_ACCELSENSOR_3D;
		static const QString PROTOTYPE_BOX_BODY;
		static const QString PROTOTYPE_BOX_INERTIA_BODY;
		static const QString PROTOTYPE_SPHERE_BODY;
		static const QString PROTOTYPE_CYLINDER_BODY;
		static const QString PROTOTYPE_PLANE_BODY;
		static const QString PROTOTYPE_CAPSULE_BODY;
		static const QString PROTOTYPE_FIXED_JOINT;
		static const QString PROTOTYPE_HINGE_JOINT;
		static const QString PROTOTYPE_DYNAMIXEL;
		static const QString PROTOTYPE_A_SERIES;
		static const QString PROTOTYPE_DISTANCE_SENSOR;
		static const QString PROTOTYPE_KHEPERA;
		static const QString PROTOTYPE_SLIDER_MOTOR;
		static const QString PROTOTYPE_SLIDER_JOINT;
		static const QString PROTOTYPE_SHAKER_BOARD;
		static const QString PROTOTYPE_SERVO_MOTOR;
		static const QString PROTOTYPE_LIGHT_SOURCE;
		static const QString PROTOTYPE_LIGHT_SENSOR;
		static const QString PROTOTYPE_FORCE_SENSOR;
		static const QString PROTOTYPE_CURRENT_CONSUMPTION_SENSOR;
		static const QString PROTOTYPE_UNIVERSAL_JOINT_MSERIES_MOTOR_ADAPTER;
		static const QString PROTOTYPE_FORCE_HANDLE;
		static const QString PROTOTYPE_HINGE_SPRING_ADAPTER;
		static const QString PROTOTYPE_SLIDER_SPRING_ADAPTER;
		static const QString PROTOTYPE_GYROSCOPE;

		static const QString PROTOTYPE_TORQUE_DYNAMIXEL_M1;
		static const QString PROTOTYPE_TORQUE_DYNAMIXEL_M2;
		static const QString PROTOTYPE_TORQUE_DYNAMIXEL_M4;
		static const QString PROTOTYPE_TORQUE_DYNAMIXEL_M6;

		static const QString PROTOTYPE_BOX_BODY_NO_PHYSICS;
		static const QString PROTOTYPE_SPHERE_BODY_NO_PHYSICS;
		static const QString PROTOTYPE_CYLINDER_BODY_NO_PHYSICS;
		static const QString PROTOTYPE_PLANE_BODY_NO_PHYSICS;
		static const QString PROTOTYPE_WAVEFRONT_BODY_NO_PHYSICS;

		static const QString PROTOTYPE_M_SERIES_V1;

		static const QString PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_TEST;
		static const QString PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_MULTIPLE_TEST;
		static const QString PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_0;
		static const QString PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_1;
		static const QString PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_2;
		static const QString PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_3;
		static const QString PROTOTYPE_M_SERIES_TORQUE_DYNAMIXEL_MOTOR_4;

		static const QString PROTOTYPE_DIFFERENTIAL_DRIVE;

	};

}

#endif


