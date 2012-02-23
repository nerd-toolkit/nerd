cmake_minimum_required(VERSION 2.6)
project(nerd_testOdePhysics)

set(nerd_testOdePhysics_SRCS
	main.cpp  
	Collision/ODE_CollisionHandlerAdapter.cpp  
	Collision/Test_ODECollisionHandler.cpp  
	Communication/TestSimbaUDPCommunication.cpp  
	Physics/Test_ODESimJoints.cpp  
	Physics/Test_ODEBodies.cpp  
	Physics/Test_ODESimulationAlgorithm.cpp
)


set(nerd_testOdePhysics_MOC_HDRS
	Collision/Test_ODECollisionHandler.h  
	Communication/TestSimbaUDPCommunication.h  
	Physics/Test_ODESimJoints.h  
	Physics/Test_ODEBodies.h  
	Physics/Test_ODESimulationAlgorithm.h  
)

set(nerd_testOdePhysics_RCS
)

set(nerd_testOdePhysics_UI_HDRS
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
QT4_WRAP_CPP(nerd_testOdePhysics_MOC_SRCS ${nerd_testOdePhysics_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_testOdePhysics_RC_SRCS ${nerd_testOdePhysics_RCS})
QT4_WRAP_UI(nerd_testOdePhysics_UI_HDRS ${nerd_testOdePhysics_UIS})


set(ODE_INCLUDE_PATH ../../../ode-0.9/include)
set(ODE_LIBRARY_PATH ../../../ode-0.9/ode/src/libode.a)

#Create library.
add_executable(testOdePhysics ${nerd_testOdePhysics_SRCS} ${nerd_testOdePhysics_MOC_SRCS} ${nerd_testOdePhysics_RC_SRCS} ${nerd_testOdePhysics_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/simulator)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/odePhysics)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../simulator/testOdePhysics)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${ODE_INCLUDE_PATH})

TARGET_LINK_LIBRARIES(testOdePhysics
	${QT_LIBRARIES}
	${CMAKE_CURRENT_BINARY_DIR}/${ODE_LIBRARY_PATH}
	${CMAKE_CURRENT_BINARY_DIR}/../../simulator/odePhysics/libodePhysics.a
	${CMAKE_CURRENT_BINARY_DIR}/../../simulator/simulator/libsimulator.a
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
)

if(WIN32)
elseif(UNIX)
TARGET_LINK_LIBRARIES(testOdePhysics
	-lGLU
	-lGL
)
endif(WIN32)



