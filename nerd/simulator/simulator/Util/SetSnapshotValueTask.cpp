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



#include "SetSnapshotValueTask.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Physics/SimulationEnvironmentManager.h"
#include "Physics/PhysicsManager.h"
#include "Physics/Physics.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SetSnapshotValueTask.
 */
SetSnapshotValueTask::SetSnapshotValueTask(Value *snapshotValue, const QString &newValue)
	: mSnapshotValue(snapshotValue), mNewValue(newValue)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the SetSnapshotValueTask object to copy.
 */
SetSnapshotValueTask::SetSnapshotValueTask(const SetSnapshotValueTask &other)
	: mSnapshotValue(other.mSnapshotValue), mNewValue(other.mNewValue)
{
}

/**
 * Destructor.
 */
SetSnapshotValueTask::~SetSnapshotValueTask() {
}


SetSnapshotValueTask* SetSnapshotValueTask::createCopy() {
	return new SetSnapshotValueTask(*this);
}

Task* SetSnapshotValueTask::create(Value *snapshotValue, const QString &newValue) {
	return new SetSnapshotValueTask(snapshotValue, newValue);
}

bool SetSnapshotValueTask::runTask() {
	if(mSnapshotValue == 0) {
		return true;
	}
	SimulationEnvironmentManager *sem = Physics::getSimulationEnvironmentManager();
	PhysicsManager *pm = Physics::getPhysicsManager();

	const QList<SimObject*> &simObjects = pm->getSimObjects();
	for(QListIterator<SimObject*> i(simObjects); i.hasNext();) {
		SimObject *obj = i.next();
	
		const QList<Value*> &params = obj->getParameters();
		for(QListIterator<Value*> j(params); j.hasNext();) {
			Value *value = j.next();
			if(value == mSnapshotValue) {
				
				//get parameter name
				QList<QString> parameterNames = obj->getParameterNames();
				for(QListIterator<QString> k(parameterNames); k.hasNext();) {
					QString name = k.next();
					if(obj->getParameter(name) == mSnapshotValue) {
						sem->storeParameter(obj, name, mNewValue);
						return true;
					}
				}
			}
		}
	}

	//if value could not be found...
	Core::log(QString("ParameterVisualization Warning: Could not find parameter!"));
	return true;
}




}


