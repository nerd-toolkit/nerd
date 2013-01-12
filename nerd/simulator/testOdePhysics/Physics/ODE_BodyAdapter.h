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

#ifndef ODE_BodyAdapter_H_
#define ODE_BodyAdapter_H_
#include "Physics/Physics.h"
#include "Physics/ODE_Body.h"
#include "Physics/SimBody.h"
#include "Physics/ODE_SimulationAlgorithm.h"
#include <ode/ode.h>

namespace nerd{


/**
 * ODE_BodyAdapter
**/

class ODE_BodyAdapter : public ODE_Body, public SimBody {

	public:
		ODE_BodyAdapter(const QString &name) : ODE_Body(), SimBody(name) {
			mRigidBody = 0;
			mCreateCounter = 0;
		}

		virtual dBodyID getRigidBodyID() {
			return mRigidBody;
		}


		virtual void setup() {
			SimBody::setup();
			ODE_SimulationAlgorithm *algorithm = 		
				dynamic_cast<ODE_SimulationAlgorithm*>(Physics::getPhysicsManager()->getPhysicalSimulationAlgorithm());
			if(algorithm == 0) {
				return;
			}
			dWorldID worldID = algorithm->getODEWorldID();
			mRigidBody = dBodyCreate(worldID);
		}

		virtual void clear() {
			SimBody::clear();
			mRigidBody = 0;
			mCreateCounter = 0;
		}
	
		virtual bool createODECollisionObjects() {
			mCreateCounter++;
			return ODE_Body::createODECollisionObjects();
		}

	public:
		int mCreateCounter;
	
	private:
		dBodyID mRigidBody;
};
}
#endif

