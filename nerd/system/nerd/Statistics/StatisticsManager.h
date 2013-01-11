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


#ifndef NERDStatisticsManager_H
#define NERDStatisticsManager_H

#include "Core/SystemObject.h"
#include "Statistics/StatisticCalculator.h"
#include "Event/Event.h"
#include <QList>
#include <QMap>
#include "Event/EventListener.h"

namespace nerd {

  /**
   * StatisticCalculatorProperty.
   * 
   * Used by the StatisticManager to store the related Events and the index 
   * for a StatisticCalculator.
   */
  struct StatisticCalculatorProperty 
  {   
    int currentIndex;
    Event *nextStatisticStepEvent;
    Event *clearStatisticEvent;
    Event *statisticUpdatedEvent; 
  };


  /**
   * StatisticsManager.
   *
   * TODO: The code should be refractored to use only the new introduced methods which are more 
   * general.  
	 */
	class StatisticsManager : public virtual SystemObject, public virtual EventListener {
	public:
		StatisticsManager();
		virtual ~StatisticsManager();

		virtual QString getName() const;

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		virtual void eventOccured(Event *event);

		bool addStepStatistics(StatisticCalculator *calculator);
		bool removeStepStatistics(StatisticCalculator *calculator);
		QList<StatisticCalculator*> getStepStatistics();

		bool addTryStatistics(StatisticCalculator *calculator);
		bool removeTryStatistics(StatisticCalculator *calculator);
		QList<StatisticCalculator*> getTryStatistics();

		bool addIndividualStatistics(StatisticCalculator *calculator);
		bool removeIndividualStatistics(StatisticCalculator *calculator);
		QList<StatisticCalculator*> getIndividualStatistics();

		bool addGenerationStatistics(StatisticCalculator *calculator);
		bool removeGenerationStatistics(StatisticCalculator *calculator);
		QList<StatisticCalculator*> getGenerationStatistics();

		int getCurrentStepIndex() const;
		int getCurrentTryIndex() const;
		int getCurrentIndividualIndex() const;
		int getCurrentGenerationIndex() const;

		//////////////////////////////////////////
		// new general components:
		//////////////////////////////////////////

		bool addStatistics(StatisticCalculator *calculator,
                       const QString &nextStatisticStepEventName,
                       const QString &clearStatisticEventName,
                       const QString &statisticUpdatedEventName);

		bool removeStatistics(StatisticCalculator *calculator);
		QList<StatisticCalculator*> getStatistics(const QString &statisticsStepEvent);

	private:
		void calculateStatistics(QList<StatisticCalculator*> calculators, int index);
    	void clearStatistics(QList<StatisticCalculator*> calculators);

	private:
		
		Event *mGenerationStatisticsUpdated;
		Event *mIndividualStatisticsUpdated;
		Event *mTryStatisticsUpdated;
		Event *mStepStatisticsUpdated;

		Event *mStepCompletedEvent;
		Event *mTryStartedEvent;
		Event *mTryCompletedEvent;
		Event *mIndividualStartedEvent;
		Event *mIndividualCompletedEvent;
		Event *mGenerationStartedEvent;
		Event *mGenerationCompletedEvent;
		Event *mGenerationCompletedPreEvent;

		QList<StatisticCalculator*> mStepStatistics;
		QList<StatisticCalculator*> mTryStatistics;
		QList<StatisticCalculator*> mIndividualStatistics;
		QList<StatisticCalculator*> mGenerationStatistics;

		int mCurrentStep;
		int mCurrentTry;
		int mCurrentIndividual;
		int mCurrentGeneration;


    //////////////////////////////////////////
    // new general components:
    //////////////////////////////////////////

    QMap<StatisticCalculator*, StatisticCalculatorProperty> mStatistics;
    Event* registerOrAddEvent(const QString &eventName);

	};

}

#endif


