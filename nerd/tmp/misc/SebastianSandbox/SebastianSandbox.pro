SOURCES += main.cpp \
  KheperaApplication.cpp
HEADERS += KheperaApplication.h

TEMPLATE = app

CONFIG += rtti \
opengl \
x11 \
 debug \
 console
QT += xml \
network \
opengl


CONFIG -= release






INCLUDEPATH += ../../evolution/neuroAndSimEvaluation \
  ../../evolution/neuroEvolution \
  ../../evolution/evolution \
  ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../neuro/neuralNetwork \
  ../../system/nerd \
  ../../../ode-0.9/include


TARGETDEPS += ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libevolution.a \
  ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

LIBS += ../../bin/libevolution.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libodePhysics.a \
  ../../bin/libnerd.a \
  ../../bin/libsimulator.a \
  -L../../bin \
  ../../../ode-0.9/lib/libode.a \
  -levolution \
  -lneuralNetwork \
  -lneuroAndSimEvaluation \
  -lneuroEvolution \
  -lodePhysics \
  -lnerd \
  -lsimulator

