



#include "TestPlugIn.h"
#include "Value/ValueManager.h"
#include "Value/StringValue.h"
#include "Core/Core.h"
#include <iostream>

using namespace std;
using namespace nerd;




TestPlugIn::TestPlugIn()
{
	cerr << "Create Plugin" << endl;
}

TestPlugIn::~TestPlugIn() {
	cerr << "Delete plugin" << endl;
}

QString TestPlugIn::getName() {
	return "TestPlugIn";
}


bool TestPlugIn::install() {
	Core::getInstance()->getValueManager()->addValue("/PlugIn/TestPlugIn", new StringValue("hi"));
	cerr << "install " << this << endl;
	return true;
}

Q_EXPORT_PLUGIN2(pTestPlugIn, TestPlugIn)

