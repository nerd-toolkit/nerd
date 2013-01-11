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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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



#include "ValueAtCommandLineHandler.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include <QStringList>

using namespace std;

namespace nerd {


/**
 * Constructs a new ValueAtCommandLineHandler.
 */
ValueAtCommandLineHandler::ValueAtCommandLineHandler()
	: mSetValueArgument(0)
{
	mSetValueArgument = new CommandLineArgument("setvalue", 
							"sv", 
							"<property> <value>",
							"Sets the property with name <property> to the given <value>.",
							2, 0, false, true);

	Core::getInstance()->addSystemObject(this);
}


/**
 * Destructor.
 */
ValueAtCommandLineHandler::~ValueAtCommandLineHandler() {
}


QString ValueAtCommandLineHandler::getName() const {
	return "ValueAtCommandLineHandler";
}

bool ValueAtCommandLineHandler::init() {

	return true;
}


bool ValueAtCommandLineHandler::bind() {

	ValueManager *vm = Core::getInstance()->getValueManager();

	int numberOfEntries = mSetValueArgument->getNumberOfEntries();
	for(int i = 0; i < numberOfEntries; ++i) {
		QStringList parameters = mSetValueArgument->getEntryParameters(i);

		if(parameters.size() != 2) {
			continue;
		}

		QList<QString> matchingValueNames = vm->getValueNamesMatchingPattern(parameters.at(0).trimmed());

		if(matchingValueNames.empty()) {
			Core::log(QString("ValueAtCommandLineHandler: Could not find value [")
						+ parameters.at(0) + "]", true);
		}

		for(QListIterator<QString> j(matchingValueNames); j.hasNext();) {
			QString name = j.next();
			Value *value = vm->getValue(name);
			if(!value->setValueFromString(parameters.at(1))) {
				Core::log(QString("ValueAtCommandLineHandler: Could not set value [")
						+ name + "] to [" + parameters.at(1) + "]", true);
			}
			else {
				Core::log(QString("ValueAtCommandLineHandler: Setting value [")
						+ name + "] to [" + parameters.at(1) + "]", true);
			}
		}
	}

	return true;
}

bool ValueAtCommandLineHandler::cleanUp() {
	return true;
}



}



