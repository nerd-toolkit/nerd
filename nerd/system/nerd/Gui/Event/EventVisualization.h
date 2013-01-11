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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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


#ifndef EVENTVISUALIZATION_H_
#define EVENTVISUALIZATION_H_

#include <QWidget>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QMutex>
#include <QWaitCondition>

#include "../../Event/Event.h"
#include "../../Event/EventListener.h"

namespace nerd {

class CloseTask;
class EventVisualizationWindow;

/**
 * EventVisualization.
 */
class EventVisualization : public QFrame, public virtual EventListener {

	Q_OBJECT

	friend class CloseTask;

	public:
		EventVisualization(Event *event, EventVisualizationWindow *mainWindow);
		~EventVisualization();
		void eventOccured(Event *event);

		virtual QString getName() const;

		Event* getEvent();
		
		static void setBlockAfterEvent(bool block);
		static void wakeUpBlockingEvent();

		void setDoUpdate(int doUpdate);

	protected:
		virtual void closeEvent(QCloseEvent *event);

	private:
		QLabel *mEventNumberOfTriggers;
		QCheckBox *mUpdateTriggerCount;
		QPushButton *mCloseButton;

		Event *mEvent;

		QMutex mMutex;

		bool mDoUpdate;
		int mNumberOfTriggering;

		QMutex mCloseDownMutex;
		QWaitCondition mCloseDownWaitCondition;

		EventVisualizationWindow *mMainWindow;
	private:
		void removeEvent();

	private slots:
		void resetTriggerCount();
		void changeFrameColorSlot(int color);
		void setDoUpdateSlot(int doUpdate);

	signals:
		void numberChanged(QString newNumber);
		void changeFrameColorSignal(int color);
		void changeTriggerStepButton(bool b);
};

}
#endif
