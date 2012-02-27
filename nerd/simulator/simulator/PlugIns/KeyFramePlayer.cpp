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



#include "KeyFramePlayer.h"
#include "Event/EventManager.h"
#include "Core/Core.h"
#include <iostream>
#include "Value/ValueManager.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"
#include <QStringList>
#include "NerdConstants.h"



using namespace std;

namespace nerd {

KeyFramePlayer::KeyFramePlayer(const QString &groupName)
	: mGroupName(groupName), mControlledGroup(0), mStepCounter(0), mCurrentIndex(0),
	  mReportedAgentFoundError(false)
{
	Core::getInstance()->addSystemObject(this);

	mKeyFrameFileToLoad = new StringValue();
	Core::getInstance()->getValueManager()->addValue(
			QString("/Control/KeyFramePlayer/").append(groupName).append("/KeyFrameFile"),
			mKeyFrameFileToLoad);

	mKeyFrameFileToLoad->addValueChangedListener(this);
	mKeyFrameFileToLoad->useAsFileName(true);
}

KeyFramePlayer::~KeyFramePlayer() {
	for(int i = 0; i < mKeyFrames.size(); ++i) {
		delete mKeyFrames.at(i);
	}
}

bool KeyFramePlayer::init() {
	return true;
}


bool KeyFramePlayer::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mResetEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_RESET, this);
	mNextStepEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, this);

	ValueManager *vm = Core::getInstance()->getValueManager();

	mStepSize = vm->getDoubleValue(SimulationConstants::VALUE_TIME_STEP_SIZE);

	if(mResetEvent == 0) {
		Core::log("KeyFramePlayer: Could not find Event [/Control/ResetSimulation]");
		ok = false;
	}
	if(mNextStepEvent == 0) {
		Core::log("KeyFramePlayer: Could not find Event [/Control/StepCompleted]");
		ok = false;
	}
	if(mStepSize == 0) {
		Core::log("KeyFramePlayer: Could not find Value [/Simulation/TimeStepSize]");
		ok = false;
	}

	return ok;
}


bool KeyFramePlayer::cleanUp() {
	return true;
}


void KeyFramePlayer::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mResetEvent) {
		mCurrentIndex = 0;
		mStepCounter = 0;
	}
	else if(event == mNextStepEvent) {
		mStepCounter++;
		updateMotors();
	}
}

void KeyFramePlayer::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mKeyFrameFileToLoad) {
		loadKeyFrames(mKeyFrameFileToLoad->get(), "#", ",");
	}
}


QString KeyFramePlayer::getName() const {
	return "KeyFramePlayer";
}

void KeyFramePlayer::setKeyFrameFile(const QString &fileName) {
	mKeyFrameFileToLoad->set(fileName);
}


bool KeyFramePlayer::loadKeyFrames(const QString &fileName, const QString &commentChar,
							const QString &separator)
{
	//remove all old data.
	mCurrentIndex = 0;
	for(int i = 0; i < mKeyFrames.size(); ++i) {
		delete mKeyFrames.at(i);
	}
	mKeyFrames.clear();

	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)) {
		file.close();
		Core::log(QString("KeyFramePlayer: Could not read key frame file")
				.append(fileName).append("!"));
		return false;
	}

	QTextStream data(&file);
	while(!data.atEnd()) {
		QString line = data.readLine().trimmed();

		if(line == "" || line.indexOf(commentChar) == 0) {
			continue;
		}

		QStringList values = line.split(separator);

		QVector<double> *keyFrame = new QVector<double>();

		for(int i = 0; i < values.size(); ++i) {
			keyFrame->append(values.at(i).toDouble());
		}
		mKeyFrames.append(keyFrame);
	}

	Core::log(QString("KeyFramePlayer: Loaded ")
			.append(QString::number(mKeyFrames.size()))
			.append(" key frames from file [")
			.append(fileName).append("]."));

	cerr << "loaded keyframe data" << endl;
	file.close();

	return true;
}


void KeyFramePlayer::updateMotors() {
	if(mControlledGroup == 0) {
		PhysicsManager *pm = Physics::getPhysicsManager();

		QList<SimObjectGroup*> groups = pm->getSimObjectGroups();

		for(int i = 0; i < groups.size(); ++i) {
			if(groups.at(i) != 0 && groups.at(i)->getName() == mGroupName) {
				mControlledGroup = groups.at(i);
				break;
			}
		}
		if(mControlledGroup != 0) {
			Core::log(QString("KeyFramePlayer: Found agent [")
				.append(mGroupName).append("] to control."));
		}
		else if(!mReportedAgentFoundError) {
			//avoid multiple error logs.
			mReportedAgentFoundError = true;
			Core::log(QString("KeyFramePlayer: Could NOT find agent [")
				.append(mGroupName).append("]."));
		}
	}
	if(mCurrentIndex >= mKeyFrames.size() || mStepSize == 0 || mControlledGroup == 0) {
		return;
	}


	long millies = (long) (((double) mStepCounter) * mStepSize->get() * 1000.0);

	QVector<double> *keyframe = mKeyFrames.at(mCurrentIndex);

	QList<InterfaceValue*> motors = mControlledGroup->getInputValues();
	if(keyframe->size() >= (motors.size() + 1)) {
		double requiredMillies = keyframe->at(0);
		if(requiredMillies <= millies) {
			mCurrentIndex++;

			for(int i = 0; i < (keyframe->size() - 1) && i < motors.size(); ++i) {
				motors.at(i)->set(keyframe->at(i + 1));
			}

// 			int motorNumber = (int)(motors.size() / 2.0);
// 			//set motors to keyframe settings.
// 			int motorCount = 0;
// 			for(int i = 0; i < motorNumber && (i + 1 + motorNumber) < keyframe->size(); ++i) {
// 				motors.at(motorCount)->set(keyframe->at(i + 1) * 150.0);
// 				double torque = (keyframe->at(i + 1 + motorNumber) + 1) / 2.0;
// 				motors.at(motorCount + 1)->set(torque);
// 				motorCount += 2;
// 			}
		}
	}

}


}


