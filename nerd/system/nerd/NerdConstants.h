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


#ifndef NERDNerdConstants_H
#define NERDNerdConstants_H

#include <QString>

namespace nerd {

	/**
	 * NerdConstants.
	 */
	class NerdConstants {
	public:

	//**************************************************************************
	//Directory and File Names
	//**************************************************************************

		static const QString NERD_CONFIG_DIRECTORY;

	//**************************************************************************
	//Object Names
	//**************************************************************************

		static const QString OBJECT_STATISTICS_MANAGER;
		static const QString OBJECT_GUI_MANAGER;

	//**************************************************************************
	//GUI Element Names
	//**************************************************************************

		static const QString GUI_SELECT_FILE_FOR_STRING_VALUE;
		static const QString GUI_MAIN_SIMULATION_WINDOW;

	//**************************************************************************
	//Value Names
	//**************************************************************************

// 		static const QString VALUE_NERD_SYSTEM_PAUSE;
		static const QString VALUE_NERD_RECENT_LOGGER_MESSAGE;
		static const QString VALUE_NERD_REPOSITORY_CHANGED_COUNTER;
		static const QString VALUE_NERD_FRAMES_PER_SECOND;
		static const QString VALUE_RUN_IN_PERFORMANCE_MODE;
		static const QString VALUE_EXECUTION_PAUSE;
		static const QString VALUE_RANDOMIZATION_SIMULATION_SEED;
		static const QString VALUE_NNM_BYPASS_NETWORKS;

	//**************************************************************************
	//Event Names
	//**************************************************************************

		static const QString EVENT_NERD_SYSTEM_INIT;
		static const QString EVENT_NERD_SYSTEM_INIT_COMPLETED;
		static const QString EVENT_NERD_SYSTEM_BIND;
		static const QString EVENT_NERD_SYSTEM_SHUTDOWN;
		static const QString EVENT_NERD_EXECUTE_PENDING_TASKS;

		static const QString EVENT_EXECUTION_NEXT_STEP;
		static const QString EVENT_EXECUTION_STEP_COMPLETED;
		static const QString EVENT_EXECUTION_RESET;
		static const QString EVENT_EXECUTION_RESET_COMPLETED;
		static const QString EVENT_EXECUTION_RESET_SETTINGS;
		static const QString EVENT_EXECUTION_RESET_SETTINGS_COMPLETED;
		static const QString EVENT_EXECUTION_NEXT_TRY;
		static const QString EVENT_EXECUTION_TRY_COMPLETED;
		static const QString EVENT_EXECUTION_NEXT_INDIVIDUAL;
		static const QString EVENT_EXECUTION_INDIVIDUAL_COMPLETED;
		static const QString EVENT_EXECUTION_GENERATION_STARTED;
		static const QString EVENT_EXECUTION_GENERATION_COMPLETED;

		static const QString EVENT_PHYSICS_ENVIRONMENT_CHANGED;

		static const QString EVENT_VALUE_REPOSITORY_CHANGED;

		static const QString EVENT_GENERATION_STATISTICS_CALCULATED;
		static const QString EVENT_INDIVIDUAL_STATISTICS_CALCULATED;
		static const QString EVENT_TRY_STATISTICS_CALCULATED;
		static const QString EVENT_STEP_STATISTICS_CALCULATED;

		static const QString EVENT_NNM_NETWORK_ITERATION_COMPLETED;
		
	};

}

#endif

