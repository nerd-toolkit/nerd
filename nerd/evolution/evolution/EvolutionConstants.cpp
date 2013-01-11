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

#include "EvolutionConstants.h"

using namespace nerd;

//**************************************************************************
//Directory Names
//**************************************************************************

const QString EvolutionConstants::GENERATION_DIRECTORY_PREFIX 
		= "gen";

const QString EvolutionConstants::GENERATION_SETTINGS_LOG_FILE
		= "evolutionSettings.val";

const QString EvolutionConstants::GENERATION_STATISTICS_LOG_FILE
		= "statistics.log";

//**************************************************************************
//Global Object Names
//**************************************************************************

const QString EvolutionConstants::OBJECT_EVOLUTION_MANAGER
		= "EvolutionManager";

const QString EvolutionConstants::OBJECT_SETTINGS_LOGGER 
		= "SettingsLogger";

const QString EvolutionConstants::OBJECT_FITNESS_MANAGER
		= "FitnessManager";

const QString EvolutionConstants::OBJECT_TERMINATE_TRY_COLLISION_RULE 
		= "TerminateTry";
		
const QString EvolutionConstants::OBJECT_SIMPLE_FITNESS_LOGGER
		= "SimpleFitnessLogger";

//**************************************************************************
//Path Names
//**************************************************************************

const QString EvolutionConstants::FITNESS_FUNCTION_PATH
		= "/FitnessFunctions/";


//**************************************************************************
//Event Names
//**************************************************************************

const QString EvolutionConstants::EVENT_EVO_TRIGGER_NEXT_GENERATION
		= "/Evolution/TriggerNextGeneration";

const QString EvolutionConstants::EVENT_EVO_GENERATION_STARTED
		= "/Evolution/GenerationStarted";

const QString EvolutionConstants::EVENT_EVO_GENERATION_COMPLETED
		= "/Evolution/GenerationCompleted";

const QString EvolutionConstants::EVENT_EVO_EVALUATION_STARTED
		= "/Evolution/EvaluationStarted";

const QString EvolutionConstants::EVENT_EVO_EVALUATION_COMPLETED
		= "/Evolution/EvaluationCompleted";

const QString EvolutionConstants::EVENT_EVO_SELECTION_STARTED
		= "/Evolution/SelectionStarted";

const QString EvolutionConstants::EVENT_EVO_SELECTION_COMPLETED
		= "/Evolution/SelectionCompleted";

const QString EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_STARTED
		= "/Evolution/EvolutionAlgorithmStarted";

const QString EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_COMPLETED
		= "/Evolution/EvolutionAlgorithmCompleted";

const QString EvolutionConstants::EVENT_EXECUTION_NEXT_TRY
		= "/Control/NextTry";

const QString EvolutionConstants::EVENT_EXECUTION_TRY_COMPLETED
		= "/Control/TryCompleted";

const QString EvolutionConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL
		= "/Control/NextIndividual";

const QString EvolutionConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED
		= "/Control/IndividualCompleted";

const QString EvolutionConstants::EVENT_EXECUTION_GENERATION_COMPLETED
		= "/Evolution/GenerationCompleted";

const QString EvolutionConstants::EVENT_EXECUTION_TERMINATE_TRY
		= "/Control/TerminateTry";

const QString EvolutionConstants::EVENT_SET_EVOLVED_OBJECT_PARAMETERS
		= "/Evolution/SetEvolvedParameters";

const QString EvolutionConstants::EVENT_EVO_CLUSTER_JOB_SUBMITTED
		= "/Evolution/ClusterJobSubmitted";

const QString EvolutionConstants::EVENT_EVO_EVOLUTION_TERMINATED 
		= "/Evolution/EvolutionTerminated";

const QString EvolutionConstants::EVENT_EVO_EVOLUTION_RESTARTED 
		= "/Evolution/EvolutionRestarted";

const QString EvolutionConstants::EVENT_PHYSICS_ENVIRONMENT_CHANGED
		= "/Simulation/PhysicsEnvironmentChanged";
		
const QString EvolutionConstants::EVENT_EVO_GENERATE_INDIVIDUAL_STARTED
		= "/Evolution/GenerateIndividualStarted";
		
const QString EvolutionConstants::EVENT_EVO_GENERATE_INDIVIDUAL_COMPLETED
		= "/Evolution/GenerateIndividualCompleted";

//**************************************************************************
//Value Names
//**************************************************************************

const QString EvolutionConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER
		= "/Evolution/CurrentGeneration";


const QString EvolutionConstants::VALUE_EVO_RUN_EVOLUTION
		= "/Evolution/RunEvolution";

const QString EvolutionConstants::VALUE_EVO_RESTART_GENERATION
		= "/Evolution/RestartGeneration";

const QString EvolutionConstants::VALUE_EVO_STASIS_MODE
		= "/Evolution/StasisMode";

const QString EvolutionConstants::VALUE_EVO_WORKING_DIRECTORY
		= "/Evolution/WorkingDirectory";

const QString EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES
		= "/Control/NumberOfTries";

const QString EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS
		= "/Control/NumberOfSteps";

const QString EvolutionConstants::VALUE_NUMBER_OF_COMPLETED_INDIVIDUALS_EVO
		= "/Evolution/EvoCompletedIndividuals";

const QString EvolutionConstants::VALUE_NUMBER_OF_COMPLETED_INDIVIDUALS_EVAL
		= "/Evolution/EvalCompletedIndividuals";


const QString EvolutionConstants::VALUE_CURRENT_GENERATION_DIRECTORY
		= "/Evolution/CurrentGenerationDirectory";

const QString EvolutionConstants::VALUE_CURRENT_INDIVIDUAL_EVALUATION_START_SCRIPT
		= "/Evolution/CurrentIndividualEvaluationStartScript";

const QString EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_INDIVIDUALS
		= "/Control/NumberOfIndividuals";


const QString EvolutionConstants::VALUE_EXECUTION_PAUSE
		= "/Control/Pause";

const QString EvolutionConstants::VALUE_EXECUTION_CURRENT_INDIVIDUAL
		= "/Control/CurrentIndividual";

const QString EvolutionConstants::VALUE_EXECUTION_CURRENT_TRY
		= "/Control/CurrentTry";

const QString EvolutionConstants::VALUE_EXECUTION_CURRENT_STEP
		= "/Control/CurrentStep";


const QString EvolutionConstants::VALUE_TIME_STEP_SIZE
		= "/Simulation/TimeStepSize";

const QString EvolutionConstants::VALUE_INCREMENTAL_LOGGER_COMMENT
		= "/Logging/IncrementalLogger/Comment";

const QString EvolutionConstants::VALUE_RUN_SIMULATION_IN_REALTIME
		= "/Simulation/RunAtRealtime";

//**************************************************************************
//Tag Names
//**************************************************************************


const QString EvolutionConstants::TAG_EVO_CREATION_DATE
		= "_CreationDate";

const QString EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE
		= "_Modified";
		
const QString EvolutionConstants::TAG_GENOME_CHANGE_SUMMARY
		= "Mutations";
		
const QString EvolutionConstants::TAG_NETWORK_MUTATION_HISTORY
		= "_MutationHistory";
		

