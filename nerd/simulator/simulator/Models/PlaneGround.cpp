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

#include "PlaneGround.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "SimulationConstants.h"
#include <iostream>
#include "Core/Core.h"

using namespace std;

#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))

namespace nerd {

PlaneGround::PlaneGround(const QString &name) : ModelInterface(name) {

}

void PlaneGround::createModel() {

	PhysicsManager *pm = Physics::getPhysicsManager();
	SimObject *plane = pm->getPrototype(SimulationConstants::PROTOTYPE_PLANE_BODY)
		->createCopy();

	QString name = getName();
	if(name.indexOf("/") != 0) {
		name.prepend("/");
	}

	plane->setName(name);
	PARAM(StringValue, plane, "Material")->set("PVC");
	PARAM(ColorValue, plane, "Color")->set(0,200,0,255);
	PARAM(DoubleValue, plane, "Distance")->set(0);
	PARAM(DoubleValue, plane, "StaticFriction")->set(0.01);
	PARAM(DoubleValue, plane, "DynamicFriction")->set(0);
	
	BoolValue *switchYZAxes = Core::getInstance()->getValueManager()->getBoolValue(SimulationConstants::VALUE_SWITCH_YZ_AXES);
	if(switchYZAxes == 0 || switchYZAxes->get()) {
		PARAM(Vector3DValue, plane, "Axis")->set(0,1,0);
	}
	else {
		PARAM(Vector3DValue, plane, "Axis")->set(0,0,1);
	}
	dynamic_cast<ColorValue*>(plane->getParameter("Color"))->set(Color(0,150,0));
	pm->addSimObject(plane);
}

SimObject* PlaneGround::createCopy() const {
	PlaneGround *modelCopy = new PlaneGround("Default");
	return modelCopy;
}

void PlaneGround::setName(const QString &name) {
	ModelInterface::setName(name);
}

void PlaneGround::valueChanged(Value *value) {
	ModelInterface::valueChanged(value);
}

}
