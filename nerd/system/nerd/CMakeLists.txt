cmake_minimum_required(VERSION 2.6)
project(nerd_nerd)

set(nerd_nerd_SRCS
	Value/Value.cpp  
	Value/RangeValue.cpp
	Value/ValueManager.cpp  
	Core/Core.cpp  
	Value/BoolValue.cpp  
	Value/DoubleValue.cpp  
	Value/IntValue.cpp  
	Value/StringValue.cpp  
	Event/Event.cpp  
	Event/EventManager.cpp  
	Core/ParameterizedObject.cpp  
	Gui/Event/EventVisualization.cpp  
	Gui/Event/EventVisualizationWindow.cpp  
	Value/Vector3DValue.cpp  
	Math/Vector3D.cpp  
	Value/InterfaceValue.cpp  
	Value/QuaternionValue.cpp  
	Math/Quaternion.cpp  
	Math/Math.cpp  
	Value/NormalizedDoubleValue.cpp  
	Value/ColorValue.cpp  
	Core/ShutDownGuard.cpp  
	Communication/UdpDatagram.cpp  
	Gui/Event/EventDetailPanel.cpp  
	Gui/MultipleWindowWidget.cpp  
	Gui/ValuePlotter/PlotterItem.cpp  
	Gui/ValuePlotter/PlotterWidget.cpp  
	Gui/ValuePlotter/ValuePlotter.cpp  
	Gui/Control/BoolValueSwitcherAction.cpp  
	Gui/Control/EventTriggerAction.cpp  
	Event/TriggerEventTask.cpp  
	Gui/Logger/ParameterLoggerWidget.cpp  
	Gui/HtmlInfoWidget/HtmlInfoWidget.cpp  
	Core/Properties.cpp  
	Gui/ValuePlotter/ValuePlotterWidget.cpp  
	Gui/AboutWidget/AboutInfoWidget.cpp  
	Gui/ValuePlotter/PlotterItemControlPanel.cpp  
	Util/Color.cpp  
	Gui/Value/ValueVisualization.cpp  
	Gui/Value/ValueVisualizationWindow.cpp  
	PlugIns/CommandLineParser.cpp  
	Util/UnitTestMacros.cpp  
	Application/BaseApplication.cpp  
	Gui/DebugLoggerPanel/DebugLoggerPanel.cpp  
	Gui/ValuePlotter/StaticDataLoader.cpp  
	Gui/Event/EventDetailVisualization.cpp  
	Gui/Value/ValueDetailPanel.cpp  
	Gui/Value/ValueDetailVisualization.cpp  
	Control/ControlInterface.cpp  
	Control/Controller.cpp  
	PlugIns/PlugInManager.cpp  
	Math/Random.cpp  
	Statistics/StatisticCalculator.cpp  
	Statistics/StatisticsManager.cpp  
	Statistics/Statistics.cpp  
	Value/ValueFileInterfaceManager.cpp  
	Gui/ValuePlotter/ValuePlotterItem.cpp  
	Util/FileLocker.cpp  
	Statistics/StatisticsLogger.cpp  
	PlugIns/StepsPerSecondCounter.cpp  
	Gui/Control/NextStepTriggerAction.cpp  
	Gui/Containers/MainWindowContainer.cpp  
	Command/Command.cpp  
	Command/CommandExecutor.cpp  
	PlugIns/CommandLineArgument.cpp  
	Gui/Containers/PositionMemoryWidget.cpp  
	Gui/Containers/ActionWrapper.cpp  
	Value/ValueChangedListener.cpp  
	Util/Tracer.cpp  
	Value/Matrix3x3Value.cpp  
	Math/Matrix3x3.cpp  
	Value/ULongLongValue.cpp  
	Util/Util.cpp  
	Value/ValueAtCommandLineHandler.cpp  
	Core/QuitApplicationTrigger.cpp  
	PlugIns/PlugIn.cpp  
	Value/ChangeValueTask.cpp  
	Math/Matrix.cpp  
	Value/MatrixValue.cpp  
	Gui/AutomaticPictureSeries/PictureSeriesCreator.cpp  
	Gui/Parameter/MultipleParameterWindowWidget.cpp  
	Gui/Parameter/ParameterVisualization.cpp  
	Gui/Parameter/ParameterVisualizationWindow.cpp  
	Collections/UniversalScriptLoader.cpp  
	Script/ScriptingContext.cpp  
	Script/UniversalScriptingContext.cpp  
	NerdConstants.cpp  
	Util/NerdFileSelector.cpp
	Gui/GuiManager.cpp
	Math/Range.cpp
	Value/CodeValue.cpp
	Value/FileNameValue.cpp
	Gui/ScriptEditor/ScriptEditor.cpp
)

set(nerd_nerd_MOC_HDRS
	Application/BaseApplication.h  
	Gui/MultipleWindowWidget.h
	Gui/Event/EventVisualization.h
	Gui/Event/EventVisualizationWindow.h
	Gui/Event/EventDetailPanel.h
	Gui/Event/EventDetailVisualization.h  
	Gui/ValuePlotter/PlotterWidget.h
	Gui/ValuePlotter/ValuePlotter.h
	Gui/ValuePlotter/ValuePlotterWidget.h
	Gui/ValuePlotter/PlotterItemControlPanel.h
	Gui/ValuePlotter/StaticDataLoader.h 
	Gui/Control/BoolValueSwitcherAction.h
	Gui/Control/EventTriggerAction.h
	Gui/Control/NextStepTriggerAction.h 
	Gui/Logger/ParameterLoggerWidget.h
	Gui/HtmlInfoWidget/HtmlInfoWidget.h
	Gui/AboutWidget/AboutInfoWidget.h
	Gui/Value/ValueVisualization.h  
	Gui/Value/ValueVisualizationWindow.h  
	Gui/Value/ValueDetailPanel.h  
	Gui/Value/ValueDetailVisualization.h  
	Gui/DebugLoggerPanel/DebugLoggerPanel.h   
	Gui/Containers/MainWindowContainer.h  
	Gui/Containers/PositionMemoryWidget.h  
	Gui/Containers/ActionWrapper.h  
	Gui/AutomaticPictureSeries/PictureSeriesCreator.h
	Gui/Parameter/MultipleParameterWindowWidget.h
	Gui/Parameter/ParameterVisualization.h
	Gui/Parameter/ParameterVisualizationWindow.h
	PlugIns/StepsPerSecondCounter.h  
	Command/CommandExecutor.h   
	Core/ShutDownGuard.h
	Script/ScriptingContext.h
	Gui/ScriptEditor/ScriptEditor.h
)

set(nerd_nerd_RCS
	resources.qrc
)

set(nerd_nerd_UIS
)

#include QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTSVG TRUE)
include(${QT_USE_FILE})


#Execute QT Stuff
QT4_WRAP_CPP(nerd_nerd_MOC_SRCS ${nerd_nerd_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_nerd_RC_SRCS ${nerd_nerd_RCS})
QT4_WRAP_UI(nerd_nerd_UI_HDRS ${nerd_nerd_UIS})


#Create subversion revision number for version string in Core.cpp.
find_package(Subversion)
if(Subversion_FOUND)
  Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} ER)
  set(SUBVERSION_REVISION ${ER_WC_REVISION})
endif(Subversion_FOUND)
ADD_DEFINITIONS(-DVER="${SUBVERSION_REVISION}")

#Create library.
add_library(nerd STATIC ${nerd_nerd_SRCS} ${nerd_nerd_MOC_SRCS} ${nerd_nerd_RC_SRCS} ${nerd_nerd_UI_HDRS})

add_definitions(-Wall)
add_definitions(-Wextra)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
