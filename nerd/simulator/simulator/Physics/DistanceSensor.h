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

#ifndef DistanceSensor_H_
#define DistanceSensor_H_

#include "DistanceRay.h"
#include "Physics/SimBody.h"
#include "Physics/SimSensor.h"
#include "Collision/DistanceSensorRule.h"
#include "Value/InterfaceValue.h"

namespace nerd {

class DistanceSensor : public SimObject, public SimSensor {
	public:
		DistanceSensor(const QString &name);
		DistanceSensor(const DistanceSensor &other);
		virtual ~DistanceSensor();
		virtual SimObject* createCopy() const;

		virtual void setup();
		virtual void clear();
		virtual void updateSensorValues();

		virtual void valueChanged(Value *value);
		virtual QList<Quaternion> calcOrientations() const;
		SimBody* getHostBody() const;

		virtual double getAvgSensorValue();
		virtual double getMinSensorValue();
		
		virtual void resetSensor();
		
	protected:
		BoolValue *mCalcMinDistance;
		DistanceSensorRule *mRule;
		DoubleValue *mAngle;
		DoubleValue *mMaxRange;
		DoubleValue *mMinRange;
		DoubleValue *mSensorNoise;
		Vector3DValue *mMinIntersectionPoint;
		IntValue *mNumberOfRays;
		InterfaceValue *mDistance;
		QList<DistanceRay*> mRays;
		QuaternionValue *mLocalOrientation;
		SimBody *mHostBody;
		StringValue *mHostBodyName;
		Vector3DValue *mLocalPosition;
		ColorValue *mActiveColor;
		ColorValue *mInactiveColor;
		ColorValue *mDisabledColor;
};

}

#endif

