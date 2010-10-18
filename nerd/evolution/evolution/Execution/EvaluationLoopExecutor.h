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



#ifndef EvaluationLoopExecutor_H_
#define EvaluationLoopExecutor_H_

#include "Evaluation/EvaluationLoop.h"
#include <QThread>
#include "Value/IntValue.h"
#include "Value/BoolValue.h"
#include "Value/StringValue.h"
#include "Value/ValueChangedListener.h"
#include "Core/Core.h"
#include "Core/SystemObject.h"
#include <QTimer>
#include <QMutex>
#include <QWaitCondition>

namespace nerd {

class EvaluationLoopExecutor : public QThread, public EvaluationLoop, public virtual ValueChangedListener 
{
	Q_OBJECT
	public:
		EvaluationLoopExecutor();
		~EvaluationLoopExecutor();
		
		//TODO maybe make EvaluationLoopExecutor a SystemObject and add to Core => handles init and shutdown.
		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		
		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value);

		virtual QString getName() const;
		void setSimulationDelay(int uSec);

	protected:
		virtual void run();
		virtual void performWait();
		virtual void pause();


	public slots:
		void realTimeStepExpired();

	signals:
		void startRealtime(int msec);
		void stopRealtime();
		void quitMainApplication();

	private:
		bool mInitialized;
		bool mRealTimeSupport;
		QTimer mRealTimeTrigger;
		IntValue *mSimulationDelay;
		BoolValue *mUseRealtimeValue;
		DoubleValue *mTimeStepSizeValue;
		QMutex mRealTimeMutex;
		QWaitCondition mRealTimeWaitCondition;
		bool mRealTimeStepExpired;

		
		Event *mNextIndividualEvent;
		Event *mIndividualCompletedEvent;
		IntValue *mNumberOfIndividuals;
		IntValue *mCurrentIndividual;

};
}
#endif

