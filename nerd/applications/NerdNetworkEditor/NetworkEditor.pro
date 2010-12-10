TEMPLATE = app

CONFIG -= release

CONFIG += debug \
rtti \
opengl \
 console
QT += xml \
network \
opengl \
 svg \
 script

SOURCES += main.cpp \
NeuralNetworkEditorApplication.cpp
HEADERS += NeuralNetworkEditorApplication.h





INCLUDEPATH += ../../evolution/evolution \
  ../../neuro/networkEditor \
  ../../neuro/neuralNetwork \
  ../../system/nerd






TARGETDEPS += ../../bin/libevolution.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

LIBS += ../../bin/libnerd.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libevolution.a \
  -L../../bin \
  -lneuralNetwork \
  -lnetworkEditor \
  -levolution \
  -lnerd

