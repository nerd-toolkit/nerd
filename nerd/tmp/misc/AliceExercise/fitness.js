/*
 * For more information on scripted fitness functions see also:
 * https://www.ikw.uni-osnabrueck.de/trac/alear_orcs/wiki/How%20to%20Write%20a%20Scripted%20Fitness%20Function
 */

/*
 * The following variables are already predefined:
 * fitness   -- currently achieved fitness of the individual
 * tryCount  -- number of tries per individual
 * stepCount -- number of simulation steps per try.
 */

/*
 * Associate values and events with local variables.
 */
defEvent("terminated", "/Control/TerminateTry");
defVar("step", "/Control/CurrentStep");
defVar("health", "/Sim/Alice/Sensors/Health/ControlParameter");
defVar("randomizationSeed", "/Simulation/Seed");
defVar("alicePos", "/Sim/Alice/Body/Position");


/******************************************************************************
 * Food stuff                                                                 *
 ******************************************************************************/

var foodTypes = new Array(4);
foodTypes[0] = 0;
foodTypes[1] = 0;
foodTypes[2] = 0;
foodTypes[3] = 0;

var foodStates = new Array(4);
foodStates[0] = 1;
foodStates[1] = 1;
foodStates[2] = 1;
foodStates[3] = 1;

var foodColor = "(150, 150, 180)";

var maxFoodEatDistance = 0.04;
var foodIncrement = 1.0;
var foodDecrement = 0.00005;
var healthChange = 0.01;

defNeuron("eatFoodNeuron", "EatFood");
defVar("food1", "/Sim/Food1/Position");
defVar("food2", "/Sim/Food2/Position");
defVar("food3", "/Sim/Food3/Position");
defVar("food4", "/Sim/Food4/Position");
defVar("food1Light", "/Sim/Food1Light/DesiredBrightness");
defVar("food2Light", "/Sim/Food2Light/DesiredBrightness");
defVar("food3Light", "/Sim/Food3Light/DesiredBrightness");
defVar("food4Light", "/Sim/Food4Light/DesiredBrightness");
defVar("food1TypeLight", "/Sim/Food1TypeLight/DesiredBrightness");
defVar("food2TypeLight", "/Sim/Food2TypeLight/DesiredBrightness");
defVar("food3TypeLight", "/Sim/Food3TypeLight/DesiredBrightness");
defVar("food4TypeLight", "/Sim/Food4TypeLight/DesiredBrightness");
defVar("food1Color", "/Sim/Food1Vis/Color");
defVar("food2Color", "/Sim/Food2Vis/Color");
defVar("food3Color", "/Sim/Food3Vis/Color");
defVar("food4Color", "/Sim/Food4Vis/Color");
defVar("hunger", "/Sim/Alice/Sensors/Hunger/ControlParameter");

function updateFoodChain() {
	hunger += foodDecrement;

	//process health
	if(health > 0.5) {
		health = health - healthChange;
		if(health < 0.5) { health = 0.5; };
	}
	else {
		health = health + healthChange;
		if(health > 0.5) { health = 0.5; };
	}

	//update food type brightness
	//if(foodTypes[0] == 1.0) { food1TypeLight = 1.0; } else { food1TypeLight = 0.5; };
	//if(foodTypes[1] == 1.0) { food2TypeLight = 1.0; } else { food2TypeLight = 0.5; };
	//if(foodTypes[2] == 1.0) { food3TypeLight = 1.0; } else { food3TypeLight = 0.5; };
	//if(foodTypes[3] == 1.0) { food4TypeLight = 1.0; } else { food4TypeLight = 0.5; };
	food1TypeLight = 1.0;
	food2TypeLight = 0.5;
	food3TypeLight = 1.0;
	food4TypeLight = 0.5;

	//check if a food source has to be removed
	checkForFood(food1, 0);
	checkForFood(food2, 1);
	checkForFood(food3, 2);
	checkForFood(food4, 3);

	updateFoodSources();
}

function disableFoodSource(index) {
	for(var i = 0; i < 4; i++) {
		foodStates[i] = 1;
	}
	foodStates[index] = 0;
}

function updateFoodSources() {
	if(foodStates[0] == 1) { food1Light = 1.0; food1Color = foodColor; } else { food1Light = 0.0; food1TypeLight = 0.0; food1Color = "transparent"};
	if(foodStates[1] == 1) { food2Light = 1.0; food2Color = foodColor; } else { food2Light = 0.0; food2TypeLight = 0.0; food2Color = "transparent"};
	if(foodStates[2] == 1) { food3Light = 1.0; food3Color = foodColor; } else { food3Light = 0.0; food3TypeLight = 0.0; food3Color = "transparent"};
	if(foodStates[3] == 1) { food4Light = 1.0; food4Color = foodColor; } else { food4Light = 0.0; food4TypeLight = 0.0; food4Color = "transparent"};
}

function checkForFood(foodPosition, index) {
	if(foodStates[index] == 1 && distance2D(alicePos, foodPosition) <= maxFoodEatDistance) {
		//print("Foood!!", index, " Good? ", foodTypes[index]);
		if(eatFoodNeuron > 0.5) {
			if(foodTypes[index] == 1) {
				health = 1.0;
				hunger -= foodIncrement;
			}
			else {
				health = 0.0;
			}
		}
		disableFoodSource(index);
	}
}

/******************************************************************************
 * Babble stuff                                                               *
 ******************************************************************************/

var babbleIncrement = 0.0002;
var babbleDecrement = 0.003;
var maxFriendBabbleDistance = 0.09;

defVar("babbleDrive", "/Sim/Alice/Sensors/BabbleDrive/ControlParameter");
defVar("friend1", "/Sim/Friend1/Body/Position");
defVar("friend2", "/Sim/Friend2/Body/Position");

function processBabbleDrive() {
	if (distance2D(alicePos, friend1) <= maxFriendBabbleDistance
			|| distance2D(alicePos, friend2) <= maxFriendBabbleDistance) {
		//print("babble");
		babbleDrive -= babbleDecrement;
	} else {
		babbleDrive += babbleIncrement;
	}
}

/******************************************************************************
 * Daylight stuff                                                             *
 ******************************************************************************/

var daylightUp = 1.0;
var daylightChange = 0.0001;

defVar("ambientLight", "/OpenGLVisualization/AmbientLight");
defVar("dayLight", "/Sim/Alice/Sensors/DayLight/ControlParameter");

function processDaylight() {
	if(dayLight > 0.8) {
		daylightUp = -1.0;
	}
	else if(dayLight <= 0.0) {
		daylightUp = 1.0;
	}

	dayLight = dayLight + (daylightChange * daylightUp);
	ambientLight.x = 0.4 + 0.4 * dayLight;
	ambientLight.y = 0.4 + 0.4 * dayLight;
	ambientLight.z = 0.6 + 0.2 * dayLight;
}

/******************************************************************************
 * BadGuys bullying Alice                                                     *
 ******************************************************************************/

var maxBullyDistance = 0.075;
var bullyPain = 0.1;

defVar("badguy1", "/Sim/BadGuy1/Body/Position");
defVar("badguy2", "/Sim/BadGuy2/Body/Position");

function processBadGuys() {
	//print(distance2D(alicePos, badguy1));
	if (distance2D(alicePos, badguy1) <= maxBullyDistance) {
		//print("Ouch, BadGuy1 is mean!");
		health -= bullyPain;
	}
	if (distance2D(alicePos, badguy2) <= maxBullyDistance) {
		//print("Ouch, BadGuy2 is mean!");
		health -= bullyPain;
	}
}

/******************************************************************************
 * Utility functions                                                          *
 ******************************************************************************/

/*
 * Euclidian distance of two objects' positions in the x-z-plane.
 */
function distance2D(a, b) {
	return Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.z - b.z, 2))
}

/*
 * Euclidian distance of two objects' positions in the x-y-z-space.
 */
function distance3D(a, b) {
	return Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2)
			+ Math.pow(a.z - b.z, 2))
}

function resetWonderland() {
	
	//initial food types
	if(randomizationSeed % 2 == 1) {
		foodTypes[0] = 0;
		foodTypes[1] = 1;
		foodTypes[2] = 0;
		foodTypes[3] = 1;
	}
	else {
		foodTypes[0] = 1;
		foodTypes[1] = 0;
		foodTypes[2] = 1;
		foodTypes[3] = 0;
	}

	for(var i = 0; i < 4; i++) {
		foodStates[i] = 1;
	}

	health = 0.5;
	hunger = 0.4;
}

function updateWonderland() {
	processDaylight();
	processBabbleDrive();
	updateFoodChain();
	processBadGuys();
}

/******************************************************************************
 * The usual scripted fitness function API.                                   *
 ******************************************************************************/

function reset() {
	resetWonderland();
}

function calc() {
	updateWonderland();

	return fitness + 1;
}

// vim: noexpandtab
