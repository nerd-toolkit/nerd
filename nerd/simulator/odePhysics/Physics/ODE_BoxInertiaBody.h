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

#ifndef ODE_BoxInertiaBody_H_
#define ODE_BoxInertiaBody_H_

#include "ODE_Body.h"
#include "Physics/BoxInertiaBody.h"
#include <ode/ode.h>

namespace nerd {

/**
* ODE specific implementation of a BoxInertiaBody. Creates on every reset an ode box-object and deletes the old one.
**/

class ODE_BoxInertiaBody : public ODE_Body, public BoxInertiaBody {

	public:
		
		ODE_BoxInertiaBody(const QString &name);
		ODE_BoxInertiaBody(const QString &name, const Matrix3x3 *inertiaTensor, double width, double height, double depth);
		ODE_BoxInertiaBody(const ODE_BoxInertiaBody &body);

		virtual ~ODE_BoxInertiaBody();

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
};
}
#endif

