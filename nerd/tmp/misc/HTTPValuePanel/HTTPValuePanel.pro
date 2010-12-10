TEMPLATE = app

SOURCES += main.cpp \
HTTPValuePanel.cpp \
 HTTPValueAccess.cpp \
 HTTPAuthDialog.cpp \
 HTTPValueVisualization.cpp
HEADERS += HTTPValuePanel.h \
 HTTPValueAccess.h \
 HTTPAuthDialog.h \
 HTTPValueVisualization.h

CONFIG += qt \
 debug
INCLUDEPATH += ../../system/nerd

LIBS += ../../bin/libnerd.a \
  -L../../bin \
  -lnerd

TARGETDEPS += ../../bin/libnerd.a

QT += core \
gui \
network
