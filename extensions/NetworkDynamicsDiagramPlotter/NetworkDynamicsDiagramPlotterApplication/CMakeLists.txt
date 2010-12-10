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


#Create library.
add_executable(NetworkDynamicsDiagramPlotterApplication ${nerd_networkDynamicsDiagramPlotter_SRCS} ${nerd_networkDynamicsDiagramPlotter_MOC_SRCS} ${nerd_networkDynamicsDiagramPlotter_RC_SRCS} ${nerd_networkDynamicsDiagramPlotter_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/evolution/evolution)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../nerd/neuro/networkEditor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../NetworkDynamicsDiagramPlotterLib)


TARGET_LINK_LIBRARIES(NetworkDynamicsDiagramPlotterApplication
	${QT_LIBRARIES}
	${CMAKE_CURRENT_BINARY_DIR}/../NetworkDynamicsDiagramPlotterLib/libNetworkDynamicsDiagramPlotterLib.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/neuro/networkEditor/libnetworkEditor.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/neuro/neuralNetwork/libneuralNetwork.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/evolution/evolution/libevolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../../nerd/system/nerd/libnerd.a
)

if(WIN32)
elseif(UNIX)
TARGET_LINK_LIBRARIES(NetworkDynamicsDiagramPlotterApplication
	-lGLU
)
endif(WIN32)

