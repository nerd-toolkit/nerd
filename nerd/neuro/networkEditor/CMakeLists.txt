cmake_minimum_required(VERSION 2.6)
project(nerd_networkEditor)

set(nerd_networkEditor_SRCS
	Gui/NetworkEditor/GroupItem.cpp  
	Gui/NetworkEditor/ModuleItem.cpp  
	Gui/NetworkEditor/NetworkVisualization.cpp  
	Gui/NetworkEditor/NetworkVisualizationHandler.cpp  
	Gui/NetworkEditor/NeuralNetworkEditor.cpp  
	Gui/NetworkEditor/NeuronItem.cpp  
	Gui/NetworkEditor/PaintItem.cpp  
	Gui/NetworkEditor/SelectionRectItem.cpp  
	Gui/NetworkEditor/SimpleGroupItem.cpp  
	Gui/NetworkEditor/SimpleModuleItem.cpp  
	Gui/NetworkEditor/SimpleNetworkVisualizationHandler.cpp  
	Gui/NetworkEditor/SimpleNeuronItem.cpp  
	Gui/NetworkEditor/SimpleSynapseItem.cpp  
	Gui/NetworkEditor/SynapseItem.cpp  
	Gui/NetworkEditorCommands/AddConstraintCommand.cpp  
	Gui/NetworkEditorCommands/ChangeActivationFunctionCommand.cpp  
	Gui/NetworkEditorCommands/ChangeNetworkFunctionCommand.cpp  
	Gui/NetworkEditorCommands/ChangeSynapseFunctionCommand.cpp  
	Gui/NetworkEditorCommands/ChangeTransferFunctionCommand.cpp  
	Gui/NetworkEditorCommands/ChangeValueCommand.cpp  
	Gui/NetworkEditorCommands/InsertGroupCommand.cpp  
	Gui/NetworkEditorCommands/InsertModuleCommand.cpp  
	Gui/NetworkEditorCommands/InsertNeuronCommand.cpp  
	Gui/NetworkEditorCommands/InsertSynapseCommand.cpp  
	Gui/NetworkEditorCommands/MoveMembersToModuleCommand.cpp  
	Gui/NetworkEditorCommands/RemoveConstraintCommand.cpp  
	Gui/NetworkEditorCommands/RemoveNetworkObjectsCommand.cpp  
	Gui/NetworkEditorCommands/RenameGroupCommand.cpp  
	Gui/NetworkEditorTools/ConstraintDetailWidget.cpp  
	Gui/NetworkEditorTools/GrabNetworkElementIdTool.cpp  
	Gui/NetworkEditorTools/GroupSelectionWidget.cpp  
	Gui/NetworkEditorTools/InsertModuleNetworkTool.cpp  
	Gui/NetworkEditorTools/InsertNeuronNetworkTool.cpp  
	Gui/NetworkEditorTools/InsertSynapseNetworkTool.cpp  
	Gui/NetworkEditorTools/NetworkDetailWidget.cpp  
	Gui/NetworkEditorTools/NetworkManipulationTool.cpp  
	Gui/NetworkEditorTools/NeuralNetworkToolbox.cpp  
	Gui/NetworkEditorTools/NeuronDetailWidget.cpp  
	Gui/NetworkEditorTools/ParameterEditor.cpp  
	Gui/NetworkEditorTools/ParameterPanel.cpp  
	Gui/NetworkEditorTools/PropertyDialog.cpp  
	Gui/NetworkEditorTools/RemoveObjectsNetworkTool.cpp  
	Gui/NetworkEditorTools/RemoveSelectedObjectsNetworkTool.cpp  
	Gui/NetworkEditorTools/SynapseDetailWidget.cpp  
	Gui/NetworkEditor/StandAloneNetworkEditor.cpp  
	Gui/NetworkEditor/EvaluationNetworkEditor.cpp  
	Collections/NetworkEditorCollection.cpp  
	Gui/NetworkEditorTools/EditorToolWidget.cpp  
	Gui/NetworkEditorTools/EditorMessageWidget.cpp  
	Gui/NetworkEditorTools/RunConstraintsAction.cpp  
	Gui/NetworkEditorTools/ChangeViewModeTool.cpp  
	Gui/NetworkEditorCommands/AlignSynapseCommand.cpp  
	Gui/NetworkEditorTools/AddActivationPlotterAction.cpp  
	Gui/Plotter/NeuronActivityPlotter.cpp  
	Gui/NetworkEditorCommands/SetLocationPropertiesCommand.cpp  
	PlugIns/ActivationFrequencyCalculator.cpp  
	Gui/NetworkEditor/ElementPairVisualization.cpp  
	Gui/NetworkEditorTools/ShowElementPairTool.cpp  
	Gui/NetworkEditorCommands/CopyPasteNetworkCommand.cpp  
	Gui/NetworkEditorTools/CopyPasteNetworkTool.cpp  
	Gui/NetworkEditorTools/ReconnectSynapseNetworkTool.cpp  
	Gui/NetworkEditorCommands/ReconnectSynapseCommand.cpp  
	Gui/NetworkEditorTools/ModuleManagementWidget.cpp  
	Util/InsertSubnetworkIntoNetwork.cpp  
	Gui/NetworkEditorCommands/InsertSubnetworkCommand.cpp  
	Util/NetworkEditorUtil.cpp  
	Gui/NetworkEditorTools/ReplaceModuleTool.cpp  
	Gui/NetworkEditorCommands/ReplaceModuleCommand.cpp  
	Gui/NetworkEditorTools/GuessModulePairs.cpp  
	Gui/NetworkEditor/NetworkSearchDialog.cpp  
	Gui/NetworkEditorCommands/AlignNeuronsCommand.cpp  
	Gui/FirstReturnMap/FirstReturnMapPlotter.cpp  
	Gui/FirstReturnMap/FirstReturnMapPlotterWidget.cpp  
	Gui/NetworkEditorTools/AddFirstReturnMapPlotterAction.cpp
	Gui/NetworkEditor/LoadRecentNetworkAction.cpp
	Gui/NetworkEditorTools/GuessModulePairsByPosition.cpp
	NetworkEditorConstants.cpp
	Gui/NetworkEditorCommands/AlignModuleSizeCommand.cpp
	Gui/NetworkEditorTools/ConnectNeuronsWithSynapsesTool.cpp
	Gui/NetworkEditor/NeuroModulatorItem.cpp
	Gui/NetworkEditor/SimpleNeuroModulatorItem.cpp
)

set(nerd_networkEditor_MOC_HDRS
	Gui/NetworkEditor/NetworkVisualization.h  
	Gui/NetworkEditor/NeuralNetworkEditor.h  
	Gui/NetworkEditorTools/ConstraintDetailWidget.h  
	Gui/NetworkEditorTools/GrabNetworkElementIdTool.h  
	Gui/NetworkEditorTools/GroupSelectionWidget.h  
	Gui/NetworkEditorTools/InsertModuleNetworkTool.h  
	Gui/NetworkEditorTools/InsertNeuronNetworkTool.h  
	Gui/NetworkEditorTools/InsertSynapseNetworkTool.h  
	Gui/NetworkEditorTools/NetworkDetailWidget.h  
	Gui/NetworkEditorTools/NetworkManipulationTool.h  
	Gui/NetworkEditorTools/NeuralNetworkToolbox.h  
	Gui/NetworkEditorTools/NeuronDetailWidget.h  
	Gui/NetworkEditorTools/ParameterEditor.h  
	Gui/NetworkEditorTools/ParameterPanel.h  
	Gui/NetworkEditorTools/PropertyDialog.h  
	Gui/NetworkEditorTools/RemoveObjectsNetworkTool.h  
	Gui/NetworkEditorTools/RemoveSelectedObjectsNetworkTool.h  
	Gui/NetworkEditorTools/SynapseDetailWidget.h  
	Gui/NetworkEditor/StandAloneNetworkEditor.h  
	Gui/NetworkEditor/EvaluationNetworkEditor.h  
	Gui/NetworkEditorTools/EditorToolWidget.h  
	Gui/NetworkEditorTools/EditorMessageWidget.h  
	Gui/NetworkEditorTools/RunConstraintsAction.h  
	Gui/NetworkEditorTools/ChangeViewModeTool.h   
	Gui/NetworkEditorTools/AddActivationPlotterAction.h  
	Gui/Plotter/NeuronActivityPlotter.h  
	Gui/NetworkEditorTools/ModuleManagementWidget.h  
	Gui/NetworkEditorTools/GuessModulePairs.h  
	Gui/NetworkEditor/NetworkSearchDialog.h  
	Gui/FirstReturnMap/FirstReturnMapPlotter.h  
	Gui/NetworkEditorTools/AddFirstReturnMapPlotterAction.h
	Gui/NetworkEditor/LoadRecentNetworkAction.h
	Gui/NetworkEditorTools/GuessModulePairsByPosition.h
	Gui/NetworkEditorTools/ConnectNeuronsWithSynapsesTool.h
)

set(nerd_networkEditor_RCS
)

set(nerd_networkEditor_UIS
)


#include QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTSVG TRUE)
include(${QT_USE_FILE})


#Execute QT Stuff
QT4_WRAP_CPP(nerd_networkEditor_MOC_SRCS ${nerd_networkEditor_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_networkEditor_RC_SRCS ${nerd_networkEditor_RCS})
QT4_WRAP_UI(nerd_networkEditor_UI_HDRS ${nerd_networkEditor_UIS})



#Create library.
add_library(networkEditor STATIC ${nerd_networkEditor_SRCS} ${nerd_networkEditor_MOC_SRCS} ${nerd_networkEditor_RC_SRCS} ${nerd_networkEditor_UI_HDRS})


add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/neuralNetwork)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../evolution/evolution)

add_dependencies(networkEditor nerd neuralNetwork evolution)

