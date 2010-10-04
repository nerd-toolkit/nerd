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

#ifndef DistanceRay_H_
#define DistanceRay_H_

#include "Collision/CollisionObject.h"
#include "Collision/DistanceSensorRule.h"
#include "Physics/RayGeom.h"

namespace nerd {

class DistanceSensor;

class DistanceRay {

	public:
		DistanceRay(const QString &name, const Vector3D &position,
				const Quaternion &orientation, double length,
				DistanceSensorRule *rule, const Color &active,
				const Color &inactive);
		virtual ~DistanceRay();

		void setOwner(DistanceSensor *sensor);
		DistanceSensor* getOwner() const;

		virtual void setName(const QString &name);
		virtual QString getName() const;

		virtual CollisionObject* getCollisionObject() const;
		virtual double getDistance();

		virtual void updateRay(double length);

	private:
		QString mName;
		DistanceSensorRule *mRule;
		CollisionObject *mRayCollisionObject;
		RayGeom *mRay;
		DistanceSensor *mOwner;
		Color mActiveColor;
		Color mInactiveColor;

};

}

#endif
