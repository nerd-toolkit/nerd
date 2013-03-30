cmake_minimum_required(VERSION 2.6)
project(nerd_NerdMultiCoreEvaluation)

set(nerd_NerdMultiCoreEvaluation_SRCS
	main.cpp
	NerdMultiCoreEvaluationApplication.cpp
	MultiCoreEvaluationRunner.cpp
)


set(nerd_NerdMultiCoreEvaluation_MOC_HDRS
	NerdMultiCoreEvaluationApplication.h
	MultiCoreEvaluationRunner.h
)

set(nerd_NerdMultiCoreEvaluation_RCS
)

set(nerd_NerdMultiCoreEvaluation_UI_HDRS
)


#select QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
include(${QT_USE_FILE})


#QT Stuff
QT4_WRAP_CPP(nerd_NerdMultiCoreEvaluation_MOC_SRCS ${nerd_NerdMultiCoreEvaluation_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_NerdMultiCoreEvaluation_RC_SRCS ${nerd_NerdMultiCoreEvaluation_RCS})
QT4_WRAP_UI(nerd_NerdMultiCoreEvaluation_UI_HDRS ${nerd_NerdMultiCoreEvaluation_UIS})


#Create library.
add_executable(NerdMultiCoreEvaluation ${nerd_NerdMultiCoreEvaluation_SRCS} ${nerd_NerdMultiCoreEvaluation_MOC_SRCS} ${nerd_NerdMultiCoreEvaluation_RC_SRCS} ${nerd_NerdMultiCoreEvaluation_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)


TARGET_LINK_LIBRARIES(NerdMultiCoreEvaluation
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
	${QT_LIBRARIES}
)

add_dependencies(NerdMultiCoreEvaluation nerd)
