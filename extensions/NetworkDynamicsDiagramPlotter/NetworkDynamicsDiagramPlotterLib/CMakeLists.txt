cmake_minimum_required(VERSION 2.6)
project(nerd_dynamicsPlotLib)

set(nerd_dynamicsPlotLib_SRCS
	Util/DynamicsPlotterUtil.cpp
	DynamicsPlot/BifurcationPlotter.cpp
	DynamicsPlot/BasinPlotter.cpp
	DynamicsPlot/IsoperiodPlotter.cpp
	DynamicsPlot/FirstReturnMap.cpp
	DynamicsPlot/TransientPlotter.cpp
	DynamicsPlot/LyapunovExponent.cpp
	Gui/DynamicsPlotterMainWindow.cpp  
	Execution/PlotterExecutionLoop.cpp  
	DynamicsPlot/DynamicsPlotManager.cpp  
	DynamicsPlotConstants.cpp  
	DynamicsPlot/DynamicsPlotter.cpp  
	Collections/DynamicsPlotCollection.cpp  
	DynamicsPlot/Exporters/Exporter.cpp  
	DynamicsPlot/OnlinePlotter/OnlinePlotter.cpp  
	DynamicsPlot/OnlinePlotter/OnlinePlotterWindow.cpp  
	DynamicsPlot/OnlinePlotter/MouseMoveLabel.cpp  
	Gui/InternalDiagramPlotter/InternalDiagramPlotterWidget.cpp
	Gui/InternalDiagramPlotter/InternalDiagramPlotterMainWidget.cpp
	Gui/InternalDiagramPlotter/AddInternalDiagramPlotterAction.cpp
	DynamicsPlot/OnlinePlotter/OnlinePlotterTabbedWindow.cpp
	DynamicsPlot/DummyPlotter.cpp
	Exporters/MatlabExporter.cpp
)

set(nerd_dynamicsPlotLib_MOC_HDRS
	Gui/DynamicsPlotterMainWindow.h   
	DynamicsPlot/OnlinePlotter/OnlinePlotter.h  
	DynamicsPlot/OnlinePlotter/OnlinePlotterWindow.h  
	Gui/InternalDiagramPlotter/InternalDiagramPlotterWidget.h
	Gui/InternalDiagramPlotter/InternalDiagramPlotterMainWidget.h
	Gui/InternalDiagramPlotter/AddInternalDiagramPlotterAction.h
	DynamicsPlot/OnlinePlotter/OnlinePlotterTabbedWindow.h
)

set(nerd_dynamicsPlotLib_RCS
)

set(nerd_dynamicsPlotLib_UIS
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
QT4_WRAP_CPP(nerd_dynamicsPlotLib_MOC_SRCS ${nerd_dynamicsPlotLib_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_dynamicsPlotLib_RC_SRCS ${nerd_dynamicsPlotLib_RCS})
QT4_WRAP_UI(nerd_dynamicsPlotLib_UI_HDRS ${nerd_dynamicsPlotLib_UIS})


#Create library.
add_library(NetworkDynamicsDiagramPlotterLib STATIC ${nerd_dynamicsPlotLib_SRCS} ${nerd_dynamicsPlotLib_MOC_SRCS} ${nerd_dynamicsPlotLib_RC_SRCS} ${nerd_dynamicsPlotLib_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/neuro/networkEditor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/simulator/simulator)

add_dependencies(NetworkDynamicsDiagramPlotterLib nerd evolution neuralNetwork networkEditor)