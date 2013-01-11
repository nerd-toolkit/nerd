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

#include "StatisticsManager.h"
#include "Event/EventManager.h"
#include "Core/Core.h"
#include "NerdConstants.h"

namespace nerd {

StatisticsManager::StatisticsManager() :
	mGenerationStatisticsUpdated(0), mIndividualStatisticsUpdated(0), 
	mTryStatisticsUpdated(0), mStepStatisticsUpdated(0), 
	mStepCompletedEvent(0), mTryStartedEvent(0), 
	mTryCompletedEvent(0), mIndividualStartedEvent(0), mIndividualCompletedEvent(0), 
	mGenerationStartedEvent(0), mGenerationCompletedEvent(0), mGenerationCompletedPreEvent(0),
	mCurrentStep(0), mCurrentTry(0), mCurrentGeneration(0)
{
	EventManager *em = Core::getInstance()->getEventManager();
	
	mGenerationStatisticsUpdated = em->createEvent(
			NerdConstants::EVENT_GENERATION_STATISTICS_CALCULATED,
			"Triggered when all generation StatisticsCalculators have been updated.");
	mIndividualStatisticsUpdated = em->createEvent(
			NerdConstants::EVENT_INDIVIDUAL_STATISTICS_CALCULATED,
			"Triggered when all individual StatisticsCalculators have been updated.");
	mTryStatisticsUpdated = em->createEvent(
			NerdConstants::EVENT_TRY_STATISTICS_CALCULATED,
			"Triggered when all try StatisticsCalculators have been updated.");
	mStepStatisticsUpdated = em->createEvent(
			NerdConstants::EVENT_STEP_STATISTICS_CALCULATED,
			"Triggered when all step StatisticsCalculators have been updated.");
}

StatisticsManager::~StatisticsManager() {
	QList<StatisticCalculator*> statisticsToDelete;
	statisticsToDelete << mStepStatistics;
	statisticsToDelete << mTryStatistics;
	statisticsToDelete << mIndividualStatistics;
	statisticsToDelete << mGenerationStatistics;
	
	while(!statisticsToDelete.empty()) {
		StatisticCalculator *sc = statisticsToDelete.at(0);
		statisticsToDelete.removeAll(sc);
		delete sc;
	}
  
  //////////////////////////////////////////
  // new general handling:
  //////////////////////////////////////////
  
  QList<StatisticCalculator*> toDelete =  mStatistics.uniqueKeys();
  
  for(int i = 0; i < toDelete.size(); i++) {
    delete toDelete.at(i);
  }

  mStatistics.clear();
  
  //////////////////////////////////////////
}

QString StatisticsManager::getName() const {
	return "StatisticsManager";
}


bool StatisticsManager::init() {
	bool ok = true;

	if(mGenerationStatisticsUpdated == 0) {
		Core::log(QString("StatisticsManager: Could not create event [")
					.append(NerdConstants::EVENT_GENERATION_STATISTICS_CALCULATED)
					.append("]! [TEMINATING]"));
		ok = false;
	}
	if(mIndividualStatisticsUpdated == 0) {
		Core::log(QString("StatisticsManager: Could not create event [")
					.append(NerdConstants::EVENT_INDIVIDUAL_STATISTICS_CALCULATED)
					.append("]! [TEMINATING]"));
		ok = false;
	}
	if(mTryStatisticsUpdated == 0) {
		Core::log(QString("StatisticsManager: Could not create event [")
					.append(NerdConstants::EVENT_TRY_STATISTICS_CALCULATED)
					.append("]! [TEMINATING]"));
		ok = false;
	}
	if(mStepStatisticsUpdated == 0) {
		Core::log(QString("StatisticsManager: Could not create event [")
					.append(NerdConstants::EVENT_STEP_STATISTICS_CALCULATED)
					.append("]! [TEMINATING]"));
		ok = false;
	}

 	mGenerationCompletedPreEvent = Core::getInstance()->getEventManager()->createEvent(
				"StatisticsManager/ExecuteStatistics", "Triggered to execute the statistics manager.");
	mGenerationCompletedPreEvent->addEventListener(this);

	return ok;
}


bool StatisticsManager::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mGenerationStartedEvent = em->getEvent(
			NerdConstants::EVENT_EXECUTION_GENERATION_STARTED, false);
	if(mGenerationStartedEvent == 0) {
		Core::log(QString("StatisticsManager: Could not find Event [")
				.append(NerdConstants::EVENT_EXECUTION_GENERATION_STARTED)
				.append("] to react on the start of generations. [FUNCTION DISABLED]"));
	}

	mGenerationCompletedEvent = em->registerForEvent(
			NerdConstants::EVENT_EXECUTION_GENERATION_COMPLETED, this, false);
	if(mGenerationCompletedEvent == 0) {
		Core::log(QString("StatisticsManager: Could not find Event [")
				.append(NerdConstants::EVENT_EXECUTION_GENERATION_COMPLETED)
				.append("] to react on the completion of generations. [FUNCTION DISABLED]"));
	}
	else {
		//add trigger event as upstream event to GenerationCompletedEvent.
		mGenerationCompletedEvent->addUpstreamEvent(mGenerationCompletedPreEvent);
	}

	mIndividualStartedEvent = em->registerForEvent(
			NerdConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL, this, false);
	if(mIndividualStartedEvent == 0) {
		Core::log(QString("StatisticsManager: Could not find Event [")
				.append(NerdConstants::EVENT_EXECUTION_NEXT_INDIVIDUAL)
				.append("] to react on the start of individuals. [FUNCTION DISABLED]"));
	}

	mIndividualCompletedEvent = em->registerForEvent(
			NerdConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED, this, false);
	if(mIndividualCompletedEvent == 0) {
		Core::log(QString("StatisticsManager: Could not find Event [")
				.append(NerdConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED)
				.append("] to react on the completion of individuals. [FUNCTION DISABLED]"));
	}

	mTryStartedEvent = em->registerForEvent(
			NerdConstants::EVENT_EXECUTION_NEXT_TRY, this, false);
	if(mTryStartedEvent == 0) {
		Core::log(QString("StatisticsManager: Could not find Event [")
				.append(NerdConstants::EVENT_EXECUTION_NEXT_TRY)
				.append("] to react on the start of tries. [FUNCTION DISABLED]"));
	}

	mTryCompletedEvent = em->registerForEvent(
			NerdConstants::EVENT_EXECUTION_TRY_COMPLETED, this, false);
	if(mTryCompletedEvent == 0) {
		Core::log(QString("StatisticsManager: Could not find Event [")
				.append(NerdConstants::EVENT_EXECUTION_TRY_COMPLETED)
				.append("] to react on the completion of tries. [FUNCTION DISABLED]"));
	}

	mStepCompletedEvent = em->registerForEvent(
			NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, this, false);
	if(mStepCompletedEvent == 0) {
		Core::log(QString("StatisticsManager: Could not find Event [")
				.append(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED)
				.append("] to react on the completion of steps. [FUNCTION DISABLED]"));
	}

	return ok;
}


bool StatisticsManager::cleanUp() {
	return true;
}

void StatisticsManager::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
  
	//////////////////////////////////////////
	// new general handling:
	//////////////////////////////////////////

  QList<Event*> statisticUpdateEvents; 

	QMap<StatisticCalculator*, StatisticCalculatorProperty>::iterator i = mStatistics.begin();
	while (i != mStatistics.end()) 
	{
		if(event == i.value().nextStatisticStepEvent) {
			i.key()->calculateNextValue(i.value().currentIndex);
			i.value().currentIndex++;
			if(!statisticUpdateEvents.contains(i.value().statisticUpdatedEvent)) {
				statisticUpdateEvents.append(i.value().statisticUpdatedEvent);
			}
		} 
		else if(event == i.value().clearStatisticEvent) {
			i.value().currentIndex = 0;
			i.key()->reset();;
		}
 
		i++;
	}

	for(int j = 0; j < statisticUpdateEvents.size(); j++) {
		statisticUpdateEvents.at(j)->trigger();
	}
	statisticUpdateEvents.clear();

	//////////////////////////////////////////
  
	if(event == mStepCompletedEvent) {
		++mCurrentStep;
		calculateStatistics(mStepStatistics, mCurrentStep);
		mStepStatisticsUpdated->trigger();
	}
	else if(event == mTryStartedEvent) {
		mCurrentStep = 0;
		clearStatistics(mStepStatistics);
	}
	else if(event == mTryCompletedEvent) {
		++mCurrentTry;
		calculateStatistics(mTryStatistics, mCurrentTry);
		mCurrentStep = 0;
		mTryStatisticsUpdated->trigger();
	}
	else if(event == mIndividualStartedEvent) {
		mCurrentTry = 0;
		clearStatistics(mTryStatistics);
	}
	else if(event == mIndividualCompletedEvent) {
		++mCurrentIndividual;
		calculateStatistics(mIndividualStatistics, mCurrentIndividual);
		mCurrentTry = 0;
		mIndividualStatisticsUpdated->trigger();
	}
	else if(event == mGenerationStartedEvent) {
		mCurrentIndividual = 0;
		clearStatistics(mIndividualStatistics);
	}
	else if(event == mGenerationCompletedPreEvent) {
		++mCurrentGeneration;
		calculateStatistics(mGenerationStatistics, mCurrentGeneration);
		mCurrentIndividual = 0;
		mGenerationStatisticsUpdated->trigger();
	}
}


bool StatisticsManager::addStepStatistics(StatisticCalculator *calculator) {
	if(calculator == 0 || mStepStatistics.contains(calculator)) {
		return false;
	}
	mStepStatistics.append(calculator);
	
	for(QListIterator<StatisticCalculator*> i(calculator->getChildStatistics()); i.hasNext();) {
		addStepStatistics(i.next());
	}
	return true;
}


bool StatisticsManager::removeStepStatistics(StatisticCalculator *calculator) {
	if(calculator == 0 || !mStepStatistics.contains(calculator)) {
		return false;
	}
	mStepStatistics.removeAll(calculator);

	for(QListIterator<StatisticCalculator*> i(calculator->getChildStatistics()); i.hasNext();) {
		removeStepStatistics(i.next());
	}
	return true;
}


QList<StatisticCalculator*> StatisticsManager::getStepStatistics() {
	return mStepStatistics;
}

bool StatisticsManager::addTryStatistics(StatisticCalculator *calculator) {
	if(calculator == 0 || mTryStatistics.contains(calculator)) {
		return false;
	}
	mTryStatistics.append(calculator);

	for(QListIterator<StatisticCalculator*> i(calculator->getChildStatistics()); i.hasNext();) {
		addTryStatistics(i.next());
	}
	return true;
}


bool StatisticsManager::removeTryStatistics(StatisticCalculator *calculator) {
	if(calculator == 0 || !mTryStatistics.contains(calculator)) {
		return false;
	}
	mTryStatistics.removeAll(calculator);

	for(QListIterator<StatisticCalculator*> i(calculator->getChildStatistics()); i.hasNext();) {
		removeTryStatistics(i.next());
	}
	return true;
}


QList<StatisticCalculator*> StatisticsManager::getTryStatistics() {
	return mTryStatistics;
}


bool StatisticsManager::addIndividualStatistics(StatisticCalculator *calculator) {
	if(calculator == 0 || mIndividualStatistics.contains(calculator)) {
		return false;
	}
	mIndividualStatistics.append(calculator);

	for(QListIterator<StatisticCalculator*> i(calculator->getChildStatistics()); i.hasNext();) {
		addIndividualStatistics(i.next());
	}
	return true;
}


bool StatisticsManager::removeIndividualStatistics(StatisticCalculator *calculator) {
	if(calculator == 0 || !mIndividualStatistics.contains(calculator)) {
		return false;
	}
	mIndividualStatistics.removeAll(calculator);

	for(QListIterator<StatisticCalculator*> i(calculator->getChildStatistics()); i.hasNext();) {
		removeIndividualStatistics(i.next());
	}
	return true;
}


QList<StatisticCalculator*> StatisticsManager::getIndividualStatistics() {
	return mIndividualStatistics;
}


bool StatisticsManager::addGenerationStatistics(StatisticCalculator *calculator) {
	if(calculator == 0 || mGenerationStatistics.contains(calculator)) {
		return false;
	}

	for(QListIterator<StatisticCalculator*> i(calculator->getChildStatistics()); i.hasNext();) {
		addGenerationStatistics(i.next());
	}
	mGenerationStatistics.append(calculator);
	return true;
}


bool StatisticsManager::removeGenerationStatistics(StatisticCalculator *calculator) {
	if(calculator == 0 || !mGenerationStatistics.contains(calculator)) {
		return false;
	}
	mGenerationStatistics.removeAll(calculator);

	for(QListIterator<StatisticCalculator*> i(calculator->getChildStatistics()); i.hasNext();) {
		removeGenerationStatistics(i.next());
	}
	return true;
}


QList<StatisticCalculator*> StatisticsManager::getGenerationStatistics() {
	return mGenerationStatistics;
}


int StatisticsManager::getCurrentStepIndex() const {
	return mCurrentStep;
}


int StatisticsManager::getCurrentTryIndex() const {
	return mCurrentTry;
}


int StatisticsManager::getCurrentIndividualIndex() const {
	return mCurrentIndividual;
}


int StatisticsManager::getCurrentGenerationIndex() const {
	return mCurrentGeneration;
}


void StatisticsManager::calculateStatistics(
			QList<StatisticCalculator*> calculators, int index) 
{
	for(QListIterator<StatisticCalculator*> i(calculators); i.hasNext();) {
		i.next()->calculateNextValue(index);
	}
}

void StatisticsManager::clearStatistics(QList<StatisticCalculator*> calculators) {
	for(QListIterator<StatisticCalculator*> i(calculators); i.hasNext();) {
		i.next()->reset();
	}
}



//////////////////////////////////////////
// new general components:
//////////////////////////////////////////

bool StatisticsManager::addStatistics(StatisticCalculator *calculator,
                                      const QString &nextStatisticStepEventName,
                                      const QString &clearStatisticEventName,
                                      const QString &statisticUpdatedEventName)
{
  bool ok = true;
  
  if(calculator == 0) {
    return false;
  }
  
  StatisticCalculatorProperty elem;
  elem.currentIndex = 0;
  
  // register to Events, if they do not exists --> create them
  elem.nextStatisticStepEvent = registerOrAddEvent(nextStatisticStepEventName);
  elem.clearStatisticEvent = registerOrAddEvent(clearStatisticEventName);
  
  if(elem.nextStatisticStepEvent == 0 || elem.clearStatisticEvent == 0){
    return false;
  }
  
  // add the statisticUpdatedEven
  EventManager *em = Core::getInstance()->getEventManager();
  elem.statisticUpdatedEvent = em->getEvent(statisticUpdatedEventName, true);
  
  if(elem.statisticUpdatedEvent == 0){
    Core::log(QString("%1: ERROR - could not find or add the event [%2]!").arg(
                     getName(),
                     statisticUpdatedEventName)); 
  }

  mStatistics.insert(calculator,elem);

  for(QListIterator<StatisticCalculator*> i(calculator->getChildStatistics()); i.hasNext();) {
    ok &= addStatistics(i.next(), 
                        nextStatisticStepEventName, 
                        clearStatisticEventName, 
                        statisticUpdatedEventName);
  }
  
  return ok;
}
    
    
bool StatisticsManager::removeStatistics(StatisticCalculator *calculator)
{
  mStatistics.remove(calculator);
  
  for(QListIterator<StatisticCalculator*> i(calculator->getChildStatistics()); i.hasNext();) {
    removeStatistics(i.next());
  }
  
  return true;
}


/**
 * Tries to register for the event with the given name. If the event does not
 * exist, it tries to create the event.
 * 
 * @param eventName Name of the Event.
 * 
 * @return Reference to the Event or 0 if an error occurs.
 */
Event* StatisticsManager::registerOrAddEvent(const QString &eventName)
{
  Event *event = 0;
  
  EventManager *em = Core::getInstance()->getEventManager();

  // try to register
  event = em->registerForEvent(eventName, this, false);
  
  // if the event exists not --> create it
  if(event == 0) 
  {
    em->createEvent(eventName);
    
    event = em->registerForEvent(eventName, this, false);
    
    if(event == 0){
      Core::log(QString("%1: ERROR - could not add and register for event [%2]!").arg(
                 getName(),
                 eventName)); 
    }
  }
  
  return event;  
}


/**
 * Returns all stored StatisticCalculator which are using the given statisticsStepEvent.
 * 
 * @return List of StatisticCalculator.
 */
QList<StatisticCalculator*> StatisticsManager::getStatistics(const QString &statisticsStepEvent)
{
  QList<StatisticCalculator*> list;
  
  QMap<StatisticCalculator*, StatisticCalculatorProperty>::iterator i = mStatistics.begin();
  while (i != mStatistics.end()) 
  {
    if( i.value().nextStatisticStepEvent->getNames().contains(statisticsStepEvent) )
    {
      if(!list.contains(i.key()))
      {
        list.append(i.key());
      }
    }
 
    i++;
  }
 
  return list;
}


}


