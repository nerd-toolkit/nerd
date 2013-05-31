

#include "IntrinsicPlasticityPlugIn.h"
#include "PlugIns/PlugIn.h"
#include "Value/ValueManager.h"
#include "Value/StringValue.h"
#include "Core/Core.h"
#include <Network/Neuro.h>
#include <Constraints/ConstraintManager.h>
#include <iostream>


#include "IntrinsicPlasticityFunction.h"

using namespace std;
using namespace nerd;


QString IntrinsicPlasticityPlugIn::getName() {
	return "IntrinsicPlasticityPlugIn";
}


bool IntrinsicPlasticityPlugIn::install() {
	
	//Create a new variable in the GVR
//	Core::getInstance()->getValueManager()->addValue("/PlugIn/IntrinsicPlasticityPlugIn/Param", new StringValue("Test Value"));
	
	//Add a new ActivationFunction prototype
	Neuro::getNeuralNetworkManager()->addActivationFunctionPrototype(IntrinsicPlasticityFunction());
	
	cerr << "installed plugin " << this << endl;
	
	return true;
}


QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(IntrinsicPlasticityPlugIn, IntrinsicPlasticityPlugIn)
QT_END_NAMESPACE
