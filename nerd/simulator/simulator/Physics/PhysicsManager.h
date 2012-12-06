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

#ifndef NERDPHYSICSMANAGER_H
#define NERDPHYSICSMANAGER_H

#include <QHash>
#include <QList>
#include <QString>
#include "SimBody.h"
#include "Core/SystemObject.h"
#include "Control/ControllerProvider.h"
#include "SimJoint.h"
#include "Event/EventListener.h"
#include "Event/Event.h"
#include "Physics/SimObjectGroup.h"
#include "Physics/SimSensor.h"
#include "Physics/SimActuator.h"
#include "Value/IntValue.h"
#include <QTime>
#include <QMutex>

namespace nerd {

class PhysicalSimulationAlgorithm;
class CollisionManager;

/**
 * PhysicsManager.
 * 
 * The PhysicsManager represents the physical abstraction layer of NERD. All physical objects
 * are registered and managed here. 
 * 
 * Prototypes can be registered and used as blue prints for objects just using their names. 
 * This allows the usage of objects without knowing their exact class.
 * 
 * SimObjectGroups are used to logically group SimObjects together into interfaces containing
 * InterfaceValues for their objects inputs, outputs and infos. 
 *
 * The PhysicalSimulationAlgorithm that is used with a PhysicsManager implements the physics
 * engine dependent algorithmic code required to execute and reset simulations.
 *
 * The PhysicsManager also handles the clean destruction of all created SimObjects and Prototypes.
 */
class PhysicsManager : public virtual SystemObject, public virtual EventListener, 
					   public virtual ControllerProvider 
{

	public:
		PhysicsManager();
		virtual ~PhysicsManager();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		//Physical simulation execution
		bool resetSimulation();
		bool executeSimulationStep();
		void clearPhysics();

		bool addSimObject(SimObject *object);
		bool removeSimObject(SimObject *object);
		QList<SimObject*> getSimObjects() const;
		SimObject* getSimObject(const QString &name);
		QList<SimObject*> getSimObjects(const QString &namePattern);

		QList<SimBody*> getSimBodies() const;
		SimBody* getSimBody(const QString &name);

		QList<SimJoint*> getSimJoints() const;
		SimJoint* getSimJoint(const QString &name);

		void destroySimObjects();

		bool addSimObjectGroup(SimObjectGroup *group);
		bool removeSimObjectGroup(SimObjectGroup *group);
		QList<SimObjectGroup*> getSimObjectGroups() const;
		SimObjectGroup* getSimObjectGroup(const QString &name) const;

		void destroySimObjectGroups();

		//PhysicalSimulationAlgorithm
		void setPhysicalSimulationAlgorithm(PhysicalSimulationAlgorithm *algorithm);
		PhysicalSimulationAlgorithm* getPhysicalSimulationAlgorithm() const;

		//Prototype Pool
		SimBody* getBodyPrototype(const QString &name) const;
		SimJoint* getJointPrototype(const QString &name) const;

		bool addPrototype(const QString &name, SimObject *prototype);
		SimObject* getPrototype(const QString &name) const;
		QList<QString> getPrototypeNames() const;
		QList<SimObject*> getPrototypes() const;
		void destroyPrototypes();

		virtual void eventOccured(Event *e);
		virtual QString getName() const;

		virtual QList<ControlInterface*> getControllerInterfaces() const;
		virtual ControlInterface* getControllerInterface(const QString &controllerName) const;

		void updateSensors();
		void updateActuators();

		QMutex* getResetMutex();
		Event* getResetEvent() const;

		//Environment modification announcement / adaptation.
		Event* getPhysicsEnvironmentChangedEvent() const;
		void triggerPhysicsEnvironmentChangedEvent();

	private:
		PhysicalSimulationAlgorithm *mPhysicalSimulationAlgorithm;
		CollisionManager *mCollisionManager;
		QHash<QString, SimObject*> mPrototypes;
		QList<SimBody*> mBodyObjects;
		QList<SimJoint*> mJointObjects;
		QList<SimSensor*> mSensorObjects;	
		QList<SimActuator*> mActuatorObjects;
		QList<SimObject*> mSimObjects;
		QList<SimObjectGroup*> mSimObjectGroups;
		bool mSynchronizeObjectsWithPhysicalModel;
		QTime mStopwatch;	

		Event *mNextStepEvent;
		Event *mCompletedNextStepEvent;
		Event *mResetEvent;
		Event *mResetFinalizedEvent;
		Event *mResetSettingsEvent;
		Event *mResetSettingsTerminatedEvent;
		Event *mPhysicsEnvironmentChangedEvent;

		QList<QString> mParameterNames;

		bool mInitialResetDone;

		DoubleValue *mCurrentSimulationTime;
		DoubleValue *mCurrentRealTime;
		IntValue *mCurrentStep;
		IntValue *mResetDuration;
		IntValue *mStepExecutionDuration;
		IntValue *mPhysicalStepDuration;
		IntValue *mSynchronizationDuration;
		IntValue *mPostStepDuration;
		IntValue *mCollisionHandlingDuration;
		BoolValue *mSwitchYZAxes;
		
		BoolValue *mDisablePhysics;
		
		QMutex mResetMutex;
};

}

#endif 
