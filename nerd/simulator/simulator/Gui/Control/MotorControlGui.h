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



#ifndef MotorControlGui_H_
#define MotorControlGui_H_

#include <QBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QWidget>

#include <QPushButton>
#include "Event/Event.h"
#include "Event/EventListener.h"
#include "Value/ValueChangedListener.h"
#include "Core/SystemObject.h"
#include "Value/BoolValue.h"

namespace nerd{

class SimObjectGroup;
class MotorControlPanel;

class MotorControlGui : public QWidget, public virtual EventListener, public virtual ValueChangedListener
{
	Q_OBJECT
	public:
		
		MotorControlGui(const QString &simObjectGroupName);
		virtual ~MotorControlGui();

		bool setUp();
		virtual bool cleanUp();
		virtual QString getName() const;
		virtual void eventOccured(Event *event);
		virtual void valueChanged (Value *value);

	protected:
		virtual void keyPressEvent(QKeyEvent *e);

	signals:
		void update();
		void createSliders();

	public slots:
		void initializeSliders();
		void changeVisibility();	

	private slots:
		void triggerReset();
		void triggerPause();
		void closeEvent(QCloseEvent *e);
		void performUpdate();

	private:

		SimObjectGroup *mObjectGroup;
		QVector <MotorControlPanel*> mControlPanels;
		QScrollArea *mValueListArea;
		QBoxLayout *mListLayout;
		QWidget *mValueList;
		QGridLayout *mControlLayout;
		QPushButton *mResetSimulationButton;
		QPushButton *mPauseSimulationButton;
		Event *mResetEvent;
		Event *mResetFinalizedEvent;
		BoolValue *mPauseValue;
// 		QPushButton *mStartButton;
		QAction* mShowWindow;
		int mXPos;
		int mYPos;

		QString mSimObjectGroupName;
		bool mInitialized;

		QVector<QVector<double>*> mKeyFrames;
	
};
}
#endif

