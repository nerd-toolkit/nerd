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



#include "MotorControlGui.h"
#include "Physics/SimObjectGroup.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include <QBoxLayout>
#include "MotorControlPanel.h"
#include "Core/Task.h"
#include <QHBoxLayout>
#include <QKeyEvent>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include "Physics/Physics.h"
#include "NerdConstants.h"
#include "SimulationConstants.h"

using namespace std;

namespace nerd {

class ChangePauseValue : public Task {

	public:
	ChangePauseValue(BoolValue *value, bool newValue) {
		mValue = value;
		mNewValue = newValue;
	}

	~ChangePauseValue() {}
		
	bool runTask() {
		mValue->set(mNewValue);
		return true;
	}

	private:
		BoolValue *mValue;	
		bool mNewValue;
};


class ResetTask : public Task {

	public:
	ResetTask(Event *resetEvent, Event *finalizedEvent) {
		mResetEvent = resetEvent;
		mResetFinalizedEvent = finalizedEvent;
	}

	~ResetTask() {}
		
	bool runTask() {
		mResetEvent->trigger();
		mResetFinalizedEvent->trigger();
		return true;
	}

	private:
		Event *mResetEvent;
		Event *mResetFinalizedEvent;
};


MotorControlGui::MotorControlGui(const QString &simObjectGroupName) {
	setWindowTitle(QString("Motor Control Sliders - ").append(simObjectGroupName));
//	setAttribute(Qt::WA_DeleteOnClose);
	
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);
	
	mControlLayout = new QGridLayout();
	setLayout(mControlLayout);

	mValueListArea = new QScrollArea(this);
	mValueListArea->setWidgetResizable(true);
	mValueList = new QWidget(mValueListArea);
	
	mListLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	mListLayout->setAlignment(Qt::AlignTop);
	mValueList->setLayout(mListLayout);
	
	mValueListArea->setWidget(mValueList);
	mControlLayout->addWidget(mValueListArea, 0,0);
	mListLayout->setSpacing(1);

	QHBoxLayout *boxLayout = new QHBoxLayout;
	mControlLayout->addLayout(boxLayout, 1,0);
	
	mPauseSimulationButton = new QPushButton("Pause");
	boxLayout->addWidget(mPauseSimulationButton);

	mResetSimulationButton = new QPushButton("Reset");
	boxLayout->addWidget(mResetSimulationButton);

	connect(mResetSimulationButton, SIGNAL(clicked()), this, SLOT(triggerReset()));
	connect(mPauseSimulationButton, SIGNAL(clicked()), this, SLOT(triggerPause()));
	connect(this, SIGNAL(update()), this, SLOT(performUpdate()));
	connect(this, SIGNAL(createSliders()), this, SLOT(initializeSliders()));

// 	resize(600,600);
	setMinimumSize(200,400);
	mXPos = 0;
	mYPos = 0;

	mInitialized = false;
	mSimObjectGroupName = simObjectGroupName;

// 	Core::getInstance()->addSystemObject(this);
	setUp();

	//Add 10 keyframe vectors.
	for(int i = 0; i < 10; ++i) {
		mKeyFrames.append(new QVector<double>());
	}

	setWhatsThis("The motor control panel can be used to control the motor angles of the robot "
				"manually. The simulation can also be paused and reset using the buttons. \n\n"
				"In addition up to 10 motor settings (key frames) can be stored in the panel "
				"by pressing CTRL + [0-9]. The current setting then is stored in one of the "
				"key frame slots. A keyframe setting can be restored by pressing the corresponding "
				"number key [0-9].");
}

MotorControlGui::~MotorControlGui() {
	while(mControlPanels.size() > 0) {
		MotorControlPanel *tmp = mControlPanels.first();
		mControlPanels.remove(0);
		delete tmp;
	}
	//delete keyframe vectors.
	for(int i = 0; i < mKeyFrames.size(); ++i) {
		delete mKeyFrames.at(i);
	}
}


bool MotorControlGui::setUp() {
	EventManager *em = Core::getInstance()->getEventManager();

	mResetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET, false);
	mResetFinalizedEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET_COMPLETED, false);

	if(mResetEvent == 0 || mResetFinalizedEvent == 0) {
		Core::log("MotorControlGui: Could not find required Event.");
		return false;
	}

	mResetEvent->addEventListener(this);

	mPauseValue = Core::getInstance()->getValueManager()->getBoolValue(
			SimulationConstants::VALUE_EXECUTION_PAUSE);

	if(mPauseValue == 0) {
		Core::log("MotorControlGui: Could not find required Value.");
		return false;
	}
	valueChanged(mPauseValue); //make sure the pause button text is correct.

	mPauseValue->addValueChangedListener(this);
	//emit createSliders();
	return true;
}


bool MotorControlGui::cleanUp() {
	if(mResetEvent != 0) {
		mResetEvent->removeEventListener(this);
	}
	for(int i = 0; i < mControlPanels.size(); ++i) {
		MotorControlPanel *tmp = mControlPanels.at(i);
		tmp->unbind();
	}
	return true;
}


void MotorControlGui::changeVisibility() {
	if(isHidden()) {
		move(mXPos, mYPos);
		if(!mInitialized) {
			initializeSliders();
		}
		show();
	} 
	else {
		hide();
		mXPos = x();
		mYPos = y();
	}
}



void MotorControlGui::triggerReset() {
	if(mResetEvent != 0) {
		Core::getInstance()->scheduleTask(new ResetTask(mResetEvent, mResetFinalizedEvent));
	}
}


void MotorControlGui::triggerPause() {
		Core::getInstance()->scheduleTask(new ChangePauseValue(mPauseValue, !mPauseValue->get()));
}


void MotorControlGui::closeEvent(QCloseEvent*) {
	mXPos = x();
	mYPos = y();
}


void MotorControlGui::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(event == mResetEvent && !mInitialized) {
		emit update();
	}
}

QString MotorControlGui::getName() const {
	return "MotorControlGui";
}

void MotorControlGui::performUpdate() {
	emit createSliders();
}

void MotorControlGui::initializeSliders() {
	QList<SimObjectGroup*> groups = Physics::getPhysicsManager()->getSimObjectGroups();
	mObjectGroup = 0;	
	for(int i = 0; i < groups.size(); ++i) {
		if(groups.at(i)->getName().compare(mSimObjectGroupName) == 0) {
			mObjectGroup = groups.at(i);
			break;
		}
	}
	
	if(mObjectGroup == 0) {
		mInitialized = false;
		return;
	}

	QList<InterfaceValue*> inputs = mObjectGroup->getInputValues();
	for(int i = 0; i < inputs.size(); i++) {
		MotorControlPanel *newPanel = new MotorControlPanel(inputs.at(i), this);
		mListLayout->addWidget(newPanel);
		mControlPanels.push_back(newPanel);
	}
	mInitialized = true;
}


void MotorControlGui::valueChanged ( Value *value ) {
	if(value == mPauseValue) {
		if(mPauseValue->get()) {
		mPauseSimulationButton->setText("Unpause");
		}else{
		mPauseSimulationButton->setText("Pause");
		}
	}
}

void MotorControlGui::keyPressEvent(QKeyEvent *e) {

	if(e->key() >= Qt::Key_0 && e->key() <= Qt::Key_0 + mKeyFrames.size()) {
		//One of the number keys was pressed (1-9)

		QVector<double> *keyframe = mKeyFrames.at(e->key() - Qt::Key_0);

		if(e->modifiers() & Qt::ControlModifier) {
			//CTRL key => store keyframe

			keyframe->clear();

			//memorize keyframe
			for(int i = 0; i < mControlPanels.size(); ++i) {
				MotorControlPanel *panel = mControlPanels.at(i);
				keyframe->append(panel->getSliderValue());
			}
		}
		else {
			//restore keyframe
			for(int i = 0; i < mControlPanels.size() && i < keyframe->size(); ++i) {
				MotorControlPanel *panel = mControlPanels.at(i);
				panel->setSliderValue(keyframe->at(i));
			}
		}
	}
	else if(e->key() == Qt::Key_S) {
		//Save keyframes to file in property directory.
		QFile file(Core::getInstance()->getConfigDirectoryPath()
						.append("/properties/keyframes.bak"));
	
		if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			file.close();
			return;
		}
		QTextStream output(&file);
		for(int i = 0; i < mKeyFrames.size(); ++i) {
			QVector<double> *keyframe = mKeyFrames.at(i);
			output << "#" << i << endl;
			for(int j = 0; j < keyframe->size(); ++j) {
				output << QString::number(keyframe->at(j)) << ";";
			}
			output << endl;
		}
		file.close();
	}
	else if(e->key() == Qt::Key_L) {
		//Load keyframes from file in property directory.
		QFile file(Core::getInstance()->getConfigDirectoryPath()
						.append("/properties/keyframes.bak"));

		if(!file.open(QIODevice::ReadOnly)) {
			file.close();
			return;
		}
		QTextStream reader(&file);
	
		int index = 0;

		while(!reader.atEnd()) {
			if(index >= mKeyFrames.size()) {
				break;
			}
			QString line = reader.readLine();
			if(line.startsWith("#")) {
				continue;
			}
			else if(line.trimmed() != "") {
				QVector<double> *keyframe = mKeyFrames.at(index);
				keyframe->clear();
				QStringList values = line.split(";");
				for(int i = 0; i < values.size(); ++i) {
					keyframe->append(values.at(i).toDouble());
				}
				index++;
			}
		}
		file.close();
	}
	else {
		e->ignore();
	}
}

}
