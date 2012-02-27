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



#ifndef NERDODE_CapsuleBody_H
#define NERDODE_CapsuleBody_H

#include <QString>
#include <QHash>
#include "Physics/CapsuleBody.h"
#include <ode/ode.h>
#include "Physics/ODE_Body.h"

namespace nerd {

	/**
	 * ODE_CapsuleBody.
	 *
	 */
	class ODE_CapsuleBody : public ODE_Body, public CapsuleBody  {
	public:
		ODE_CapsuleBody(const QString &name, double length = 1.0, double radius = 0.1);
		ODE_CapsuleBody(const ODE_CapsuleBody &other);
		virtual ~ODE_CapsuleBody();

		virtual SimBody* createCopy() const;
		virtual dBodyID getRigidBodyID();
		virtual void setup();
		virtual void clear();
		virtual void synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa);

		virtual void valueChanged(Value *value);


	protected:
		void updateOrientation();

	
	private:
		dBodyID mRigidBodyID;
		bool mRigidBodyInitialized;
		bool mCollisionGeometryInitialized;
		bool mSynchronizing;
	};

}

#endif



