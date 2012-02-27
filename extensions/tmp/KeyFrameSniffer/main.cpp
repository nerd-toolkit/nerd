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




#include "Core/Core.h"
#include <QCoreApplication>
#include <iostream>
#include "KeyFrameSniffer.h"
#include <QString>
#include "Physics/SimObject.h"
#include "Value/InterfaceValue.h"

using namespace std;

namespace nerd {


class SimObjectAdapter : public SimObject {
	public:
		SimObjectAdapter(const QString name) : SimObject(name) {};

		void addInput(InterfaceValue *value) {
			mInputValues.append(value);
		}
};

}

using namespace nerd;

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	if(argc < 2) {
		cout << "No file name specified." << endl;
		return 0;
	}

	SimObjectGroup *group = new SimObjectGroup("Humanoid", "Agent");
	SimObjectAdapter *obj = new SimObjectAdapter("HumanoidDummy");
	for(int i = 0; i < 42; ++i) {
		obj->addInput(new InterfaceValue("", "Input", 0.0, -10000.0, 10000.0, -10000.0, 10000.0));
	}
    group->addObject(obj);

	new KeyFrameSniffer(QString(argv[1]), group);

	Core::getInstance()->init();

	app.exec();

	Core::getInstance()->shutDown();
	Core::getInstance()->waitForAllThreadsToComplete();
	Core::resetCore();


	return 0;
};


