TEMPLATE = lib

CONFIG += staticlib \
rtti \
warn_on \
 qt \
 debug \
 console


RESOURCES = resources.qrc


SOURCES += Value/Value.cpp \
 Value/ValueManager.cpp \
 Core/Core.cpp \
 Value/BoolValue.cpp \
 Value/DoubleValue.cpp \
 Value/IntValue.cpp \
 Value/StringValue.cpp \
 Event/Event.cpp \
 Event/EventManager.cpp \
 Core/ParameterizedObject.cpp \
 Gui/Event/EventVisualization.cpp \
 Gui/Event/EventVisualizationWindow.cpp \
 Value/Vector3DValue.cpp \
 Math/Vector3D.cpp \
 Value/InterfaceValue.cpp \
 Value/QuaternionValue.cpp \
 Math/Quaternion.cpp \
 Math/Math.cpp \
 Value/NormalizedDoubleValue.cpp \
 Value/ColorValue.cpp \
 Core/ShutDownGuard.cpp \
 Communication/UdpDatagram.cpp \
 Gui/Event/EventDetailPanel.cpp \
 Gui/MultipleWindowWidget.cpp \
 Gui/ValuePlotter/PlotterItem.cpp \
 Gui/ValuePlotter/PlotterWidget.cpp \
 Gui/ValuePlotter/ValuePlotter.cpp \
 Gui/Control/BoolValueSwitcherAction.cpp \
 Gui/Control/EventTriggerAction.cpp \
 Event/TriggerEventTask.cpp \
 Gui/Logger/ParameterLoggerWidget.cpp \
 Gui/HtmlInfoWidget/HtmlInfoWidget.cpp \
 Core/Properties.cpp \
 Gui/ValuePlotter/ValuePlotterWidget.cpp \
 Gui/AboutWidget/AboutInfoWidget.cpp \
 Gui/ValuePlotter/PlotterItemControlPanel.cpp \
 Util/Color.cpp \
 Gui/Value/ValueVisualization.cpp \
 Gui/Value/ValueVisualizationWindow.cpp \
 PlugIns/CommandLineParser.cpp \
 Util/UnitTestMacros.cpp \
 Application/BaseApplication.cpp \
 Gui/DebugLoggerPanel/DebugLoggerPanel.cpp \
 Gui/ValuePlotter/StaticDataLoader.cpp \
 Gui/Event/EventDetailVisualization.cpp \
 Gui/Value/ValueDetailPanel.cpp \
 Gui/Value/ValueDetailVisualization.cpp \
 Control/ControlInterface.cpp \
 Control/Controller.cpp \
 PlugIns/PlugInManager.cpp \
 Math/Random.cpp \
 Statistics/StatisticCalculator.cpp \
 Statistics/StatisticsManager.cpp \
 Statistics/Statistics.cpp \
 Value/ValueFileInterfaceManager.cpp \
 Gui/ValuePlotter/ValuePlotterItem.cpp \
 Util/FileLocker.cpp \
 Statistics/StatisticsLogger.cpp \
 PlugIns/FramesPerSecondCounter.cpp \
 Gui/Control/NextStepTriggerAction.cpp \
 Gui/Containers/MainWindowContainer.cpp \
 Command/Command.cpp \
 Command/CommandExecutor.cpp \
 PlugIns/CommandLineArgument.cpp \
 Gui/Containers/PositionMemoryWidget.cpp \
 Gui/Containers/ActionWrapper.cpp \
 Value/ValueChangedListener.cpp \
 Util/Tracer.cpp \
 Value/Matrix3x3Value.cpp \
 Math/Matrix3x3.cpp \
 Value/ULongLongValue.cpp \
 Util/Util.cpp \
 Value/ValueAtCommandLineHandler.cpp \
 Core/QuitApplicationTrigger.cpp \
 PlugIns/PlugIn.cpp \
 Value/ChangeValueTask.cpp \
 Math/Matrix.cpp \
 Value/MatrixValue.cpp \
 Gui/AutomaticPictureSeries/PictureSeriesCreator.cpp \
 Gui/Parameter/MultipleParameterWindowWidget.cpp \
 Gui/Parameter/ParameterVisualization.cpp \
 Gui/Parameter/ParameterVisualizationWindow.cpp \
 Collections/UniversalScriptLoader.cpp \
 Script/ScriptingContext.cpp \
 Script/UniversalScriptingContext.cpp \
 NerdConstants.cpp \
 Util/NerdFileSelector.cpp


HEADERS += Value/Value.h \
 Value/ValueChangedListener.h \	
 Value/ValueManager.h \
 Core/Object.h \
 Core/Core.h \
 Value/BoolValue.h \
 Value/DoubleValue.h \
 Value/IntValue.h \
 Value/StringValue.h \
 Event/Event.h \
 Event/EventListener.h \
 Event/EventManager.h \
 Core/SystemObject.h \
 Core/ParameterizedObject.h \
 Gui/Value/ValueVisualization.h \
 Gui/Value/ValueVisualizationWindow.h \
 Gui/Event/EventVisualization.h \
 Gui/Event/EventVisualizationWindow.h \
 Value/Vector3DValue.h \
 Math/Vector3D.h \
 Value/InterfaceValue.h \
 Value/QuaternionValue.h \
 Math/Quaternion.h \
 Math/Math.h \
 Value/NormalizedDoubleValue.h \
 Value/ColorValue.h \
 Core/Task.h \
 Core/ShutDownGuard.h \
 Gui/Event/EventDetailPanel.h \
 Gui/MultipleWindowWidget.h \
 Gui/ValuePlotter/PlotterItem.h \
 Gui/ValuePlotter/PlotterWidget.h \
 Gui/ValuePlotter/ValuePlotter.h \
 Gui/Control/BoolValueSwitcherAction.h \
 Gui/Control/EventTriggerAction.h \
 Event/TriggerEventTask.h \
 Gui/Logger/ValueProvider.h \
 Gui/Logger/ParameterLoggerWidget.h \
 Gui/HtmlInfoWidget/HtmlInfoWidget.h \
 Core/Properties.h \
 Value/MultiPartValue.h \
 Gui/ValuePlotter/ValuePlotterWidget.h \
 Gui/AboutWidget/AboutInfoWidget.h \
 Gui/ValuePlotter/PlotterItemControlPanel.h \
 Util/Color.h \
 Communication/UdpDatagram.h \
 PlugIns/CommandLineParser.h \
 Util/UnitTestMacros.h \
 Application/BaseApplication.h \
 Gui/DebugLoggerPanel/DebugLoggerPanel.h \
 Gui/ValuePlotter/StaticDataLoader.h \
 Gui/Event/EventDetailVisualization.h \
 Gui/Value/ValueDetailPanel.h \
 Gui/Value/ValueDetailVisualization.h \
 Control/ControlInterface.h \
 Control/Controller.h \
 PlugIns/PlugInManager.h \
 PlugIns/PlugIn.h \
 Math/Random.h \
 Control/ControllerProvider.h \
 Statistics/StatisticCalculator.h \
 Statistics/StatisticsManager.h \
 Statistics/Statistics.h \
 Value/ValueFileInterfaceManager.h \
 Gui/ValuePlotter/ValuePlotterItem.h \
 Util/FileLocker.h \
 Statistics/StatisticsLogger.h \
 PlugIns/FramesPerSecondCounter.h \
 Gui/Control/NextStepTriggerAction.h \
 Gui/Containers/MainWindowContainer.h \
 Command/Command.h \
 Command/CommandExecutor.h \
 Command/CommandExecutionListener.h \
 PlugIns/CommandLineArgument.h \
 Gui/Containers/PositionMemoryWidget.h \
 Gui/Containers/ActionWrapper.h \
 Util/Tracer.h \
 Value/Matrix3x3Value.h \
 Math/Matrix3x3.h \
 Value/ULongLongValue.h \
 Util/Util.h \
 Value/ValueAtCommandLineHandler.h \
 Core/PropertyChangedListener.h \
 Core/QuitApplicationTrigger.h \
 Value/ChangeValueTask.h \
 Math/Matrix.h \
 Value/MatrixValue.h \
 Gui/AutomaticPictureSeries/PictureSeriesCreator.h \
 Gui/Parameter/MultipleParameterWindowWidget.h \
 Gui/Parameter/ParameterVisualization.h \
 Gui/Parameter/ParameterVisualizationWindow.h \
 Gui/Parameter/SetInitValueTask.h \
 Version.h \
 Collections/UniversalScriptLoader.h \
 Script/ScriptingContext.h \
 Script/UniversalScriptingContext.h \
 NerdConstants.h \
 Util/NerdFileSelector.h


DISTFILES += Gui/Parameter \
 . \
 trst

DESTDIR = ../../bin


CONFIG -= release



QT += network \
 script



SVNREV = """""$(shell svnversion -n $(srcdir))"""""
VERSTR = '\\"$${SVNREV}\\"'
DEFINES += VER=\"$${VERSTR}\"
