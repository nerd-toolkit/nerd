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



#include "MotorAdapter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "MotorModels/MotorModel.h"
#include "Physics/Physics.h"
#include <QListIterator>

using namespace std;

namespace nerd {


/**
 * Constructs a new MotorAdapter.
 */
MotorAdapter::MotorAdapter(int jointType)
	: mActiveMotorModel(0), mActiveMotorModelName(0), mJointType(jointType)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the MotorAdapter object to copy.
 */
MotorAdapter::MotorAdapter(const MotorAdapter &other) 
	: mActiveMotorModel(0), mActiveMotorModelName(0), mJointType(other.mJointType)
{
}

/**
 * Destructor.
 */
MotorAdapter::~MotorAdapter() {
}

void MotorAdapter::setup() {

	mActiveMotorModel = mMotorModels.value(mActiveMotorModelName->get());
	if(mActiveMotorModel != 0) {
		mActiveMotorModel->setup();
	}
}


void MotorAdapter::clear() {
	if(mActiveMotorModel != 0) {
		mActiveMotorModel->clear();
	}
	mActiveMotorModel = 0;
}


void MotorAdapter::updateSensorValues() {
	if(mActiveMotorModel != 0) {
		mActiveMotorModel->updateOutputValues();
	}
}


void MotorAdapter::updateActuators() {
	if(mActiveMotorModel != 0) {
		mActiveMotorModel->updateInputValues();
	}
}


MotorModel* MotorAdapter::getActiveMotorModel() const {
	return mActiveMotorModel;
}

void MotorAdapter::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
}

void MotorAdapter::collectCompatibleMotorModels() {
	PhysicsManager *pm = Physics::getPhysicsManager();


	QList<SimObject*> prototypes = pm->getPrototypes();

	for(QListIterator<SimObject*> i(prototypes); i.hasNext();) {
		MotorModel *model = dynamic_cast<MotorModel*>(i.next());
		if(model != 0) {
			//TODO
			if(model->getType() == mJointType && isValidMotorModel(model)) {
				QString name = model->getName().mid(11); //remove "Prototypes/" prefix.
				MotorModel *newModel = dynamic_cast<MotorModel*>(model->createCopy());
				if(newModel != 0) {
					newModel->setOwner(this);
					mMotorModels.insert(name, newModel);
					// Add parameter from model to the adapter. These parameters are published when the adapter is added
					// to the PhysicsManager.
					QListIterator<QString> modelParaNamesIt = newModel->getParameterNames();
					while(modelParaNamesIt.hasNext()) {
						QString modelParaName = modelParaNamesIt.next();
						Value* modelParameter = newModel->getParameter(modelParaName);
						addParameter(name + "/" + modelParaName, modelParameter);
					}
				}
			}
		}
	}
	if(mActiveMotorModel == 0 && !mMotorModels.empty()) {
		mActiveMotorModelName->set(mMotorModels.keys().at(0));
	}
}


void MotorAdapter::initializeConstruction() {
	mActiveMotorModelName = new StringValue("");
	
	addParameter("ActiveMotorModel", mActiveMotorModelName);
}


void MotorAdapter::initializeCopyConstruction() {
	mActiveMotorModelName = dynamic_cast<StringValue*>(getParameter("ActiveMotorModel"));
}

}



