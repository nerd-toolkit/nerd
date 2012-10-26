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



#ifndef NERDMultipleParameterWindowWidget_H
#define NERDMultipleParameterWindowWidget_H

#include "Gui/MultipleWindowWidget.h"
#include "Event/Event.h"
#include "Event/EventListener.h"
#include <QString>
#include "Gui/Parameter/ParameterVisualizationWindow.h"
#include "Gui/Parameter/SetInitValueTask.h"

namespace nerd {

	/**
	 * MultipleParameterWindowWidget
	 */
	class MultipleParameterWindowWidget : public MultipleWindowWidget {
	Q_OBJECT
	public:
		MultipleParameterWindowWidget(const QString &name, SetInitValueTask *setInitValueTaskPrototpye = 0, 
									  QWidget *parent = 0);
		virtual ~MultipleParameterWindowWidget();

		virtual void addTab();
		virtual ParameterVisualizationWindow* addNewVisualizationTab();

		virtual void eventOccured(Event *event);

	signals:
		void initPhaseCompleted();
		void shutDownPhaseStarted();

	public slots:
		void handleInitPhase();
		void handleShutDownPhase();
		void saveAllPanelsToFile();
		void saveSinglePanelToFile();
		void loadAllPanelsFromFile();
		void loadSinglePanelFromFile();
		void addPanelsFromFile();
		void applyAllProperties();
		void applyAndSelectAllProperties();
		void selectAllProperties();
		void deselectAllProperties();
		void copyFirstValueToAllOtherValues();
		void setInitToAllValues();
		void applySinglePanel();
		void selectSinglePanel();
		void clearSinglePanel();
		

	protected:
		virtual void timerEvent(QTimerEvent * event);
		
	private:		
		int mCounter;
		SetInitValueTask *mSetInitValueTaskPrototype;
	};

}

#endif



