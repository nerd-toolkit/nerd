TEMPLATE = app

CONFIG -= release

CONFIG += debug \
rtti \
opengl \
x11 \
qtestlib \
console
QT += xml \
network \
opengl \
script
INCLUDEPATH += ../OrcsModelOptimizerLib \
  ../../evolution/neuroAndSimEvaluation \
  ../../evolution/neuroEvolution \
  ../../evolution/evolution \
  ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../neuro/networkEditor \
  ../../neuro/neuralNetwork \
  ../../system/nerd

SOURCES += main.cpp \
OrcsModelOptimizerApplication.cpp
HEADERS += OrcsModelOptimizerApplication.h




TARGETDEPS += ../../bin/libOrcsModelOptimizerLib.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libevolution.a \
  ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

LIBS += ../../bin/libevolution.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libodePhysics.a \
  ../../bin/libnerd.a \
  ../../bin/libsimulator.a \
  ../../bin/libOrcsModelOptimizerLib.a \
  -L../../bin \
  ../../../ode-0.9/lib/libode.a \
  -levolution \
  -lnetworkEditor \
  -lneuralNetwork \
  -lneuroAndSimEvaluation \
  -lneuroEvolution \
  -lodePhysics \
  -lnerd \
  -lsimulator \
  -lOrcsModelOptimizerLib

