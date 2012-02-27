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



#include "MotorControlPanel.h"
#include "Value/InterfaceValue.h"
#include "Core/Task.h"
#include "Core/Core.h"
#include <QFont>
#include <QBoxLayout>
#include "NerdConstants.h"


namespace nerd {

class SetValueTask : public Task {

	public:
		SetValueTask(InterfaceValue *value, double newValue) {
			mValue = value;
			mNewValue = newValue;
		}
	
		virtual ~SetValueTask() {}
			
		bool runTask() {
			mValue->set(mNewValue);
			return true;
		}

	private:
		InterfaceValue *mValue;	
		double mNewValue;
};

MotorControlPanel::MotorControlPanel(InterfaceValue *valueToControl, QWidget *parent)
		: QFrame(parent) 
{
	mControlledValue = valueToControl;
	QBoxLayout *mLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
	mNameLabel = new QLabel(mControlledValue->getName(), this);	
	mLayout->setAlignment(mNameLabel, Qt::AlignLeft);
	QFont testFont;
	testFont.setPointSize(9);
	mNameLabel->setFont(testFont);
	mLayout->addWidget(mNameLabel);	

	mControlSlider = new QSlider(Qt::Horizontal, this);
	mControlSlider->setMinimum(-50);
	mControlSlider->setMaximum(50);
	mControlSlider->setMaximumWidth(250);
	mControlSlider->setMinimumWidth(250);
	mLayout->addWidget(mControlSlider);
	mLayout->setAlignment(mNameLabel, Qt::AlignLeft);
	mLayout->setAlignment(mControlSlider, Qt::AlignRight);

	mControlSlider->setFocusPolicy(Qt::StrongFocus);
	mControlSlider->setTickPosition(QSlider::NoTicks);
	mControlSlider->setTickInterval(1);
	mControlSlider->setSingleStep(1);
	mControlledValue->addValueChangedListener(this);
	
	mLayout->setMargin(1);
	setLayout(mLayout);
	setFrameStyle(QFrame::Panel | QFrame::Plain);
	setLineWidth(2);
	
	mMin = mControlledValue->getMin();
	mMax = mControlledValue->getMax();
	
	double value = mControlledValue->get();
	double	mapped = (-1 + (((1 +1)
					/ (mMax - mMin)) * (value - mMin)));
	mapped *= 50;
	mControlSlider->setValue((int)mapped);
	
	connect(mControlSlider, SIGNAL(sliderMoved(int)), this, SLOT(setValue(int)));
	connect(mControlSlider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
	connect(this, SIGNAL(actuatorNameChanged(QString)), mNameLabel, SLOT(setText(QString)));

	mResetEvent = Core::getInstance()->getEventManager()
			->getEvent(NerdConstants::EVENT_EXECUTION_RESET, false);
	if(mResetEvent == 0) {
		Core::log("MotorControlGui: Required Event could not be found.");
		return;
	}
	mResetEvent->addEventListener(this);

}

MotorControlPanel::~MotorControlPanel() {
	if(mControlledValue != 0) {
		mControlledValue->removeValueChangedListener(this);
	}
	if(mResetEvent != 0) {
		mResetEvent->removeEventListener(this);
	}
}

/**
 * Schedules a task to midify the value of the motor that is controlled with this slider. 
 * @param value The value to be set. This is a value between [-1, 1] that is mapped internally to the working-range of the motor that is controlled with this slider.
 */
void MotorControlPanel::setValue(int value) {

	double currentValue = value / 50.0;
	double mapped = 0.0;
	
	if(mMin < mMax) {
		currentValue = (currentValue > 1) ? 1: currentValue;
		currentValue = (currentValue < -1) ? -1: currentValue;

		mapped = (mMin + (((mMax - mMin)
					/ 2) * (currentValue +1)));
	}
	if(mControlledValue != 0) {
		SetValueTask *task = new SetValueTask(mControlledValue, mapped);
		Core::getInstance()->scheduleTask(task);
	}
	
}

void MotorControlPanel::setSliderValue(double value) {
	mControlSlider->setValue((int)value);
}

double MotorControlPanel::getSliderValue() const {
	if(mControlledValue != 0) {
		return mControlSlider->value();
	}
	return 0.0;
}

void MotorControlPanel::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	if(value == mControlledValue) {
		emit actuatorNameChanged(mControlledValue->getName());
	}
}

void MotorControlPanel::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(event == mResetEvent) {
		mMin = mControlledValue->getMin();
		mMax = mControlledValue->getMax();
		mControlSlider->setValue(0);
	}
}

QString MotorControlPanel::getName() const {
	return "MotorControlPanel";
}

void MotorControlPanel::unbind() {
	if(mControlledValue != 0) {
		mControlledValue->removeValueChangedListener(this);
		mControlledValue = 0;
	}
	if(mResetEvent != 0) {
		mResetEvent->removeEventListener(this);
		mResetEvent = 0;
	}
}

}
