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



#include "Simple2D_BoxBody.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new Simple2D_BoxBody.
 */
Simple2D_BoxBody::Simple2D_BoxBody(const QString &name)
	: BoxBody(name), Simple2D_Body()
{
}

Simple2D_BoxBody::Simple2D_BoxBody(const QString &name, double width, double height, double depth)
	: BoxBody(name, width, height, depth), Simple2D_Body()
{

}


/**
 * Copy constructor. 
 * 
 * @param other the Simple2D_BoxBody object to copy.
 */
Simple2D_BoxBody::Simple2D_BoxBody(const Simple2D_BoxBody &other) 
	: ValueChangedListener(), BoxBody(other), Simple2D_Body(other)
{
}

/**
 * Destructor.
 */
Simple2D_BoxBody::~Simple2D_BoxBody() {
}

SimBody* Simple2D_BoxBody::createCopy() const {
	return new Simple2D_BoxBody(*this);
}


void Simple2D_BoxBody::setup() {
	BoxBody::setup();
}


void Simple2D_BoxBody::clear() {
	BoxBody::clear();
	clearChildBodies();
}


void Simple2D_BoxBody::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa) {
	BoxBody::synchronizeWithPhysicalModel(psa);
}


void Simple2D_BoxBody::valueChanged(Value *value) {
	BoxBody::valueChanged(value);
	if(value == 0) {
		return;
	}
}

bool Simple2D_BoxBody::addParameter(const QString &name, Value *value) {
	return BoxBody::addParameter(name, value);
}


Value* Simple2D_BoxBody::getParameter(const QString &name) const {
	return BoxBody::getParameter(name);
}


Vector3DValue* Simple2D_BoxBody::getPositionValue() const {
	return mPositionValue;
}


Vector3DValue* Simple2D_BoxBody::getOrientationValue() const {
	return mOrientationValue;
}




}



