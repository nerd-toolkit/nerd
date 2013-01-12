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


#ifndef NERDEvolutionConstants_H
#define NERDEvolutionConstants_H

#include <QString>

namespace nerd {

	/**
	 * EvolutionConstants.
	 */
	class EvolutionConstants {
	public:

	//**************************************************************************
	//Directory Names
	//**************************************************************************

		static const QString GENERATION_DIRECTORY_PREFIX;
		static const QString GENERATION_SETTINGS_LOG_FILE;
		static const QString GENERATION_STATISTICS_LOG_FILE;

	//**************************************************************************
	//Global Object Names
	//**************************************************************************

		static const QString OBJECT_EVOLUTION_MANAGER;
		static const QString OBJECT_SETTINGS_LOGGER;
		static const QString OBJECT_FITNESS_MANAGER;
		static const QString OBJECT_TERMINATE_TRY_COLLISION_RULE;
		static const QString OBJECT_SIMPLE_FITNESS_LOGGER;

	//**************************************************************************
	//Path Names
	//**************************************************************************

		static const QString FITNESS_FUNCTION_PATH;

	//**************************************************************************
	//Event Names
	//**************************************************************************

		static const QString EVENT_EVO_TRIGGER_NEXT_GENERATION;
		static const QString EVENT_EVO_GENERATION_STARTED;
		static const QString EVENT_EVO_GENERATION_COMPLETED;
		static const QString EVENT_EVO_EVALUATION_STARTED;
		static const QString EVENT_EVO_EVALUATION_COMPLETED;
		static const QString EVENT_EVO_SELECTION_STARTED;
		static const QString EVENT_EVO_SELECTION_COMPLETED;
		static const QString EVENT_EVO_EVOLUTION_ALGORITHM_STARTED;
		static const QString EVENT_EVO_EVOLUTION_ALGORITHM_COMPLETED;
		static const QString EVENT_EXECUTION_NEXT_TRY;
		static const QString EVENT_EXECUTION_TRY_COMPLETED;
		static const QString EVENT_EXECUTION_NEXT_INDIVIDUAL;
		static const QString EVENT_EXECUTION_INDIVIDUAL_COMPLETED;
		static const QString EVENT_EXECUTION_GENERATION_COMPLETED;
		static const QString EVENT_EXECUTION_TERMINATE_TRY;
		static const QString EVENT_SET_EVOLVED_OBJECT_PARAMETERS;
		static const QString EVENT_EVO_CLUSTER_JOB_SUBMITTED;
		static const QString EVENT_EVO_EVOLUTION_TERMINATED;
		static const QString EVENT_EVO_EVOLUTION_RESTARTED;
		static const QString EVENT_PHYSICS_ENVIRONMENT_CHANGED;
		static const QString EVENT_EVO_GENERATE_INDIVIDUAL_STARTED;
		static const QString EVENT_EVO_GENERATE_INDIVIDUAL_COMPLETED;


	//**************************************************************************
	//Value Names
	//**************************************************************************

		static const QString VALUE_EVO_CURRENT_GENERATION_NUMBER;
		static const QString VALUE_EVO_RUN_EVOLUTION;
		static const QString VALUE_EVO_RESTART_GENERATION;
		static const QString VALUE_EVO_STASIS_MODE;
		static const QString VALUE_EVO_WORKING_DIRECTORY;
		static const QString VALUE_EXECUTION_NUMBER_OF_TRIES;
		static const QString VALUE_EXECUTION_NUMBER_OF_STEPS;
		static const QString VALUE_NUMBER_OF_COMPLETED_INDIVIDUALS_EVO;
		static const QString VALUE_NUMBER_OF_COMPLETED_INDIVIDUALS_EVAL;

		static const QString VALUE_CURRENT_GENERATION_DIRECTORY;
		static const QString VALUE_CURRENT_INDIVIDUAL_EVALUATION_START_SCRIPT;

		static const QString VALUE_EXECUTION_NUMBER_OF_INDIVIDUALS;
		static const QString VALUE_EXECUTION_CURRENT_INDIVIDUAL;
		static const QString VALUE_EXECUTION_CURRENT_TRY;
		static const QString VALUE_EXECUTION_CURRENT_STEP;
		static const QString VALUE_EXECUTION_PAUSE;
		static const QString VALUE_TIME_STEP_SIZE;

		static const QString VALUE_INCREMENTAL_LOGGER_COMMENT;
		static const QString VALUE_RUN_SIMULATION_IN_REALTIME;

	//**************************************************************************
	//Tag Names
	//**************************************************************************

		static const QString TAG_EVO_CREATION_DATE;
		static const QString TAG_GENOME_SIGNIFICANT_CHANGE;
		static const QString TAG_GENOME_CHANGE_SUMMARY;
		static const QString TAG_NETWORK_MUTATION_HISTORY;
		
	};

}

#endif


