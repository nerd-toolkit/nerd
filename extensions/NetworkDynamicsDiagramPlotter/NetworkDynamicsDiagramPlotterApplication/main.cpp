/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   tfaber@uni-osnabrueck.de
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Project homepage: nerd.x-bot.org                                      *
 *                                                                         *
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Toolkit is part of the EU project ALEAR                      *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work was funded (2008 - 2011) by EU-Project Number ICT 214856    *
 *                                                                         *
 *                                                                         *
 *   License Agreement:                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   Publications based on work using the NERD kit have to state this      *
 *   clearly by citing the NERD homepage and the NERD overview paper.      *  
 ***************************************************************************/


#include <QCoreApplication>
#include <QApplication>
#include "Core/Core.h"
#include "NetworkDynamicsPlotterApplication.h"

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

using namespace std;
using namespace nerd;

int main(int argc, char *argv[])
{

#ifdef _WIN32
	timeBeginPeriod(1);
#endif

#ifdef Q_WS_X11
	XInitThreads();
#endif
	
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
	plotter->prepareApplication();
	plotter->startApplication();

	app->exec();

	Core::getInstance()->waitForAllThreadsToComplete();

	Core::resetCore();

	delete app;
	
#ifdef _WIN32
	timeEndPeriod(1);
#endif

	Q_CLEANUP_RESOURCE(resources);

	return 0;
}

