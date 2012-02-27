/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Project homepage: nerd.x-bot.org                                      *
 *                                                                         *
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
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
 *   clearly by citing the nerd homepage and the nerd overview paper.      *
 ***************************************************************************/




#include "ASeriesSimbaClientSimulator.h"
#include "Core/Core.h"
#include <QCoreApplication>
#include <QApplication>

using namespace nerd;

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(resources);
	
	Core::resetCore();

	//Start QApplication with or without GUI support.
	bool useGui = true;
	for(int i = 0; i < argc; ++i) {
		if(QString(argv[i]) == "-nogui") {
			useGui = false;
		}
		else if(QString(argv[i]) == "-gui") {
			useGui = true;
		}
	}
	QCoreApplication *app = 0;
	if(useGui) {
		app = new QApplication(argc, argv);
	}
	else {
		app = new QCoreApplication(argc, argv); 
	}

	ASeriesSimbaClientSimulator *nerd = new ASeriesSimbaClientSimulator(argc, argv);
	nerd->startApplication();
	
	app->exec();
	
	Core::getInstance()->waitForAllThreadsToComplete();
	Core::resetCore();

	delete app;

	return 0;
}

