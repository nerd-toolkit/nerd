TEMPLATE = app




SOURCES += main.cpp \
Collision/ODE_CollisionHandlerAdapter.cpp \
Collision/Test_ODECollisionHandler.cpp \
Communication/TestSimbaUDPCommunication.cpp \
 Physics/Test_ODESimJoints.cpp \
 Physics/Test_ODEBodies.cpp \
 Physics/Test_ODESimulationAlgorithm.cpp
HEADERS += Collision/ODE_CollisionHandlerAdapter.h \
Collision/Test_ODECollisionHandler.h \
Communication/TestActuator.h \
Communication/TestSimbaUDPCommunication.h \
Physics/SimObjectAdapter.h \
 Physics/Test_ODESimJoints.h \
 Physics/ODE_BodyAdapter.h \
 Physics/Test_ODEBodies.h \
 Physics/Test_ODESimulationAlgorithm.h \
 Physics/ODE_JointAdapter.h
CONFIG += opengl \
qtestlib \
 rtti \
 debug
QT += xml \
network \
opengl








INCLUDEPATH += ../odePhysics \
  ../simulator \
  ../../system/nerd \
  ../../../ode-0.9/include



CONFIG -= release

LIBS += -L../../bin \
  -lodePhysics \
  -lsimulator \
  -lnerd \
  ../../../ode-0.9/lib/libode.a

TARGETDEPS += ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libnerd.a

