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



#include "Simple2D_SimulationAlgorithm.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/Physics.h"
#include "Collision/CollisionHandler.h"
#include "Collision/Simple2D_CollisionHandler.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new Simple2D_SimulationAlgorithm.
 */
Simple2D_SimulationAlgorithm::Simple2D_SimulationAlgorithm()
	: PhysicalSimulationAlgorithm("Simple2D"), mInitialized(false), mSimple2DCollisionHandler(0)
{
}



/**
 * Destructor.
 */
Simple2D_SimulationAlgorithm::~Simple2D_SimulationAlgorithm() {
}


bool Simple2D_SimulationAlgorithm::resetPhysics() {

	bool ok = true;




	mSimple2DCollisionHandler = dynamic_cast<Simple2D_CollisionHandler*>(
			Physics::getCollisionManager()->getCollisionHandler());
	if(mSimple2DCollisionHandler == 0) {
		Core::log("Simple2D_SimulationAlgorithm::resetPhysics: Could not find matching "
				  "Simple2D_CollisionHandler.");
		return false;
	}

	return ok;
}


bool Simple2D_SimulationAlgorithm::finalizeSetup() {
	return true;
}


void Simple2D_SimulationAlgorithm::valueChanged(Value *value) {
	ParameterizedObject::valueChanged(value);
}



bool Simple2D_SimulationAlgorithm::executeSimulationStep(PhysicsManager *pm) {

	int iterationsPerStep = mIterationsPerStepValue->get();	

	for(int i = 0; i < iterationsPerStep; ++i) {
		pm->updateActuators();
		mSimple2DCollisionHandler->prepare();
		simulateWorld(pm);
		mSimple2DCollisionHandler->detectCollisions();
		pm->updateSensors();
	}

	return true;
}

bool Simple2D_SimulationAlgorithm::simulateWorld(PhysicsManager *pm) {

	QList<SimBody*> mBodies = pm->getSimBodies();
	
	
	return true;
}


}



