cmake_minimum_required(VERSION 2.6)
project(nerd_NerdNetworkEditor)

set(nerd_NerdNetworkEditor_SRCS
	main.cpp
	NeuralNetworkEditorApplication.cpp
)


set(nerd_NerdNetworkEditor_MOC_HDRS
)

set(nerd_NerdNetworkEditor_RCS
)

set(nerd_NerdNetworkEditor_UI_HDRS
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
QT4_WRAP_CPP(nerd_NerdNetworkEditor_MOC_SRCS ${nerd_NerdNetworkEditor_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_NerdNetworkEditor_RC_SRCS ${nerd_NerdNetworkEditor_RCS})
QT4_WRAP_UI(nerd_NerdNetworkEditor_UI_HDRS ${nerd_NerdNetworkEditor_UIS})


#Create library.
add_executable(NerdNetworkEditor ${nerd_NerdNetworkEditor_SRCS} ${nerd_NerdNetworkEditor_MOC_SRCS} ${nerd_NerdNetworkEditor_RC_SRCS} ${nerd_NerdNetworkEditor_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/networkEditor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/evolution)

TARGET_LINK_LIBRARIES(NerdNetworkEditor
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/networkEditor/libnetworkEditor.a
	${CMAKE_CURRENT_BINARY_DIR}/../../evolution/evolution/libevolution.a
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/neuralNetwork/libneuralNetwork.a
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
	${QT_LIBRARIES}
)

if(WIN32)
elseif(UNIX)
TARGET_LINK_LIBRARIES(NerdNetworkEditor
	-lGLU
	-lGL
)
endif(WIN32)

add_dependencies(NerdNetworkEditor nerd simulator evolution odePhysics neuroEvolution neuroAndSimEvaluation networkEditor)
