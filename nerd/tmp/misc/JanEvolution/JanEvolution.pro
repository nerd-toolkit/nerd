TEMPLATE = app

CONFIG += debug \
stl \
rtti \
qt \
opengl \
thread \
x11 \
warn_on \
qtestlib \
uitools \
 console
QT += core \
gui \
xml \
network \
opengl \
script \
 svg
INCLUDEPATH += ../Nehring \
  ../../evolution/neuroAndSimEvaluation \
  ../../evolution/neuroEvolution \
  ../../evolution/evolution \
  ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../neuro/networkEditor \
  ../../neuro/neuralNetwork \
  ../../system/nerd


SOURCES += main.cpp \
JanEvolution.cpp
HEADERS += JanEvolution.h


CONFIG -= release




TARGETDEPS += ../Nehring/libNehring.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libevolution.a \
  ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

LIBS += ../Nehring/libNehring.a \
  ../../bin/libsimulator.a \
  ../../bin/libnerd.a \
  ../../bin/libodePhysics.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libevolution.a \
  -L../../bin \
  ../../misc/Nehring/libNehring.a \
  -levolution \
  -lnetworkEditor \
  -lneuralNetwork \
  -lneuroAndSimEvaluation \
  -lodePhysics \
  -lsimulator \
  -lneuroEvolution \
  -lnerd \
  ../../../ode-0.9/lib/libode.a

