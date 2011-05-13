cmake_minimum_required(VERSION 2.6)
project(nerd_testNerd)

set(nerd_testNerd_SRCS
	main.cpp  
	Value/MyChangedListener.cpp  
	Value/TestValue.cpp  
	Core/TestParameterizedObject.cpp  
	Value/MyRepositoryChangedListener.cpp  
	Value/TestValueManager.cpp  
	Core/TestCore.cpp  
	Core/MSystemObject.cpp  
	Math/TestVector3D.cpp  
	Event/EventListenerAdapter.cpp  
	Value/TestInterfaceValue.cpp  
	Math/TestQuaternion.cpp  
	Event/TestEventManager.cpp  
	Event/TestEvent.cpp  
	Event/EventManagerAdapter.cpp  
	Value/TestNormalizedDoubleValue.cpp  
	Math/TestMath.cpp  
	Communication/TestUdpDatagram.cpp  
	Util/TestColor.cpp  
	Event/TestTriggerEventTask.cpp  
	Util/TestFileLocker.cpp  
	Math/TestMatrix.cpp
)


set(nerd_testNerd_MOC_HDRS
	Value/TestValue.h  
	Core/TestParameterizedObject.h  
	Value/TestValueManager.h  
	Core/TestCore.h  
	Math/TestVector3D.h   
	Value/TestInterfaceValue.h  
	Math/TestQuaternion.h  
	Event/TestEventManager.h  
	Event/TestEvent.h   
	Value/TestNormalizedDoubleValue.h  
	Math/TestMath.h  
	Communication/TestUdpDatagram.h  
	Util/TestColor.h  
	Event/TestTriggerEventTask.h  
	Util/TestFileLocker.h  
	Math/TestMatrix.h
)

set(nerd_testNerd_RCS
)

set(nerd_testNerd_UI_HDRS
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
QT4_WRAP_CPP(nerd_testNerd_MOC_SRCS ${nerd_testNerd_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_testNerd_RC_SRCS ${nerd_testNerd_RCS})
QT4_WRAP_UI(nerd_testNerd_UI_HDRS ${nerd_testNerd_UIS})


#Create library.
add_executable(testNerd ${nerd_testNerd_SRCS} ${nerd_testNerd_MOC_SRCS} ${nerd_testNerd_RC_SRCS} ${nerd_testNerd_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)

TARGET_LINK_LIBRARIES(testNerd
	${QT_LIBRARIES}
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
)

