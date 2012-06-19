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

#include "NerdConstants.h"

using namespace nerd;


const bool NerdConstants::HackMode = false;


//**************************************************************************
//Directory names
//**************************************************************************

const QString NerdConstants::NERD_CONFIG_DIRECTORY
		= "config";


//**************************************************************************
//Object Names
//**************************************************************************

const QString NerdConstants::OBJECT_STATISTICS_MANAGER
		= "StatisticsManager";

const QString NerdConstants::OBJECT_GUI_MANAGER
		= "GuiManager";

//**************************************************************************
//GUI Element Names
//**************************************************************************

const QString NerdConstants::GUI_SELECT_FILE_FOR_STRING_VALUE
		= "Select file ...";

const QString NerdConstants::GUI_MAIN_SIMULATION_WINDOW
		= "MainSimulationWindow";

//**************************************************************************
//Value names
//**************************************************************************


const QString NerdConstants::VALUE_NERD_ENABLE_PERFORMANCE_MEASUREMENTS
		= "/Performance/EnablePerformanceMeasurements";

// const QString NerdConstants::VALUE_NERD_SYSTEM_PAUSE
// 		= "/Simulation/Pause";
const QString NerdConstants::VALUE_NERD_RECENT_LOGGER_MESSAGE
		= "/Logger/RecentMessage";

const QString NerdConstants::VALUE_NERD_REPOSITORY_CHANGED_COUNTER
		= "/ValueManager/RepositoryChangedCounter";

const QString NerdConstants::VALUE_NERD_STEPS_PER_SECOND
		= "/Performance/StepsPerSecond";

const QString NerdConstants::VALUE_RUN_IN_PERFORMANCE_MODE
		= "/Performance/RunPerformanceMode";

const QString NerdConstants::VALUE_EXECUTION_PAUSE
		= "/Control/Pause";

const QString NerdConstants::VALUE_RANDOMIZATION_SIMULATION_SEED
		= "/Simulation/Seed";

const QString NerdConstants::VALUE_NNM_BYPASS_NETWORKS
		= "/NeuralNetwork/BypassNetworks";
		

//**************************************************************************
//Event names
//**************************************************************************


const QString NerdConstants::EVENT_NERD_SYSTEM_INIT
		= "/Control/SystemInit";

const QString NerdConstants::EVENT_NERD_SYSTEM_INIT_COMPLETED
		= "/Control/SystemInitCompleted";

const QString NerdConstants::EVENT_NERD_SYSTEM_BIND
		= "/Control/SystemBind";

const QString NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN
		= "/Control/SystemShutDown";

const QString NerdConstants::EVENT_NERD_EXECUTE_PENDING_TASKS
		= "/Control/PendingTasksExecuted";

const QString NerdConstants::EVENT_EXECUTION_NEXT_STEP
		= "/Control/NextStep";

const QString NerdConstants::EVENT_EXECUTION_STEP_COMPLETED
		= "/Control/StepCompleted";

const QString NerdConstants::EVENT_EXECUTION_PRE_STEP_COMPLETED
		= "/Control/PreStepCompleted";

const QString NerdConstants::EVENT_EXECUTION_RESET
		= "/Control/ResetSimulation";

const QString NerdConstants::EVENT_EXECUTION_RESET_COMPLETED
		= "/Control/ResetSimulationFinalized";

const QString NerdConstants::EVENT_EXECUTION_RESET_SETTINGS
		= "/Control/ResetSimulationSettings";

const QString NerdConstants::EVENT_EXECUTION_RESET_SETTINGS_COMPLETED
		= "/Control/ResetSimulationSettingsCompleted";

const QString NerdConstants::EVENT_EXECUTION_NEXT_TRY
		= "/Control/NextTry";

const QString NerdConstants::EVENT_EXECUTION_TRY_COMPLETED
		= "/Control/TryCompleted";

const QString NerdConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL
		= "/Control/NextIndividual";

const QString NerdConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED
		= "/Control/IndividualCompleted";

const QString NerdConstants::EVENT_EXECUTION_GENERATION_STARTED
		= "/Evolution/GenerationStarted";

const QString NerdConstants::EVENT_EXECUTION_GENERATION_COMPLETED
		= "/Evolution/GenerationCompleted";

const QString NerdConstants::EVENT_PHYSICS_ENVIRONMENT_CHANGED
		= "/Simulation/PhysicsEnvironmentChanged";

const QString NerdConstants::EVENT_VALUE_REPOSITORY_CHANGED 
		= "/ValueRepository/RepositoryChanged";

const QString NerdConstants::EVENT_GENERATION_STATISTICS_CALCULATED
		= "/Statistics/GenerationStatisticsCalculated";

const QString NerdConstants::EVENT_INDIVIDUAL_STATISTICS_CALCULATED
		= "/Statistics/IndividualStatisticsCalculated";

const QString NerdConstants::EVENT_TRY_STATISTICS_CALCULATED
		= "/Statistics/TryStatisticsCalculated";

const QString NerdConstants::EVENT_STEP_STATISTICS_CALCULATED
		= "/Statistics/StepStatisticsCalculated";

const QString NerdConstants::EVENT_NNM_NETWORK_ITERATION_COMPLETED
		= "/NeuralNetwork/NetworkIterationCompleted";
		
		
//**************************************************************************
//Environment Variable Names
//**************************************************************************
		
		
const QString NerdConstants::ENV_VAR_VIDEO_MODE
		= "NERD_VIDEO_MODE";
