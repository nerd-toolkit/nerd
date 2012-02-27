cmake_minimum_required(VERSION 2.6)
project(nerd_testNeuralNetwork)

set(nerd_testNeuralNetwork_SRCS
	main.cpp  
	TransferFunction/TestTransferFunction.cpp  
	TransferFunction/TransferFunctionAdapter.cpp  
	SynapseFunction/TestSynapseFunction.cpp  
	ActivationFunction/ActivationFunctionAdapter.cpp  
	ActivationFunction/TestActivationFunction.cpp  
	Network/SynapseAdapter.cpp  
	Network/TestSynapse.cpp  
	SynapseFunction/SynapseFunctionAdapter.cpp  
	Network/NeuronAdapter.cpp  
	Network/TestNeuron.cpp  
	Network/TestNeuralNetworkManager.cpp  
	Network/TestNeuralNetwork.cpp  
	IO/TestNeuralNetworkIO.cpp  
	Network/NeuralNetworkAdapter.cpp  
	Event/EventListenerAdapter.cpp  
	Control/ControlInterfaceAdapter.cpp  
	TestASeriesFunctions.cpp  
	ModularNeuralNetwork/TestNeuronGroup.cpp  
	Constraints/GroupConstraintAdapter.cpp  
	ModularNeuralNetwork/TestNeuroModule.cpp  
	ModularNeuralNetwork/TestModularNeuralNetwork.cpp  
	ModularNeuralNetwork/NeuronGroupAdapter.cpp  
	TestNeuralNetworkConstants.cpp  
	Util/TestNeuralNetworkUtil.cpp  
	Constraints/TestSymmetryConstraint.cpp  
	IO/TestNeuralNetworkIONerdV1Xml.cpp
)


set(nerd_testNeuralNetwork_MOC_HDRS
	TransferFunction/TestTransferFunction.h  
	SynapseFunction/TestSynapseFunction.h  
	ActivationFunction/TestActivationFunction.h  
	Network/TestSynapse.h  
	Network/TestNeuron.h  
	Network/TestNeuralNetworkManager.h  
	Network/TestNeuralNetwork.h  
	IO/TestNeuralNetworkIO.h  
	TestASeriesFunctions.h  
	ModularNeuralNetwork/TestNeuronGroup.h  
	ModularNeuralNetwork/TestNeuroModule.h  
	ModularNeuralNetwork/TestModularNeuralNetwork.h  
	TestNeuralNetworkConstants.h  
	Util/TestNeuralNetworkUtil.h  
	Constraints/TestSymmetryConstraint.h  
	IO/TestNeuralNetworkIONerdV1Xml.h
)

set(nerd_testNeuralNetwork_RCS
)

set(nerd_testNeuralNetwork_UI_HDRS
)


#select QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTTEST TRUE)
set(QT_USE_QTSCRIPT TRUE)
include(${QT_USE_FILE})


#QT Stuff
QT4_WRAP_CPP(nerd_testNeuralNetwork_MOC_SRCS ${nerd_testNeuralNetwork_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_testNeuralNetwork_RC_SRCS ${nerd_testNeuralNetwork_RCS})
QT4_WRAP_UI(nerd_testNeuralNetwork_UI_HDRS ${nerd_testNeuralNetwork_UIS})


#Create library.
add_executable(testNeuralNetwork ${nerd_testNeuralNetwork_SRCS} ${nerd_testNeuralNetwork_MOC_SRCS} ${nerd_testNeuralNetwork_RC_SRCS} ${nerd_testNeuralNetwork_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../neuro/neuralNetwork)

TARGET_LINK_LIBRARIES(testNeuralNetwork
	${CMAKE_CURRENT_BINARY_DIR}/../../neuro/neuralNetwork/libneuralNetwork.a
	${CMAKE_CURRENT_BINARY_DIR}/../../system/nerd/libnerd.a
	${QT_LIBRARIES}
)

