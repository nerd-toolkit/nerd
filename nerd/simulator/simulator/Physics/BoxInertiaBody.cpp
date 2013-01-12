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


#include "BoxInertiaBody.h"

namespace nerd {

/**
 * Constructs a new BoxInertiaBody object.
 *
 * @param name the name of this BoxInertiaBody.
 */
BoxInertiaBody::BoxInertiaBody(const QString &name, const Matrix3x3 *inertiaTensor, double width, double height, double depth)
		: BoxBody(name, width, height, depth)
{
	if(inertiaTensor != 0) {
		mInertiaTensorValue = new Matrix3x3Value(*inertiaTensor);
	} else {
		mInertiaTensorValue = new Matrix3x3Value();
	}

	addParameter("InertiaTensor", mInertiaTensorValue);
}


/**
 * Creates a copy of object body.
 * @param body the BoxInertiaBody to copy.
 */
BoxInertiaBody::BoxInertiaBody(const BoxInertiaBody &body) 
	: Object(), ValueChangedListener(), BoxBody(body) 
{
	mInertiaTensorValue = dynamic_cast<Matrix3x3Value*>(getParameter("InertiaTensor"));
}


/**
 * Destructor.
 */
BoxInertiaBody::~BoxInertiaBody() {
}


/**
 * Creates a copy of this object, using the copy constructor.
 *
 * @return a copy of this object.
 */
SimBody* BoxInertiaBody::createCopy() const {
	return new BoxInertiaBody(*this);
}


void BoxInertiaBody::setup() {
	BoxBody::setup();
}


void BoxInertiaBody::clear() {
	BoxBody::clear();

}

/**
 * Called when a parameter Value changed.
 * @param value the value that changed.
 */
void BoxInertiaBody::valueChanged(Value *value) {
	BoxBody::valueChanged(value);
}




}


