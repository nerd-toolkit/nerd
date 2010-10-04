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

#ifndef NERDODE_Body_H
#define NERDODE_Body_H

#include <ode/ode.h>
#include <QList>
#include <QHash>

namespace nerd {

class SimBody;

/**
 * Abstract base class of all Body objects that can be physically simulated by
 * ODE. 
 * During a reset, each simulation body calls createODECollisionObjects, where the collision geometries are added to the (ode-based) physical simulation. 
 * Additionally, for composite objects (objects with several collision geometries) the COM and mass are changed in accordance with the given collision objects.
 */

class ODE_Body {
	public:
			ODE_Body();
			virtual ~ODE_Body();
	
			virtual dBodyID getRigidBodyID() = 0;
			
			virtual bool createODECollisionObjects();
			virtual void synchronizePositionAndOrientation(dGeomID rigidBody, SimBody *simBody);

			virtual void setup();
			virtual void clear();	

			void enableContactJointFeedback(bool enable);
			bool isFeedbackEnabled() const;
			virtual void clearFeedbackList();

			QHash<dGeomID, dJointFeedback*>& getJointFeedbackList();
			QList<dJointFeedback*>& getOwnedJointFeedbackList();

	protected:
		float *mGeomPoints;
		int* mGeomTriangles;
		dMass mBodyMass;
		bool mCollectForceFeedback;
		QHash<dGeomID, dJointFeedback*> mContactJointFeedback;
		QList<dJointFeedback*> mOwnedContactJointFeedbacks;

};

}

#endif


