cmake_minimum_required(VERSION 2.6)
project(nerd_testSimulator)

set(nerd_testSimulator_SRCS
	main.cpp
	Collision/TestCollisionManager.cpp  
	Collision/TestCollisionObject.cpp  
	Collision/TestCollisionRule.cpp  
	Physics/TestAccelSensor.cpp  
	Physics/TestBoxBody.cpp  
	Physics/TestPhysicsManager.cpp  
	Physics/TestSimBody.cpp  
	Physics/TestSimJoint.cpp  
	Physics/TestSimObject.cpp  
	Physics/TestSphereBody.cpp  
	Event/EventListenerAdapter.cpp  
	Randomization/TestRandomizer.cpp  
	Collision/TestMaterialProperties.cpp  
	Physics/TestPhysics.cpp  
	Physics/TestCylinderBody.cpp  
	Physics/TestGeom.cpp  
	Physics/TestSimulationEnvironmentManager.cpp  
	Physics/TestSimObjectGroup.cpp  
	Physics/TestRayAndDistanceSensor.cpp  
	Signal/TestSignals.cpp  
	TestSimulationConstants.cpp
)


set(nerd_testSimulator_MOC_HDRS
	Collision/TestCollisionManager.h  
	Collision/TestCollisionObject.h  
	Collision/TestCollisionRule.h  
	Physics/TestAccelSensor.h  
	Physics/TestBoxBody.h  
	Physics/TestPhysicsManager.h  
	Physics/TestSimBody.h  
	Physics/TestSimJoint.h  
	Physics/TestSimObject.h  
	Physics/TestSphereBody.h  
	Randomization/TestRandomizer.h  
	Collision/TestMaterialProperties.h  
	Physics/TestPhysics.h  
	Physics/TestCylinderBody.h  
	Physics/TestGeom.h  
	Physics/TestSimulationEnvironmentManager.h  
	Physics/TestSimObjectGroup.h  
	Physics/TestRayAndDistanceSensor.h  
	Signal/TestSignals.h  
	TestSimulationConstants.h
)

set(nerd_testSimulator_RCS
)

set(nerd_testSimulator_UI_HDRS
)


#select QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTTEST TRUE)
set(QT_USE_QTSVG TRUE)
include(${QT_USE_FILE})


#QT Stuff
QT4_WRAP_CPP(nerd_testSimulator_MOC_SRCS ${nerd_testSimulator_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_testSimulator_RC_SRCS ${nerd_testSimulator_RCS})
QT4_WRAP_UI(nerd_testSimulator_UI_HDRS ${nerd_testSimulator_UIS})


#Create library.
add_executable(testSimulator ${nerd_testSimulator_SRCS} ${nerd_testSimulator_MOC_SRCS} ${nerd_testSimulator_RC_SRCS} ${nerd_testSimulator_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/simulator)

TARGET_LINK_LIBRARIES(testSimulator
	${QT_LIBRARIES}
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
	${CMAKE_CURRENT_BINARY_DIR}/../../simulator/simulator/libsimulator.a
)

