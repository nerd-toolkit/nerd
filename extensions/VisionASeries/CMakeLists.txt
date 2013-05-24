cmake_minimum_required(VERSION 2.6)

#NOTE: Change the name of your new plugin project here!
project(nerd_VisionASeries)

set(nerd_pluginLib_SRCS
	main.cpp
    VisionASeries.cpp
    VisionASeriesSensor.cpp
)

set(nerd_pluginLib_MOC_HDRS
    VisionASeries.h
    VisionASeriesSensor.h
)

set(nerd_pluginLib_RCS
)

set(nerd_pluginLib_UIS
)

#include QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTSVG TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTSCRIPT TRUE)
include(${QT_USE_FILE})

#BasePath for moc

set(NERD_BASE ${CMAKE_CURRENT_SOURCE_DIR}/../..)


#Execute QT Stuff
QT4_WRAP_CPP(nerd_plugin_MOC_SRCS ${nerd_pluginLib_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_plugin_RC_SRCS ${nerd_pluginLib_RCS})
QT4_WRAP_UI(nerd_plugin_UI_HDRS ${nerd_pluginLib_UIS})


#plugin specifics
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
#ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
#ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

#Create library.
#NOTE: Change the name of the shared library file here!
add_library(VisionASeries MODULE ${nerd_pluginLib_SRCS} ${nerd_plugin_MOC_SRCS} ${nerd_pluginLib_RC_SRCS} ${nerd_pluginLib_UI_HDRS})

add_definitions(-Wall)
include_directories(${NERD_BASE})
include_directories(${NERD_BASE}/nerd/system/nerd)
include_directories(${NERD_BASE}/nerd/neuro/neuralNetwork)
include_directories(${NERD_BASE}/nerd/neuro/networkEditor)
include_directories(${NERD_BASE}/nerd/simulator/simulator)
include_directories(${NERD_BASE}/nerd/evolution/evolution)
include_directories(${NERD_BASE}/nerd/evolution/neuroAndSimEvaluation)
include_directories(${NERD_BASE}/nerd/evolution/neuroEvolution)
include_directories(${NERD_BASE}/nerd/evolution/neuroEvolution)

#TARGET_LINK_LIBRARIES(VisionASeries
#	${CMAKE_CURRENT_BINARY_DIR}/../../nerd/neuro/neuralNetwork/libneuralNetwork.a
#	${CMAKE_CURRENT_BINARY_DIR}/../../nerd/simulator/simulator/libsimulator.a
#	${CMAKE_CURRENT_BINARY_DIR}/../../nerd/system/nerd/libnerd.a
#	${QT_LIBRARIES}
#)



#NOTE: Set the library dependencies here. Usuually, it is sufficient to exchange the first project name with your project name.
add_dependencies(VisionASeries nerd neuralNetwork networkEditor simulator evolution neuroAndSimEvaluation neuroEvolution)

