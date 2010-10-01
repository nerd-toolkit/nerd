


#include <QCoreApplication>
#include <QApplication>
#include "Core/Core.h"
#include "NetworkDynamicsPlotterApplication.h"

using namespace nerd;

int main(int argc, char *argv[])
{

	//initialize ressources (compiled images, etc.)
	Q_INIT_RESOURCE(resources);

	Core::resetCore();

	//Start QApplication with or without GUI support.
	bool useGui = true;
	for(int i = 0; i < argc; ++i) {
		if(QString(argv[i]) == "-nogui") {
			useGui = false;
		}
	}
	QCoreApplication *app = 0;
	if(useGui) {
		app = new QApplication(argc, argv);
	}
	else {
		app = new QCoreApplication(argc, argv); 
	}

	NetworkDynamicsPlotterApplication *plotter = new NetworkDynamicsPlotterApplication();
	
	plotter->startApplication();

	app->exec();

	Core::getInstance()->waitForAllThreadsToComplete();

	Core::resetCore();

	delete app;

	return 0;
}

