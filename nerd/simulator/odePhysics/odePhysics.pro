TEMPLATE = lib

CONFIG += rtti \
staticlib \
 debug


DESTDIR = ../../bin

SOURCES += Physics/ODE_Body.cpp \
Physics/ODE_BoxBody.cpp \
Physics/ODE_FixedJoint.cpp \
Physics/ODE_HingeJoint.cpp \
Physics/ODE_Joint.cpp \
Physics/ODE_ServoMotor.cpp \
Physics/ODE_SimulationAlgorithm.cpp \
Physics/ODE_SphereBody.cpp \
Collections/ODE_Physics.cpp \
Physics/ODE_Plane.cpp \
 Collision/ODE_CollisionHandler.cpp \
 Physics/ODE_CylinderBody.cpp \
 Physics/ODE_TorqueDrivenDynamixel.cpp \
 Physics/ODE_Dynamixel.cpp \
 Physics/ODE_SliderJoint.cpp \
 Physics/ODE_SliderMotor.cpp \
 Physics/ODE_CapsuleBody.cpp \
 Physics/ODE_BoxInertiaBody.cpp \
 Physics/ODE_ForceSensor.cpp \
 MotorModels/ODE_DynamixelFrictionMotor.cpp \
 Physics/ODE_UniversalJoint.cpp \
 MotorModels/ODE_FrictionTorqueMotorModel.cpp \
 Physics/ODE_MSeriesSimpleDynamixel.cpp \
 Physics/SimpleToeMotor.cpp \
 MotorModels/ODE_MSeriesTorqueSpringMotorModel.cpp \
 Physics/ODE_ForceHandle.cpp \
 MotorModels/ODE_LinearSpringModel.cpp
HEADERS += Physics/ODE_Body.h \
Physics/ODE_BoxBody.h \
Physics/ODE_FixedJoint.h \
Physics/ODE_HingeJoint.h \
Physics/ODE_Joint.h \
Physics/ODE_ServoMotor.h \
Physics/ODE_SimulationAlgorithm.h \
Physics/ODE_SphereBody.h \
Collections/ODE_Physics.h \
Physics/ODE_Plane.h \
 Collision/ODE_CollisionHandler.h \
 Physics/ODE_CylinderBody.h \
 Physics/ODE_Dynamixel.h \
 Physics/ODE_SliderJoint.h \
 Physics/ODE_SliderMotor.h \
 Physics/ODE_CapsuleBody.h \
 Physics/ODE_BoxInertiaBody.h \
 Physics/ODE_ForceSensor.h \
 Physics/ODE_TorqueDrivenDynamixel.h \
 MotorModels/ODE_DynamixelFrictionMotor.h \
 Physics/ODE_UniversalJoint.h \
 MotorModels/ODE_FrictionTorqueMotorModel.h \
 ODE_MSeriesSimpleDynamixel.h \
 Physics/ODE_MSeriesSimpleDynamixel.h \
 Physics/SimpleToeMotor.h \
 MotorModels/ODE_MSeriesTorqueSpringMotorModel.h \
 Physics/ODE_ForceHandle.h \
 MotorModels/ODE_LinearSpringModel.h

QT += xml


CONFIG -= release

INCLUDEPATH += ../simulator \
  ../../system/nerd \
 ../../../ode-0.9/include





LIBS += -L../../bin \
  -lnerd \
  -lsimulator

TARGETDEPS += ../../bin/libsimulator.a \
  ../../bin/libnerd.a

