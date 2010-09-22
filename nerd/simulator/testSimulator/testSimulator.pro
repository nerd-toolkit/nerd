TEMPLATE = app

CONFIG += rtti \
opengl \
qtestlib \
 debug \
 console
QT += network \
opengl



SOURCES += main.cpp \
Collision/TestCollisionManager.cpp \
Collision/TestCollisionObject.cpp \
Collision/TestCollisionRule.cpp \
Physics/TestAccelSensor.cpp \
Physics/TestBoxBody.cpp \
Physics/TestPhysicsManager.cpp \
Physics/TestSimBody.cpp \
Physics/TestSimJoint.cpp \
Physics/TestSimObject.cpp \
Physics/TestSphereBody.cpp \
Event/EventListenerAdapter.cpp \
 Randomization/TestRandomizer.cpp \
 Collision/TestMaterialProperties.cpp \
 Physics/TestPhysics.cpp \
 Physics/TestCylinderBody.cpp \
 Physics/TestGeom.cpp \
 Physics/TestSimulationEnvironmentManager.cpp \
 Physics/TestSimObjectGroup.cpp \
 Physics/TestRayAndDistanceSensor.cpp \
 Signal/TestSignals.cpp \
 TestSimulationConstants.cpp
HEADERS += Collision/CollisionHandlerAdapter.h \
Collision/CollisionObjectAdapter.h \
Collision/CollisionRuleAdapter.h \
Collision/TestCollisionManager.h \
Collision/TestCollisionObject.h \
Collision/TestCollisionRule.h \
Communication/TestActuator.h \
Physics/SimBodyAdapter.h \
Physics/SimGeomAdapter.h \
Physics/SimObjectAdapter.h \
Physics/SimObjectGroupAdapter.h \
Physics/TestAccelSensor.h \
Physics/TestBoxBody.h \
Physics/TestPhysicsManager.h \
Physics/TestSimBody.h \
Physics/TestSimJoint.h \
Physics/TestSimObject.h \
Physics/TestSphereBody.h \
Event/EventListenerAdapter.h \
 Randomization/InstallAdapter.h \
 Randomization/RandomizerAdapter.h \
 Randomization/TestRandomizer.h \
 Collision/TestMaterialProperties.h \
 Physics/TestPhysics.h \
 Physics/TestCylinderBody.h \
 Physics/TestGeom.h \
 Physics/TestSimulationEnvironmentManager.h \
 Physics/TestSimObjectGroup.h \
 Physics/TestRayAndDistanceSensor.h \
 Signal/TestSignals.h \
 TestSimulationConstants.h



CONFIG -= release





INCLUDEPATH += ../simulator \
  ../../system/nerd


TARGETDEPS += ../../bin/libsimulator.a \
  ../../bin/libnerd.a


LIBS += ../../bin/libnerd.a \
  ../../bin/libsimulator.a \
  -L../../bin \
  -lsimulator \
  -lnerd

