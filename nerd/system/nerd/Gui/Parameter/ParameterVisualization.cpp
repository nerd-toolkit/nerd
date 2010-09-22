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



#include "ParameterVisualization.h"
#include "ParameterVisualizationWindow.h"
#include <QString>
#include <QGridLayout>
#include <QPalette>
#include "Core/Task.h"
#include "Core/Core.h"
#include <iostream>
#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace std;

namespace nerd{

class ChangeValue : public Task {

	public:
		ChangeValue(Value *value, const QString &newValue) {
			mValue = value;
			mNewValue = newValue;
		}
	
		virtual ~ChangeValue() {}
			
		bool runTask() {
			if(mValue == 0) {
				return true;
			}
			mValue->setValueFromString(mNewValue);
			return true;
		}

	private:
		Value *mValue;	
		QString mNewValue;
};


// class SetSnapshotValue : public Task {
// 
// 	public:
// 		SetSnapshotValue(Value *value, const QString &newValue) {
// 			mValue = value;
// 			mNewValue = newValue;
// 		}
// 	
// 		virtual ~SetSnapshotValue() {}
// 			
// 		bool runTask() {
// 			if(mValue == 0) {
// 				return true;
// 			}
// 			SimulationEnvironmentManager *sem = Physics::getSimulationEnvironmentManager();
// 			PhysicsManager *pm = Physics::getPhysicsManager();
// 
// 			const QList<SimObject*> &simObjects = pm->getSimObjects();
// 			for(QListIterator<SimObject*> i(simObjects); i.hasNext();) {
// 				SimObject *obj = i.next();
// 			
// 				const QList<Value*> &params = obj->getParameters();
// 				for(QListIterator<Value*> j(params); j.hasNext();) {
// 					Value *value = j.next();
// 					if(value == mValue) {
// 						
// 						//get parameter name
// 						QList<QString> parameterNames = obj->getParameterNames();
// 						for(QListIterator<QString> k(parameterNames); k.hasNext();) {
// 							QString name = k.next();
// 							if(obj->getParameter(name) == mValue) {
// 								sem->storeParameter(obj, name, mNewValue);
// 								return true;
// 							}
// 						}
// 					}
// 				}
// 			}
// 		
// 			//if value could not be found...
// 			Core::log(QString("ParameterVisualization Warning: Could not find parameter!"));
// 			return true;
// 		}
// 
// 	private:
// 		Value *mValue;
// 		QString mNewValue;
// };


ParameterVisualization::ParameterVisualization(ParameterVisualizationWindow *list, 
				Value *value, QString name, SetInitValueTask *setInitValueTaskPrototype) 
	: QFrame(), mUpdateSnapshotButton(0), mSetInitValueTaskPrototype(setInitValueTaskPrototype)
{
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	mValueList = list;
	mValue = value;
	mValueName = name;
	
	mNameLabel = new QLabel(mValueName);
	mValueField = new QLineEdit(this);
	mCloseButton = new QPushButton("x");
	
	mUpdateSnapshotButton = new QPushButton("Set Init");
	mUpdateValue = new QCheckBox();

	connect(mValueField, SIGNAL(returnPressed()), 
			this, SLOT(changeValue()));
	connect(mValueField, SIGNAL(textEdited(const QString&)), 
			this, SLOT(markAsValueEdited()));
	connect(mCloseButton, SIGNAL(clicked()), 
			this, SLOT(destroy()));
	connect(mUpdateSnapshotButton, SIGNAL(clicked()), 
			this, SLOT(updateValueInEnvironmentManager())); 
	connect(this, SIGNAL(lineEditTextChanged(QString)), 
			mValueField,  SLOT(setText(QString)));
	connect(this, SIGNAL(lineEditTextChanged(QString)),
			this, SLOT(markAsValueUpdated()));
	connect(mUpdateValue, SIGNAL(stateChanged(int)), 
			this, SLOT(setDoUpdateValue(int)));
	
	lineEditTextChanged(mValue->getValueAsString());
	mUpdateValue->setCheckState(Qt::Checked);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(1);
	setLayout(layout);
	layout->addWidget(mNameLabel);
	layout->setAlignment(mNameLabel, Qt::AlignLeft);

	QHBoxLayout *mFrameLayout = new QHBoxLayout();
	layout->addLayout(mFrameLayout);

	mFrameLayout->setMargin(1);
	mFrameLayout->addWidget(mUpdateValue);
	mFrameLayout->addWidget(mValueField);
	if(mSetInitValueTaskPrototype != 0 && name.startsWith("/Sim/")) {
		mFrameLayout->addWidget(mUpdateSnapshotButton);
	}
	mFrameLayout->addStretch(1000);
	mFrameLayout->addWidget(mCloseButton);

	mUpdateSnapshotButton->setFixedWidth(60);
	mCloseButton->setFixedWidth(20);
	mValueField->setFixedWidth(250);

	setFrameStyle(QFrame::Panel | QFrame::Plain);
	setLineWidth(2);

	mValue->addValueChangedListener(this);
}


/** 
 * Destructor.
 */
ParameterVisualization::~ParameterVisualization() {
	reset();
}


/**
 * Called whenever the observer checkbox is checked or unchecked.
 * @param doUpdate indicates whether the update is enabled or not.
 */
void ParameterVisualization::setDoUpdateValue(int doUpdate)  {
	mDoUpdateValue = doUpdate;

	if(mValue == 0) {
		return;
	}

	if(!mDoUpdateValue) {
		mValue->removeValueChangedListener(this);
		mUpdateValue->setCheckState(Qt::Unchecked);
	}
	else {
		//regiser as listener and set the edit text to the current value.
		lineEditTextChanged(mValue->getValueAsString());
		mValue->addValueChangedListener(this);
		mUpdateValue->setCheckState(Qt::Checked);
		
		markAsValueUpdated();
	}
}


/**
 * This method copies the content of the text field into the 
 * reset snapshot of the SimulationEnvironmentManager. This applies 
 * value changes permanently, as the values from the reset shapshot are
 * taken to initialize the environment and agents during a reset.
 */
void ParameterVisualization::updateValueInEnvironmentManager() {


	if(mValue == 0 || mSetInitValueTaskPrototype == 0 || !mValueName.startsWith("/Sim/")) {
		return;
	}
	Core::getInstance()->scheduleTask(mSetInitValueTaskPrototype->create(mValue, mValueField->text()));
	
}




/**
 * Triggers the destruction of this object. In principle this method simply
 * resets the visualization and then emits signal destroyThis().
 * Exactly one object (usually the owning ParameterVisualizationWindow) should
 * connect to this signal WITH MODIFIER Qt::QueuedConnectionto ensure that 
 * the ParameterVisualization stopped processing before it is finally deleted 
 * in the owner ParameterVisualizationWindow.
 */
void ParameterVisualization::destroy() {
	reset();
	emit destroyThis(mValueName);
}


/**
 * Returns the name of this ParameterVisualization object.
 *
 * @return the name
 */
QString ParameterVisualization::getName() const{
	return QString("ParameterVisualization [").append(mValueName).append("]");
}


/**
 * Returns the name that is associated with the observed Value.
 * 
 * @return the name of the Value.
 */
QString ParameterVisualization::getValueName() const {
	return mValueName;
}


/**
 * Called when the observed Value changed. This will update the text field with the 
 * current value of the Value.
 *
 * @param value the value that changed.
 */
void ParameterVisualization::valueChanged(Value *value) {

	if(value == 0) {
		return;
	}
	else if(value == mValue) {
		if(mDoUpdateValue) {
			lineEditTextChanged(value->getValueAsString());
		}
	}
}



/**
 * Returns the currently observed Value object.
 *
 * @return the currently observed Value.
 */
Value* ParameterVisualization::getValue() {
	return mValue;
}


/**
 * Returns true if the visualization currently is observing the Value.
 *
 * @return true if observation is active, otherwise false.
 */
bool ParameterVisualization::isValueUpdateActive() const {
	return (mUpdateValue->checkState() == Qt::Checked);
}


void ParameterVisualization::setValueObjectByName(const QString &name) {
	ValueManager *vm = Core::getInstance()->getValueManager();

	if(mValue != 0) {
		mValue->removeValueChangedListener(this);
	}
	mValueName = name;
	mValue = vm->getValue(mValueName);

	if(mValue == 0) {
		mNameLabel->setText("Not Found: " + mValueName);
	}
	else {
		mNameLabel->setText(mValueName);
		
		setDoUpdateValue(mDoUpdateValue);
	}
}


/**
 * Sets the current value content for the text field.
 * 
 * @param currentValue the content of the text field.
 */
void ParameterVisualization::setCurrentValue(const QString &currentValue) {
	emit lineEditTextChanged(currentValue);
	markAsValueEdited();
	mValueModified = true;
}


QString ParameterVisualization::getCurrentValue() const {
	return mValueField->text();
}


/**
 * Called when the user pressed the enter key in the text field.
 * If the text has been changed, then the content is set at the Value object.
 * Setting the new value is done via Task in the simulation thread, so the change 
 * might be applied with a short delay.
 */
void ParameterVisualization::changeValue() {
	markAsValueUpdated();
	if(mValueField->isModified() || mValueModified) {
		ChangeValue *changeValueTask = new ChangeValue(mValue, mValueField->text());
		Core::getInstance()->scheduleTask(changeValueTask);
	}
	mValueModified = false;
}


/**
 * Resets the Object. Hereby the Value object is released and observation canceled.
 */
void ParameterVisualization::reset() {
	if(mValue != 0) {
		mValue->removeValueChangedListener(this);
	}
	mValue = 0;
}


/**
 * Marks the background color of the text label red to show that the content
 * has been changed manually.
 */
void ParameterVisualization::markAsValueEdited() {
	QPalette p = mValueField->palette();
	p.setColor(QPalette::Base, QColor(255,120,120));
	mValueField->setPalette(p);
}


/**
 * Marks the background color of the text label white to show that the content
 * originates from the observed Value.
 */
void ParameterVisualization::markAsValueUpdated() {
	QPalette p = mValueField->palette();
	p.setColor(QPalette::Base, Qt::white);
	mValueField->setPalette(p);
}


}
