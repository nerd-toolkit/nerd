SOURCES += ASeriesDirectPositionControlApplication.cpp \
main.cpp
HEADERS += ASeriesDirectPositionControlApplication.h

TEMPLATE = app

CONFIG += rtti \
opengl \
 debug \
 console
QT += network \
opengl \
xml \
 script


CONFIG -= release


















TARGETDEPS += ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

INCLUDEPATH += ../../evolution/neuroAndSimEvaluation \
  ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../neuro/neuralNetwork \
  ../../system/nerd \
  ../../../ode-0.9/include


LIBS += ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libodePhysics.a \
  ../../bin/libnerd.a \
  ../../bin/libsimulator.a \
  -L../../bin \
  -lneuroAndSimEvaluation \
  -lneuralNetwork \
  -lsimulator \
  -lnerd \
  -lodePhysics \
  ../../../ode-0.9/lib/libode.a

