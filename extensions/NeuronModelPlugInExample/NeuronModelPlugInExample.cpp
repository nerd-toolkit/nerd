



#include "NeuronModelPlugInExample.h"
#include "PlugIns/PlugIn.h"
#include "Value/ValueManager.h"
#include "Value/StringValue.h"
#include "Core/Core.h"
#include <Network/Neuro.h>
#include <Constraints/ConstraintManager.h>
#include <iostream>


#include "ExampleActivationFunction.h"
#include "ExampleSynapseFunction.h"
// #include "ExampleActiveConstraint.h"

using namespace std;
using namespace nerd;





QString NeuronModelPlugInExample::getName() {
	return "NeuronModelPlugInExample";
}


bool NeuronModelPlugInExample::install() {
	
	//Create a new variable in the GVR
	Core::getInstance()->getValueManager()->addValue("/PlugIn/NeuronModelPlugInExample/Param", new StringValue("Test Value"));
	
	//Add a new ActivationFunction prototype
	Neuro::getNeuralNetworkManager()->addActivationFunctionPrototype(ExampleActivationFunction());
	
	//Add a new SynapseFunction prototype
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(ExampleSynapseFunction());
	
// 	//Add a new Cosntraint prototype.
// 	ConstraintManager::getInstance()->addConstraintPrototype(new ExampleActiveConstraint());
	
	cerr << "installed plugin " << this << endl;
	
	return true;
}


QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(NeuronModelPlugInExample, NeuronModelPlugInExample)
QT_END_NAMESPACE
