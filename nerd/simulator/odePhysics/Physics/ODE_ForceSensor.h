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



#ifndef NERDODE_ForceSensor_H
#define NERDODE_ForceSensor_H

#include <QString>
#include <QHash>
#include "ODE_Body.h"
#include "Value/StringValue.h"
#include "Value/InterfaceValue.h"
#include "Value/DoubleValue.h"
#include "Value/Vector3DValue.h"
#include "Physics/SimSensor.h"
#include "Physics/SimObject.h"
#include "Physics/SimBody.h"
#include "Physics/SimGeom.h"
#include "Collision/CollisionObject.h"



namespace nerd {

	/**
	 * ODE_ForceSensor.
	 *
	 */
	class ODE_ForceSensor : public SimObject, public virtual SimSensor {
	public:
		ODE_ForceSensor(const QString &name, double minForce, double maxForce);
		ODE_ForceSensor(const ODE_ForceSensor &other);
		virtual ~ODE_ForceSensor();

		virtual SimObject* createCopy() const;
		virtual void setup();
		virtual void clear();
		
		virtual void valueChanged(Value *value);
		virtual void updateSensorValues();

		ODE_Body* getHostBody() const;
		QList<CollisionObject*> getReferenceCollisionObject() const;

		void setLocalSensorAxis(const Vector3D &axis);
		Vector3D getLocalSensorAxis() const;

	private:
		InterfaceValue *mForceSensorValue;
		DoubleValue *mMinForce;
		DoubleValue *mMaxForce;
		StringValue *mHostBodyName;
		StringValue *mCollisionObjectIndices;
		Vector3DValue *mLocalSensorAxis;

		ODE_Body *mHostBody;
		QList<CollisionObject*> mReferenceCollisionObjects;
		
	};

}

#endif



