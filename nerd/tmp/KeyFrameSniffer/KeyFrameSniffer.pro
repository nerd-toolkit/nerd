TEMPLATE = app

CONFIG += rtti

QT += network




SOURCES += KeyFrameSniffer.cpp \
main.cpp
HEADERS += KeyFrameSniffer.h

CONFIG -= release



INCLUDEPATH += ../../simulator/simulator \
  ../../system/nerd

TARGETDEPS += ../../bin/libsimulator.a \
  ../../bin/libnerd.a





LIBS += ../../bin/libnerd.a \
  ../../bin/libsimulator.a \
  -L../../bin \
  -lsimulator \
  -lnerd

