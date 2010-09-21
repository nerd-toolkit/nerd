TEMPLATE = lib

CONFIG += staticlib \
debug \
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
script
INCLUDEPATH += ../../evolution/neuroAndSimEvaluation \
  ../../evolution/neuroEvolution \
  ../../evolution/evolution \
  ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../neuro/networkEditor \
  ../../neuro/neuralNetwork \
  ../../system/nerd



SOURCES += Physics/FoodSource.cpp \
Models/JanPrey.cpp \
Collections/JanPrototypes.cpp
HEADERS += Physics/FoodSource.h \
Models/JanPrey.h \
Collections/JanPrototypes.h
CONFIG -= release


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
  ../../../ode-0.9/lib/libode.a \
  -lneuralNetwork \
  -levolution \
  -lneuroEvolution \
  -lnetworkEditor \
  -lneuroAndSimEvaluation \
  -lodePhysics \
  -lsimulator \
  -lnerd

