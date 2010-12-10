TEMPLATE = app

CONFIG += console \
qt \
 debug
QT += core \
xml \
 script
INCLUDEPATH += ../../neuro/neuralNetwork \
  ../../system/nerd

TARGETDEPS += ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a


SOURCES += main.cpp

CONFIG -= release



LIBS += ../../bin/libnerd.a \
  ../../bin/libneuralNetwork.a \
  -L../../bin \
  -lneuralNetwork \
  -lnerd

