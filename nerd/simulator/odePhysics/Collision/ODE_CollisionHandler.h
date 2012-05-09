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

#ifndef ODE_CollisionHandler_H_
#define ODE_CollisionHandler_H_

#include "Collision/CollisionHandler.h"
#include "Value/DoubleValue.h"
#include "OdeConstants.h"
#include <ode/ode.h>
#include <QHash>

namespace nerd {

class ODE_SimulationAlgorithm;
class CollisionObject;
class CollisionManager;
class IBDS_Body;
class MaterialProperties;

/**
* ODE specific CollisionHandler, which performes the collision handling and creates the required data structure that reports all collision-pairs as well as their contact points.
* The ODE_CollisionHandler adds some values into the simulation, which can be used to manipulate the collision handling between objects. These values are:
* /Simulation/ODE/SoftERP
* /Simulation/ODE/SoftCFM
* /Simulation/ODE/MinimumBouncingVelocity
* /Simulation/ODE/FrictionScaling
* /Simulation/ODE/ContactSlip
* /Simulation/ODE/StaticFrictionAccuracy.
* To learn more about these values and their meaning for the "quality" of collision handling please visit the nkg-wiki or check the ODE-manual.
**/
class ODE_CollisionHandler : public CollisionHandler { 
	
	public:
		ODE_CollisionHandler();
		virtual ~ODE_CollisionHandler();

		virtual QList<Contact> getContacts() const;
		virtual void disableCollisions(CollisionObject *firstCollisionPartner, 
									CollisionObject *secondCollisionPartner, 
									bool disable);
		virtual void updateCollisionHandler(CollisionManager *cManager); 

		void collisionCallback(void *data, dGeomID o1, dGeomID o2);
		void clearContactList();	

	private:
		QList<Contact> mCurrentContacts;
		ODE_SimulationAlgorithm *mAlgorithm;
		IntValue *mMaxContactPointsValue;

		DoubleValue *mSoftERPValue;
		DoubleValue *mSoftCFMValue;
		DoubleValue *mBouncingVelocityValue;
		DoubleValue *mFrictionScalingValue;
		DoubleValue *mContactSlipValue;
		IntValue *mStaticFrictionAccuracyValue;
		double mSoftERP;
		double mSoftCFM;
		double mBouncingVelocity;
		double mFrictionScalingFactor;
		double mContactSlip;
		int mStaticFrictionAccuracy;

	protected:
		int mMaxContactPoints;
		QHash<dGeomID, CollisionObject*> mLookUpTable;
		QHash<CollisionObject*, QList<CollisionObject*> > mAllowedCollisionPairs;
		MaterialProperties *mGlobalMaterialProperties;
};
}
#endif

