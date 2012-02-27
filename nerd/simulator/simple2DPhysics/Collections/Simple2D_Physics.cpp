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



#include "Simple2D_Physics.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Collections/SimulatorPrototypes.h"
#include "Physics/Simple2D_SimulationAlgorithm.h"
#include "Collision/Simple2D_CollisionHandler.h"
#include "Physics/Simple2D_BoxBody.h"
#include "Physics/Simple2D_FixedJoint.h"
#include "Physics/Simple2D_SliderMotor.h"
#include "Physics/Simple2D_DifferentialDrive.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new Simple2D_Physics.
 */
Simple2D_Physics::Simple2D_Physics()
{
	PhysicsManager *pm = Physics::getPhysicsManager();
	pm->setPhysicalSimulationAlgorithm(new Simple2D_SimulationAlgorithm());

	CollisionManager *cm = Physics::getCollisionManager();
	cm->setCollisionHandler(new Simple2D_CollisionHandler());

	SimulatorPrototypes();

	pm->addPrototype(SimulationConstants::PROTOTYPE_BOX_BODY, 
		new Simple2D_BoxBody(SimulationConstants::PROTOTYPE_BOX_BODY));

	pm->addPrototype(SimulationConstants::PROTOTYPE_FIXED_JOINT,
		new Simple2D_FixedJoint(SimulationConstants::PROTOTYPE_FIXED_JOINT));

	pm->addPrototype(SimulationConstants::PROTOTYPE_SLIDER_MOTOR,
		new Simple2D_SliderMotor(SimulationConstants::PROTOTYPE_SLIDER_MOTOR));

	pm->addPrototype(SimulationConstants::PROTOTYPE_DIFFERENTIAL_DRIVE,
		new Simple2D_DifferentialDrive(SimulationConstants::PROTOTYPE_DIFFERENTIAL_DRIVE));
}

/**
 * Destructor.
 */
Simple2D_Physics::~Simple2D_Physics() {
}




}



