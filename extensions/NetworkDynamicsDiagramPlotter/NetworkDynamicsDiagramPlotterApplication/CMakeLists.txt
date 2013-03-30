cmake_minimum_required(VERSION 2.6)
project(nerd_networkDynamicsDiagramPlotter)

set(nerd_networkDynamicsDiagramPlotter_SRCS
	main.cpp
	NetworkDynamicsPlotterApplication.cpp
)


set(nerd_networkDynamicsDiagramPlotter_MOC_HDRS
)

set(nerd_networkDynamicsDiagramPlotter_RCS
)

set(nerd_networkDynamicsDiagramPlotter_UI_HDRS
)


#select QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTSVG TRUE)
include(${QT_USE_FILE})


#QT Stuff
QT4_WRAP_CPP(nerd_networkDynamicsDiagramPlotter_MOC_SRCS ${nerd_networkDynamicsDiagramPlotter_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_networkDynamicsDiagramPlotter_RC_SRCS ${nerd_networkDynamicsDiagramPlotter_RCS})
QT4_WRAP_UI(nerd_networkDynamicsDiagramPlotter_UI_HDRS ${nerd_networkDynamicsDiagramPlotter_UIS})


#Create executable.
#add_executable(NetworkDynamicsDiagramPlotterApplication ${nerd_networkDynamicsDiagramPlotter_SRCS} ${nerd_networkDynamicsDiagramPlotter_MOC_SRCS} ${nerd_networkDynamicsDiagramPlotter_RC_SRCS} ${nerd_networkDynamicsDiagramPlotter_UI_HDRS})

add_executable(NerdDynamicsAnalyzer ${nerd_networkDynamicsDiagramPlotter_SRCS} ${nerd_networkDynamicsDiagramPlotter_MOC_SRCS} ${nerd_networkDynamicsDiagramPlotter_RC_SRCS} ${nerd_networkDynamicsDiagramPlotter_UI_HDRS})

set(ODE_INCLUDE_PATH ../../../ode-0.12/include)
set(ODE_LIBRARY_PATH ../../../ode-0.12/ode/src/.libs/libode.a)


add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/evolution/evolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/evolution/neuroAndSimEvaluation)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/neuro/networkEditor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../NetworkDynamicsDiagramPlotterLib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/simulator/simulator)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/simulator/odePhysics)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/simulator/simple2DPhysics)


TARGET_LINK_LIBRARIES(NerdDynamicsAnalyzer
	${CMAKE_CURRENT_BINARY_DIR}/../NetworkDynamicsDiagramPlotterLib/libNetworkDynamicsDiagramPlotterLib.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/evolution/neuroAndSimEvaluation/libneuroAndSimEvaluation.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/neuro/networkEditor/libnetworkEditor.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/neuro/neuralNetwork/libneuralNetwork.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/simulator/odePhysics/libodePhysics.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/simulator/simple2DPhysics/libsimple2DPhysics.a
	${CMAKE_CURRENT_BINARY_DIR}/${ODE_LIBRARY_PATH}
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/simulator/simulator/libsimulator.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/evolution/evolution/libevolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/system/nerd/libnerd.a
	${QT_LIBRARIES}
	#-lode
)

if(WIN32)
elseif(UNIX)
TARGET_LINK_LIBRARIES(NerdDynamicsAnalyzer
	-lGLU
	-lGL
)

endif(WIN32)

add_dependencies(NerdDynamicsAnalyzer
	nerd 
	evolution 
	neuralNetwork 
	networkEditor 
	NetworkDynamicsDiagramPlotterLib 
	neuroAndSimEvaluation 
	odePhysics 
	simple2DPhysics)
