cmake_minimum_required(VERSION 2.6)
project(nerd_evolution)

set(nerd_evolution_SRCS
	Evolution/Individual.cpp  
	Evolution/Population.cpp  
	SelectionMethod/SelectionMethod.cpp  
	Phenotype/GenotypePhenotypeMapper.cpp  
	Evolution/World.cpp  
	Evolution/EvolutionAlgorithm.cpp  
	Evolution/EvolutionManager.cpp  
	EvolutionConstants.cpp  
	Evolution/Evolution.cpp  
	Phenotype/IdentityGenotypePhenotypeMapper.cpp  
	SelectionMethod/TournamentSelectionMethod.cpp  
	Execution/EvolutionRunner.cpp  
	Collections/LoadAndStoreValueManagerPerGeneration.cpp  
	Math/IndividualSorter.cpp  
	Logging/SettingsLogger.cpp  
	Gui/PopulationOverview/PopulationOverviewWidget.cpp  
	Gui/PopulationOverview/MultiplePopulationOverviewWindowWidget.cpp  
	Statistics/IndividualStatistics/IndividualStatistic.cpp  
	Statistics/IndividualStatistics/IndividualAge.cpp  
	Collections/StandardIndividualStatistics.cpp  
	Collections/FitnessPrototypes.cpp  
	Fitness/ControllerFitnessFunction.cpp  
	Fitness/Fitness.cpp  
	Fitness/FitnessFunction.cpp  
	Fitness/FitnessManager.cpp  
	FitnessFunctions/OptimizeValueFitness.cpp  
	FitnessFunctions/SurvivalTimeFitnessFunction.cpp  
	Gui/FitnessPlotter/OnlineFitnessPlotter.cpp  
	PlugIns/ControllerFitnessFunctionParser.cpp  
	PlugIns/FitnessLogger.cpp  
	FitnessFunctions/MinimizeErrorFitness.cpp  
	SelectionMethod/PoissonDistributionRanking.cpp  
	Statistics/IndividualStatistics/OffspringCountStatistic.cpp  
	PlugIns/WorkspaceCleaner.cpp  
	PlugIns/EvolutionTerminationTrigger.cpp  
	SelectionMethod/StochasticUniversalSamplingSelection.cpp  
	SelectionMethod/StructureNiechingSelection.cpp  
	Gui/EvolutionParameterPanel/EvolutionParameterPanel.cpp  
	Gui/EvolutionParameterPanel/EvolutionOperatorPanel.cpp  
	Collections/EvolutionPropertyPanelCollection.cpp  
	Gui/EvolutionParameterPanel/AlgorithmParameterPanel.cpp  
	Gui/EvolutionParameterPanel/ParameterControlWidget.cpp  
	Gui/EvolutionParameterPanel/EvolutionMainControlParameterPanel.cpp  
	Gui/ScriptedFitnessEditor/ScriptedFitnessEditor.cpp  
	Gui/ScriptedFitnessEditor/MultipleScriptedFitnessEditor.cpp  
	SelectionMethod/MultiObjectiveTournamentSelection.cpp  
	Execution/EvaluationLoopExecutor.cpp  
	Evaluation/EvaluationGroupsBuilder.cpp  
	Evaluation/EvaluationLoop.cpp  
	Evaluation/EvaluationMethod.cpp  
	Evaluation/SimpleEvaluationGroupsBuilder.cpp  
	Evaluation/SinglePopulationGroupsBuilder.cpp  
	ClusterEvaluation/ClusterEvaluationMethod.cpp  
	Application/EvaluationBaseApplication.cpp  
	FitnessFunctions/ScriptedFitnessFunction.cpp  
	Gui/EvolutionProgressBar/EvolutionProgressBar.cpp
)

set(nerd_evolution_MOC_HDRS
	Execution/EvolutionRunner.h  
	Gui/PopulationOverview/PopulationOverviewWidget.h  
	Gui/PopulationOverview/MultiplePopulationOverviewWindowWidget.h  
	Gui/FitnessPlotter/OnlineFitnessPlotter.h  
	Gui/EvolutionParameterPanel/EvolutionParameterPanel.h  
	Gui/EvolutionParameterPanel/EvolutionOperatorPanel.h  
	Gui/EvolutionParameterPanel/AlgorithmParameterPanel.h  
	Gui/EvolutionParameterPanel/ParameterControlWidget.h  
	Gui/EvolutionParameterPanel/EvolutionMainControlParameterPanel.h  
	Gui/ScriptedFitnessEditor/ScriptedFitnessEditor.h  
	Gui/ScriptedFitnessEditor/MultipleScriptedFitnessEditor.h  
	Execution/EvaluationLoopExecutor.h  
	ClusterEvaluation/ClusterEvaluationMethod.h  
	Gui/EvolutionProgressBar/EvolutionProgressBar.h
	FitnessFunctions/ScriptedFitnessFunction.h
)

set(nerd_evolution_RCS
)

set(nerd_evolution_UIS
)


#include QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTSCRIPT TRUE)
include(${QT_USE_FILE})


#Execute QT Stuff
QT4_WRAP_CPP(nerd_evolution_MOC_SRCS ${nerd_evolution_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_evolution_RC_SRCS ${nerd_evolution_RCS})
QT4_WRAP_UI(nerd_evolution_UI_HDRS ${nerd_evolution_UIS})



#Create library.
add_library(evolution STATIC ${nerd_evolution_SRCS} ${nerd_evolution_MOC_SRCS} ${nerd_evolution_RC_SRCS} ${nerd_evolution_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)

