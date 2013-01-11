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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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



#ifndef NERDBoxInertiaBody_H
#define NERDBoxInertiaBody_H

#include "BoxBody.h"
#include "TriangleGeom.h"
#include "Collision/CollisionObject.h"
#include "Value/Matrix3x3Value.h"

namespace nerd {


/**
 * An BoxInertiaBody is the same as a BoxBody except that the InertiaTensor can be set
 * explicitly and free (can be completely different from a box).
 *
 * Shapes can be added with CollisionObjects as in all SimBodies. (Note that the masses of this
 * CollisionObjects should be zero, because the mass is added to the mass of the whole InertiaBody,
 * but the inertia tensor is not changed!)
 *
 */
class BoxInertiaBody : public BoxBody {

	public:
		BoxInertiaBody(const QString &name, const Matrix3x3 *inertiaTensor = 0, double width = 0.0, double height = 0.0, double depth = 0.0);
		BoxInertiaBody(const BoxInertiaBody &body);
		virtual ~BoxInertiaBody();
    
		virtual SimBody* createCopy() const;
		
		virtual void setup();
		virtual void clear();
		
		virtual void valueChanged(Value *value);

	protected:

		Matrix3x3Value *mInertiaTensorValue;
};

}

#endif
