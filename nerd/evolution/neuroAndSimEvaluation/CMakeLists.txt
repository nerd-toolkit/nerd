cmake_minimum_required(VERSION 2.6)
project(nerd_neuroAndSimEvaluation)

set(nerd_neuroAndSimEvaluation_SRCS
	Application/NeuroAndSimEvaluationBaseApplication.cpp  
	Application/NeuroAndSimEvaluationStandardGuiApplication.cpp  
	Evaluation/LocalNetworkInSimulationEvaluationMethod.cpp  
	ClusterEvaluation/ClusterNetworkInSimEvaluationMethod.cpp  
	Physics/EvolvableParameter.cpp  
	Collections/EvolutionSimulationPrototypes.cpp  
	Control/NetworkAgentControlParser.cpp  
	Gui/SimpleEvolutionMainWindow/SimpleEvolutionMainWindow.cpp
	ClusterEvaluation/MultiCoreNetworkInSimulationEvaluationMethod.cpp
	Gui/NetworkSimulationRecorder/NetworkSimulationRecorder.cpp
)

set(nerd_neuroAndSimEvaluation_MOC_HDRS
	Application/NeuroAndSimEvaluationStandardGuiApplication.h  
	Gui/SimpleEvolutionMainWindow/SimpleEvolutionMainWindow.h
)

set(nerd_neuroAndSimEvaluation_RCS
)

set(nerd_neuroAndSimEvaluation_UIS
)


#include QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
include(${QT_USE_FILE})


#Execute QT Stuff
QT4_WRAP_CPP(nerd_neuroAndSimEvaluation_MOC_SRCS ${nerd_neuroAndSimEvaluation_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_neuroAndSimEvaluation_RC_SRCS ${nerd_neuroAndSimEvaluation_RCS})
QT4_WRAP_UI(nerd_neuroAndSimEvaluation_UI_HDRS ${nerd_neuroAndSimEvaluation_UIS})



#Create library.
add_library(neuroAndSimEvaluation STATIC ${nerd_neuroAndSimEvaluation_SRCS} ${nerd_neuroAndSimEvaluation_MOC_SRCS} ${nerd_neuroAndSimEvaluation_RC_SRCS} ${nerd_neuroAndSimEvaluation_UI_HDRS})


add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/evolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/neuroEvolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/simulator)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/neuralNetwork)

add_dependencies(neuroAndSimEvaluation nerd evolution neuroEvolution simulator neuralNetwork networkEditor)

