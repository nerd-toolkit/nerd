cmake_minimum_required(VERSION 2.6)
project(nerd_NerdNeuroSim)

set(nerd_NerdNeuroSim_SRCS
	main.cpp
	NerdNeuroSimApplication.cpp
)


set(nerd_NerdNeuroSim_MOC_HDRS
	NerdNeuroSimApplication.h
)

set(nerd_NerdNeuroSim_RCS
)

set(nerd_NerdNeuroSim_UI_HDRS
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
QT4_WRAP_CPP(nerd_NerdNeuroSim_MOC_SRCS ${nerd_NerdNeuroSim_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_NerdNeuroSim_RC_SRCS ${nerd_NerdNeuroSim_RCS})
QT4_WRAP_UI(nerd_NerdNeuroSim_UI_HDRS ${nerd_NerdNeuroSim_UIS})


#Create library.
add_executable(NerdNeuroSim ${nerd_NerdNeuroSim_SRCS} ${nerd_NerdNeuroSim_MOC_SRCS} ${nerd_NerdNeuroSim_RC_SRCS} ${nerd_NerdNeuroSim_UI_HDRS})

set(ODE_INCLUDE_PATH ../../../ode-0.9/include)
set(ODE_LIBRARY_PATH ../../../ode-0.9/ode/src/libode.a)

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/simulator)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/odePhysics)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/simple2DPhysics)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/networkEditor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/evolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/neuroEvolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/neuroAndSimEvaluation)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${ODE_INCLUDE_PATH})

TARGET_LINK_LIBRARIES(NerdNeuroSim
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/neuroAndSimEvaluation/libneuroAndSimEvaluation.a
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/neuroEvolution/libneuroEvolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/networkEditor/libnetworkEditor.a
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/evolution/libevolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/neuralNetwork/libneuralNetwork.a
	${CMAKE_CURRENT_BINARY_DIR}/../../simulator/odePhysics/libodePhysics.a
	${CMAKE_CURRENT_BINARY_DIR}/${ODE_LIBRARY_PATH}
	${CMAKE_CURRENT_BINARY_DIR}/../../simulator/simple2DPhysics/libsimple2DPhysics.a
	${CMAKE_CURRENT_BINARY_DIR}/../../simulator/simulator/libsimulator.a
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
	${QT_LIBRARIES}
)

if(WIN32)
elseif(UNIX)
TARGET_LINK_LIBRARIES(NerdNeuroSim
	-lGLU
	-lGL
)
endif(WIN32)


