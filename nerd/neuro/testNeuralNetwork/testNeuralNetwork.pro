TEMPLATE = app

CONFIG += qtestlib \
 debug \
 rtti


QT += xml \
 script

SOURCES += main.cpp \
TransferFunction/TestTransferFunction.cpp \
TransferFunction/TransferFunctionAdapter.cpp \
 SynapseFunction/TestSynapseFunction.cpp \
 ActivationFunction/ActivationFunctionAdapter.cpp \
 ActivationFunction/TestActivationFunction.cpp \
 Network/SynapseAdapter.cpp \
 Network/TestSynapse.cpp \
 SynapseFunction/SynapseFunctionAdapter.cpp \
 Network/NeuronAdapter.cpp \
 Network/TestNeuron.cpp \
 Network/TestNeuralNetworkManager.cpp \
 Network/TestNeuralNetwork.cpp \
 IO/TestNeuralNetworkIO.cpp \
 Network/NeuralNetworkAdapter.cpp \
 Event/EventListenerAdapter.cpp \
 Control/ControlInterfaceAdapter.cpp \
 TestASeriesFunctions.cpp \
 ModularNeuralNetwork/TestNeuronGroup.cpp \
 Constraints/GroupConstraintAdapter.cpp \
 ModularNeuralNetwork/TestNeuroModule.cpp \
 ModularNeuralNetwork/TestModularNeuralNetwork.cpp \
 ModularNeuralNetwork/NeuronGroupAdapter.cpp \
 TestNeuralNetworkConstants.cpp \
 Util/TestNeuralNetworkUtil.cpp \
 Constraints/TestSymmetryConstraint.cpp \
 IO/TestNeuralNetworkIONerdV1Xml.cpp

TARGETDEPS += ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

INCLUDEPATH += ../neuralNetwork \
  ../../system/nerd

HEADERS += TransferFunction/TestTransferFunction.h \
TransferFunction/TransferFunctionAdapter.h \
 SynapseFunction/TestSynapseFunction.h \
 ActivationFunction/ActivationFunctionAdapter.h \
 ActivationFunction/TestActivationFunction.h \
 Network/SynapseAdapter.h \
 Network/TestSynapse.h \
 SynapseFunction/SynapseFunctionAdapter.h \
 Network/NeuronAdapter.h \
 Network/TestNeuron.h \
 Network/TestNeuralNetworkManager.h \
 Network/TestNeuralNetwork.h \
 Network/NeuralNetworkAdapter.h \
 IO/TestNeuralNetworkIO.h \
 Event/EventListenerAdapter.h \
 Control/ControlInterfaceAdapter.h \
 TestASeriesFunctions.h \
 ModularNeuralNetwork/TestNeuronGroup.h \
 Constraints/GroupConstraintAdapter.h \
 ModularNeuralNetwork/TestNeuroModule.h \
 ModularNeuralNetwork/TestModularNeuralNetwork.h \
 ModularNeuralNetwork/NeuronGroupAdapter.h \
 TestNeuralNetworkConstants.h \
 Util/TestNeuralNetworkUtil.h \
 Constraints/TestSymmetryConstraint.h \
 IO/TestNeuralNetworkIONerdV1Xml.h







CONFIG -= release


LIBS += ../../bin/libnerd.a \
  ../../bin/libneuralNetwork.a \
  -L../../bin \
  -lneuralNetwork \
  -lnerd

