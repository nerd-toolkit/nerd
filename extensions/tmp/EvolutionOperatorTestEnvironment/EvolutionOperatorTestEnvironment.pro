SOURCES += main.cpp \
EvolutionOperatorTestApplication.cpp \
FirstParentsTestSelectionMethod.cpp
HEADERS += EvolutionOperatorTestApplication.h \
FirstParentsTestSelectionMethod.h
TEMPLATE = app

CONFIG += rtti \
opengl \
x11 \
console
QT += xml \
network \
opengl \
script \
 svg
INCLUDEPATH += ../../evolution/neuroAndSimEvaluation \
  ../../evolution/neuroEvolution \
  ../../evolution/evolution \
  ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../neuro/networkEditor \
  ../../neuro/neuralNetwork \
  ../../system/nerd





TARGETDEPS += ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libevolution.a \
  ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

LIBS += ../../bin/libsimulator.a \
  ../../bin/libnerd.a \
  ../../bin/libodePhysics.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libevolution.a \
  -L../../bin \
  -lnerd \
  -lsimulator \
  ../../../ode-0.9/lib/libode.a \
  -lodePhysics \
  -lneuralNetwork \
  -levolution \
  -lnetworkEditor \
  -lneuroEvolution \
  -lneuroAndSimEvaluation

