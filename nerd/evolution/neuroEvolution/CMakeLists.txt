cmake_minimum_required(VERSION 2.6)
project(nerd_neuroEvolution)

set(nerd_neuroEvolution_SRCS
	NeuralNetworkManipulationChain/NeuralNetworkManipulationChainAlgorithm.cpp  
	NeuralNetworkManipulationChain/NeuralNetworkManipulationOperator.cpp  
	NeuralNetworkManipulationChain/CreateNetworkOperator.cpp  
	NeuralNetworkManipulationChain/CloneNetworkOperator.cpp  
	Collections/ENS3EvolutionAlgorithm.cpp  
	NeuralNetworkManipulationChain/InsertNeuronOperator.cpp  
	NeuralNetworkManipulationChain/InsertSynapseOperator.cpp  
	NeuralNetworkManipulationChain/InitializeSynapsesOperator.cpp  
	NeuralNetworkManipulationChain/InitializeBiasOperator.cpp  
	NeuralNetworkManipulationChain/ChangeBiasOperator.cpp  
	NeuralNetworkManipulationChain/ChangeSynapseStrengthOperator.cpp  
	NeuralNetworkManipulationChain/RemoveNeuronOperator.cpp  
	NeuralNetworkManipulationChain/RemoveSynapseOperator.cpp  
	Statistics/BasicNeuralNetworkStatistics.cpp  
	Statistics/IndividualStatistics/HiddenNeuronStatistic.cpp  
	NeuralNetworkManipulationChain/ResolveConstraintsOperator.cpp  
	Collections/NeuralNetworkIndividualStatistics.cpp  
	NeuralNetworkManipulationChain/OperatorSorter.cpp  
	PlugIns/SaveBestNetworksHandler.cpp  
	NeuralNetworkManipulationChain/ConnectNeuronClassesFilter.cpp  
	NeuralNetworkManipulationChain/EnableSynapseOperator.cpp  
	Collections/NeuroEvolutionSelector.cpp  
	Neat/NeatEvolutionAlgorithm.cpp  
	Neat/NeatGenome.cpp  
	Neat/NeatGenotypePhenotypeMapper.cpp  
	Neat/NeatSelectionMethod.cpp  
	Neat/NeatConstants.cpp  
	Collections/NeatAlgorithm.cpp  
	NeuroEvolutionConstants.cpp  
	NeuralNetworkManipulationChain/ModuleCrossOverOperator.cpp  
	Collections/ModularNeuroEvolution1.cpp  
	NeuralNetworkManipulationChain/InsertSynapseModularOperator.cpp  
	NeuralNetworkManipulationChain/InsertNeuroModuleOperator.cpp  
	Statistics/IndividualStatistics/NetworkStructureStatistics.cpp  
	NeuralNetworkManipulationChain/InsertBiasOperator.cpp  
	NeuralNetworkManipulationChain/RemoveBiasOperator.cpp  
	Collections/NeuroFitnessPrototypes.cpp  
	FitnessFunctions/ScriptedNeuroFitnessFunction.cpp
	Util/NeuroEvolutionUtil.cpp
	NeuralNetworkManipulationChain/AdaptSRNParametersOperator.cpp
)

set(nerd_neuroEvolution_MOC_HDRS
	FitnessFunctions/ScriptedNeuroFitnessFunction.h
)

set(nerd_neuroEvolution_RCS
)

set(nerd_neuroEvolution_UIS
)


#include QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})


#Execute QT Stuff
QT4_WRAP_CPP(nerd_neuroEvolution_MOC_SRCS ${nerd_neuroEvolution_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_neuroEvolution_RC_SRCS ${nerd_neuroEvolution_RCS})
QT4_WRAP_UI(nerd_neuroEvolution_UI_HDRS ${nerd_neuroEvolution_UIS})



#Create library.
add_library(neuroEvolution STATIC ${nerd_neuroEvolution_SRCS} ${nerd_neuroEvolution_MOC_SRCS} ${nerd_neuroEvolution_RC_SRCS} ${nerd_neuroEvolution_UI_HDRS})


add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/evolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/networkEditor)

add_dependencies(neuroEvolution nerd neuralNetwork evolution networkEditor)
