TEMPLATE = app

CONFIG += rtti \
opengl \
x11 \
uitools \
 qtestlib \
 exceptions \
 warn_on \
 debug
QT += xml \
network \
opengl

SOURCES += main.cpp \
 Value/MyChangedListener.cpp \
 Value/TestValue.cpp \
 Core/TestParameterizedObject.cpp \
 Value/MyRepositoryChangedListener.cpp \
 Value/TestValueManager.cpp \
 Core/TestCore.cpp \
 Core/MSystemObject.cpp \
 Math/TestVector3D.cpp \
 Event/EventListenerAdapter.cpp \
 Value/TestInterfaceValue.cpp \
 Math/TestQuaternion.cpp \
 Event/TestEventManager.cpp \
 Event/TestEvent.cpp \
 Event/EventManagerAdapter.cpp \
 Value/TestNormalizedDoubleValue.cpp \
 Math/TestMath.cpp \
 Communication/TestUdpDatagram.cpp \
 Util/TestColor.cpp \
 Event/TestTriggerEventTask.cpp \
 Util/TestFileLocker.cpp \
 Math/TestMatrix.cpp



HEADERS += Value/MyChangedListener.h \
Value/TestValue.h \
 Core/TestParameterizedObject.h \
 Value/MyRepositoryChangedListener.h \
 Value/TestValueManager.h \
 Core/TestCore.h \
 Core/MSystemObject.h \
 Math/TestVector3D.h \
 Event/EventListenerAdapter.h \
 Core/ParameterizedObjectAdapter.h \
 Value/TestInterfaceValue.h \
 Math/TestQuaternion.h \
 Event/TestEventManager.h \
 Event/TestEvent.h \
 Event/EventAdapter.h \
 Event/EventManagerAdapter.h \
 Core/TaskAdapter.h \
 Value/TestNormalizedDoubleValue.h \
 Math/TestMath.h \
 Communication/TestUdpDatagram.h \
 Util/TestColor.h \
 Event/TestTriggerEventTask.h \
 Util/TestFileLocker.h \
 Math/TestMatrix.h



DISTFILES += ../orcs/Gui/Parameter









CONFIG -= release

TARGETDEPS += ../../bin/libnerd.a
LIBS += ../../bin/libnerd.a \
  -L../../bin \
  -lnerd

INCLUDEPATH += ../nerd

