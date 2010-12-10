SOURCES += main.cpp \
LocalNNSimulatorEvolutionApplication.cpp \
 AutoStoreNetworksHelper.cpp
HEADERS += LocalNNSimulatorEvolutionApplication.h \
 AutoStoreNetworksHelper.h

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













INCLUDEPATH += ../../evolution/neuroAndSimEvaluation \
  ../../evolution/neuroEvolution \
  ../../evolution/evolution \
  ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../neuro/networkEditor \
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

LIBS += ../../bin/libsimulator.a \
  ../../bin/libnerd.a \
  ../../bin/libodePhysics.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libevolution.a \
  ../../bin/libnetworkEditor.a \
  -L../../bin \
  -lnetworkEditor \
  ../../../ode-0.9/lib/libode.a \
  -lsimulator \
  -lnerd \
  -lodePhysics \
  -lneuroEvolution \
  -lneuroAndSimEvaluation \
  -lneuralNetwork \
  -levolution

