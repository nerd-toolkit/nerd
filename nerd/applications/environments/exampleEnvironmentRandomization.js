

var box1;
var box2;

function createEnvironment() {

	var aseries = model.createObject("ASeries", "ASeries");
	set("Position", "0, 0.1, 0");

	var ground = model.createObject("Ground", "Floor");

	box1 = model.createObject("Box", "Box1");
	set("Position", "(0.1, 0.5, 0.1)");
	box2 = model.createObject("Box", "Box2");
	set("Position", "(-0.1, 0.5, -0.1)");
}

function setupEnvironment() { //add collision rules and randomization

	var colRule = model.createCollisionRule("InvalidCollisions");
	model.setProperty(colRule, "Events", "/Control/TerminateTry");
	model.crAddTarget(colRule, "/Floor");
	model.crAddSource(colRule, "ASeries/.*Head.*"); //regular expression of all body parts in the target list (not including /Sim prefix)
	model.crAddSource(colRule, "ASeries/.*Arm.*");

	//model.setProperty("/ScreenRecorder/Control/Run", "T");
	model.setProperty("/Sim/ASeries/L**/Foot/Mass", "100"); //QScript has problems with ".../**" because it is interpreted as block comment?

}


function randomizeEnvironment() {
	print("Rand!");

	makeCurrent(box1);
	set("Color", "red");
	set("Width", model.random().toString());

	makeCurrent(box2);
	set("Color", model.toColorString(model.randomInt(255), model.randomInt(255), model.randomInt(255), 255));
}
