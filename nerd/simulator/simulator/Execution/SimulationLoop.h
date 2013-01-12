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

#ifndef SimulationLoop_H_
#define SimulationLoop_H_

#include <QThread>
#include "Event/EventListener.h"
#include "Value/IntValue.h"
#include "Value/BoolValue.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Core/SystemObject.h"
#include <QTimer>
#include <QMutex>
#include <QWaitCondition>

namespace nerd {

class SimulationLoop : public QThread, public virtual SystemObject,
		public virtual EventListener, public virtual ValueChangedListener 
{
	Q_OBJECT
	public:
		SimulationLoop();
		~SimulationLoop();
		
		void run();
		
		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		
		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value);

		virtual QString getName() const;
		void setSimulationDelay(int mSec);

	public slots:
		void realTimeStepExpired();

	signals:
		void startRealtime(int msec);
		void stopRealtime();
		void quitMainApplication();

	private:

		IntValue *mSimulationDelay;
		BoolValue *mPauseSimulation;	
		Event *mNextStepEvent;
		Event *mCompletedStepEvent;
		Event *mShutDownEvent;
		Event *mResetEvent;
		Event *mResetFinalizedEvent;
		bool mDoShutDown;
		bool mSimulationState;
		QTimer mRealTimeTrigger;
		BoolValue *mUseRealtimeValue;
		DoubleValue *mTimeStepSizeValue;
		QMutex mRealTimeMutex;
		QWaitCondition mRealTimeWaitCondition;
		bool mRealTimeStepExpired;
};
}
#endif

