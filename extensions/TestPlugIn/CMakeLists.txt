cmake_minimum_required(VERSION 2.6)
project(nerd_plugin)

set(nerd_pluginLib_SRCS
	main.cpp
	TestPlugIn.cpp
)

set(nerd_pluginLib_MOC_HDRS
	TestPlugIn.h
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
add_library(TestPlugin MODULE ${nerd_pluginLib_SRCS} ${nerd_plugin_MOC_SRCS} ${nerd_pluginLib_RC_SRCS} ${nerd_pluginLib_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../nerd/system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../nerd/neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../nerd/neuro/networkEditor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../nerd/simulator/simulator)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../nerd/evolution/evolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../nerd/evolution/neuroAndSimEvaluation)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../nerd/evolution/neuroEvolution)

