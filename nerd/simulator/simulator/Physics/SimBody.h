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


#ifndef NERDSIMBODY_H
#define NERDSIMBODY_H

#include <QString>
#include "SimObject.h"
#include "Value/Vector3DValue.h"
#include "Value/BoolValue.h"
#include "Value/DoubleValue.h"
#include "Value/QuaternionValue.h"

namespace nerd {


class CollisionObject;

/**
 * A SimObject represents a rigid body that can be simulated physically. 
 * SimObject is the physics engine independent 
 * base class of all more specific body objects. 
 *
 * All simulated bodies share a set of common parameters, like position, 
 * orientation or mass. These parameters can be accessed via the 
 * getParameter() method. 
 * The available parameters for simulated bodies are:
 * Position/Orientation/OrientationQuaternion: Global position and orientation of the simulated body 
 * (Orientation: measures are given in euler angles in degrees). 
 * CenterOfMass: Position of the center of mass, relative to the position of the simulated body.
 * Dynamic: This parameter defines, whether the simulated body is static or dynamic (gravity 
 * and forces induced on the object manipulate the position of the object).
 * Mass: The mass of the body (this mass is used for the main collision geometry.
 * DynamicFriction/StaticFriction/Elasticity: Fricition coefficients and elasticity 
 * for collisions. These paramaters are used, if the material of one of the colliding bodies is not 
 * defined/invalid. In this case, the friction coefficients of the two bodies are summed
 * to define the fricition of the contact, whereas the elasticities are multiplied.
 * Material: The name of the material for the simulated body. 
 * Texture: The name of the texture, to be used during visualization.
 * Color: The color of the simulated bodies. All geometries of the body, where 
 * the color can be modified, do change their color as well.
 */
class SimBody : public SimObject {

	public:
    	SimBody(const QString &name, const QString &prefix = "");
		SimBody(const SimBody &body);
    	virtual ~SimBody();

		virtual SimObject* createCopy() const;
		
		virtual void setup();
		virtual void clear();

		virtual void valueChanged(Value *value);

		Vector3DValue* getPositionValue() const;
		Vector3DValue* getOrientationValue() const;
		Vector3DValue* getCenterOfMassValue() const;
		QuaternionValue* getQuaternionOrientationValue() const;
		BoolValue* getDynamicValue() const;
		
		virtual bool addCollisionObject(CollisionObject *object);
		virtual bool removeCollisionObject(CollisionObject *object);
		virtual QList<CollisionObject*> getCollisionObjects() const;

		DoubleValue* getElasticityValue() const;
		DoubleValue* getDynamicFrictionValue() const;
		DoubleValue* getStaticFrictionValue() const;

	protected:
		ColorValue *mGeometryColorValue;
		Vector3DValue *mPositionValue;
		Vector3DValue *mOrientationValue;
		Vector3DValue *mCenterOfMassValue;
		QuaternionValue* mQuaternionOrientationValue;
		BoolValue *mDynamicValue;
		DoubleValue *mMassValue;
		DoubleValue *mDynamicFrictionValue;
		DoubleValue *mStaticFrictionValue;
		DoubleValue *mElasticityValue;

		StringValue *mTexturesValue;
		QList<CollisionObject*> mCollisionObjects;
		StringValue *mMaterialValue;

		CollisionObject *mBodyCollisionObject;
		bool mUpdatingQuaternion;
		bool mUpdatingOrientation;
};

}

#endif
