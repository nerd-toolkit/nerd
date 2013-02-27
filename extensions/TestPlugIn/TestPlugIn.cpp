



#include "TestPlugIn.h"
#include "PlugIns/PlugIn.h"
#include "Value/ValueManager.h"
#include "Value/StringValue.h"
#include "Core/Core.h"
#include <iostream>

using namespace std;
using namespace nerd;





QString TestPlugIn::getName() {
	return "TestPlugIn";
}


bool TestPlugIn::install() {
	Core::getInstance()->getValueManager()->addValue("/PlugIn/TestPlugIn/Param", new StringValue("Test Value"));
	cerr << "installed " << this << endl;
	return true;
}


QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(TestPlugIn, TestPlugIn)
QT_END_NAMESPACE
