TEMPLATE = lib

CONFIG += staticlib \
 rtti \
 debug \
 console

DESTDIR = ../../bin


HEADERS += Application/NeuroAndSimEvaluationBaseApplication.h \
Application/NeuroAndSimEvaluationStandardGuiApplication.h \
 Evaluation/LocalNetworkInSimulationEvaluationMethod.h \
 ClusterEvaluation/ClusterNetworkInSimEvaluationMethod.h \
 Physics/EvolvableParameter.h \
 Collections/EvolutionSimulationPrototypes.h \
 Control/NetworkAgentControlParser.h \
 Gui/SimpleEvolutionMainWindow/SimpleEvolutionMainWindow.h
SOURCES += Application/NeuroAndSimEvaluationBaseApplication.cpp \
Application/NeuroAndSimEvaluationStandardGuiApplication.cpp \
 Evaluation/LocalNetworkInSimulationEvaluationMethod.cpp \
 ClusterEvaluation/ClusterNetworkInSimEvaluationMethod.cpp \
 Physics/EvolvableParameter.cpp \
 Collections/EvolutionSimulationPrototypes.cpp \
 Control/NetworkAgentControlParser.cpp \
 Gui/SimpleEvolutionMainWindow/SimpleEvolutionMainWindow.cpp
QT += opengl \
network \
 xml


CONFIG -= release


INCLUDEPATH += ../neuroEvolution \
  ../evolution \
  ../../simulator/simulator \
  ../../neuro/neuralNetwork \
  ../../system/nerd
