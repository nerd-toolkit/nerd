TEMPLATE = app

CONFIG += qtestlib \
 rtti \
 debug \
 console




SOURCES += main.cpp \
Evolution/TestIndividual.cpp \
Evolution/TestPopulation.cpp \
SelectionMethod/SelectionMethodAdapter.cpp \
Evolution/IndividualAdapter.cpp \
Phenotype/GenotypePhenotypeMapperAdapter.cpp \
 Evolution/TestWorld.cpp \
 Evolution/EvolutionAlgorithmAdapter.cpp \
 Evolution/EvaluationMethodAdapter.cpp \
 Evolution/TestEvolutionManager.cpp \
 Evolution/WorldAdapter.cpp \
 TestConstants.cpp \
 Control/ControlInterfaceAdapter.cpp \
 Fitness/FitnessFunctionAdapter.cpp \
 Fitness/TestFitnessFunction.cpp \
 Fitness/TestFitnessManager.cpp \
 FitnessFunctions/TestScriptedFitnessFunction.cpp
HEADERS += Evolution/TestIndividual.h \
Core/ObjectAdapter.h \
Evolution/TestPopulation.h \
SelectionMethod/SelectionMethodAdapter.h \
Evolution/IndividualAdapter.h \
Phenotype/GenotypePhenotypeMapperAdapter.h \
 Evolution/TestWorld.h \
 Evolution/PopulationAdapter.h \
 Evolution/EvolutionAlgorithmAdapter.h \
 Evolution/EvaluationMethodAdapter.h \
 Evolution/TestEvolutionManager.h \
 Evolution/WorldAdapter.h \
 TestConstants.h \
 Control/ControlInterfaceAdapter.h \
 Fitness/FitnessFunctionAdapter.h \
 Fitness/TestFitnessFunction.h \
 Fitness/TestFitnessManager.h \
 FitnessFunctions/TestScriptedFitnessFunction.h




CONFIG -= release







QT += script


INCLUDEPATH += ../neuroEvolution \
  ../evolution \
  ../../simulator/simulator \
  ../../neuro/neuralNetwork \
  ../../system/nerd




TARGETDEPS += ../../bin/libneuroEvolution.a \
  ../../bin/libevolution.a \
  ../../bin/libsimulator.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

LIBS += ../../bin/libsimulator.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a \
  ../../bin/libevolution.a \
  -L../../bin \
  -lsimulator \
  -lneuroEvolution \
  -lneuralNetwork \
  -levolution \
  -lnerd

