cmake_minimum_required(VERSION 2.6)
project(nerd_NerdSim)

set(nerd_NerdSim_SRCS
	main.cpp
	NerdSimApplication.cpp
)


set(nerd_NerdSim_MOC_HDRS
)

set(nerd_NerdSim_RCS
)

set(nerd_NerdSim_UI_HDRS
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
QT4_WRAP_CPP(nerd_NerdSim_MOC_SRCS ${nerd_NerdSim_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_NerdSim_RC_SRCS ${nerd_NerdSim_RCS})
QT4_WRAP_UI(nerd_NerdSim_UI_HDRS ${nerd_NerdSim_UIS})

set(ODE_INCLUDE_PATH ../../../ode-0.9/include)
set(ODE_LIBRARY_PATH ../../../ode-0.9/ode/src/libode.a)


#Create library.
add_executable(NerdSim ${nerd_NerdSim_SRCS} ${nerd_NerdSim_MOC_SRCS} ${nerd_NerdSim_RC_SRCS} ${nerd_NerdSim_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/simulator)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/odePhysics)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/networkEditor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/evolution)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${ODE_INCLUDE_PATH})

TARGET_LINK_LIBRARIES(NerdSim
	${QT_LIBRARIES}
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/networkEditor/libnetworkEditor.a
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/evolution/libevolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/neuralNetwork/libneuralNetwork.a
	${CMAKE_CURRENT_BINARY_DIR}/../../simulator/odePhysics/libodePhysics.a
	${CMAKE_CURRENT_BINARY_DIR}/../../simulator/simulator/libsimulator.a
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
	${CMAKE_CURRENT_BINARY_DIR}/${ODE_LIBRARY_PATH}
)

if(WIN32)
elseif(UNIX)
TARGET_LINK_LIBRARIES(NerdSim
	-lGLU
	-lGL
)
endif(WIN32)

