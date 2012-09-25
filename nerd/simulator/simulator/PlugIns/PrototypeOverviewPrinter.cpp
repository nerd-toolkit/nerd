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



#include "PrototypeOverviewPrinter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QStringList>
#include "Physics/Physics.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new PrototypeOverviewPrinter.
 */
PrototypeOverviewPrinter::PrototypeOverviewPrinter()
	: mOverviewArg(0)
{
	mOverviewArg = new CommandLineArgument("print-prototypes", "pp", "[script]", 
					"Prints all known physics prototypes to the screen.\n"
					"If this option is followed by a 'script', then the parameters are formatted "
					"to be directly used in model scripts.", 0, 1, true);

	Core::getInstance()->addSystemObject(this);
}


/**
 * Destructor.
 */
PrototypeOverviewPrinter::~PrototypeOverviewPrinter() {
}

QString PrototypeOverviewPrinter::getName() const {
	return "PrototypeOverviewPrinter";
}


bool PrototypeOverviewPrinter::init() {
	return true;
}


bool PrototypeOverviewPrinter::bind() {
	if(mOverviewArg->getEntries().size() > 0) {
		//print overview
		cout << "\nPhysics Prototype Overview:" << endl;
		cout << "\n---------------------------" << endl;
		
		QStringList entryParams = mOverviewArg->getEntryParameters(0);
		if(entryParams.size() > 0 && entryParams.at(0) == "script") {
			QList<QString> prototypeNames = Physics::getPhysicsManager()->getPrototypeNames();
			for(QListIterator<QString> i(prototypeNames); i.hasNext();) {
				QString name = i.next();
				SimObject *obj = Physics::getPhysicsManager()->getPrototype(name);
				if(obj == 0) {
					continue;
				}
				cout << endl << endl << name.toStdString().c_str() << endl << "------------" << endl;
				//TODO add description of object?
				
				//use name without the leading "Prototypes/"
				if(name.startsWith("Prototypes/")) {
					name = name.mid(11);
				}
				cout << "var object = model.createObject(\"" << name.toStdString().c_str() << "\", \"\");" << endl;
				
				QList<QString> paramNames = obj->getParameterNames();
				for(QListIterator<QString> j(paramNames); j.hasNext();) {
					QString paramName = j.next();
					Value *value = obj->getParameter(paramName);
					if(value != 0) {
						cout << "set(\"" << paramName.toStdString().c_str() << "\", \""
							 << value->getValueAsString().toStdString().c_str() << "\");" << endl;
					}
				}
			}
		}
		else {
			//use standard output.
			QList<QString> prototypeNames = Physics::getPhysicsManager()->getPrototypeNames();
			for(QListIterator<QString> i(prototypeNames); i.hasNext();) {
				QString name = i.next();
				SimObject *obj = Physics::getPhysicsManager()->getPrototype(name);
				if(obj == 0) {
					continue;
				}
				cout << " " << name.toStdString().c_str() << endl;
				//TODO add description of object.
				
				QList<QString> paramNames = obj->getParameterNames();
				for(QListIterator<QString> j(paramNames); j.hasNext();) {
					QString paramName = j.next();
					Value *value = obj->getParameter(paramName);
					if(value != 0) {
						cout << "   " << paramName.toStdString().c_str() << " : "
							<< value->getValueAsString().toStdString().c_str();
							
						QString desc = value->getDescription();
						if(desc != "") {
							cout << endl << "      [" << desc.toStdString().c_str() << "]";
						}
						cout << endl;
					}
				}
			}
		}
	}

	return true;
}


bool PrototypeOverviewPrinter::cleanUp() {
	return true;
}


}



