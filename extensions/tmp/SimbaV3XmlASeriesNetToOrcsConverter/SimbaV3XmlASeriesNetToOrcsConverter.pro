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

LIBS += ../../bin/libnerd.a \
  ../../bin/libneuralNetwork.a \
  -L../../bin \
  -lneuralNetwork \
  -lnerd

