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

#include "ControlParameterModel.h"
#include "Physics/Physics.h"
#include "Core/Core.h"
#include <iostream>

using namespace std;

namespace nerd {

ControlParameterModel::ControlParameterModel(const QString &name, 
					ControlParameter *controlParameter)
	: ModelInterface(name), mAddedControlParameter(false)
{
	mControlParameterPrototype = controlParameter;
}


ControlParameterModel::ControlParameterModel(const ControlParameterModel &other) 
	: Object(), ValueChangedListener(), EventListener(), ModelInterface(other),
	  mAddedControlParameter(false)
{
	mControlParameterPrototype = dynamic_cast<ControlParameter*>(
				other.mControlParameterPrototype->createCopy());
}


ControlParameterModel::~ControlParameterModel() {
	if(!mAddedControlParameter) {
		delete mControlParameterPrototype;
	}
}


SimObject* ControlParameterModel::createCopy() const {
	return new ControlParameterModel(*this);
}


void ControlParameterModel::createModel() {

	PhysicsManager *pm = Physics::getPhysicsManager();

	QString groupName = getName();
	QString name = getName();
	if(name.indexOf(":") != -1) {
		name = name.mid(name.indexOf(":") + 1);
		groupName = groupName.mid(0, groupName.indexOf(":"));
	}

	if(name.indexOf("/") != 0) {
		name.prepend("/");
	}

	SimObjectGroup *group = pm->getSimObjectGroup(groupName);
	if(group == 0) {
		Core::log(QString("ControlParameterModel: Could not add ControlParameter "
					"to SimObjectGroup [")
					.append(getName()).append("]"));
	}
	else {
		mAddedControlParameter = true;

		QString prefix = name;
		prefix = prefix.mid(0, prefix.lastIndexOf("/"));
		QString parameterName = name;
		parameterName = parameterName.mid(parameterName.lastIndexOf("/") + 1);

		mControlParameterPrototype->setName(name);
		group->addObject(mControlParameterPrototype);
		mControlParameterPrototype->getControlParameter()->setPrefix(prefix);
		mControlParameterPrototype->getControlParameter()->setInterfaceName(parameterName);
		pm->addSimObject(mControlParameterPrototype);
	}
}


}


