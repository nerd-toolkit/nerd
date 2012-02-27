cmake_minimum_required(VERSION 2.6)
project(nerd_NerdClusterNeuroEvo)

set(nerd_NerdClusterNeuroEvo_SRCS
	main.cpp
	NerdClusterNeuroEvoApplication.cpp
)


set(nerd_NerdClusterNeuroEvo_MOC_HDRS
	NerdClusterNeuroEvoApplication.h
)

set(nerd_NerdClusterNeuroEvo_RCS
)

set(nerd_NerdClusterNeuroEvo_UI_HDRS
)


#select QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTTEST TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTSVG TRUE)
include(${QT_USE_FILE})


#QT Stuff
QT4_WRAP_CPP(nerd_NerdClusterNeuroEvo_MOC_SRCS ${nerd_NerdClusterNeuroEvo_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_NerdClusterNeuroEvo_RC_SRCS ${nerd_NerdClusterNeuroEvo_RCS})
QT4_WRAP_UI(nerd_NerdClusterNeuroEvo_UI_HDRS ${nerd_NerdClusterNeuroEvo_UIS})


#Create library.
add_executable(NerdClusterNeuroEvo ${nerd_NerdClusterNeuroEvo_SRCS} ${nerd_NerdClusterNeuroEvo_MOC_SRCS} ${nerd_NerdClusterNeuroEvo_RC_SRCS} ${nerd_NerdClusterNeuroEvo_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/networkEditor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/evolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/neuroEvolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/neuroAndSimEvaluation)


TARGET_LINK_LIBRARIES(NerdClusterNeuroEvo
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/neuroAndSimEvaluation/libneuroAndSimEvaluation.a
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/neuroEvolution/libneuroEvolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/evolution/libevolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/networkEditor/libnetworkEditor.a
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/neuralNetwork/libneuralNetwork.a
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
	${QT_LIBRARIES}
)

