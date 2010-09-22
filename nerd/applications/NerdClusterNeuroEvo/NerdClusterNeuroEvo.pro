TEMPLATE = app


CONFIG -= release


SOURCES += main.cpp \
 NerdClusterNeuroEvoApplication.cpp



HEADERS += NerdClusterNeuroEvoApplication.h

QT += xml \
script \
 svg \
 opengl



CONFIG += debug \
console




INCLUDEPATH += ../../evolution/neuroAndSimEvaluation \
  ../../evolution/neuroEvolution \
  ../../evolution/evolution \
  ../../neuro/networkEditor \
  ../../neuro/neuralNetwork \
  ../../system/nerd \
  ../../../ode-0.9/include







TARGETDEPS += ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libevolution.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

LIBS += ../../bin/libnerd.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libevolution.a \
  -L../../bin \
  -lnetworkEditor \
  -lnerd \
  -levolution \
  -lneuroEvolution \
  -lneuroAndSimEvaluation \
  -lneuralNetwork

