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



#include "EvolutionProgressBar.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Event/EventManager.h"
#include "EvolutionConstants.h"
#include <QHBoxLayout>
#include "Core/Core.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new EvolutionProgressBar.
 */
EvolutionProgressBar::EvolutionProgressBar()
	: mCompletedIndividualsDuringEvolution(0), mCompletedIndividualsDuringEvaluation(0),
	  mCurrentPopulationSize(0), mEvaluationStartedEvent(0), mEvolutionStartedEvent(0),
	  mMode(0)
{
	QHBoxLayout *layout = new QHBoxLayout(this);

	mProgressBar = new QProgressBar();
	mProgressBar->setRange(0, 1);
	layout->addWidget(mProgressBar);
	setLayout(layout);

	connect(this, SIGNAL(setRange(int, int)),
			mProgressBar, SLOT(setRange(int, int)));
	connect(this, SIGNAL(setValue(int)),
			mProgressBar, SLOT(setValue(int)));

	Core::getInstance()->addSystemObject(this);
}



/**
 * Destructor.
 */
EvolutionProgressBar::~EvolutionProgressBar() {
	Core::getInstance()->removeSystemObject(this);
}


QString EvolutionProgressBar::getName() const {
	return "EvolutionProgressBar";
}

bool EvolutionProgressBar::init() {
	return true;
}


bool EvolutionProgressBar::bind() {

	ValueManager *vm = Core::getInstance()->getValueManager();

	mCompletedIndividualsDuringEvolution = 
			vm->getIntValue(EvolutionConstants::VALUE_NUMBER_OF_COMPLETED_INDIVIDUALS_EVO);
	mCompletedIndividualsDuringEvaluation = 
			vm->getIntValue(EvolutionConstants::VALUE_NUMBER_OF_COMPLETED_INDIVIDUALS_EVAL);

	QList<QString> popSizeValues = vm->getValueNamesMatchingPattern("/Evo/Main/Pop/.*/PopulationSize", true);
	if(!popSizeValues.empty()) {
			mCurrentPopulationSize = vm->getIntValue(popSizeValues.at(0));
	}



	EventManager *em = Core::getInstance()->getEventManager();

	mEvolutionStartedEvent = em->registerForEvent(
				EvolutionConstants::EVENT_EVO_EVOLUTION_ALGORITHM_STARTED, this);
	mEvaluationStartedEvent = em->registerForEvent(
				EvolutionConstants::EVENT_EVO_EVALUATION_STARTED, this);

	if(mEvolutionStartedEvent == 0 
		|| mEvaluationStartedEvent == 0 
		|| mCompletedIndividualsDuringEvolution == 0 
		|| mCompletedIndividualsDuringEvaluation == 0
		|| mCurrentPopulationSize == 0)
	{
		mProgressBar->setEnabled(false);
		mMode = 0;
	}
	else {
		mProgressBar->setEnabled(true);
		mCompletedIndividualsDuringEvolution->addValueChangedListener(this);
		mCompletedIndividualsDuringEvaluation->addValueChangedListener(this);
		mCurrentPopulationSize->addValueChangedListener(this);

		mProgressBar->setRange(0, Math::max(1, mCurrentPopulationSize->get()));
		mProgressBar->setValue(0);
		mMode = 1;
	}

	return true;
}


bool EvolutionProgressBar::cleanUp() {
	if(mEvolutionStartedEvent != 0) {
		mEvolutionStartedEvent->removeEventListener(this);
	}
	if(mEvaluationStartedEvent != 0) {
		mEvaluationStartedEvent->removeEventListener(this);
	}
	if(mCompletedIndividualsDuringEvolution != 0) {
		mCompletedIndividualsDuringEvolution->removeValueChangedListener(this);
	}
	if(mCompletedIndividualsDuringEvaluation != 0) {
		mCompletedIndividualsDuringEvaluation->removeValueChangedListener(this);
	}
	if(mCurrentPopulationSize != 0) {
		mCurrentPopulationSize->removeValueChangedListener(this);
	}
	return true;
}


void EvolutionProgressBar::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(event == mEvolutionStartedEvent) {
		mMode = 2;
	}
	else if(event == mEvaluationStartedEvent) {
		mMode = 1;
	}
}


void EvolutionProgressBar::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	if(value == mCurrentPopulationSize) {
		emit setRange(0, Math::max(1, mCurrentPopulationSize->get()));
	}
	else if(value == mCompletedIndividualsDuringEvaluation && mMode == 1) {
		emit setValue(mCompletedIndividualsDuringEvaluation->get());
	}
	else if(value == mCompletedIndividualsDuringEvolution && mMode == 2) {
		emit setValue(mCompletedIndividualsDuringEvolution->get());
	}
}


}



