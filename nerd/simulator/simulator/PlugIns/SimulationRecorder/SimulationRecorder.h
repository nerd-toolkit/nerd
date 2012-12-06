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



#ifndef NERDSimulationRecorder_H
#define NERDSimulationRecorder_H

#include <QString>
#include <QFile>
#include <QDataStream>
#include "Value/InterfaceValue.h"
#include "Physics/SimObjectGroup.h"
#include "Core/SystemObject.h"
#include "Event/EventListener.h"
#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include "Value/FileNameValue.h"
#include "Value/BoolValue.h"
#include <QFile>
#include <QVarLengthArray>

namespace nerd {

	/**
	 * SimulationRecorder
	 */
	class SimulationRecorder : public virtual SystemObject, public virtual EventListener,
							public virtual ValueChangedListener 
	{
	public:
		SimulationRecorder();
		virtual ~SimulationRecorder();

		virtual QString getName() const;
		
		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		
		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value) ;
		
		virtual void startRecording();
		virtual void stopRecording();
		virtual void recordData(bool forceRecording = false);
		
		
		virtual bool startPlayback();
		virtual bool stopPlayback();
		virtual void playbackData();
		
	protected:
		
		virtual void updateListOfRecordedValues();
		virtual void updateRecordedData(QDataStream &dataStream);
		virtual void updatePlaybackData(QDataStream &dataStream);

	private:	
		Event *mResetEvent;
		Event *mStepStartedEvent;
		Event *mStepCompletedEvent;
		
		IntValue *mCurrentStep;
		BoolValue *mPhysicsDisabled;
		
		BoolValue *mActivateRecording;
		BoolValue *mActivatePlayback;
		FileNameValue *mRecordingDirectory;
		StringValue *mFileNamePrefix;
		FileNameValue *mPlaybackFile;
		BoolValue *mIncludeSimulation;
		IntValue *mRecordingInterval;
		
		QList<DoubleValue*> mRecordedValues;
		
		bool mPhysicsWasDisabled;
		
	protected:
		QFile *mFile;
		QByteArray mData;
		QDataStream *mDataStream;
		QDataStream mFileDataStream;
		
		int mExecutionMode;
		bool mReachedAndOfFile;
		
		int mStepCounter;
		bool mFirstPlaybackStep;
		bool readStepNumber;
	};

}

#endif


