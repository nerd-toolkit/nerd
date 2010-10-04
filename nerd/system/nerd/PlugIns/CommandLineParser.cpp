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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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



#include "CommandLineParser.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Value/StringValue.h"
#include <iostream>

using namespace std;

namespace nerd{

const QString CommandLineParser::ARGUMENT_NAME_PATH = QString("/CommandLineArguments/");

CommandLineParser::CommandLineParser() {
}

CommandLineParser::CommandLineParser(int count, char *argv[]) {
	parseCommandLine(count, argv);
}


CommandLineParser::~CommandLineParser() {
}

void CommandLineParser::parseCommandLine(int count, char *argv[]) {
	ValueManager *vm = Core::getInstance()->getValueManager();
	for(int i = 0; i < count;) {
		QString element = argv[i];
		if(element.startsWith("-")) {
				QString buffer = "";
				++i;
				while(i < count && !QString(argv[i]).startsWith("-")) {
					buffer.append(QString(argv[i]).trimmed()).append(" ");
					++i;
				}
				StringValue *argumentValue;

				if(vm->getValue(ARGUMENT_NAME_PATH + element) == 0) {
					argumentValue = new StringValue();

					vm->addValue(ARGUMENT_NAME_PATH + element, argumentValue);
					//TODO replace nonmatching values to allow also late descriptions.
				}else{
					argumentValue = dynamic_cast<StringValue*>(vm->getValue(ARGUMENT_NAME_PATH + element));
				}
				if(argumentValue->get().compare("") == 0) {
					argumentValue->set(buffer);
				}
				else {
					argumentValue->set(argumentValue->get() + ";" + buffer);
				}
		}else{
			++i;
		}
	}
}

}