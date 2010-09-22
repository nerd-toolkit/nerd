TEMPLATE = lib

CONFIG += staticlib \
rtti \
 debug \
 console
CONFIG -= release

DESTDIR = ../../bin


SOURCES += Evolution/Individual.cpp \
Evolution/Population.cpp \
SelectionMethod/SelectionMethod.cpp \
 Phenotype/GenotypePhenotypeMapper.cpp \
 Evolution/World.cpp \
 Evolution/EvolutionAlgorithm.cpp \
 Evolution/EvolutionManager.cpp \
 EvolutionConstants.cpp \
 Evolution/Evolution.cpp \
 Phenotype/IdentityGenotypePhenotypeMapper.cpp \
 SelectionMethod/TournamentSelectionMethod.cpp \
 Execution/EvolutionRunner.cpp \
 Collections/LoadAndStoreValueManagerPerGeneration.cpp \
 Math/IndividualSorter.cpp \
 Logging/SettingsLogger.cpp \
 Gui/PopulationOverview/PopulationOverviewWidget.cpp \
 Gui/PopulationOverview/MultiplePopulationOverviewWindowWidget.cpp \
 Statistics/IndividualStatistics/IndividualStatistic.cpp \
 Statistics/IndividualStatistics/IndividualAge.cpp \
 Collections/StandardIndividualStatistics.cpp \
 Collections/FitnessPrototypes.cpp \
 Fitness/ControllerFitnessFunction.cpp \
 Fitness/Fitness.cpp \
 Fitness/FitnessFunction.cpp \
 Fitness/FitnessManager.cpp \
 FitnessFunctions/OptimizeValueFitness.cpp \
 FitnessFunctions/SurvivalTimeFitnessFunction.cpp \
 Gui/FitnessPlotter/OnlineFitnessPlotter.cpp \
 PlugIns/ControllerFitnessFunctionParser.cpp \
 PlugIns/FitnessLogger.cpp \
 FitnessFunctions/MinimizeErrorFitness.cpp \
 SelectionMethod/PoissonDistributionRanking.cpp \
 Statistics/IndividualStatistics/OffspringCountStatistic.cpp \
 PlugIns/WorkspaceCleaner.cpp \
 PlugIns/EvolutionTerminationTrigger.cpp \
 SelectionMethod/StochasticUniversalSamplingSelection.cpp \
 SelectionMethod/StructureNiechingSelection.cpp \
 Gui/EvolutionParameterPanel/EvolutionParameterPanel.cpp \
 Gui/EvolutionParameterPanel/EvolutionOperatorPanel.cpp \
 Collections/EvolutionPropertyPanelCollection.cpp \
 Gui/EvolutionParameterPanel/AlgorithmParameterPanel.cpp \
 Gui/EvolutionParameterPanel/ParameterControlWidget.cpp \
 Gui/EvolutionParameterPanel/EvolutionMainControlParameterPanel.cpp \
 Gui/ScriptedFitnessEditor/ScriptedFitnessEditor.cpp \
 Gui/ScriptedFitnessEditor/MultipleScriptedFitnessEditor.cpp \
 SelectionMethod/MultiObjectiveTournamentSelection.cpp \
 Execution/EvaluationLoopExecutor.cpp \
 Evaluation/EvaluationGroupsBuilder.cpp \
 Evaluation/EvaluationLoop.cpp \
 Evaluation/EvaluationMethod.cpp \
 Evaluation/SimpleEvaluationGroupsBuilder.cpp \
 Evaluation/SinglePopulationGroupsBuilder.cpp \
 ClusterEvaluation/ClusterEvaluationMethod.cpp \
 Application/EvaluationBaseApplication.cpp \
 FitnessFunctions/ScriptedFitnessFunction.cpp \
 Gui/EvolutionProgressBar/EvolutionProgressBar.cpp
HEADERS += Evolution/Individual.h \
Evolution/Population.h \
SelectionMethod/SelectionMethod.h \
 Phenotype/GenotypePhenotypeMapper.h \
 Evolution/World.h \
 Evolution/EvolutionAlgorithm.h \
 Evolution/EvolutionManager.h \
 EvolutionConstants.h \
 Evolution/Evolution.h \
 Phenotype/IdentityGenotypePhenotypeMapper.h \
 SelectionMethod/TournamentSelectionMethod.h \
 Execution/EvolutionRunner.h \
 Collections/LoadAndStoreValueManagerPerGeneration.h \
 Math/IndividualSorter.h \
 Logging/SettingsLogger.h \
 Gui/PopulationOverview/PopulationOverviewWidget.h \
 Gui/PopulationOverview/MultiplePopulationOverviewWindowWidget.h \
 Statistics/IndividualStatistics/IndividualStatistic.h \
 Statistics/IndividualStatistics/IndividualAge.h \
 Collections/StandardIndividualStatistics.h \
 Collections/FitnessPrototypes.h \
 Fitness/ControllerFitnessFunction.h \
 Fitness/FitnessFunction.h \
 Fitness/Fitness.h \
 Fitness/FitnessManager.h \
 FitnessFunctions/OptimizeValueFitness.h \
 FitnessFunctions/SurvivalTimeFitnessFunction.h \
 Gui/FitnessPlotter/OnlineFitnessPlotter.h \
 PlugIns/ControllerFitnessFunctionParser.h \
 PlugIns/FitnessLogger.h \
 FitnessFunctions/MinimizeErrorFitness.h \
 SelectionMethod/PoissonDistributionRanking.h \
 Statistics/IndividualStatistics/OffspringCountStatistic.h \
 PlugIns/WorkspaceCleaner.h \
 PlugIns/EvolutionTerminationTrigger.h \
 SelectionMethod/StochasticUniversalSamplingSelection.h \
 SelectionMethod/StructureNiechingSelection.h \
 Gui/EvolutionParameterPanel/EvolutionParameterPanel.h \
 Gui/EvolutionParameterPanel/EvolutionOperatorPanel.h \
 Collections/EvolutionPropertyPanelCollection.h \
 Gui/EvolutionParameterPanel/AlgorithmParameterPanel.h \
 Gui/EvolutionParameterPanel/ParameterControlWidget.h \
 Gui/EvolutionParameterPanel/EvolutionMainControlParameterPanel.h \
 Gui/ScriptedFitnessEditor/ScriptedFitnessEditor.h \
 Gui/ScriptedFitnessEditor/MultipleScriptedFitnessEditor.h \
 SelectionMethod/MultiObjectiveTournamentSelection.h \
 Execution/EvaluationLoopExecutor.h \
 Evaluation/EvaluationGroupsBuilder.h \
 Evaluation/EvaluationLoop.h \
 Evaluation/EvaluationMethod.h \
 Evaluation/SimpleEvaluationGroupsBuilder.h \
 Evaluation/SinglePopulationGroupsBuilder.h \
 ClusterEvaluation/ClusterEvaluationMethod.h \
 Application/EvaluationBaseApplication.h \
 FitnessFunctions/ScriptedFitnessFunction.h \
 Gui/EvolutionProgressBar/EvolutionProgressBar.h



TARGETDEPS += ../../bin/libneuralNetwork.a

QT += script \
 xml

INCLUDEPATH += ../../system/nerd

