TEMPLATE = app

CONFIG += rtti \
qtestlib \
 debug \
 console
QT += xml \
network



SOURCES += main.cpp \
 NeuralNetworkManipulationChain/TestNetworkManipulationChainAlgorithm.cpp \
 NeuralNetworkManipulationChain/NeuralNetworkManipulationOperatorAdapter.cpp \
 Evolution/WorldAdapter.cpp \
 Evolution/IndividualAdapter.cpp \
 TestNeuroEvolutionConstants.cpp

HEADERS += NeuralNetworkManipulationChain/TestNetworkManipulationChainAlgorithm.h \
 NeuralNetworkManipulationChain/NeuralNetworkManipulationOperatorAdapter.h \
 Evolution/PopulationAdapter.h \
 Evolution/WorldAdapter.h \
 Evolution/IndividualAdapter.h \
 TestNeuroEvolutionConstants.h


CONFIG -= release




INCLUDEPATH += ../neuroEvolution \
  ../evolution \
  ../../neuro/neuralNetwork \
  ../../system/nerd



TARGETDEPS += ../../bin/libneuroEvolution.a \
  ../../bin/libevolution.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

LIBS += ../../bin/libnerd.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libevolution.a \
  -L../../bin \
  -lneuroEvolution \
  -lneuralNetwork \
  -levolution \
  -lnerd

