cmake_minimum_required(VERSION 2.6)
project(nerd_odePhysics)

set(nerd_odePhysics_SRCS
	Physics/ODE_Body.cpp  
	Physics/ODE_BoxBody.cpp  
	Physics/ODE_FixedJoint.cpp  
	Physics/ODE_HingeJoint.cpp  
	Physics/ODE_Joint.cpp  
	Physics/ODE_ServoMotor.cpp  
	Physics/ODE_SimulationAlgorithm.cpp  
	Physics/ODE_SphereBody.cpp  
	Collections/ODE_Physics.cpp  
	Physics/ODE_Plane.cpp  
	Collision/ODE_CollisionHandler.cpp  
	Physics/ODE_CylinderBody.cpp  
	Physics/ODE_TorqueDrivenDynamixel.cpp  
	Physics/ODE_Dynamixel.cpp  
	Physics/ODE_SliderJoint.cpp  
	Physics/ODE_SliderMotor.cpp  
	Physics/ODE_CapsuleBody.cpp  
	Physics/ODE_BoxInertiaBody.cpp  
	Physics/ODE_ForceSensor.cpp  
	MotorModels/ODE_DynamixelFrictionMotor.cpp  
	Physics/ODE_UniversalJoint.cpp  
	MotorModels/ODE_FrictionTorqueMotorModel.cpp  
	Physics/ODE_MSeriesSimpleDynamixel.cpp  
	Physics/SimpleToeMotor.cpp  
	MotorModels/ODE_MSeriesTorqueSpringMotorModel.cpp  
	Physics/ODE_ForceHandle.cpp  
	MotorModels/ODE_LinearSpringModel.cpp
	MotorModels/ODE_PID_PassiveActuatorModel.cpp
)

set(nerd_odePhysics_MOC_HDRS
)

set(nerd_odePhysics_RCS
)

set(nerd_odePhysics_UIS
)


#include QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
include(${QT_USE_FILE})


#Execute QT Stuff
QT4_WRAP_CPP(nerd_odePhysics_MOC_SRCS ${nerd_odePhysics_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_odePhysics_RC_SRCS ${nerd_odePhysics_RCS})
QT4_WRAP_UI(nerd_odePhysics_UI_HDRS ${nerd_odePhysics_UIS})

set(ODE_INCLUDE_PATH ../../../ode-0.9/include)
#set(ODE_LIBRARY_PATH ../../../ode-0.9/lib/libode.a)

#Create library.
add_library(odePhysics STATIC ${nerd_odePhysics_SRCS} ${nerd_odePhysics_MOC_SRCS} ${nerd_odePhysics_RC_SRCS} ${nerd_odePhysics_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/simulator)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/odePhysics)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${ODE_INCLUDE_PATH})

