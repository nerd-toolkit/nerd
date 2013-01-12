/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
 *                                                                         *
 *   OrcsModelOptimizer by Chris Reinke (creinke@uni-osnabrueck.de         *
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


#ifndef OrcsModelOptimizerConstants_H
#define OrcsModelOptimizerConstants_H

#include <QString>

namespace nerd {

	/**
   * OrcsModelOptimizer Constants.
	 */
  class OrcsModelOptimizerConstants {
	public:

    //////////////////////////////////////////////////
    // Configuration Settings
    //////////////////////////////////////////////////
    
    static const QString CONFIG_FOLDER;
    static const QString FILE_CONFIG_DESCRIPTION;
    static const QString FILE_MOTION_CONFIG_DESCRIPTION;
    static const QString FILE_PHYSICALDATASOURCE_CONFIG_DESCRIPTION;
		static const QString FILE_PHYSICALDATAPLAYER_CONFIG_DESCRIPTION;
		static const QString FILE_ENDING_MOTION_DESCRIPTION_FILE;
    static const QString FILE_ENDING_VALUE_PARAMETER;  
    static const QString FILE_ENDING_MOTION_CONFIG;    
    static const QString FILE_ENDING_ENVIRONMENT;
    static const QString STANDARD_ENVIRONMENT_FILE;
    
    /**
     * Valid time before the trigger of a motion have to get started in milliseconds
     */
    static const int STANDARD_MAX_MOTION_TRIGGER_ONSET;
    
    static const QString TEMPLATE_MOTION_PATH;
		static const QString TEMPLATE_PLAYER_PATH;
    
    
    //////////////////////////////////////////////////
    // ValueManger Value names
    //////////////////////////////////////////////////
    
    static const QString VM_BASE_PATH;
    static const QString VM_INPUTDATA_PATH;
    static const QString VM_MOTION_LIST;
    static const QString VM_OPTIMIZER_PATH;
    static const QString VM_MODEL_LIST;
    static const QString VM_PARAMETER_LIST;
    static const QString VM_TRAINDATA_PATH;
    static const QString VM_VALUE_LIST;
    static const QString VM_TRIGGER_PATH;
    static const QString VM_NORMALIZATION_PATH;
    static const QString VM_SIMULATIONDATA_PATH;
    static const QString VM_PHYSICALDATA_PATH;
    static const QString VM_FILENAME_LIST;
    static const QString VM_MODELVALUES_PATH;
    static const QString VM_INITIALVALUES_PATH;
		static const QString VM_PLAYER_PATH;
		static const QString VM_CHANNELS_PATH;
		static const QString VM_CHANNEL_LIST;
		static const QString VM_NORMALIZATIONGROUP_LIST;
  
    static const QString VM_NAME_VALUE;
    static const QString VM_AGENT_VALUE;
    static const QString VM_VERSION_VALUE;
    static const QString VM_FORMAT_VALUE;
    static const QString VM_SCENARIO_VALUE;
    static const QString VM_LENGTH_VALUE;
    static const QString VM_VALUE_VALUE;
    static const QString VM_MODUS_VALUE;
    static const QString VM_PARAMETER_VALUE; 
    static const QString VM_ORGMIN_VALUE;
    static const QString VM_ORGMAX_VALUE;
    static const QString VM_NORMMIN_VALUE;
    static const QString VM_NORMMAX_VALUE;
    static const QString VM_SOURCETYPE_VALUE;
    static const QString VM_START_VALUE;
    static const QString VM_END_VALUE;
    static const QString VM_STEP_VALUE;
    static const QString VM_EXCLUDE_VALUE;
    static const QString VM_OUTPUTDIR_VALUE;
    static const QString VM_FILE_VALUE;
    static const QString VM_PARAMETERVALUETYPE_VALUE;    
    static const QString VM_MAXITERATIONS_VALUE;
    static const QString VM_ONEDIMMAXITERATIONS_VALUE;
    static const QString VM_MULTIDIMTOLERANCE_VALUE;
    static const QString VM_ONEDIMTOLERANCE_VALUE;
    static const QString VM_MAXSIMULATIONRUNS_VALUE;
    static const QString VM_PRINTTRAINDATASTATISTICS_VALUE;
    static const QString VM_TOLERANCE_VALUE;
    static const QString VM_TIMETOLERANCE_VALUE;
		static const QString VM_MIN_VALUE;
		static const QString VM_MAX_VALUE;
		static const QString VM_INIT_VALUE;
		static const QString VM_MU_VALUE;
		static const QString VM_RHO_VALUE;
		static const QString VM_LAMBDA_VALUE;
		static const QString VM_SELECTION_VALUE;
		static const QString VM_STRATPARARECOMB_VALUE;
		static const QString VM_OBJPARARECOMB_VALUE;
		static const QString VM_PARAMUTATION_VALUE;
		static const QString VM_LEARNRATEKOEFF_VALUE;
		static const QString VM_CHECKINTERVAL_VALUE;
		static const QString VM_INITSTRATPARA_VALUE;
		static const QString VM_NUMBEROFRESULTSFILES_VALUE;
		static const QString VM_TYPE_VALUE;
		static const QString VM_PLAYERSOURCE_VALUE;		
		static const QString VM_CONFIGFILE_VALUE;	
		static const QString VM_PLAYEVENT_VALUE;
		static const QString VM_VALUEFILEDIRECTORY_VALUE;	
		static const QString VM_SAVEMOTIONCOURSE_VALUE;
    
    static const QString VM_REPORT_FILEPATH_VALUE;
    static const QString VM_REPORT_MESSAGE_VALUE;
 
    static const QString VM_RUNOPTIMIZATION_VALUE;
    static const QString VM_PAUSEOPTIMIZATION_VALUE;
    static const QString VM_SHUTDOWNAFTEROPTIMIZATION_VALUE;
    
       
    //////////////////////////////////////////////////
    // ModelOptimizer names
    //////////////////////////////////////////////////

    static const QString MO_TEST_OPTIMIZER;
    static const QString MO_PRINT_OPTIMIZER;
    static const QString MO_POWELL_OPTIMIZER;
    static const QString MO_CHECKDATASOURCEINTEGRITY;
		static const QString MO_MUSLASHRHOLAMBDAES;
		static const QString MO_CALCERRORFUNCTION;
    
    //////////////////////////////////////////////////
    // PhysicalDataSource Description Elements
    //////////////////////////////////////////////////

    static const QString PDS_DESCR_NAME_VALUE;       
    static const QString PDS_DESCR_DATAFORMAT_ELEMENT;       
    static const QString PDS_DESCR_TIMEPOS_VALUE;    
    static const QString PDS_DESCR_FRAMELENGTH_VALUE;    
    static const QString PDS_DESCR_FRAMETIMELENGTH_VALUE;    
    static const QString PDS_DESCR_VALUE_ELEMENT;
    static const QString PDS_DESCR_BINARYPOS_VALUE;
    static const QString PDS_DESCR_COLUMNPOS_VALUE;
    static const QString PDS_DESCR_ORCSNAME_VALUE;  
    static const QString PDS_DESCR_SYNCVALUE_VALUE;  
    static const QString PDS_DESCR_TIMECHANGEINTERVALL_VALUE;  
    static const QString PDS_DESCR_BASE_ELEMENT;
    
    
    //////////////////////////////////////////////////
    // PhysicalDataSource names
    //////////////////////////////////////////////////

    static const QString PDS_BINARY;    
	static const QString PDS_TEXT;
    
    //////////////////////////////////////////////////
    // Event names
    //////////////////////////////////////////////////

    static const QString EVENT_MODELOPTIMIZATIONSTEP_COMPLETED;
    
    static const QString EVENT_MODELOPTIMIZATION_COMPLETED;
    
    static const QString EVENT_MODELOPTIMIZATION_STATISTIC_COMPLETED;
      
		
		//////////////////////////////////////////////////
    // Player names
		//////////////////////////////////////////////////
		
		static const QString PLAYER_KEYFRAME;
		
		static const QString PLAYER_PHYSICALDATA;
	};

}

#endif


