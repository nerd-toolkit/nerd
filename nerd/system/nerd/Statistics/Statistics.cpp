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

#include "Statistics.h"

#include "Core/Core.h"
#include "NerdConstants.h"

namespace nerd {


bool Statistics::install() {
	bool ok = true;
	Core *core = Core::getInstance();

	StatisticsManager *sm = dynamic_cast<StatisticsManager*>(Core::getInstance()
			->getGlobalObject(NerdConstants::OBJECT_STATISTICS_MANAGER));
	if(sm == 0) {
		sm = new StatisticsManager();
		core->addGlobalObject(NerdConstants::OBJECT_STATISTICS_MANAGER, sm);
	}
	else if(dynamic_cast<StatisticsManager*>(sm) == 0) {
		Core::log(QString("Statistics::install() : There was a global object named [")
				.append(NerdConstants::OBJECT_STATISTICS_MANAGER)
				.append("] which is not a subclass of StatisticsManager!"));
		ok = false;
	}
	return ok;
}


StatisticsManager* Statistics::getStatisticsManager() {
	StatisticsManager *sm = dynamic_cast<StatisticsManager*>(Core::getInstance()
			->getGlobalObject(NerdConstants::OBJECT_STATISTICS_MANAGER));
	if(sm == 0) {
		Statistics::install();
		sm = dynamic_cast<StatisticsManager*>(Core::getInstance()
			->getGlobalObject(NerdConstants::OBJECT_STATISTICS_MANAGER));
	}
	return sm;
}

}


