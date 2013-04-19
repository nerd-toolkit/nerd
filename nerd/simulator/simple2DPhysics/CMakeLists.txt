cmake_minimum_required(VERSION 2.6)
project(nerd_simple2DPhysics)

set(nerd_simple2DPhysics_SRCS
	Physics/Simple2D_SimulationAlgorithm.cpp
	Collision/Simple2D_CollisionHandler.cpp
	Collections/Simple2D_Physics.cpp
	Physics/Simple2D_BoxBody.cpp
	Physics/Simple2D_FixedJoint.cpp
	Physics/Simple2D_Body.cpp
	Physics/Simple2D_SliderMotor.cpp
	Physics/Simple2D_DifferentialDrive.cpp
)

set(nerd_simple2DPhysics_MOC_HDRS
)

set(nerd_simple2DPhysics_RCS
)

set(nerd_simple2DPhysics_UIS
)


#include QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
include(${QT_USE_FILE})


#Execute QT Stuff
QT4_WRAP_CPP(nerd_simple2DPhysics_MOC_SRCS ${nerd_simple2DPhysics_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_simple2DPhysics_RC_SRCS ${nerd_simple2DPhysics_RCS})
QT4_WRAP_UI(nerd_simple2DPhysics_UI_HDRS ${nerd_simple2DPhysics_UIS})

#Create library.
add_library(simple2DPhysics STATIC ${nerd_simple2DPhysics_SRCS} ${nerd_simple2DPhysics_MOC_SRCS} ${nerd_simple2DPhysics_RC_SRCS} ${nerd_simple2DPhysics_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/simulator)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/simple2DPhysics)


add_dependencies(simple2DPhysics nerd simulator)