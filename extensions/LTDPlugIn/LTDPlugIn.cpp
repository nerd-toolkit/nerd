

#include "LTDPlugIn.h"
#include "PlugIns/PlugIn.h"
#include "Value/ValueManager.h"
#include "Value/StringValue.h"
#include "Core/Core.h"
#include <Network/Neuro.h>
#include <iostream>


#include "LTD1Function.h"

using namespace std;
using namespace nerd;


QString LTDPlugIn::getName() {
	return "LTDPlugIn";
}


bool LTDPlugIn::install() {
	//Add a new SynapseFunction prototype
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(LTD1Function());
	
	cerr << "installed plugin " << this << endl;
	
	return true;
}


QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(LTDPlugIn, LTDPlugIn)
QT_END_NAMESPACE
