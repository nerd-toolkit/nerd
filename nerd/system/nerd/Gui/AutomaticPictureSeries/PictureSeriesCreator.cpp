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



#include "PictureSeriesCreator.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "NerdConstants.h"
#include "Event/EventManager.h"
#include "Math/Math.h"
#include <QCoreApplication>
#include <QApplication>
#include <QPixmap>
#include <QDesktopWidget>
#include <QProcess>
#include <QStringList>
#include <QDir>
#include "Gui/GuiManager.h"
#include <QCoreApplication>
#include "Value/ChangeValueTask.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new PictureSeriesCreator.
 */
PictureSeriesCreator::PictureSeriesCreator()
	: mTriggerEvent(0), mPictureCounter(0), mScreenshotInProgress(false), mCurrentIteration(0),
		mCurrentWorkingDirectory(""), mFirstFrame(false)
{
	mScreenShotDelay = new IntValue(100);
	mRunCreator = new BoolValue(false);
	mFileNamePrefix = new StringValue("pic");
	mTargetDirectory = new StringValue(Core::getInstance()->getConfigDirectoryPath() + "/picSeries");
	mTargetDirectory->useAsFileName(true);
	mTriggerEventName = new StringValue(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);
	mIterationsPerFrame = new IntValue(10);
	mOutputFileName = new StringValue("output.avi");
	mOutputFileName->useAsFileName(true);
	mTryVideoCreation = new BoolValue(true);
	mFramePerSecond = new IntValue(10);
	mVideoCreationCommand = new StringValue("");
	mRecordingRectangle = new StringValue("");
	mVideoResolutionWidth = new IntValue(800);
	mVideoResolutionHeight = new IntValue(600);

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/ScreenRecorder/Config/ScreenShotDelay", mScreenShotDelay);
	vm->addValue("/ScreenRecorder/Control/Run", mRunCreator);
	vm->addValue("/ScreenRecorder/Config/FileNamePrefix", mFileNamePrefix);
	vm->addValue("/ScreenRecorder/Config/TargetDirectory", mTargetDirectory);
	vm->addValue("/ScreenRecorder/Config/TriggerEvent", mTriggerEventName);
	vm->addValue("/ScreenRecorder/Config/IterationsPerFrame", mIterationsPerFrame);
	vm->addValue("/ScreenRecorder/Video/VideoFileName", mOutputFileName);
	vm->addValue("/ScreenRecorder/Video/TryVideoCreation", mTryVideoCreation);
	vm->addValue("/ScreenRecorder/Video/FramesPerSecond", mFramePerSecond);
	vm->addValue("/ScreenRecorder/Video/OptionalVideoCreationCommand", mVideoCreationCommand);
	vm->addValue("/ScreenRecorder/Config/RecordingRectangle", mRecordingRectangle);
	//vm->addValue("/PictureCreator/Video/VideoResolutionWidth", mVideoResolutionWidth);
	//vm->addValue("/PictureCreator/Video/VideoResolutionHeight", mVideoResolutionHeight);

	mRunCreator->addValueChangedListener(this);

	Core::getInstance()->addSystemObject(this);

	connect(this, SIGNAL(takeScreenshot()),
			this, SLOT(createScreenshot()));
}



/**
 * Destructor.
 */
PictureSeriesCreator::~PictureSeriesCreator() {
}

QString PictureSeriesCreator::getName() const {
	return "PictureSeriesCreator";
}

bool PictureSeriesCreator::init() {
	bool ok = true;

	return ok;
}

bool PictureSeriesCreator::bind() {
	bool ok = true;

	mShutDownEvent = Core::getInstance()->getShutDownEvent();
	mShutDownEvent->addEventListener(this);

	return ok;
}

bool PictureSeriesCreator::cleanUp() {
	return true;
}

void PictureSeriesCreator::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mRunCreator) {
		if(mRunCreator->get()) {
			activate();
		}
		else {
			deactivate();
		}
	}
}

void PictureSeriesCreator::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mTriggerEvent) {

		mCurrentIteration++;
		if(mCurrentIteration >= mIterationsPerFrame->get()) {

			mCurrentIteration = 0;

			mScreenshotInProgress = true;
			QCoreApplication::instance()->thread()->wait(Math::min(1000, mScreenShotDelay->get()));
	
			emit takeScreenshot();
	
			while(mScreenshotInProgress) {
				QCoreApplication::instance()->thread()->wait(10);
			}
		}
	}
	else if(event == mShutDownEvent) {
		if(mRunCreator->get()) {
			//make sure that the video is completed before shutdown.
			deactivate();
		}
	}
}

void PictureSeriesCreator::createScreenshot() {

	if(mFirstFrame) {
		guessMainSimulationWindowRectangle();
		mFirstFrame = false;
	}

	QPixmap originalPixmap = QPixmap::grabWindow(QApplication::desktop()->winId());

	if(mCurrentRecordingRectangle != QRect()) {
		originalPixmap = originalPixmap.copy(mCurrentRecordingRectangle);
	}

	Core::getInstance()->enforceDirectoryPath(mTargetDirectory->get());

	QString numberString = QString::number(mPictureCounter++);
	while(numberString.length() < 6) {
		numberString.prepend("0");
	}

	QString fileName =  mCurrentWorkingDirectory + "/" + mFileNamePrefix->get() 
						+ numberString + ".png";

	originalPixmap.save(fileName, QString("png").toAscii());

	mScreenshotInProgress = false;
}

void PictureSeriesCreator::activate() {
	if(mTriggerEvent != 0) {
		mTriggerEvent->removeEventListener(this);
	}

	mCurrentWorkingDirectory = mTargetDirectory->get();

	mFirstFrame = true;

	EventManager *em = Core::getInstance()->getEventManager();
	mTriggerEvent = em->registerForEvent(mTriggerEventName->get(), this);

	if(mTriggerEvent == 0) {
		Core::log("PictureSeriesCreator: Warning, could not switch to trigger event ["
					+ mTriggerEventName->get() + "]");
	}
}

void PictureSeriesCreator::deactivate() {
	if(mTriggerEvent != 0) {
		mTriggerEvent->removeEventListener(this);
	}

	mTriggerEvent = 0;

	if(mTryVideoCreation->get()) {
		//try mencoder
		int framePerSecond = mFramePerSecond->get();
		
		QProcess encoderProc;
		encoderProc.setWorkingDirectory(mCurrentWorkingDirectory);

		if(mVideoCreationCommand->get().trimmed() == "") {
			QStringList args;
			args << "mf://*.png"
				<< "-mf" << "w=" + mVideoResolutionWidth->getValueAsString()
						 + ":h=" + mVideoResolutionHeight->getValueAsString() 
						 + ":fps=" + QString::number(framePerSecond) + ":type=png"
				<< "-ovc" << "lavc"
				<< "-lavcopts" << "vcodec=mpeg4:mbd=2:trell"
				<< "-oac" << "copy"
				<< "-o" << mOutputFileName->get();
			Core::log("Execute: " + args.join(" "), true);
			encoderProc.start("mencoder", args);
		}
		else {
			QStringList args = mVideoCreationCommand->get().split(" ");
			if(args.size() > 0) {
				QString command = args.first();
				args.removeFirst();
				encoderProc.start(command, args);
			}
		}

		cerr << "Encoding video (please wait)! ";

		encoderProc.waitForStarted();
	
		while(encoderProc.state() == QProcess::Running) {
			cerr << ".";
			encoderProc.waitForFinished(500);
			Core::getInstance()->executePendingTasks();
		}
		cerr << "." << endl;
		Core::getInstance()->executePendingTasks();
	
		//check for autoclear
		if(encoderProc.exitCode() == 0) {
			cerr << "Video [" << QString(mCurrentWorkingDirectory + "/" + mOutputFileName->get())
					.toStdString().c_str() 
				 << "] created successfully!" << endl;
	
			QDir dir(mCurrentWorkingDirectory);
	
			QStringList files = dir.entryList();
			for(QListIterator<QString> i(files); i.hasNext();) {
				QString fileName = i.next();
				if(fileName.endsWith(".png")) {
					dir.remove(fileName);
				}
			}
		}
		else {
			cerr << "Failed to create Video! Exit code " << encoderProc.exitCode() << endl;
		}
	}
}


void PictureSeriesCreator::guessMainSimulationWindowRectangle() {

	mCurrentRecordingRectangle = QRect();
	QString rectangleString = mRecordingRectangle->get().trimmed();

	if(rectangleString == "") {
		QWidget *mainSimulationWidget = GuiManager::getGlobalGuiManager()
					->getWidget(NerdConstants::GUI_MAIN_SIMULATION_WINDOW);
		
		if(mainSimulationWidget != 0 && mainSimulationWidget->isWindow()) {
			if(!(mainSimulationWidget->isHidden() || !mainSimulationWidget->isVisible())) {
				mCurrentRecordingRectangle = mainSimulationWidget->frameGeometry();
		
				Core::getInstance()->scheduleTask(new ChangeValueTask(mRecordingRectangle, 
						QString("(" )
							+ QString::number(mCurrentRecordingRectangle.x()) + ","
							+ QString::number(mCurrentRecordingRectangle.y()) + ","
							+ QString::number(mCurrentRecordingRectangle.bottomRight().x()) + ","
							+ QString::number(mCurrentRecordingRectangle.bottomRight().y()) + ")"));
			}
		}
	}

	//determine recording rectangle
	if(rectangleString != "") {
		QString rectangleString = mRecordingRectangle->get();
		if(rectangleString.startsWith("(") && rectangleString.endsWith(")")) {
			QStringList coords = rectangleString.mid(1, rectangleString.size() - 2).split(",");
			if(coords.size() == 4) {
				double x1 = coords[0].toDouble();
				double y1 = coords[1].toDouble();
				double x2 = coords[2].toDouble();
				double y2 = coords[3].toDouble();

				mCurrentRecordingRectangle = QRect(QPoint(x1, y1), QPoint(x2, y2));
			}
		}
	}

}



}


