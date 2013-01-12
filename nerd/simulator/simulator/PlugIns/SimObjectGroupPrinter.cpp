/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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



#include "SimObjectGroupPrinter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "PlugIns/CommandLineArgument.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SimObjectGroupPrinter.
 */
SimObjectGroupPrinter::SimObjectGroupPrinter()
{

	mCommandLineArgument = new CommandLineArgument(
		"groups", "groups", "", "Prints all registered SimObjectGroups.",
		0, 0, false, false);
	mCommandLineArgument->setProperty("debug");

	Core::getInstance()->scheduleTask(this);
}


/**
 * Destructor.
 */
SimObjectGroupPrinter::~SimObjectGroupPrinter() {
}


bool SimObjectGroupPrinter::runTask() {
	if(mCommandLineArgument == 0) {
		return false;
	}
	if(mCommandLineArgument->getNumberOfEntries() > 0) {
		cerr << endl << "SimObject Overview: " << endl 
			 << "--------------------------------------------------------" << endl;

		QList<SimObjectGroup*> groups = Physics::getPhysicsManager()->getSimObjectGroups();
		for(QListIterator<SimObjectGroup*> i(groups); i.hasNext();) {
			SimObjectGroup *group = i.next();
			cerr << "SimObjectGroup [" << group->getName().toStdString().c_str() << "]" << endl;

			cerr << " ---------------" << endl << " Input Values:" << endl << " ---------------" << endl;
			QList<InterfaceValue*> inputValues = group->getInputValues();
			for(QListIterator<InterfaceValue*> j(inputValues); j.hasNext();) {
				InterfaceValue *value = j.next();
				cerr << "  " << value->getName().toStdString().c_str() << " [" 
					 << value->getProperties().toStdString().c_str() << "]" << endl;
			}
			cerr << " ---------------" << endl << " Output Values:" << endl << " ---------------" << endl;
			QList<InterfaceValue*> outputValues = group->getOutputValues();
			for(QListIterator<InterfaceValue*> j(outputValues); j.hasNext();) {
				InterfaceValue *value = j.next();
				cerr << "  " << value->getName().toStdString().c_str() << " [" 
					 << value->getProperties().toStdString().c_str() << "]" << endl;
			}
			cerr << " ---------------" << endl << " Info Values:" << endl << " ---------------" << endl;
			QList<InterfaceValue*> infoValues = group->getInfoValues();
			for(QListIterator<InterfaceValue*> j(infoValues); j.hasNext();) {
				InterfaceValue *value = j.next();
				cerr << "  " << value->getName().toStdString().c_str() << " [" 
					 << value->getProperties().toStdString().c_str() << "]" << endl;
			}
			cerr << endl << endl;
		}
		cerr << "--------------------------------------------------------" << endl;
	}
	return true;
}



}



