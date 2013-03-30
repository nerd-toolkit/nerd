cmake_minimum_required(VERSION 2.6)
project(nerd_testNetworkDynamicsDiagramPlotterLib)

set(nerd_testNetworkDynamicsDiagramPlotterLib_SRCS
	main.cpp
	Util/TestDynamicsPlotterUtil.cpp
	Exporters/TestMatlabExporter.cpp
	DynamicsPlot/TestDynamicsPlotManager.cpp
	DynamicsPlot/TestDynamicsPlotter.cpp
	DynamicsPlot/TestBasinPlotter.cpp
	DynamicsPlot/TestBifurcationPlotter.cpp
	DynamicsPlot/TestFirstReturnMap.cpp
	DynamicsPlot/TestTransientPlotter.cpp
	DynamicsPlot/TestIsoperiodPlotter.cpp
	DynamicsPlot/TestLyapunovExponent.cpp
	Adapters/DynamicsPlotterAdapter.cpp
	Adapters/TransferFunctionAdapter.cpp
)


set(nerd_testNetworkDynamicsDiagramPlotterLib_MOC_HDRS
	Util/TestDynamicsPlotterUtil.h
	Exporters/TestMatlabExporter.h
	DynamicsPlot/TestDynamicsPlotManager.h
	DynamicsPlot/TestDynamicsPlotter.h
	DynamicsPlot/TestBasinPlotter.h
	DynamicsPlot/TestBifurcationPlotter.h
	DynamicsPlot/TestFirstReturnMap.h
	DynamicsPlot/TestTransientPlotter.h
	DynamicsPlot/TestIsoperiodPlotter.h
	DynamicsPlot/TestLyapunovExponent.h
)

set(nerd_testNetworkDynamicsDiagramPlotterLib_RCS
)

set(nerd_testNetworkDynamicsDiagramPlotterLib_UI_HDRS
)


#select QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTSVG TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTTEST TRUE)
set(QT_USE_QTSCRIPT TRUE)
include(${QT_USE_FILE})


#QT Stuff
QT4_WRAP_CPP(nerd_testNetworkDynamicsDiagramPlotterLib_MOC_SRCS ${nerd_testNetworkDynamicsDiagramPlotterLib_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_testNetworkDynamicsDiagramPlotterLib_RC_SRCS ${nerd_testNetworkDynamicsDiagramPlotterLib_RCS})
QT4_WRAP_UI(nerd_testNetworkDynamicsDiagramPlotterLib_UI_HDRS ${nerd_testNetworkDynamicsDiagramPlotterLib_UIS})


#Create library.
add_executable(testNetworkDynamicsDiagramPlotterLib ${nerd_testNetworkDynamicsDiagramPlotterLib_SRCS} ${nerd_testNetworkDynamicsDiagramPlotterLib_MOC_SRCS} ${nerd_testNetworkDynamicsDiagramPlotterLib_RC_SRCS} ${nerd_testNetworkDynamicsDiagramPlotterLib_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/evolution/evolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/neuro/networkEditor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../NetworkDynamicsDiagramPlotterLib)

TARGET_LINK_LIBRARIES(testNetworkDynamicsDiagramPlotterLib
	${CMAKE_CURRENT_BINARY_DIR}/../NetworkDynamicsDiagramPlotterLib/libNetworkDynamicsDiagramPlotterLib.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/neuro/networkEditor/libnetworkEditor.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/neuro/neuralNetwork/libneuralNetwork.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/evolution/evolution/libevolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/system/nerd/libnerd.a
	${QT_LIBRARIES}
)

if(WIN32)
elseif(UNIX)
TARGET_LINK_LIBRARIES(testNetworkDynamicsDiagramPlotterLib
	-lGLU
	-lGL
)
endif(WIN32)


add_dependencies(testNetworkDynamicsDiagramPlotterLib nerd evolution neuralNetwork networkEditor NetworkDynamicsDiagramPlotterLib)
