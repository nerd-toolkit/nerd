TEMPLATE = app

SOURCES += main.cpp

CONFIG += debug \
qt \
console \
warn_on
QT += core \
xml \
 script
INCLUDEPATH += ../../neuro/neuralNetwork \
  ../../system/nerd

TARGETDEPS += ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a



CONFIG -= release

LIBS += ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a \
  -L../../bin \
  -lnerd \
  -lneuralNetwork

