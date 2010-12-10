CONFIG -= release

CONFIG += debug \
rtti \
opengl \
staticlib
QT += xml \
network \
opengl \
 svg \
 script

TEMPLATE = lib

HEADERS += Gui/KeyListener.h \
Gui/NetworkEditor/GroupItem.h \
Gui/NetworkEditor/ModuleItem.h \
Gui/NetworkEditor/NetworkVisualization.h \
Gui/NetworkEditor/NetworkVisualizationHandler.h \
Gui/NetworkEditor/NeuralNetworkEditor.h \
Gui/NetworkEditor/NeuronItem.h \
Gui/NetworkEditor/PaintItem.h \
Gui/NetworkEditor/SelectionListener.h \
Gui/NetworkEditor/SelectionRectItem.h \
Gui/NetworkEditor/SimpleGroupItem.h \
Gui/NetworkEditor/SimpleModuleItem.h \
Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h \
Gui/NetworkEditor/SimpleNeuronItem.h \
Gui/NetworkEditor/SimpleSynapseItem.h \
Gui/NetworkEditor/SynapseItem.h \
Gui/NetworkEditor/VisualizationMouseListener.h \
Gui/NetworkEditorCommands/AddConstraintCommand.h \
Gui/NetworkEditorCommands/ChangeActivationFunctionCommand.h \
Gui/NetworkEditorCommands/ChangeNetworkFunctionCommand.h \
Gui/NetworkEditorCommands/ChangeSynapseFunctionCommand.h \
Gui/NetworkEditorCommands/ChangeTransferFunctionCommand.h \
Gui/NetworkEditorCommands/ChangeValueCommand.h \
Gui/NetworkEditorCommands/InsertGroupCommand.h \
Gui/NetworkEditorCommands/InsertModuleCommand.h \
Gui/NetworkEditorCommands/InsertNeuronCommand.h \
Gui/NetworkEditorCommands/InsertSynapseCommand.h \
Gui/NetworkEditorCommands/MoveMembersToModuleCommand.h \
Gui/NetworkEditorCommands/RemoveConstraintCommand.h \
Gui/NetworkEditorCommands/RemoveNetworkObjectsCommand.h \
Gui/NetworkEditorCommands/RenameGroupCommand.h \
Gui/NetworkEditorTools/ConstraintDetailWidget.h \
Gui/NetworkEditorTools/GrabNetworkElementIdTool.h \
Gui/NetworkEditorTools/GroupSelectionWidget.h \
Gui/NetworkEditorTools/InsertModuleNetworkTool.h \
Gui/NetworkEditorTools/InsertNeuronNetworkTool.h \
Gui/NetworkEditorTools/InsertSynapseNetworkTool.h \
Gui/NetworkEditorTools/NetworkDetailWidget.h \
Gui/NetworkEditorTools/NetworkManipulationTool.h \
Gui/NetworkEditorTools/NeuralNetworkToolbox.h \
Gui/NetworkEditorTools/NeuronDetailWidget.h \
Gui/NetworkEditorTools/ParameterEditor.h \
Gui/NetworkEditorTools/ParameterPanel.h \
Gui/NetworkEditorTools/PropertyDialog.h \
Gui/NetworkEditorTools/RemoveObjectsNetworkTool.h \
Gui/NetworkEditorTools/RemoveSelectedObjectsNetworkTool.h \
Gui/NetworkEditorTools/SynapseDetailWidget.h \
Gui/NetworkEditor/StandAloneNetworkEditor.h \
Gui/NetworkEditor/EvaluationNetworkEditor.h \
 Collections/NetworkEditorCollection.h \
 Gui/NetworkEditorTools/EditorToolWidget.h \
 Gui/NetworkEditorTools/EditorMessageWidget.h \
 Gui/NetworkEditorTools/RunConstraintsAction.h \
 Gui/NetworkEditorTools/ChangeViewModeTool.h \
 Gui/NetworkEditor/NeuralNetworkObserver.h \
 Gui/NetworkEditorCommands/AlignSynapseCommand.h \
 Gui/NetworkEditorTools/AddActivationPlotterAction.h \
 Gui/Plotter/NeuronActivityPlotter.h \
 Gui/NetworkEditorCommands/SetLocationPropertiesCommand.h \
 PlugIns/ActivationFrequencyCalculator.h \
 Gui/NetworkEditor/ElementPairVisualization.h \
 Gui/NetworkEditorTools/ShowElementPairTool.h \
 Gui/NetworkEditorCommands/CopyPasteNetworkCommand.h \
 Gui/NetworkEditorTools/CopyPasteNetworkTool.h \
 Gui/NetworkEditorTools/ReconnectSynapseNetworkTool.h \
 Gui/NetworkEditorCommands/ReconnectSynapseCommand.h \
 Gui/NetworkEditorTools/ModuleManagementWidget.h \
 Util/InsertSubnetworkIntoNetwork.h \
 Gui/NetworkEditorCommands/InsertSubnetworkCommand.h \
 Util/NetworkEditorUtil.h \
 Gui/NetworkEditorTools/ReplaceModuleTool.h \
 Gui/NetworkEditorCommands/ReplaceModuleCommand.h \
 Gui/NetworkEditorTools/GuessModulePairs.h \
 Gui/NetworkEditor/NetworkSearchDialog.h \
 Gui/NetworkEditorCommands/AlignNeuronsCommand.h \
 Gui/FirstReturnMap/FirstReturnMapPlotter.h \
 Gui/FirstReturnMap/FirstReturnMapPlotterWidget.h \
 Gui/NetworkEditorTools/AddFirstReturnMapPlotterAction.h
SOURCES += Gui/NetworkEditor/GroupItem.cpp \
Gui/NetworkEditor/ModuleItem.cpp \
Gui/NetworkEditor/NetworkVisualization.cpp \
Gui/NetworkEditor/NetworkVisualizationHandler.cpp \
Gui/NetworkEditor/NeuralNetworkEditor.cpp \
Gui/NetworkEditor/NeuronItem.cpp \
Gui/NetworkEditor/PaintItem.cpp \
Gui/NetworkEditor/SelectionRectItem.cpp \
Gui/NetworkEditor/SimpleGroupItem.cpp \
Gui/NetworkEditor/SimpleModuleItem.cpp \
Gui/NetworkEditor/SimpleNetworkVisualizationHandler.cpp \
Gui/NetworkEditor/SimpleNeuronItem.cpp \
Gui/NetworkEditor/SimpleSynapseItem.cpp \
Gui/NetworkEditor/SynapseItem.cpp \
Gui/NetworkEditorCommands/AddConstraintCommand.cpp \
Gui/NetworkEditorCommands/ChangeActivationFunctionCommand.cpp \
Gui/NetworkEditorCommands/ChangeNetworkFunctionCommand.cpp \
Gui/NetworkEditorCommands/ChangeSynapseFunctionCommand.cpp \
Gui/NetworkEditorCommands/ChangeTransferFunctionCommand.cpp \
Gui/NetworkEditorCommands/ChangeValueCommand.cpp \
Gui/NetworkEditorCommands/InsertGroupCommand.cpp \
Gui/NetworkEditorCommands/InsertModuleCommand.cpp \
Gui/NetworkEditorCommands/InsertNeuronCommand.cpp \
Gui/NetworkEditorCommands/InsertSynapseCommand.cpp \
Gui/NetworkEditorCommands/MoveMembersToModuleCommand.cpp \
Gui/NetworkEditorCommands/RemoveConstraintCommand.cpp \
Gui/NetworkEditorCommands/RemoveNetworkObjectsCommand.cpp \
Gui/NetworkEditorCommands/RenameGroupCommand.cpp \
Gui/NetworkEditorTools/ConstraintDetailWidget.cpp \
Gui/NetworkEditorTools/GrabNetworkElementIdTool.cpp \
Gui/NetworkEditorTools/GroupSelectionWidget.cpp \
Gui/NetworkEditorTools/InsertModuleNetworkTool.cpp \
Gui/NetworkEditorTools/InsertNeuronNetworkTool.cpp \
Gui/NetworkEditorTools/InsertSynapseNetworkTool.cpp \
Gui/NetworkEditorTools/NetworkDetailWidget.cpp \
Gui/NetworkEditorTools/NetworkManipulationTool.cpp \
Gui/NetworkEditorTools/NeuralNetworkToolbox.cpp \
Gui/NetworkEditorTools/NeuronDetailWidget.cpp \
Gui/NetworkEditorTools/ParameterEditor.cpp \
Gui/NetworkEditorTools/ParameterPanel.cpp \
Gui/NetworkEditorTools/PropertyDialog.cpp \
Gui/NetworkEditorTools/RemoveObjectsNetworkTool.cpp \
Gui/NetworkEditorTools/RemoveSelectedObjectsNetworkTool.cpp \
Gui/NetworkEditorTools/SynapseDetailWidget.cpp \
Gui/NetworkEditor/StandAloneNetworkEditor.cpp \
Gui/NetworkEditor/EvaluationNetworkEditor.cpp \
 Collections/NetworkEditorCollection.cpp \
 Gui/NetworkEditorTools/EditorToolWidget.cpp \
 Gui/NetworkEditorTools/EditorMessageWidget.cpp \
 Gui/NetworkEditorTools/RunConstraintsAction.cpp \
 Gui/NetworkEditorTools/ChangeViewModeTool.cpp \
 Gui/NetworkEditorCommands/AlignSynapseCommand.cpp \
 Gui/NetworkEditorTools/AddActivationPlotterAction.cpp \
 Gui/Plotter/NeuronActivityPlotter.cpp \
 Gui/NetworkEditorCommands/SetLocationPropertiesCommand.cpp \
 PlugIns/ActivationFrequencyCalculator.cpp \
 Gui/NetworkEditor/ElementPairVisualization.cpp \
 Gui/NetworkEditorTools/ShowElementPairTool.cpp \
 Gui/NetworkEditorCommands/CopyPasteNetworkCommand.cpp \
 Gui/NetworkEditorTools/CopyPasteNetworkTool.cpp \
 Gui/NetworkEditorTools/ReconnectSynapseNetworkTool.cpp \
 Gui/NetworkEditorCommands/ReconnectSynapseCommand.cpp \
 Gui/NetworkEditorTools/ModuleManagementWidget.cpp \
 Util/InsertSubnetworkIntoNetwork.cpp \
 Gui/NetworkEditorCommands/InsertSubnetworkCommand.cpp \
 Util/NetworkEditorUtil.cpp \
 Gui/NetworkEditorTools/ReplaceModuleTool.cpp \
 Gui/NetworkEditorCommands/ReplaceModuleCommand.cpp \
 Gui/NetworkEditorTools/GuessModulePairs.cpp \
 Gui/NetworkEditor/NetworkSearchDialog.cpp \
 Gui/NetworkEditorCommands/AlignNeuronsCommand.cpp \
 Gui/FirstReturnMap/FirstReturnMapPlotter.cpp \
 Gui/FirstReturnMap/FirstReturnMapPlotterWidget.cpp \
 Gui/NetworkEditorTools/AddFirstReturnMapPlotterAction.cpp
DESTDIR = ../../bin


INCLUDEPATH += ../../evolution/evolution \
  ../neuralNetwork \
  ../../system/nerd

