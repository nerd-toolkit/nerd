cmake_minimum_required(VERSION 2.6)
project(nerd_neuralNetwork)

set(nerd_neuralNetwork_SRCS
	Network/NeuralNetwork.cpp  
	Network/Synapse.cpp  
	Network/Neuron.cpp  
	ActivationFunction/ActivationFunction.cpp  
	TransferFunction/TransferFunction.cpp  
	SynapseFunction/SynapseFunction.cpp  
	ActivationFunction/AdditiveTimeDiscreteActivationFunction.cpp  
	SynapseFunction/SimpleSynapseFunction.cpp  
	TransferFunction/TransferFunctionTanh.cpp  
	ModularNeuralNetwork/ModularNeuralNetwork.cpp  
	Network/Neuro.cpp  
	IO/NeuralNetworkIO.cpp  
	IO/NeuralNetworkIOSimbaV3Xml.cpp  
	Collections/StandardTransferFunctions.cpp  
	Collections/StandardSynapseFunctions.cpp  
	Collections/StandardActivationFunctions.cpp  
	Collections/StandardNeuralNetworkFunctions.cpp  
	NeuralNetworkConstants.cpp  
	IO/NeuralNetworkIOEvosunXml.cpp  
	ModularNeuralNetwork/NeuronGroup.cpp  
	ModularNeuralNetwork/NeuroModule.cpp  
	TransferFunction/TransferFunctionRamp.cpp  
	ActivationFunction/ASeriesActivationFunction.cpp  
	SynapseFunction/ASeriesSynapseFunction.cpp  
	TransferFunction/TransferFunctionASeriesTanh.cpp  
	Math/ASeriesFunctions.cpp  
	Constraints/SymmetryConstraint.cpp  
	Constraints/GroupConstraint.cpp  
	Constraints/NumberOfNeuronsConstraint.cpp  
	Constraints/NumberOfReadNeuronsConstraint.cpp  
	Constraints/ConstraintManager.cpp  
	Constraints/Constraints.cpp  
	Collections/StandardConstraintCollection.cpp  
	IO/NeuralNetworkIOBytecode.cpp  
	Util/LoadNetworkIntoExistingNetwork.cpp  
	ActivationFunction/SignalGeneratorActivationFunction.cpp  
	TransferFunction/TransferFunctionSigmoid.cpp  
	TransferFunction/TransferFunctionParameterizedSigmoid.cpp  
	Constraints/NetworkElementPair.cpp  
	SynapseFunction/AbsoluteValueSynapseFunction.cpp  
	SynapseFunction/MultiplicativeSynapseFunction.cpp  
	ModularNeuralNetwork/NeuroModuleManager.cpp  
	Network/NeuralNetworkManager.cpp  
	Collections/NeuroModuleCollection.cpp  
	Util/NeuralNetworkUtil.cpp  
	TransferFunction/TransferFunctionNeutral.cpp  
	Network/NeuroTagManager.cpp  
	Constraints/PreventMutualConnectionsConstraint.cpp  
	PlugIns/NeuralNetworkAttributes.cpp  
	Util/ReplaceModuleHandler.cpp  
	Network/NeuralNetworkElement.cpp  
	Constraints/SimpleScriptedConstraint.cpp  
	Collections/StandardTagCollection.cpp  
	Constraints/ConnectionSymmetryConstraint.cpp  
	SynapseFunction/ASeriesMultiSynapseFunction.cpp  
	IO/NeuralNetworkIOMSeriesBDN.cpp  
	IO/NeuralNetworkIOASeriesBDN.cpp  
	IO/DebugNeuralNetworkIOMSeriesComponentsBDN.cpp  
	Constraints/WeightAndBiasCalculatorConstraint.cpp  
	Script/ScriptedNetworkManipulator.cpp  
	IO/NeuralNetworkBDNExporter.cpp  
	Collections/UniversalNeuroScriptLoader.cpp  
	Script/UniversalNeuroScriptingContext.cpp  
	IO/NeuralNetworkIONerdV1Xml.cpp
	Constraints/FeedForwardConstraint.cpp
	ActivationFunction/DelayLineActivationFunction.cpp
	Learning/Backpropagation/Backpropagation.cpp
	Constraints/BackpropagationConstraint.cpp
	ActivationFunction/ChaoticNeuronActivationFunction.cpp
)

set(nerd_neuralNetwork_MOC_HDRS
	Script/ScriptedNetworkManipulator.h  
)

set(nerd_neuralNetwork_RCS
)

set(nerd_neuralNetwork_UIS
)


#include QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTXML TRUE)
include(${QT_USE_FILE})


#Execute QT Stuff
QT4_WRAP_CPP(nerd_neuralNetwork_MOC_SRCS ${nerd_neuralNetwork_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_neuralNetwork_RC_SRCS ${nerd_neuralNetwork_RCS})
QT4_WRAP_UI(nerd_neuralNetwork_UI_HDRS ${nerd_neuralNetwork_UIS})



#Create library.
add_library(neuralNetwork STATIC ${nerd_neuralNetwork_SRCS} ${nerd_neuralNetwork_MOC_SRCS} ${nerd_neuralNetwork_RC_SRCS} ${nerd_neuralNetwork_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)


