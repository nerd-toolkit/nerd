cmake_minimum_required(VERSION 2.6)
project(nerd_testNeuroEvolution)

set(nerd_testNeuroEvolution_SRCS
	main.cpp  
	NeuralNetworkManipulationChain/TestNetworkManipulationChainAlgorithm.cpp  
	NeuralNetworkManipulationChain/NeuralNetworkManipulationOperatorAdapter.cpp  
	Evolution/WorldAdapter.cpp  
	Evolution/IndividualAdapter.cpp  
	TestNeuroEvolutionConstants.cpp
)


set(nerd_testNeuroEvolution_MOC_HDRS
	NeuralNetworkManipulationChain/TestNetworkManipulationChainAlgorithm.h  
	TestNeuroEvolutionConstants.h
)

set(nerd_testNeuroEvolution_RCS
)

set(nerd_testNeuroEvolution_UI_HDRS
)


#select QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTTEST TRUE)
set(QT_UDE_QTSCRIPT TRUE)
set(QT_USE_QTSVG TRUE)
include(${QT_USE_FILE})


#QT Stuff
QT4_WRAP_CPP(nerd_testNeuroEvolution_MOC_SRCS ${nerd_testNeuroEvolution_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_testNeuroEvolution_RC_SRCS ${nerd_testNeuroEvolution_RCS})
QT4_WRAP_UI(nerd_testNeuroEvolution_UI_HDRS ${nerd_testNeuroEvolution_UIS})


#Create library.
add_executable(testNeuroEvolution ${nerd_testNeuroEvolution_SRCS} ${nerd_testNeuroEvolution_MOC_SRCS} ${nerd_testNeuroEvolution_RC_SRCS} ${nerd_testNeuroEvolution_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/evolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/networkEditor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/neuroEvolution)

TARGET_LINK_LIBRARIES(testNeuroEvolution
	${QT_LIBRARIES}
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/neuroEvolution/libneuroEvolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/evolution/libevolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/networkEditor/libnetworkEditor.a
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/neuralNetwork/libneuralNetwork.a
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
)

