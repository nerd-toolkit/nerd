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
#ifndef NERDCollisionObject_H
#define NERDCollisionObject_H

#include "Physics/SimObject.h"
#include "Physics/SimGeom.h"

namespace nerd {

	class SimBody;
	
	
	/**
	 * A CollisionObject holds the physical properties of an object, that has a 
	 * physical body (mass, geometry, material property (-> friction property)). 
	 * A CollisionObject has a geometry and an owner SimObject to which the 
	 * CollisionObject is attached.
	 */
	class CollisionObject {
	
	public:
		CollisionObject(const SimGeom &geometry, SimObject *owner = 0, 
										bool reportCollision = true);
		CollisionObject(const CollisionObject &other);
		virtual ~CollisionObject(); 
		
		void setOwner(SimObject *owner);
		SimObject* getOwner() const;
		
		void setHostBody(SimBody *body);
		SimBody* getHostBody() const;
		
		void setGeometry(const SimGeom &geometry);
		SimGeom* getGeometry() const;
		
		void setCollisionReport(bool enableReport);
		bool areCollisionsReported() const;
		
		void setNativeCollisionObject(void *object);
		void* getNativeCollisionObject() const;
		
		void setMaterialType(const QString &materialName);
		int getMaterialType() const;

		void disableCollisions(bool penetrable);
		bool areCollisionsDisabled() const;

		void setTextureType(const QString &materialName);
		void setFaceTexture(int index, const QString &textureName);
// 		int getTextureType();
		QList<int> getTextureTypeInformation() const;

		void setMass(double mass);
		double getMass() const;

	private:		
		SimObject *mOwner;	
		SimBody *mHostBody;
		SimGeom *mGeometry;
		bool mReportCollisions;
		bool mIsPenetrable;
		void *mNativeCollisionObject;
		QString mTextureInformation;
	
		int mMaterialType;
		QList<int> mFaceTextureTypes;
	
		double mMass;
	};

}

#endif




