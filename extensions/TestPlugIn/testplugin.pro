SOURCES += TestPlugIn.cpp \
main.cpp
HEADERS += TestPlugIn.h

TEMPLATE = lib

CONFIG += dll \
plugin \
debug \
rtti \
opengl
VERSION = 1

CONFIG -= release

QT += sql \
svg \
xml \
network \
opengl \
script
INCLUDEPATH += ../../nerd/evolution/evolution/ \
  ../../nerd/evolution/neuroEvolution/ \
  ../..//nerd/evolution/neuroAndSimEvaluation \
  ../../nerd/neuro/neuralNetwork/ \
  ../../nerd/simulator/odePhysics/ \
  ../../nerd/simulator/simulator \
  ../../nerd/system/nerd

