TEMPLATE = app

CONFIG -= release

CONFIG += rtti \
opengl \
 debug \
 console
QT += xml \
network \
opengl \
 script



SOURCES += ASeriesSimbaClientSimulator.cpp \
main.cpp
HEADERS += ASeriesSimbaClientSimulator.h










INCLUDEPATH += ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../system/nerd \
  ../../../ode-0.9/include


TARGETDEPS += ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libnerd.a

LIBS += ../../bin/libodePhysics.a \
  ../../bin/libnerd.a \
  ../../bin/libsimulator.a \
  -L../../bin \
  -lodePhysics \
  -lsimulator \
  -lnerd \
  ../../../ode-0.9/lib/libode.a

