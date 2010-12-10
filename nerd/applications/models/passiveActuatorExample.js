

model.prototypeName = "PassiveActuatorTest";

function createModel() {

	var body1 = model.createObject("Box", "Body1");
	set("Width", "0.1");
	set("Height", "0.05");
	set("Depth", "0.05");
	set("Position", "(0,0.1,0)");

	var body2 = model.copyObject(body1, "Body2");
	set("Position", "(0.2, 0.1, 0.0)");

	var body3 = model.copyObject(body1, "Body3");
	set("Position", "(0.4, 0.1, 0.0)");

	makeCurrent(body1);
	set("Dynamic", "F");
	set("Color", "red");

	var dynamixel = model.createObject("DynamixelMotorAdapter", "Dynamixel");
	set("FirstBody", model.modelName.concat("/", "Body1"));
	set("SecondBody", model.modelName.concat("/", "Body2"));
	set("AxisPoint1", "(0.1, 0.1, 0.0)");
	set("AxisPoint2", "(0.1, -0.1, 0.0)");

	var passiveHinge = model.createObject("PassiveHingeActuatorAdapter", "PassiveHinge");
	print("Body1: ", model.modelName.concat("/", "Body2"));
	set("FirstBody", model.modelName.concat("/", "Body2"));
	set("SecondBody", model.modelName.concat("/", "Body3"));
	set("ReferenceAngleName", "/Sim/".concat(model.modelName, "/Dynamixel/JointAngle"));
	set("AxisPoint1", "(0.3, 0.1, 0.0)");
	set("AxisPoint2", "(0.3, -0.1, 0.0)");
	

}

function setupModel() {

}

function createEnvironment() {

	model.createObject("Ground", "Floor");
	model.createObject("PassiveActuatorTest", "Test");
}
