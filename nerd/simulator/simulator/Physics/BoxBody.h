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


#ifndef NERDBoxBody_H
#define NERDBoxBody_H

#include "SimBody.h"
#include "TriangleGeom.h"
#include "BoxGeom.h"
#include "Collision/CollisionObject.h"

namespace nerd {


/**
 * A BoxBody represents an object shaped like a box, whose appearance
 * can be modified by three parameters: width, height, depth. 
 * The center of the box is defined as the position of the object. This 
 * center can be obtained by method getPositionValue(). 
 * The center of mass initially is located in the same center. 
 * However the center of mass can be moved to any point relative to the 
 * object position. 
 *
 * Parameters of BoxBody (in addition to those inherited by SimBody):
 * Width, Height, Depth: Define the geometric properties of the box object.
 */
class BoxBody : public SimBody {

	public:
		BoxBody(const QString &name, double width = 0.01, double height = 0.01, double depth = 0.01);
		BoxBody(const BoxBody &body);
		virtual ~BoxBody();
    
		virtual SimBody* createCopy() const;
		
		virtual void setup();
		virtual void clear();
		
		virtual void valueChanged(Value *value);

	private:		
		void createBoxGeom();
		
	protected:
		DoubleValue *mWidth;
		DoubleValue *mHeight;
		DoubleValue *mDepth;
	
};

}

#endif
