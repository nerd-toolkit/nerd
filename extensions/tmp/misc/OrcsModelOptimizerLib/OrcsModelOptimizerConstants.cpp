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

#include "OrcsModelOptimizerConstants.h"

using namespace nerd;

//////////////////////////////////////////////////
// Configuration Settings
//////////////////////////////////////////////////    

const QString OrcsModelOptimizerConstants::CONFIG_FOLDER
    = "./config/";

const QString OrcsModelOptimizerConstants::FILE_CONFIG_DESCRIPTION
    = "ConfigDescription.xml";

const QString OrcsModelOptimizerConstants::FILE_MOTION_CONFIG_DESCRIPTION
    = "MotionConfigDescription.xml";

const QString OrcsModelOptimizerConstants::FILE_PHYSICALDATASOURCE_CONFIG_DESCRIPTION
    = "PhysicalDataSourceConfigDescription.xml";
    
const QString OrcsModelOptimizerConstants::FILE_PHYSICALDATAPLAYER_CONFIG_DESCRIPTION
		= "PhysicalDataPlayerConfigDescription.xml";
    
const QString OrcsModelOptimizerConstants::FILE_ENDING_MOTION_DESCRIPTION_FILE
    = ".md";

const QString OrcsModelOptimizerConstants::FILE_ENDING_VALUE_PARAMETER
    = ".val";
    
const QString OrcsModelOptimizerConstants::FILE_ENDING_MOTION_CONFIG
    = ".config";
    
const QString OrcsModelOptimizerConstants::FILE_ENDING_ENVIRONMENT
    = ".xml";

const int OrcsModelOptimizerConstants::STANDARD_MAX_MOTION_TRIGGER_ONSET
    = 60000;
    
const QString OrcsModelOptimizerConstants::TEMPLATE_MOTION_PATH
    = "%MOTION_PATH";
    
const QString OrcsModelOptimizerConstants::TEMPLATE_PLAYER_PATH
		= "%PLAYER_PATH";
      

//////////////////////////////////////////////////
// ValueManger Value names
//////////////////////////////////////////////////

const QString OrcsModelOptimizerConstants::VM_BASE_PATH
    = "/ModelOptimization/";

const QString OrcsModelOptimizerConstants::VM_INPUTDATA_PATH
    = "InputData/";

const QString OrcsModelOptimizerConstants::VM_MOTION_LIST
    = "Motion_%1/";
    
const QString OrcsModelOptimizerConstants::VM_OPTIMIZER_PATH
    = "Optimizer/";

const QString OrcsModelOptimizerConstants::VM_MODEL_LIST
    = "Model_%1/";
    
const QString OrcsModelOptimizerConstants::VM_PARAMETER_LIST
    = "Parameter_%1/";

const QString OrcsModelOptimizerConstants::VM_TRAINDATA_PATH
    = "TrainData/";

const QString OrcsModelOptimizerConstants::VM_VALUE_LIST
    = "Value_%1/";
    
const QString OrcsModelOptimizerConstants::VM_TRIGGER_PATH
    = "Trigger/";

const QString OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH
    = "Normalization/";

const QString OrcsModelOptimizerConstants::VM_SIMULATIONDATA_PATH
    = "SimulationData/";

const QString OrcsModelOptimizerConstants::VM_PHYSICALDATA_PATH
    = "PhysicalData/";

const QString OrcsModelOptimizerConstants::VM_FILENAME_LIST
    = "File_%1";

const QString OrcsModelOptimizerConstants::VM_MODELVALUES_PATH
    = "ModelValues/";

const QString OrcsModelOptimizerConstants::VM_INITIALVALUES_PATH
    = "InitialValues/";
    
const QString OrcsModelOptimizerConstants::VM_PLAYER_PATH
		= "Player/";

const QString OrcsModelOptimizerConstants::VM_CHANNELS_PATH
		= "Channels/";

const QString OrcsModelOptimizerConstants::VM_CHANNEL_LIST
		= "Channel_%1/";

const QString OrcsModelOptimizerConstants::VM_NORMALIZATIONGROUP_LIST
		= "NormalizationGroup_%1/";



const QString OrcsModelOptimizerConstants::VM_NAME_VALUE
    = "Name";

const QString OrcsModelOptimizerConstants::VM_AGENT_VALUE
    = "Agent";

const QString OrcsModelOptimizerConstants::VM_VERSION_VALUE
    = "Version";

const QString OrcsModelOptimizerConstants::VM_FORMAT_VALUE
    = "Format";

const QString OrcsModelOptimizerConstants::VM_SCENARIO_VALUE
    = "Scenario";
    
const QString OrcsModelOptimizerConstants::VM_LENGTH_VALUE
    = "Length";
    
const QString OrcsModelOptimizerConstants::VM_VALUE_VALUE
    = "Value";

const QString OrcsModelOptimizerConstants::VM_MODUS_VALUE
    = "Modus";

const QString OrcsModelOptimizerConstants::VM_PARAMETER_VALUE
    = "Parameter";
    
const QString OrcsModelOptimizerConstants::VM_ORGMIN_VALUE
    = "OriginalMin";
    
const QString OrcsModelOptimizerConstants::VM_ORGMAX_VALUE
    = "OriginalMax";
    
const QString OrcsModelOptimizerConstants::VM_NORMMIN_VALUE
    = "NormalizedMin";

const QString OrcsModelOptimizerConstants::VM_NORMMAX_VALUE
    = "NormalizedMax" ;

const QString OrcsModelOptimizerConstants::VM_SOURCETYPE_VALUE
    = "SourceType" ;

const QString OrcsModelOptimizerConstants::VM_START_VALUE
    = "Start";

const QString OrcsModelOptimizerConstants::VM_END_VALUE
    = "End";

const QString OrcsModelOptimizerConstants::VM_STEP_VALUE
    = "Step";
    
const QString OrcsModelOptimizerConstants::VM_EXCLUDE_VALUE
    = "Exclude";    
    
const QString OrcsModelOptimizerConstants::VM_OUTPUTDIR_VALUE
    = "OutputDirectory";  
    
const QString OrcsModelOptimizerConstants::VM_FILE_VALUE
    = "File";  
    
const QString OrcsModelOptimizerConstants::VM_PARAMETERVALUETYPE_VALUE
    = "ParameterValueType";  
    
const QString OrcsModelOptimizerConstants::VM_MAXITERATIONS_VALUE
    = "MaxIterations";  
    
const QString OrcsModelOptimizerConstants::VM_ONEDIMMAXITERATIONS_VALUE
    = "OneDimMaxIterations";  
    
const QString OrcsModelOptimizerConstants::VM_MULTIDIMTOLERANCE_VALUE
    = "MultiDimTolerance";  
    
const QString OrcsModelOptimizerConstants::VM_ONEDIMTOLERANCE_VALUE
    = "OneDimTolerance";  
    
const QString OrcsModelOptimizerConstants::VM_MAXSIMULATIONRUNS_VALUE
    = "MaxSimulationRuns";  
    
const QString OrcsModelOptimizerConstants::VM_PRINTTRAINDATASTATISTICS_VALUE
    = "PrintTraindataStatistics";  
    
const QString OrcsModelOptimizerConstants::VM_TOLERANCE_VALUE
    = "Tolerance";  
    
const QString OrcsModelOptimizerConstants::VM_TIMETOLERANCE_VALUE
    = "TimeTolerance";  
    
const QString OrcsModelOptimizerConstants::VM_MIN_VALUE
		= "Min";
		    
const QString OrcsModelOptimizerConstants::VM_MAX_VALUE
		= "Max";
		
const QString OrcsModelOptimizerConstants::VM_INIT_VALUE
		= "Init";
    
const QString OrcsModelOptimizerConstants::VM_MU_VALUE
		= "Mu";
    
const QString OrcsModelOptimizerConstants::VM_RHO_VALUE
		= "Rho";
		
const QString OrcsModelOptimizerConstants::VM_LAMBDA_VALUE
		= "Lambda";

const QString OrcsModelOptimizerConstants::VM_SELECTION_VALUE
		= "SelectionType";
 
const QString OrcsModelOptimizerConstants::VM_STRATPARARECOMB_VALUE
		= "StrategyParaRecombinationType";

const QString OrcsModelOptimizerConstants::VM_OBJPARARECOMB_VALUE
		= "ObjectParaRecombinationType";
		    
const QString OrcsModelOptimizerConstants::VM_PARAMUTATION_VALUE
		= "ParameterMutationsType";

const QString OrcsModelOptimizerConstants::VM_LEARNRATEKOEFF_VALUE
		= "LearnRateKoeff";
		
const QString OrcsModelOptimizerConstants::VM_CHECKINTERVAL_VALUE
		= "CheckInterval";
		
const QString OrcsModelOptimizerConstants::VM_INITSTRATPARA_VALUE
		= "InitStrategyParameter"; 
		   
const QString OrcsModelOptimizerConstants::VM_NUMBEROFRESULTSFILES_VALUE
		= "NumberOfResultFiles"; 
		   
const QString OrcsModelOptimizerConstants::VM_TYPE_VALUE
		= "Type"; 
		
const QString OrcsModelOptimizerConstants::VM_PLAYERSOURCE_VALUE
		= "PlayerSource"; 
		
const QString OrcsModelOptimizerConstants::VM_CONFIGFILE_VALUE
		= "ConfigFile"; 
		
const QString OrcsModelOptimizerConstants::VM_PLAYEVENT_VALUE
		= "PlayEvent"; 
		
const QString OrcsModelOptimizerConstants::VM_VALUEFILEDIRECTORY_VALUE
		= "ValueFileDirectory"; 
		
const QString OrcsModelOptimizerConstants::VM_SAVEMOTIONCOURSE_VALUE
		= "SaveMotionCourse";
    
    
   
const QString OrcsModelOptimizerConstants::VM_REPORT_FILEPATH_VALUE
    = "ReportFilePath";  
        
const QString OrcsModelOptimizerConstants::VM_REPORT_MESSAGE_VALUE
    = "ReportMessage";     

    
const QString OrcsModelOptimizerConstants::VM_RUNOPTIMIZATION_VALUE
    = OrcsModelOptimizerConstants::VM_BASE_PATH + "RunOptimization";
    
const QString OrcsModelOptimizerConstants::VM_PAUSEOPTIMIZATION_VALUE
    = OrcsModelOptimizerConstants::VM_BASE_PATH + "PauseOptimization";
    
const QString OrcsModelOptimizerConstants::VM_SHUTDOWNAFTEROPTIMIZATION_VALUE
    = OrcsModelOptimizerConstants::VM_BASE_PATH + "ShutDownAfterOptimization";


//////////////////////////////////////////////////
// ModelOptimizer names
//////////////////////////////////////////////////

const QString OrcsModelOptimizerConstants::MO_TEST_OPTIMIZER
    = "Test";    

const QString OrcsModelOptimizerConstants::MO_PRINT_OPTIMIZER
    = "Print";
 
const QString OrcsModelOptimizerConstants::MO_POWELL_OPTIMIZER
    = "Powell"; 
    
const QString OrcsModelOptimizerConstants::MO_CHECKDATASOURCEINTEGRITY
    = "CheckDataSourceIntegrity";     
 
const QString OrcsModelOptimizerConstants::MO_MUSLASHRHOLAMBDAES
		= "MuSlashRhoLambdaEs";  
		   
const QString OrcsModelOptimizerConstants::MO_CALCERRORFUNCTION
		= "CalcErrorFunction";
 
//////////////////////////////////////////////////
// PhysicalDataSource Description Elements
//////////////////////////////////////////////////

const QString OrcsModelOptimizerConstants::PDS_DESCR_BASE_ELEMENT
    = "physicalDataSourceDescription";

const QString OrcsModelOptimizerConstants::PDS_DESCR_NAME_VALUE
    = "name";    
     
const QString OrcsModelOptimizerConstants::PDS_DESCR_DATAFORMAT_ELEMENT
    = "dataFormat";    
     
const QString OrcsModelOptimizerConstants::PDS_DESCR_TIMEPOS_VALUE
    = "timePos";    

const QString OrcsModelOptimizerConstants::PDS_DESCR_FRAMELENGTH_VALUE
    = "frameLength";
    
const QString OrcsModelOptimizerConstants::PDS_DESCR_FRAMETIMELENGTH_VALUE
    = "frameTimeLength";    

const QString OrcsModelOptimizerConstants::PDS_DESCR_VALUE_ELEMENT
    = "value";    

const QString OrcsModelOptimizerConstants::PDS_DESCR_BINARYPOS_VALUE
    = "binPos";    

const QString OrcsModelOptimizerConstants::PDS_DESCR_COLUMNPOS_VALUE
    = "colPos";    

const QString OrcsModelOptimizerConstants::PDS_DESCR_ORCSNAME_VALUE
    = "orcs";    

const QString OrcsModelOptimizerConstants::PDS_DESCR_SYNCVALUE_VALUE
    = "syncValue";
      
const QString OrcsModelOptimizerConstants::PDS_DESCR_TIMECHANGEINTERVALL_VALUE
    = "timeChangeIntervall";

//////////////////////////////////////////////////
// PhysicalDataSource names
//////////////////////////////////////////////////

const QString OrcsModelOptimizerConstants::PDS_BINARY
    = "Binary";   
const QString OrcsModelOptimizerConstants::PDS_TEXT
    = "Text";   
    
    
//////////////////////////////////////////////////
// Event names
//////////////////////////////////////////////////

const QString OrcsModelOptimizerConstants::EVENT_MODELOPTIMIZATIONSTEP_COMPLETED
    = "/ModelOptimization/%1/ModelOptimizationStepCompleted";

const QString OrcsModelOptimizerConstants::EVENT_MODELOPTIMIZATION_COMPLETED
    = "/ModelOptimization/%1/ModelOptimizationCompleted";

const QString OrcsModelOptimizerConstants::EVENT_MODELOPTIMIZATION_STATISTIC_COMPLETED
    = "/ModelOptimization/%1/ModelOptimizationStatisticCompleted";
    
//////////////////////////////////////////////////
// Player names
//////////////////////////////////////////////////

const QString OrcsModelOptimizerConstants::PLAYER_KEYFRAME
		= "KeyFrame";

const QString OrcsModelOptimizerConstants::PLAYER_PHYSICALDATA
		= "PhysicalData";

