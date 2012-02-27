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

#include "FitnessFunction.h"
#include "EvolutionConstants.h"
#include "Value/DoubleValue.h"
#include "Value/ValueManager.h"
#include "Core/Core.h"
#include "Value/Value.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "Statistics/StatisticsManager.h"
#include "Statistics/Statistics.h"
#include <limits>
#include "Fitness/Fitness.h"
#include "Fitness/FitnessManager.h"
#include "Statistics/StatisticCalculator.h"
#include <iostream>

using namespace std;

namespace nerd {

/**
 * Constructor.
 * @param name Name of the fitness function.
 */
FitnessFunction::FitnessFunction(const QString &name) 
	: ParameterizedObject(name), mAttachedToSystem(false), mHelpMessage(0), 
		mErrorState(0),	mMeanFitnessStatistics(0),
		mMinFitnessStatistics(0),mMaxFitnessStatistics(0), 
		mVarianceStatistics(0), mCurrentFitness(0), mFitnessValue(0)
{
	setPrefix(EvolutionConstants::FITNESS_FUNCTION_PATH + name + "/");

	mFitnessValue = new DoubleValue(0.0);
	addParameter("Fitness/Fitness", mFitnessValue);

	mCurrentFitness = new DoubleValue(0.0);
	addParameter("Fitness/CurrentFitness", mCurrentFitness);

	mHelpMessage = new StringValue("");
	addParameter("Config/Help", mHelpMessage);

	mErrorState = new StringValue("");
	addParameter("Config/ErrorState", mErrorState);

	mFitnessCalculationMode = new IntValue(FitnessFunction::MEAN_FITNESS);
	addParameter("Fitness/CalculationMode", mFitnessCalculationMode);

	mMeanFitness = new DoubleValue();
	mMinFitness = new DoubleValue();
	mMaxFitness = new DoubleValue();
	mVariance = new DoubleValue();

	addParameter("Fitness/MeanFitness", mMeanFitness);
	addParameter("Fitness/MinFitness", mMinFitness);
	addParameter("Fitness/MaxFitness", mMaxFitness);
	addParameter("Fitness/Variance", mVariance);

}


/**
 * Copy-constructor.
 * @param copy 
 */
FitnessFunction::FitnessFunction(const FitnessFunction &copy) : Object(), 
		ValueChangedListener(), ParameterizedObject(copy), mAttachedToSystem(false), 
		mHelpMessage(0), mErrorState(0), 
		mMeanFitnessStatistics(0), mMinFitnessStatistics(0), mMaxFitnessStatistics(0), 
		mVarianceStatistics(0),
		mCurrentFitness(0), mFitnessValue(0)
		
{
	mFitnessValue = dynamic_cast<DoubleValue*>(getParameter("Fitness/Fitness"));
	mCurrentFitness = dynamic_cast<DoubleValue*>(getParameter("Fitness/CurrentFitness"));
	mHelpMessage = dynamic_cast<StringValue*>(getParameter("Config/Help"));
	mErrorState = dynamic_cast<StringValue*>(getParameter("Config/ErrorState"));
	mFitnessCalculationMode = dynamic_cast<IntValue*>(getParameter("Fitness/CalculationMode"));

	mMeanFitness = dynamic_cast<DoubleValue*>(getParameter("Fitness/MeanFitness"));
	mMinFitness = dynamic_cast<DoubleValue*>(getParameter("Fitness/MinFitness"));
	mMaxFitness = dynamic_cast<DoubleValue*>(getParameter("Fitness/MaxFitness"));
	mVariance = dynamic_cast<DoubleValue*>(getParameter("Fitness/Variance"));

}

FitnessFunction::~FitnessFunction() {
	detachFromSystem();
}


/**
 * Integrates the FitnessFunction into the NERD system. 
 * Hereby default statistic methods for mean, min and max fitness and for 
 * fitness variance are created and added both to the StatisticsManager
 * and the ValueManager.
 *
 * Furthermore all FitnessFunction parameters are published in the ValueManager.
 */
void FitnessFunction::attachToSystem() {
	if(mAttachedToSystem) {
		//do nothing because the FitnessFunction is already attached.
		return;
	}

	mMeanFitnessStatistics = new StatisticCalculator(
		QString("Fitness/").append(getName()).append("/Mean"), 
		QString("/Statistics/"));
	mMeanFitnessStatistics->setObservedValue(mMeanFitness);

	mMinFitnessStatistics = new StatisticCalculator(
		QString("Fitness/").append(getName()).append("/Min"), 
		QString("/Statistics/"));
	mMinFitnessStatistics->setObservedValue(mMinFitness);

	mMaxFitnessStatistics = new StatisticCalculator(
		QString("Fitness/").append(getName()).append("/Max"), 
		QString("/Statistics/"));
	mMaxFitnessStatistics->setObservedValue(mMaxFitness);

	mVarianceStatistics = new StatisticCalculator(
		QString("Fitness/").append(getName()).append("/Variance"), 
		QString("/Statistics/"));
	mVarianceStatistics->setObservedValue(mVariance);
	
	StatisticsManager *sm = Statistics::getStatisticsManager();
	sm->addGenerationStatistics(mMeanFitnessStatistics);
	sm->addGenerationStatistics(mMinFitnessStatistics);
	sm->addGenerationStatistics(mMaxFitnessStatistics);
	sm->addGenerationStatistics(mVarianceStatistics);

	publishAllParameters();

	mAttachedToSystem = true;
}


/**
 * Removes all default StatisticsCalcualtors from ValueManager and StatisticsManager
 * and unpublishes all parameters of the FitnessFunction.
 */
void FitnessFunction::detachFromSystem() {
	if(!mAttachedToSystem) {
		//do nothing because the FitnessFunction is already detached.
		return;
	}

	StatisticsManager *sm = Statistics::getStatisticsManager();

	if(mMeanFitnessStatistics != 0) {
		sm->removeGenerationStatistics(mMeanFitnessStatistics);
		delete mMeanFitnessStatistics;
		mMeanFitnessStatistics = 0;
	}
	if(mMinFitnessStatistics != 0) {
		sm->removeGenerationStatistics(mMinFitnessStatistics);
		delete mMinFitnessStatistics;
		mMinFitnessStatistics = 0;
	}
	if(mMaxFitnessStatistics != 0) {
		sm->removeGenerationStatistics(mMaxFitnessStatistics);
		delete mMaxFitnessStatistics;
		mMaxFitnessStatistics = 0;
	}
	if(mVarianceStatistics != 0) {
		sm->removeGenerationStatistics(mVarianceStatistics);
		delete mVarianceStatistics;
		mVarianceStatistics = 0;
	}
	unpublishAllParameters();

	mAttachedToSystem = false;
}

/**
 *  Overwrites ParameterizedObject::setName().
 * @param name 
 */
void FitnessFunction::setName(const QString &name) {
	setPrefix(EvolutionConstants::FITNESS_FUNCTION_PATH + name + "/");
	ParameterizedObject::setName(name);
}

/**
 * Convenience Function to access the current value of a Double- or IntValue using the name of the value.
 *
 * @param valueName The name of the value (Double- or IntValue).
 * @return  The current double-value of the value with the given name.
 */
double 	FitnessFunction::getValue(const QString &valueName) const {

	Value *value = Core::getInstance()->getValueManager()->getValue(valueName);

	if(dynamic_cast<DoubleValue*>(value) != 0) {
		return (dynamic_cast<DoubleValue*>(value))->get();
	}
	else if(dynamic_cast<IntValue*>(value) != 0) {
		return (double) (dynamic_cast<IntValue*>(value))->get();
	}
	else {
		Core::log("Fitness " + getName() + ": Could not find value "
			+ valueName + "!");
	}
	return 0.0;
}


QString FitnessFunction::getValueAsString(const QString &valueName) const {

	Value *value = Core::getInstance()->getValueManager()->getValue(valueName);

	if(value != 0) {
		return value->getValueAsString();
	}
	return "";
}

/**
 * @return The value of the total fitness parameter. The parameter is updated 
 *         after each try, so the returned value represents the total value for 
 *         all tries of that are completed so far.
 */
double FitnessFunction::getFitness() {
	return mFitnessValue->get();
}

/**
 * @return The value of the current fitness parameter. This parameter is 
 *         updated during each execution step and represents the 
 *         fitness value so far for the current try.
 */
double FitnessFunction::getCurrentFitness() {
	return mCurrentFitness->get();
}
	
/**
 * Calculate the total fitness based on the list of fitness values for 
 * different tries of the same individual.
 * The default implementation creates the mean over all tries as total 
 * fitness value for the individual. To use a different type fitness calculation, 
 * this method needs to be overwritten. You can also switch parameter 
 * Fitness/CalculationMode to one of the supported modes.
 */
void FitnessFunction::calculateFitness() {
	double fitness = 0.0;
	if(mFitnessOfTries.empty()) {
		return;
	}
	switch(mFitnessCalculationMode->get()) {
		case FitnessFunction::MEAN_FITNESS:
		{
			for(int i = 0; i < mFitnessOfTries.size(); i++) {
				fitness += mFitnessOfTries.at(i);
			}
			mFitnessValue->set(fitness / ((double) mFitnessOfTries.size()));
			break;
		}
		case FitnessFunction::MIN_FITNESS:
		{
			if(mFitnessOfTries.size() == 1) {
				mFitnessValue->set(mFitnessOfTries.at(0));
				return;
			}
			fitness = mFitnessOfTries.at(0);
			for(int i = 1; i < mFitnessOfTries.size(); i++) {
				if(mFitnessOfTries.at(i) < fitness) {
					fitness = mFitnessOfTries.at(i);
				}
			}
			mFitnessValue->set(fitness);
			break;
		}
		case FitnessFunction::MAX_FITNESS:
		{
			if(mFitnessOfTries.size() == 1) {
				mFitnessValue->set(mFitnessOfTries.at(0));
				return;
			}
			fitness = mFitnessOfTries.at(0);
			for(int i = 1; i < mFitnessOfTries.size(); i++) {
				if(mFitnessOfTries.at(i) > fitness) {
					fitness = mFitnessOfTries.at(i);
				}
			}
			mFitnessValue->set(fitness);
			break;
		}
	}
	
}


/**
 * Performs a complete reset of the fitness function. This method is called 
 * before the evaluation of a new individual starts.
 */
void FitnessFunction::reset() {
	mFitnessOfTries.clear();
	mCurrentFitness->set(0.0);
	mFitnessValue->set(0.0);
}
	
/**
 * Performes a reset of the fitness function to prepare for a new try of the 
 * same individual. 
 */
void FitnessFunction::resetTry() {
	mCurrentFitness->set(0.0);
	prepareNextTry();
}

/**
 * Pushes the currentFitness value into a list of fitness values and calls 
 * calculateFitness() to calculate the total fitness for the fitness values 
 * stored in the list of values.
 */
void FitnessFunction::finishTry() {
	mFitnessOfTries.push_back(mCurrentFitness->get());
	calculateFitness();
}

void FitnessFunction::finishIndividual() {
	mFitnessOfIndividuals.append(getFitness());

	int numberOfFitnessValues = mFitnessOfIndividuals.size();

	double min = mFitnessOfIndividuals.at(0);
	double max = min;
	double sum = min;

	for(int i = 1; i < numberOfFitnessValues; ++i) {
		double fitness = mFitnessOfIndividuals.at(i);
		sum += fitness;

		if(fitness < min) {
			min = fitness;
		}
		if(fitness > max) {
			max = fitness;
		}
	}

	//ensure that a division by zero is impossible
	if(numberOfFitnessValues == 0) {
		numberOfFitnessValues = 1;
	}

	double mean = sum / ((double) numberOfFitnessValues);

	double sumOfSquares = 0.0;
	for(QListIterator<double> i(mFitnessOfIndividuals); i.hasNext();) {
		double error = mean - i.next();
		sumOfSquares += (error * error);
	}

	double variance = sumOfSquares / ((double) numberOfFitnessValues);

	mMinFitness->set(min);
	mMaxFitness->set(max);
	mMeanFitness->set(mean);
	mVariance->set(variance);

	
}


void FitnessFunction::finishGeneration() {
	mFitnessOfIndividuals.clear();
}


/**
 * Updates the fitness function and calculates the new current fitness value. 
 */
void FitnessFunction::update() {
	mCurrentFitness->set(calculateCurrentFitness());
}

/**
*  Create a new DoubleValue-parameter for this fitness function.
* @param name The name of the new parameter. This name needs to be unique.
* @param initialValue The value of the new parameter (optional).
*/
void FitnessFunction::defineDoubleParam(const QString &name, double initialValue) {
	if(getParameter(name) != 0) {
		return;
	}
	DoubleValue *newDoubleParameter = new DoubleValue(initialValue);
	addParameter(name, newDoubleParameter);
}

/**
*  Create a new IntVlaue-parameter for this fitness function.
* @param name The name of the new parameter. This name needs to be unique.
* @param initialValue The value of the new parameter (optional).
*/
void FitnessFunction::defineIntParam(const QString &name, int initialValue) {
	if(getParameter(name) != 0) {
		return;
	}
	IntValue *newDoubleParameter = new IntValue(initialValue);
	addParameter(name, newDoubleParameter);
}

/**
*  Allows a name-based access to parameters of the fitness function that are Double- or IntValues.
* @param name The name of the parameter.
* @return The current value of the paramter.
*/
double FitnessFunction::getParam(const QString &name) const {
	if(dynamic_cast<DoubleValue*>(getParameter(name)) != 0) {
		return (dynamic_cast<DoubleValue*>(getParameter(name)))->get();
	} 
	else if(dynamic_cast<IntValue*>(getParameter(name)) != 0) {
		return (double) (dynamic_cast<IntValue*>(getParameter(name)))->get();
	}
	return 0.0; 
}

/**
*  Set the value of a parameter of the fitness function.
* @param name The name of the parameter (needs to be a Double- or IntValue).
* @param value The new value of this parameter.
*/
void FitnessFunction::setParam(const QString &name, double value) {
	if(dynamic_cast<DoubleValue*>(getParameter(name)) != 0) {
		(dynamic_cast<DoubleValue*>(getParameter(name)))->set(value);
	} 
	else if(dynamic_cast<IntValue*>(getParameter(name)) != 0) {
		(dynamic_cast<IntValue*>(getParameter(name)))->set((int)value);
	}
}

void FitnessFunction::setPrototypeName(const QString &prototypeName) {
	mPrototypeName = prototypeName;
}

QString FitnessFunction::getPrototypeName() const {
	return mPrototypeName;
}

bool FitnessFunction::isAttachedToSystem() const {
	return mAttachedToSystem;
}

void FitnessFunction::terminateTry() {
	Fitness::getFitnessManager()->suggestTryTermination();
}


int FitnessFunction::getNumberOfSteps() {
	return Fitness::getFitnessManager()->getNumberOfSteps();
}


int FitnessFunction::getNumberOfTries() {
	return Fitness::getFitnessManager()->getNumberOfTries();
}


DoubleValue* FitnessFunction::getMinFitnessValue() const {
	return mMinFitness;
}


DoubleValue* FitnessFunction::getMaxFitnessValue() const {
	return mMaxFitness;
}


DoubleValue* FitnessFunction::getMeanFitnessValue() const {
	return mMeanFitness;
}


DoubleValue* FitnessFunction::getFitnessVarianceValue() const {
	return mVariance;
}


}
