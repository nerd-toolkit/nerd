/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
 *                                                                         *
 *   Nehring library by Jan Nehring (Diploma Thesis)                       *
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



#ifndef NERDFoodSource_H
#define NERDFoodSource_H


#include "Physics/Physics.h"
#include "Physics/SimBody.h"
#include "Collision/CollisionObject.h"
#include "Physics/SimSensor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/SphereGeom.h"
#include "Physics/CylinderBody.h"
#include "Physics/CylinderGeom.h"
#include "SimulationConstants.h"
#include "Collision/CollisionManager.h"


namespace nerd {

	/**
	 * FoodSource.
	 *
	 */
	class FoodSource : public SimBody, public virtual SimSensor {
	public:
		FoodSource(const QString &name);
		FoodSource(const FoodSource &other);
		virtual ~FoodSource();

		virtual SimBody* createCopy() const;
		
		virtual void setup();
		virtual void clear();
		
		virtual void valueChanged(Value *value);
		virtual void updateSensorValues();
		
		double getMengeNahrung();
		
		double getRange() const;
		void setRange(double range);

		double getRadius();
		double exploit();

		//7SimObject* getBody();

	private:
		void createCollisionObject();
		static const double opacity = 255;
		InterfaceValue *mFoodAmount;
		InterfaceValue *mFoodControl;
		BoolValue *mHideLightCone;
		DoubleValue *mRange;
		//SimObject *mBody;
	};

}



#endif



