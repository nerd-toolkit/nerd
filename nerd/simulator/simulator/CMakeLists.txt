cmake_minimum_required(VERSION 2.6)
project(nerd_simulator)

set(nerd_simulator_SRCS
	Collision/CollisionHandler.cpp  
	Collision/CollisionManager.cpp  
	Collision/CollisionObject.cpp  
	Collision/CollisionRule.cpp  
	Collision/Contact.cpp  
	Collision/EventCollisionRule.cpp  
	Collision/MaterialProperties.cpp  
	Communication/SeedUDPClientHandler.cpp  
	Communication/SeedUDPCommunication.cpp  
	Communication/SimbaUDPCommunication.cpp  
	Communication/SimpleMotorInterfaceRunner.cpp  
	Communication/SimpleUdpMotorInterface.cpp  
	Execution/SimulationLoop.cpp  
	Gui/ControlWindow.cpp  
	Gui/GuiMainWindow.cpp  
	Gui/Control/MotorControlGui.cpp  
	Gui/Control/MotorControlManager.cpp  
	Gui/Control/MotorControlPanel.cpp  
	Gui/Visualization/OpenGLVisualization.cpp  
	Gui/Visualization/VisualizationChooser.cpp  
	Physics/AccelSensor.cpp  
	Physics/AngleSensor.cpp  
	Physics/BoxBody.cpp  
	Physics/BoxGeom.cpp  
	Physics/FixedJoint.cpp  
	Physics/HingeJoint.cpp  
	Physics/PhysicalSimulationAlgorithm.cpp  
	Physics/Physics.cpp  
	Physics/PhysicsManager.cpp  
	Physics/PlaneBody.cpp  
	Physics/SimBody.cpp  
	Physics/SimGeom.cpp  
	Physics/SimJoint.cpp  
	Physics/SimObject.cpp  
	Physics/SimObjectGroup.cpp  
	Physics/SimulationEnvironmentManager.cpp  
	Physics/SphereBody.cpp  
	Physics/SphereGeom.cpp  
	Physics/TriangleGeom.cpp  
	Physics/WavefrontBody.cpp  
	Physics/WavefrontGeom.cpp  
	PlugIns/KeyFramePlayer.cpp  
	Application/OSP_BaseApplication.cpp  
	Application/ODCP_BaseApplication.cpp  
	Application/ODCP_StandardGuiApplication.cpp  
	Application/OSP_StandardGuiApplication.cpp  
	Physics/CylinderGeom.cpp  
	Physics/CylinderBody.cpp  
	Physics/ServoMotor.cpp  
	Physics/Dynamixel.cpp  
	Physics/SliderJoint.cpp  
	SimulationConstants.cpp  
	Physics/SimSensor.cpp  
	Physics/RayGeom.cpp  
	Collections/TerminateTryCollisionRule.cpp  
	Models/HumanoidModel.cpp  
	Models/ModelInterface.cpp  
	Models/PlaneGround.cpp  
	Collections/ModelPrototypes.cpp  
	Models/Volksbot.cpp  
	Collections/SimulatorPrototypes.cpp  
	Physics/SliderMotor.cpp  
	Physics/SimActuator.cpp  
	Randomization/DoubleRandomizer.cpp  
	Randomization/Randomizer.cpp  
	Randomization/ListRandomizer.cpp  
	Physics/ControlParameter.cpp  
	Physics/ListSwitchControlParameter.cpp  
	Models/ControlParameterModel.cpp  
	Collision/DistanceSensorRule.cpp  
	Models/Khepera.cpp  
	Physics/DistanceRay.cpp  
	Physics/DistanceSensor.cpp  
	PlugIns/SimObjectGroupPrinter.cpp  
	Models/ShakerBoard.cpp  
	Signal/Signal.cpp  
	Signal/SignalDouble.cpp  
	Signal/SignalSinus.cpp  
	Signal/SignalRandom.cpp  
	Physics/LightSource.cpp  
	Physics/LightSensor.cpp  
	Physics/CapsuleGeom.cpp  
	Physics/CapsuleBody.cpp  
	Models/ASeriesUpperBodyPart.cpp  
	Models/Alice.cpp  
	Models/MSeriesV1Model.cpp  
	Models/ASeriesModel.cpp  
	Physics/HingeMotorAdapter.cpp  
	Physics/HingeMotor.cpp  
	Physics/BoxInertiaBody.cpp  
	MotorModels/DynamixelFrictionMotor.cpp  
	MotorModels/DynamixelMotorAdapter.cpp  
	MotorModels/DynamixelMotor.cpp  
	MotorModels/TorqueDynamixelMotorAdapter.cpp  
	MotorModels/TorqueDynamixelMotor.cpp  
	Physics/TorqueDrivenDynamixel.cpp  
	Physics/CurrentConsumptionSensor.cpp  
	MotorModels/MotorModel.cpp  
	Physics/MotorAdapter.cpp  
	Physics/UniversalJointMotorAdapter.cpp  
	Physics/UniversalJoint.cpp  
	MotorModels/FrictionTorqueMotorModel.cpp  
	Physics/InterfaceSimObject.cpp  
	Physics/HingeJointMotorAdapter.cpp  
	MotorModels/MSeriesTorqueSpringMotorModel.cpp  
	MotorModels/H_MSeriesTorqueSpringMotorModel.cpp  
	Models/MSeriesMotorTestModel.cpp  
	Collections/YarsCommunication.cpp  
	Communication/YarsComSimulationAlgorithm.cpp  
	PlugIns/PrototypeOverviewPrinter.cpp  
	Models/ASeriesMorph.cpp  
	Models/MSeriesMotorChainTest.cpp  
	Models/MSeriesMotorMultipleTest.cpp  
	Physics/SinkBox.cpp  
	PlugIns/ModelParameterRotator.cpp  
	Models/SimpleMultiLeggedRobot.cpp  
	Models/DummyModel.cpp  
	Models/InvertedPendulum.cpp  
	Models/MSeriesV2Model.cpp  
	Models/CantileverArm.cpp  
	Physics/ForceHandle.cpp  
	Models/MSeriesV2Model_Obsolete.cpp  
	MotorModels/SpringAdapter.cpp  
	MotorModels/SpringModel.cpp  
	MotorModels/LinearSpringModel.cpp  
	Util/SetSnapshotValueTask.cpp  
	PlugIns/SimpleObjectFileParser.cpp  
	Application/NerdBaseApplication.cpp
	Models/ConfigurablePendulum.cpp
	Models/CustomModel.cpp
	Models/ScriptedModel.cpp
	Collections/ScriptedModelLoader.cpp
	Physics/Gyroscope.cpp
	MotorModels/PassiveActuatorModel.cpp
	MotorModels/PassiveActuatorAdapter.cpp
	MotorModels/PID_PassiveActuatorModel.cpp
	Util/PID_Controller.cpp
	Physics/ActiveDistanceSensor.cpp
)

set(nerd_simulator_MOC_HDRS
	Communication/SeedUDPClientHandler.h  
	Communication/SeedUDPCommunication.h  
	Communication/SimbaUDPCommunication.h  
	Communication/SimpleMotorInterfaceRunner.h  
	Communication/SimpleUdpMotorInterface.h  
	Execution/SimulationLoop.h  
	Gui/ControlWindow.h  
	Gui/GuiMainWindow.h  
	Gui/Control/MotorControlGui.h  
	Gui/Control/MotorControlManager.h  
	Gui/Control/MotorControlPanel.h  
	Gui/Visualization/OpenGLVisualization.h  
	Gui/Visualization/VisualizationChooser.h  
	Application/ODCP_StandardGuiApplication.h  
	Application/OSP_StandardGuiApplication.h  
	Models/ScriptedModel.h
)

set(nerd_simulator_RCS
)

set(nerd_simulator_UIS
)


#include QT extensions
FIND_PACKAGE(Qt4)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTSCRIPT TRUE)
include(${QT_USE_FILE})


#Execute QT Stuff
QT4_WRAP_CPP(nerd_simulator_MOC_SRCS ${nerd_simulator_MOC_HDRS})
QT4_ADD_RESOURCES(nerd_simulator_RC_SRCS ${nerd_simulator_RCS})
QT4_WRAP_UI(nerd_simulator_UI_HDRS ${nerd_simulator_UIS})



#Create library.
add_library(simulator STATIC ${nerd_simulator_SRCS} ${nerd_simulator_MOC_SRCS} ${nerd_simulator_RC_SRCS} ${nerd_simulator_UI_HDRS})

add_definitions(-Wall)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../system/nerd)

