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


#include "SimObject.h"
#include "Core/ParameterizedObject.h"
#include <QList>
#include "Physics/SimObjectGroup.h"
#include <iostream>

using namespace std;

namespace nerd {


/**
 * Constructor.
 *
 * @param name the name of the SimObject.
 */
SimObject::SimObject(const QString &name)
		: ParameterizedObject(name), mNameValue(new StringValue(name)), mOwnerGroup(0),
		  mSynchronizingWithPhysicalModel(false)
{
	addParameter("Name", mNameValue);
	mNameValue->addValueChangedListener(this);
}


/**
 * Constructor.
 *
 * @param name the name of the SimObject.
 * @param prefix the prefix used to publish parameters globally.
 */
SimObject::SimObject(const QString &name, const QString &prefix)
		: ParameterizedObject(name, prefix), mNameValue(new StringValue(name)), mOwnerGroup(0),
		  mSynchronizingWithPhysicalModel(false)
{
	addParameter("Name", mNameValue);
	mNameValue->addValueChangedListener(this);
}


/**
 * Copy constructor.
 * Creates a deep copy of the other object including all parameters.
 * Copies of published paraemters of the other object are NOT published
 * at the ValueManager. They can only be used locally.
 * The geometries of the original SimObject are not added to the copy.
 * @param object the SimObject to copy.
 */
SimObject::SimObject(const SimObject &object) : Object(), ValueChangedListener(), 
		ParameterizedObject(object), mNameValue(0), mOwnerGroup(0), 
		mSynchronizingWithPhysicalModel(false)
{
	mNameValue = dynamic_cast<StringValue*>(getParameter("Name"));
	mNameValue->addValueChangedListener(this);

	mInputValues.clear();
	mInfoValues.clear();
	mOutputValues.clear();

	//copy all child objects
	for(QListIterator<SimObject*> i(object.mChildObjects); i.hasNext();) {
		SimObject *child = i.next();
		if(child != 0) {
			addChildObject(child->createCopy());
		}
	}
}


/**
 * Destructor.
 */
SimObject::~SimObject() {
// 	for(int i = 0; i < mSimGeometries.size(); i++) {
// 		delete mSimGeometries.at(i);
// 	}
// 	mNameValue->removeValueChangedListener(this);

	//do not delete child objects
	//this is done by the PhysicsManager. If not registered there, the child objects
	//have to be deleted manually.
}


/**
 * Creates a copy of this SimObject using the copy constructor.
 *
 * @return a copy of this SimObject.
 */
SimObject* SimObject::createCopy() const {
	return new SimObject(*this);
}

void SimObject::setup() {
}

void SimObject::clear() {
}

/**
 * Synchronizes internal parameters with the current state of the
 * physical model. This method has to be overwritten in subclasses
 * of SimObject, which have direct access to the chosen physics model.
 *
 * This implementation does nothing.
 */
void SimObject::synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *) {
}


/**
 * Returns the name of this SimObject.
 *
 * @return the name.
 */
QString SimObject::getName() const {
	return mNameValue->get();
}


/**
 * Returns the name including the group name as namespace.
 * Note: The absolute name always starts with a /. If the name of the SimObject  
 * already starts with a / then no additional / is added.
 *
 */
QString SimObject::getAbsoluteName() const {
	QString name = getName();
	if(!name.startsWith("/")) {
		name.prepend("/");
	}
	if(mOwnerGroup == 0) {
		return name;
	}
	return QString("/").append(mOwnerGroup->getName()).append(name);
}


/**
 * Sets the name of this SimObject.
 *
 * @param name the new name.
 */
void SimObject::setName(const QString &name) {
	mNameValue->set(name);
}


/**
 * Returns a QList with all available input InterfaceValues. Such values are used as 
 * interface to set control parameters during the simulation execution, e.g. to control 
 * a motor.
 * @return a QList with all available input control Values.
 */
const QList<InterfaceValue*>& SimObject::getInputValues() const {
	return mInputValues;
}


/**
 * Returns a QList with all available output InterfaceValues. Such values are used as
 * interface to export sensor or internal parameters during the simulation execution, e.g.
 * sensor measurements.
 *
 * @return a QList with all available output control Values.
 */
const QList<InterfaceValue*>& SimObject::getOutputValues() const {
	return mOutputValues;
}


/**
 * Returns a QList with all available info InterfaceValues. Such values are used as
 * interface to export sensor or internal parameters during the simulation execution, e.g.
 * sensor measurements as information only.
 *
 * @return a QList with all available output control Values.
 */
const QList<InterfaceValue*>& SimObject::getInfoValues() const {
	return mInfoValues;
}


/**
 * Moves an OutputValue to the list of InfoValues. Thus this Value is not part of the 
 * control interface of a SimObjectGroup any more. 
 *
 * @param outputValue the OutputValue to switch to a InfoNeuron.
 * @param useAsInfoValue if true, then the Value is used as InfoValue, otherwise as OutputValue.
 */
bool SimObject::useOutputAsInfoValue(InterfaceValue *outputValue, bool useAsInfoValue) {
	if(outputValue == 0 
		|| (!mOutputValues.contains(outputValue) && !mInfoValues.contains(outputValue)))
	{
		return false;
	}
	if(useAsInfoValue) {
		if(mOutputValues.contains(outputValue)) {
			mOutputValues.removeAll(outputValue);
			if(!mInfoValues.contains(outputValue)) {
				mInfoValues.append(outputValue);
			}
		}
	}
	else {
		if(mInfoValues.contains(outputValue)) {
			mInfoValues.removeAll(outputValue);
			if(!mOutputValues.contains(outputValue)) {
				mOutputValues.append(outputValue);
			}
		}
	}
	return true;
}

/**
 * Called when one of the paraemters changed.
 *
 * @param value the value that changed.
 */
void SimObject::valueChanged(Value *value) {
	ParameterizedObject::valueChanged(value);
	if(value == mNameValue) {
		updateInterfaceNames();	
	}
}


/**
 * Returns the registered SimGeom objects of this SimGeometry.
 * In case, that any change in the geometry have been observed,
 * updatedGeometries() is called first.
 */
QList<SimGeom*> SimObject::getGeometries() {
	return mSimGeometries;
}


/**
 * Replaces the current SimGeoms with a new set of SimGeoms.
 * Note: all previously registered SimGeoms are destroyed!
 *
 * @param geometries the new set of geometries.
 */
void SimObject::setGeometries(const QVector<SimGeom*> &geometries){
	//destroy all previously registered SimGeoms.
	for(int i = 0; i < mSimGeometries.size(); ++i) {
		delete mSimGeometries.at(i);
	}
	//set the new SimGeoms
	mSimGeometries.clear();
	for(int i = 0; i < geometries.size(); ++i) {
		//prevent duplicated geometry pointers which would lead to double destruction.
		addGeometry(geometries.at(i));
	}
}


/**
 * Adds a new SimGeom to the set of geometries.
 *
 * @param simGeometry the SimGeom to add.
 */
bool SimObject::addGeometry(SimGeom* simGeometry) {
	if(simGeometry == 0 || mSimGeometries.contains(simGeometry)) {
		//this ensures that a SimGeom is not added twice, which would lead to double destruction.
		return false;
	}
	mSimGeometries.append(simGeometry);
	simGeometry->setSimObject(this);
	return true;
}


/**
 * Removes a SimGeom from the set of geometries.
 *
 * @param simGeometry the SimGeom to remove.
 * @return true if successful, false if it was not contained.
 */
bool SimObject::removeGeometry(SimGeom *simGeometry) {
	if(simGeometry == 0 || !mSimGeometries.contains(simGeometry)) {
		return false;
	}
	mSimGeometries.removeAll(simGeometry);
	simGeometry->setSimObject(0);
	return true;
}

void SimObject::updateInterfaceNames() {
	QList<InterfaceValue*> interfaces;
	interfaces << mInputValues;
	interfaces << mOutputValues;
	interfaces << mInfoValues;
	for(QListIterator<InterfaceValue*> i(interfaces); i.hasNext();) {
		i.next()->setPrefix(getName());
	}
}


/**
 * Sets the SimObjectGroup this SimObject belongs to.
 *
 * @param group the current SimObjectGroup.
 */
void SimObject::setObjectGroup(SimObjectGroup *group) {
	mOwnerGroup = group;
}


/**
 * Returns the SimObjectGroup this SimObject belongs to.
 * If this SimObject does not belong to a SimObjectGroup, then NULL is returned.
 */
SimObjectGroup* SimObject::getObjectGroup() const {
	return mOwnerGroup;
}


/**
 * Adds a SimObject as child to this SimObject.
 * Child objects are automatically copied when the parent SimObject is 
 * copied. 
 *
 * @param child the SimObject to add.
 */
bool SimObject::addChildObject(SimObject *child) {
	if(child == 0 || mChildObjects.contains(child)) {
		return false;
	}
	mChildObjects.append(child);
	return true;
}	


/** 
 * Removes a child SimObject from this SimObject.  
 *
 * @param child the SimObject to remove.
 */
bool SimObject::removeChildObject(SimObject *child) {
	if(child == 0 || !mChildObjects.contains(child)) {
		return false;
	}
	mChildObjects.removeAll(child);
	return true;
}

/**
 * Returns a list with all registered child objects.
 */
const QList<SimObject*>& SimObject::getChildObjects() const {
	return mChildObjects;
}


}


