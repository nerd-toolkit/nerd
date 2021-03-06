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
#include <Value/CodeValue.h>
#include <QFile>
#include <QVarLengthArray>
#include <qstringlist.h>
#include <qtextstream.h>
#include "Value/RangeValue.h"
#include "Value/ULongLongValue.h"

namespace nerd {


	/**
	 * SimulationRecorder
	 */
	class SimulationRecorder : public virtual SystemObject, public virtual EventListener,
							public virtual ValueChangedListener 
	{
	public:
		enum {SIMREC_OFF, SIMREC_RECORDING, SIMREC_PLAYBACK};
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
		
		virtual void updateRecordedValueNameList();

		
	protected:
		
		virtual void updateListOfRecordedValues();
		virtual void syncWithListOfRecordedValues();
		virtual void updateRecordedData(QDataStream &dataStream);
		virtual void updatePlaybackData(QDataStream &dataStream);
		virtual void writeInfoFile(QTextStream &dataStream);

	private:	
		Event *mResetEvent;
		Event *mStepStartedEvent;
		Event *mStepCompletedEvent;
		
		IntValue *mNumberOfSteps;
		IntValue *mCurrentStep;
		BoolValue *mPhysicsDisabled;
		BoolValue *mSimulationPaused;
		
		BoolValue *mActivateRecording;
		BoolValue *mActivatePlayback;
		BoolValue *mPlaybackSafeMode;
		FileNameValue *mRecordingDirectory;
		StringValue *mFileNamePrefix;
		IntValue *mRecordingInterval;
		RangeValue *mStartEndFrameValue;
		IntValue *mNumberOfFramesValue;
		IntValue *mDesiredFrameValue;
		IntValue *mCurrentFrameValue;
		BoolValue *mResetTotalStepsCounter;
		ULongLongValue *mTotalStepsCounters;
		
		
		bool mPhysicsWasDisabled;
		Range mStartEndFrameRange;
		int mNumberOfFrames;
		uint mFrameNumber;
		bool mWasPaused;
		int mPreviousNumberOfStepsSetting;
		
	protected:
		FileNameValue *mPlaybackFile;
		QList<Value*> mRecordedValues;
		QStringList mRecordedValueNames;
		
		QFile *mFile;
		QByteArray mData;
		QDataStream *mDataStream;
		QDataStream mFileDataStream;
		
		CodeValue *mObservedValues;
		CodeValue *mRecordedValueNameList;
		
		int mExecutionMode;
		bool mReachedAndOfFile;
		
		int mStepCounter;
		bool mFirstPlaybackStep;
		bool mReadStepNumber;
		uint mVersion;
	};

}

#endif


