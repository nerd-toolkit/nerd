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

#include "ModelInterface.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Core/Core.h"
#include "NerdConstants.h"
#include <iostream>

using namespace std;

namespace nerd {

ModelInterface::ModelInterface(const QString &name) : SimObject(name), 
				mResetSettingsCompleted(0), mRandomizeEnvironmentEvent(0), mIsInitialized(false)
{
	setPrefix(name + "/");
	mPosition = new Vector3DValue();
	mOrientation = new Vector3DValue();
	addParameter("Position", mPosition);
	addParameter("Orientation", mOrientation);
	mResetSettingsCompleted = 0;
	mMorphologyParametersPrefix = "MorphologyParameters/";
}

ModelInterface::ModelInterface(const ModelInterface &model) : Object(),
		ValueChangedListener(), EventListener(), SimObject(model), 
		mResetSettingsCompleted(0), mRandomizeEnvironmentEvent(0),
		mIsInitialized(false)
		
{	
	mPosition = dynamic_cast<Vector3DValue*>(getParameter("Position"));
	mOrientation = dynamic_cast<Vector3DValue*>(getParameter("Orientation"));
	mResetSettingsCompleted = 0;
	mMorphologyParametersPrefix = "MorphologyParameters/";
}

ModelInterface::~ModelInterface() {
	if(mResetSettingsCompleted != 0) {
		mResetSettingsCompleted->removeEventListener(this);
	}
}

void ModelInterface::setup() {
	SimObject::setup();
	if(!mIsInitialized) {
		mResetSettingsCompleted = Core::getInstance()->getEventManager()->getEvent(
			NerdConstants::EVENT_EXECUTION_RESET_SETTINGS_COMPLETED, true);
		if(mResetSettingsCompleted == 0) {
			Core::log("ModelInterface: Could not create required Reset event.", true);
			return;
		}
		mResetSettingsCompleted->addEventListener(this);	

		//make sure the simulation environment manager is available.
		Physics::getSimulationEnvironmentManager(); 

		mRandomizeEnvironmentEvent = Core::getInstance()->getEventManager()
			->registerForEvent(SimulationConstants::EVENT_RANDOMIZE_ENVIRONMENT, this, false);
		if(mRandomizeEnvironmentEvent == 0) {
			Core::log("ModelInterface: Could not find RandomizeEnvironmentEvent", true);
		}
		createModel();
		switchInputs();
		mIsInitialized = true;
	}
}

void ModelInterface::addTransformation(const QString &transformationType, 
		const Vector3D &transformation) 
{
	if(transformationType.compare("rotation") == 0 
		|| transformationType.compare("translation") == 0) 
	{
		mTransformationNames.push_back(transformationType);	
		mTransformationData.push_back(transformation);
	}
}

void ModelInterface::performTransformations() {

	if(mTransformationData.size() == mTransformationNames.size()) {
		for(int i = 0; i < mTransformationData.size(); i++) {
			if(mTransformationNames.at(i).compare("rotation") == 0) {
				Physics::rotateSimObjects(mSimObjects, mTransformationData.at(i));
			}
			else if( mTransformationNames.at(i).compare("translation") == 0) {
				Physics::translateSimObjects(mSimObjects, mTransformationData.at(i));
			}
			else {
				Core::log("ModelInterface: Found a not supported transformation tag name.");
			}
		}
	}
	
	Physics::rotateSimObjects(mSimObjects, mOrientation->get());
	Physics::translateSimObjects(mSimObjects, mPosition->get());
}


void ModelInterface::layoutObjects() {

}

void ModelInterface::randomizeObjects() {

}

void ModelInterface::reset() {
	layoutObjects();
	performTransformations();
}

void ModelInterface::switchInput(const QString &simObjectName, const QString &valueName, 
								bool makeInfo) 
{
	mSwitchInputObjectNames.append(simObjectName);
	mSwitchInputValueNames.append(valueName);
	mSwitchInputMakeInfoFlags.append(makeInfo);
}

void ModelInterface::switchInputs() {
	PhysicsManager *pm = Physics::getPhysicsManager();

	for(int i = 0; i < mSwitchInputValueNames.size() && i < mSwitchInputMakeInfoFlags.size()
			&& i < mSwitchInputObjectNames.size(); ++i)
	{
		SimObject *object = pm->getSimObject(mSwitchInputObjectNames.at(i));
		if(object == 0) {
			Core::log("ModelInterface: Could not find simobject " + mSwitchInputObjectNames.at(i));
			return;
		}
		InterfaceValue *value = dynamic_cast<InterfaceValue*>(object->getParameter(
													mSwitchInputValueNames.at(i)));
		if(value == 0) {
			Core::log("ModelInterface: Could not find interface value " 
					+ mSwitchInputValueNames.at(i) + " of SimObject " 
					+ mSwitchInputObjectNames.at(i));
			return;
		}
		object->useOutputAsInfoValue(value, mSwitchInputMakeInfoFlags.at(i));
	}
}

void ModelInterface::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mResetSettingsCompleted) {
		reset();
	}
	else if(event == mRandomizeEnvironmentEvent) {
		randomizeObjects();
	}
}

QString ModelInterface::getName() const {
	return mNameValue->get();
}

void ModelInterface::setName(const QString &name) {	
	SimObject::setName(name);
	setPrefix(mNameValue->get() + "/");
}

}
