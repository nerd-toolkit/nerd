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


#ifndef ODE_SphereBody_H_
#define ODE_SphereBody_H_

#include "ODE_Body.h"
#include "Physics/SphereBody.h"
#include <ode/ode.h>

namespace nerd {

/**
* ODE specific implementation of a SphereBody. On every reset, an ode Sphere objects is created and the old one deleted.
*/

class ODE_SphereBody : public ODE_Body, public SphereBody {

	public:
		ODE_SphereBody(const QString &name);
		ODE_SphereBody(const QString &name, double radius);
		ODE_SphereBody(const ODE_SphereBody &body);

		virtual ~ODE_SphereBody();

		virtual SimBody* createCopy() const;
		virtual void setup();
		virtual void clear();
		virtual void synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa);

		virtual dBodyID getRigidBodyID();
		virtual void valueChanged(Value *value);

	protected:
		void updateOrientation();

	private:
		dBodyID mRigidBodyID;
		bool mRigidBodyInitialized;
		bool mCollisionGeometryInitialized;
};
}
#endif

