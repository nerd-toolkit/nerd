#include "VisionASeries.h"
#include "PlugIns/PlugIn.h"
#include "Core/Core.h"
#include <iostream>
#include "Physics/Physics.h"

#include "VisionASeriesSensor.h"


using namespace std;
using namespace nerd;

QString VisionASeries::getName() {
	return "VisionASeries";
}


bool VisionASeries::install() {

    //Add VisionASeries Sensor to Prototypemanager
    Physics::getPhysicsManager()->addPrototype("Prototypes/VisionASeriesSensor", new VisionASeriesSensor("VisionASeries") );

	cerr << "installed plugin " << this << endl;
	
	return true;
}


QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2(VisionASeries, VisionASeries)
QT_END_NAMESPACE
