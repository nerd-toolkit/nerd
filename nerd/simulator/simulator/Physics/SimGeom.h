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


#ifndef SimGeom_H_
#define SimGeom_H_

#include <QVector>
#include "Math/Vector3D.h"
#include "Util/Color.h"
#include "Math/Quaternion.h"

namespace nerd {

class SimObject;

/**
 * SimGeom is the base class for all geometries that SimObject object can posses. The 
 * SimGeom of an object is needed for the visualization as well as the collision detection 
 * and handling. 
 */
class SimGeom {

	public:
		SimGeom(SimObject *simObject = 0);
		SimGeom(const SimGeom &geom);
		virtual ~SimGeom();
		
		virtual SimGeom* createCopy() const;
		
		virtual SimObject* getSimObject() const;
		virtual void setSimObject(SimObject* simObject);
		
		virtual void setColor(const Color &newColor);
		virtual void setColor(int red, int green, int blue, int alpha);
		virtual Color getColor() const;
		void setAutomaticColor(bool enable);
		bool hasAutomaticColor() const;
		
		virtual void setLocalPosition(const Vector3D &localPosition);
		virtual Vector3D getLocalPosition() const;

		virtual void setLocalOrientation(const Quaternion &localOrientation);
		virtual Quaternion getLocalOrientation() const;
	
		virtual void enableDrawing(bool enable);
		virtual bool isDrawingEnabled() const;
	
	protected:
		SimObject* mSimObject;
		Color mGeomColor;
		Vector3D mLocalPosition;
		Quaternion mLocalOrientation;
		bool mEnableDrawing;
		bool mAutoColor;

};
}
#endif
