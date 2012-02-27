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

#ifndef SimpleEvolutionMainWindow_H_
#define SimpleEvolutionMainWindow_H_

#include <QWidget>
#include <QBoxLayout>
#include <QMenuBar>
#include "Gui/DebugLoggerPanel/DebugLoggerPanel.h"
#include "Gui/Event/EventVisualizationWindow.h"
#include "Gui/Event/EventDetailPanel.h"
#include "Gui/Value/ValueDetailPanel.h"
#include "Gui/Parameter/MultipleParameterWindowWidget.h"
#include "Gui/PopulationOverview/MultiplePopulationOverviewWindowWidget.h"
#include "Gui/EvolutionProgressBar/EvolutionProgressBar.h"

namespace nerd {


/**
 * SimpleEvolutionMainWindow
**/

class SimpleEvolutionMainWindow : public QWidget {

	Q_OBJECT

	public:
		SimpleEvolutionMainWindow(bool enableDebugging=false);
		virtual ~SimpleEvolutionMainWindow();

		QMenu* getMenu(const QString &name);

	private:
		void setup(bool enableDebugging);

	protected:
		virtual void timerEvent(QTimerEvent * event);

protected slots:
	void closeEvent(QCloseEvent *e);

	private:
		QBoxLayout *mWidgetLayout;
		QMenuBar *mMainMenuBar;

		DebugLoggerPanel *mEvolutionLoggerPanel;
		DebugLoggerPanel *mDebugLoggerPanel;
	
		EventVisualizationWindow *mEventList;
		EventDetailPanel *mEventDetailPanel;

		ValueDetailPanel *mValueDetailPanel;
		MultipleParameterWindowWidget *mParameterLists;
	
		bool mShutDownTriggered;
		QMap<QString, QMenu*> mMenuLookup;

		EvolutionProgressBar *mProgressBar;

		MultiplePopulationOverviewWindowWidget  *mOverview;

};
}
#endif

