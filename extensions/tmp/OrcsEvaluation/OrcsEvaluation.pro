SOURCES += main.cpp \
OrcsEvaluationApplication.cpp
HEADERS += OrcsEvaluationApplication.h

TEMPLATE = app


CONFIG += rtti \
opengl \
 console \
 debug
QT += network \
opengl \
xml \
 script \
 svg


























INCLUDEPATH += ../NetworkEditor \
  ../../evolution/neuroAndSimEvaluation \
  ../../evolution/neuroEvolution \
  ../../evolution/evolution \
  ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../neuro/networkEditor \
  ../../neuro/testNeuralNetwork \
  ../../neuro/neuralNetwork \
  ../../system/nerd \
  ../../../ode-0.9/include


CONFIG -= release





TARGETDEPS += ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libevolution.a \
  ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

LIBS += ../../bin/libnetworkEditor.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libsimulator.a \
  ../../bin/libnerd.a \
  ../../bin/libodePhysics.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libevolution.a \
  -L../../bin \
  -lnetworkEditor \
  -levolution \
  -lneuroEvolution \
  -lneuralNetwork \
  -lsimulator \
  -lnerd \
  -lodePhysics \
  ../../../ode-0.9/lib/libode.a

