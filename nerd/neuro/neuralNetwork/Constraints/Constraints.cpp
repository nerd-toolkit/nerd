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

#include "Constraints.h"
#include "NeuralNetworkConstants.h"
#include "Core/Core.h"
#include "Constraints/ConstraintManager.h"

namespace nerd {

Constraints::Constraints()
{
}

Constraints::~Constraints() {
}

bool Constraints::install() {
	bool ok = true;

	ConstraintManager *cm = dynamic_cast<ConstraintManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER));
	if(cm == 0) {
		cm = new ConstraintManager();
		//core->addGlobalObject(NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER, cm);
		cm->registerAsGlobalObject();
	}
	else if(dynamic_cast<ConstraintManager*>(cm) == 0) {
		Core::log(QString("Constraint::install() : There was a global object named [")
				.append(NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER)
				.append("] which is not a subclass of ConstraintManager!"));
		ok = false;
	}
	return ok;
}


ConstraintManager* Constraints::getConstraintManager() {
	ConstraintManager *cm = dynamic_cast<ConstraintManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER));
	if(cm == 0) {
		Constraints::install();
		cm = dynamic_cast<ConstraintManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER));
	}
	return cm;
}

}


