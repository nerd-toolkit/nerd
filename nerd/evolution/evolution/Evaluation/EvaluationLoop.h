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



#ifndef EvaluationLoop_H_
#define EvaluationLoop_H_

#include "Event/EventListener.h"
#include "Value/IntValue.h"
#include "Value/BoolValue.h"
#include "Value/StringValue.h"
#include "Value/ValueChangedListener.h"
#include "Core/Core.h"
#include "Core/SystemObject.h"
#include "PlugIns/CommandLineArgument.h"

namespace nerd {

class EvaluationLoop : public virtual SystemObject,
		public virtual EventListener, public virtual ValueChangedListener
{
	public:
		EvaluationLoop(int numberOfSteps = -1, int numberOfTries = -1);
		~EvaluationLoop();
		
		virtual QString getName() const;

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		
		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value);

		void executeEvaluationLoop();
		void setClusterMode(bool isClusterMode);
	
	protected:
		virtual void performWait();
		virtual void pause();

	protected:

		Event *mNextTryEvent;
		Event *mTryCompletedEvent;
		Event *mNextStepEvent;
		Event *mStepCompletedEvent;
		Event *mShutDownEvent;
		Event *mResetEvent;
		Event *mResetFinalizedEvent;
		Event *mTerminateTryEvent;
		Event *mEvaluationStartedEvent;
		IntValue *mNumberOfTriesValue;
		IntValue *mNumberOfStepsValue;
		IntValue *mCurrentStep;
		BoolValue *mPauseSimulation;
		int mNumberOfTries;
		int mNumberOfSteps;
		CommandLineArgument *mIsEvolutionModeArgument;

	protected:
		bool mDoShutDown;
		bool mTerminateTry;
		bool mSimulationState;
		IntValue *mCurrentTry;
		BoolValue *mRunInRealTime;
	
		bool mIsEvolutionMode;
		int mInitialNumberOfTries;
		int mInitialNumberOfSteps;

};
}
#endif

