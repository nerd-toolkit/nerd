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

#ifndef SimulationEnvironmentManager_H_
#define SimulationEnvironmentManager_H_

#include "Event/EventListener.h"
#include "Core/SystemObject.h"
#include "Randomization/Randomizer.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"
#include "SimulationConstants.h"
#include <QHash>

namespace nerd {

class Event;
class EventManager;
class SimObject;
class Value;

/**
 * SimulationEnvironmentManager stores all parameter settings of the 
 * SimObject objects, that exist in the Simulation. The manager is an 
 * EventListener for the event "/Control/ResetSimulationSettings" and 
 * reacts to the event by changing the value of all stored parameters 
 * to the values that are saved. 
 */

class SimulationEnvironmentManager : public virtual SystemObject, 
									 public virtual EventListener 
{	
	public:
		SimulationEnvironmentManager();
		virtual ~SimulationEnvironmentManager();
		
		//trigger the initialization.
		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		
		virtual void eventOccured(Event *e);
		virtual QString getName() const;
		
		void storeParameter(SimObject *object, const QString &paramValueName, const QString &valueAsString);
	
		void clearParameter(SimObject *object, const QString &paramValueName);
		void clearParameters(SimObject *object);
		void clearAllParameters();
	
		void resetStartConditions();
		void createSnapshot();
	
		bool addRandomizer(Randomizer *randomizer);
		bool removeRandomizer(Randomizer *randomizer);

		QHash<SimObject*, QHash<Value*, QString> > getSnapshot() const;
		void setSnapshot(QHash<SimObject*, QHash<Value*, QString> > snapshot);
	
	private:
		void performRandomization();
			
	private:
		Event *mResetSettingsEvent;
		Event *mRandomizeEnvironmentEvent;
		IntValue *mCurrentTry;
		IntValue *mCurrentSimulationSeed;
		BoolValue *mRandomizeSeedAtReset;
	
		QHash<SimObject*, QHash<Value*, QString> > mParameterSettings;	
		QList<Randomizer*> mSimulationRandomizer;
};
}
#endif

