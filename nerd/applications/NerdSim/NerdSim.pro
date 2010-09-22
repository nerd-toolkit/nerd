TEMPLATE = app

SOURCES += main.cpp \
NerdSimApplication.cpp
HEADERS += NerdSimApplication.h

CONFIG -= release

CONFIG += rtti \
opengl \
 debug \
 console
QT += xml \
network \
opengl \
 script










INCLUDEPATH += ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../system/nerd \
  ../../../ode-0.9/include

TARGETDEPS += ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libnerd.a




LIBS += ../../bin/libsimulator.a \
  ../../bin/libnerd.a \
  ../../bin/libodePhysics.a \
  -L../../bin \
  -lsimulator \
  -lnerd \
  -lodePhysics \
  ../../../ode-0.9/lib/libode.a

