cmake_minimum_required(VERSION 2.6)
project(nerd_testEvolution)

set(nerd_testEvolution_SRCS
	main.cpp  
	Evolution/TestIndividual.cpp  
	Evolution/TestPopulation.cpp  
	SelectionMethod/SelectionMethodAdapter.cpp  
	Evolution/IndividualAdapter.cpp  
	Phenotype/GenotypePhenotypeMapperAdapter.cpp  
	Evolution/TestWorld.cpp  
	Evolution/EvolutionAlgorithmAdapter.cpp  
	Evolution/EvaluationMethodAdapter.cpp  
	Evolution/TestEvolutionManager.cpp  
	Evolution/WorldAdapter.cpp  
	TestConstants.cpp  
	Control/ControlInterfaceAdapter.cpp  
	Fitness/FitnessFunctionAdapter.cpp  
	Fitness/TestFitnessFunction.cpp  
	Fitness/TestFitnessManager.cpp  
	FitnessFunctions/TestScriptedFitnessFunction.cpp
)


set(nerd_testEvolution_MOC_HDRS
	Evolution/TestIndividual.h  
	Evolution/TestPopulation.h   
	Evolution/TestWorld.h  
	Evolution/TestEvolutionManager.h  
	TestConstants.h  
	Fitness/TestFitnessFunction.h  
	Fitness/TestFitnessManager.h  
	FitnessFunctions/TestScriptedFitnessFunction.h
)

set(nerd_testEvolution_RCS
)

set(nerd_testEvolution_UI_HDRS
)


#select QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTTEST TRUE)
set(QT_USE_QTSVG TRUE)
include(${QT_USE_FILE})


#QT Stuff
QT4_WRAP_CPP(nerd_testEvolution_MOC_SRCS ${nerd_testEvolution_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_testEvolution_RC_SRCS ${nerd_testEvolution_RCS})
QT4_WRAP_UI(nerd_testEvolution_UI_HDRS ${nerd_testEvolution_UIS})


#Create library.
add_executable(testEvolution ${nerd_testEvolution_SRCS} ${nerd_testEvolution_MOC_SRCS} ${nerd_testEvolution_RC_SRCS} ${nerd_testEvolution_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/evolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/simulator)

TARGET_LINK_LIBRARIES(testEvolution
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/evolution/libevolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/neuralNetwork/libneuralNetwork.a
	${CMAKE_CURRENT_BINARY_DIR}/../../simulator/simulator/libsimulator.a
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
	${QT_LIBRARIES}
)

add_dependencies(testEvolution evolution nerd neuralNetwork networkEditor simulator)
