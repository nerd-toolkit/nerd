TEMPLATE = lib

CONFIG += staticlib \
 rtti \
 debug \
 opengl \
 console



DESTDIR = ../../bin

SOURCES += NeuralNetworkManipulationChain/NeuralNetworkManipulationChainAlgorithm.cpp \
 NeuralNetworkManipulationChain/NeuralNetworkManipulationOperator.cpp \
 NeuralNetworkManipulationChain/CreateNetworkOperator.cpp \
 NeuralNetworkManipulationChain/CloneNetworkOperator.cpp \
 Collections/ENS3EvolutionAlgorithm.cpp \
 NeuralNetworkManipulationChain/InsertNeuronOperator.cpp \
 NeuralNetworkManipulationChain/InsertSynapseOperator.cpp \
 NeuralNetworkManipulationChain/InitializeSynapsesOperator.cpp \
 NeuralNetworkManipulationChain/InitializeBiasOperator.cpp \
 NeuralNetworkManipulationChain/ChangeBiasOperator.cpp \
 NeuralNetworkManipulationChain/ChangeSynapseStrengthOperator.cpp \
 NeuralNetworkManipulationChain/RemoveNeuronOperator.cpp \
 NeuralNetworkManipulationChain/RemoveSynapseOperator.cpp \
 Statistics/BasicNeuralNetworkStatistics.cpp \
 Statistics/IndividualStatistics/HiddenNeuronStatistic.cpp \
 NeuralNetworkManipulationChain/ResolveConstraintsOperator.cpp \
 Collections/NeuralNetworkIndividualStatistics.cpp \
 NeuralNetworkManipulationChain/OperatorSorter.cpp \
 PlugIns/SaveBestNetworksHandler.cpp \
 NeuralNetworkManipulationChain/ConnectNeuronClassesFilter.cpp \
 NeuralNetworkManipulationChain/EnableSynapseOperator.cpp \
 Collections/NeuroEvolutionSelector.cpp \
 Neat/NeatEvolutionAlgorithm.cpp \
 Neat/NeatGenome.cpp \
 Neat/NeatGenotypePhenotypeMapper.cpp \
 Neat/NeatSelectionMethod.cpp \
 Neat/NeatConstants.cpp \
 Collections/NeatAlgorithm.cpp \
 NeuroEvolutionConstants.cpp \
 NeuralNetworkManipulationChain/ModuleCrossOverOperator.cpp \
 Collections/ModularNeuroEvolution1.cpp \
 NeuralNetworkManipulationChain/InsertSynapseModularOperator.cpp \
 NeuralNetworkManipulationChain/InsertNeuroModuleOperator.cpp \
 Statistics/IndividualStatistics/NetworkStructureStatistics.cpp \
 NeuralNetworkManipulationChain/InsertBiasOperator.cpp \
 NeuralNetworkManipulationChain/RemoveBiasOperator.cpp \
 Collections/NeuroFitnessPrototypes.cpp \
 FitnessFunctions/ScriptedNeuroFitnessFunction.cpp

HEADERS += NeuralNetworkManipulationChain/NeuralNetworkManipulationChainAlgorithm.h \
 NeuralNetworkManipulationChain/NeuralNetworkManipulationOperator.h \
 NeuralNetworkManipulationChain/CreateNetworkOperator.h \
 NeuralNetworkManipulationChain/CloneNetworkOperator.h \
 Collections/ENS3EvolutionAlgorithm.h \
 NeuralNetworkManipulationChain/InsertNeuronOperator.h \
 NeuralNetworkManipulationChain/InsertSynapseOperator.h \
 NeuralNetworkManipulationChain/InitializeSynapsesOperator.h \
 NeuralNetworkManipulationChain/InitializeBiasOperator.h \
 NeuralNetworkManipulationChain/ChangeBiasOperator.h \
 NeuralNetworkManipulationChain/ChangeSynapseStrengthOperator.h \
 NeuralNetworkManipulationChain/RemoveNeuronOperator.h \
 NeuralNetworkManipulationChain/RemoveSynapseOperator.h \
 Statistics/BasicNeuralNetworkStatistics.h \
 Statistics/IndividualStatistics/HiddenNeuronStatistic.h \
 NeuralNetworkManipulationChain/ResolveConstraintsOperator.h \
 Collections/NeuralNetworkIndividualStatistics.h \
 NeuralNetworkManipulationChain/OperatorSorter.h \
 PlugIns/SaveBestNetworksHandler.h \
 NeuralNetworkManipulationChain/ConnectNeuronClassesFilter.h \
 NeuralNetworkManipulationChain/EnableSynapseOperator.h \
 Collections/NeuroEvolutionSelector.h \
 Neat/NeatEvolutionAlgorithm.h \
 Neat/NeatGenome.h \
 Neat/NeatGenotypePhenotypeMapper.h \
 Neat/NeatSelectionMethod.h \
 Neat/NeatSpeciesOrganism.h \
 Neat/NeatConstants.h \
 Collections/NeatAlgorithm.h \
 NeuroEvolutionConstants.h \
 NeuralNetworkManipulationChain/ModuleCrossOverOperator.h \
 Collections/ModularNeuroEvolution1.h \
 NeuralNetworkManipulationChain/InsertSynapseModularOperator.h \
 NeuralNetworkManipulationChain/InsertNeuroModuleOperator.h \
 Statistics/IndividualStatistics/NetworkStructureStatistics.h \
 NeuralNetworkManipulationChain/InsertBiasOperator.h \
 NeuralNetworkManipulationChain/RemoveBiasOperator.h \
 Collections/NeuroFitnessPrototypes.h \
 FitnessFunctions/ScriptedNeuroFitnessFunction.h

QT += xml \
 opengl \
 script









CONFIG -= release









INCLUDEPATH += ../evolution \
  ../../neuro/networkEditor \
  ../../neuro/neuralNetwork \
  ../../system/nerd

TARGETDEPS += ../../bin/libevolution.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

LIBS += ../../bin/libevolution.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a \
  ../../bin/libnetworkEditor.a

