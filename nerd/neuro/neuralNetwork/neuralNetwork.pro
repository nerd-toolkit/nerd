TEMPLATE = lib

CONFIG += staticlib \
qt \
 warn_on \
 rtti \
 debug
CONFIG -= thread \
 release

INCLUDEPATH += ../../system/nerd

TARGETDEPS += ../../bin/libnerd.a

DESTDIR = ../../bin

HEADERS += Network/NeuralNetwork.h \
Network/SynapseTarget.h \
Network/Synapse.h \
Network/Neuron.h \
 ActivationFunction/ActivationFunction.h \
 TransferFunction/TransferFunction.h \
 SynapseFunction/SynapseFunction.h \
 ActivationFunction/AdditiveTimeDiscreteActivationFunction.h \
 SynapseFunction/SimpleSynapseFunction.h \
 TransferFunction/TransferFunctionTanh.h \
 ModularNeuralNetwork/ModularNeuralNetwork.h \
 Network/Neuro.h \
 Network/NeuralNetworkManager.h \
 IO/NeuralNetworkIO.h \
 IO/NeuralNetworkIOSimbaV3Xml.h \
 Collections/StandardTransferFunctions.h \
 Collections/StandardSynapseFunctions.h \
 Collections/StandardActivationFunctions.h \
 Collections/StandardNeuralNetworkFunctions.h \
 NeuralNetworkConstants.h \
 IO/NeuralNetworkIOEvosunXml.h \
 ModularNeuralNetwork/NeuronGroup.h \
 ModularNeuralNetwork/NeuroModule.h \
 TransferFunction/TransferFunctionRamp.h \
 ActivationFunction/ASeriesActivationFunction.h \
 SynapseFunction/ASeriesSynapseFunction.h \
 TransferFunction/TransferFunctionASeriesTanh.h \
 Math/ASeriesFunctions.h \
 Network/NeuralNetworkElement.h \
 Constraints/SymmetryConstraint.h \
 Constraints/GroupConstraint.h \
 Constraints/NumberOfNeuronsConstraint.h \
 Constraints/NumberOfReadNeuronsConstraint.h \
 Constraints/ConstraintManager.h \
 Constraints/Constraints.h \
 Collections/StandardConstraintCollection.h \
 IO/NeuralNetworkIOBytecode.h \
 Util/LoadNetworkIntoExistingNetwork.h \
 ActivationFunction/SignalGeneratorActivationFunction.h \
 TransferFunction/TransferFunctionSigmoid.h \
 TransferFunction/TransferFunctionParameterizedSigmoid.h \
 Constraints/NetworkElementPair.h \
 SynapseFunction/AbsoluteValueSynapseFunction.h \
 SynapseFunction/MultiplicativeSynapseFunction.h \
 ModularNeuralNetwork/NeuroModuleManager.h \
 Collections/NeuroModuleCollection.h \
 Util/NeuralNetworkUtil.h \
 TransferFunction/TransferFunctionNeutral.h \
 Network/NeuroTagManager.h \
 Constraints/PreventMutualConnectionsConstraint.h \
 PlugIns/NeuralNetworkAttributes.h \
 Util/ReplaceModuleHandler.h \
 Constraints/SimpleScriptedConstraint.h \
 Collections/StandardTagCollection.h \
 SynapseFunction/ASeriesMultiSynapseFunction.h \
 Constraints/ConnectionSymmetryConstraint.h \
 IO/NeuralNetworkIOMSeriesBDN.h \
 IO/NeuralNetworkIOASeriesBDN.h \
 IO/DebugNeuralNetworkIOMSeriesComponentsBDN.h\
 Constraints/WeightAndBiasCalculatorConstraint.h \
 Script/ScriptedNetworkManipulator.h \
 IO/NeuralNetworkBDNExporter.h \
 Collections/UniversalNeuroScriptLoader.h \
 Script/UniversalNeuroScriptingContext.h \
 IO/NeuralNetworkIONerdV1Xml.h


SOURCES += Network/NeuralNetwork.cpp \
Network/Synapse.cpp \
Network/Neuron.cpp \
 ActivationFunction/ActivationFunction.cpp \
 TransferFunction/TransferFunction.cpp \
 SynapseFunction/SynapseFunction.cpp \
 ActivationFunction/AdditiveTimeDiscreteActivationFunction.cpp \
 SynapseFunction/SimpleSynapseFunction.cpp \
 TransferFunction/TransferFunctionTanh.cpp \
 ModularNeuralNetwork/ModularNeuralNetwork.cpp \
 Network/Neuro.cpp \
 IO/NeuralNetworkIO.cpp \
 IO/NeuralNetworkIOSimbaV3Xml.cpp \
 Collections/StandardTransferFunctions.cpp \
 Collections/StandardSynapseFunctions.cpp \
 Collections/StandardActivationFunctions.cpp \
 Collections/StandardNeuralNetworkFunctions.cpp \
 NeuralNetworkConstants.cpp \
 IO/NeuralNetworkIOEvosunXml.cpp \
 ModularNeuralNetwork/NeuronGroup.cpp \
 ModularNeuralNetwork/NeuroModule.cpp \
 TransferFunction/TransferFunctionRamp.cpp \
 ActivationFunction/ASeriesActivationFunction.cpp \
 SynapseFunction/ASeriesSynapseFunction.cpp \
 TransferFunction/TransferFunctionASeriesTanh.cpp \
 Math/ASeriesFunctions.cpp \
 Constraints/SymmetryConstraint.cpp \
 Constraints/GroupConstraint.cpp \
 Constraints/NumberOfNeuronsConstraint.cpp \
 Constraints/NumberOfReadNeuronsConstraint.cpp \
 Constraints/ConstraintManager.cpp \
 Constraints/Constraints.cpp \
 Collections/StandardConstraintCollection.cpp \
 IO/NeuralNetworkIOBytecode.cpp \
 Util/LoadNetworkIntoExistingNetwork.cpp \
 ActivationFunction/SignalGeneratorActivationFunction.cpp \
 TransferFunction/TransferFunctionSigmoid.cpp \
 TransferFunction/TransferFunctionParameterizedSigmoid.cpp \
 Constraints/NetworkElementPair.cpp \
 SynapseFunction/AbsoluteValueSynapseFunction.cpp \
 SynapseFunction/MultiplicativeSynapseFunction.cpp \
 ModularNeuralNetwork/NeuroModuleManager.cpp \
 Network/NeuralNetworkManager.cpp \
 Collections/NeuroModuleCollection.cpp \
 Util/NeuralNetworkUtil.cpp \
 TransferFunction/TransferFunctionNeutral.cpp \
 Network/NeuroTagManager.cpp \
 Constraints/PreventMutualConnectionsConstraint.cpp \
 PlugIns/NeuralNetworkAttributes.cpp \
 Util/ReplaceModuleHandler.cpp \
 Network/NeuralNetworkElement.cpp \
 Constraints/SimpleScriptedConstraint.cpp \
 Collections/StandardTagCollection.cpp \
 Constraints/ConnectionSymmetryConstraint.cpp \
 SynapseFunction/ASeriesMultiSynapseFunction.cpp \
 IO/NeuralNetworkIOMSeriesBDN.cpp \
 IO/NeuralNetworkIOASeriesBDN.cpp \
 IO/DebugNeuralNetworkIOMSeriesComponentsBDN.cpp \
 Constraints/WeightAndBiasCalculatorConstraint.cpp \
 Script/ScriptedNetworkManipulator.cpp \
 IO/NeuralNetworkBDNExporter.cpp \
 Collections/UniversalNeuroScriptLoader.cpp \
 Script/UniversalNeuroScriptingContext.cpp \
 IO/NeuralNetworkIONerdV1Xml.cpp


QT += xml \
 script



