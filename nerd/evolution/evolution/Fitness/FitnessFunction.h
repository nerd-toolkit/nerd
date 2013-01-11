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

#ifndef FitnessFunction_H_
#define FitnessFunction_H_

#include "Core/ParameterizedObject.h"
#include <QString>


namespace nerd {

class DoubleValue;
class StatisticCalculator;
/**
 * FitnessFunction.
 *
 * Abstract base class for fitness functions.
 * Defines the methods that need to be implemented by subclasses-
 * Provides methods to easily create and access parameters and Double- or IntValues 
 * in the currently running application. Thereby the direct access to Values from 
 * the simulation is possible as well.
 * A FitnessFunction calculates the current fitness after each step. After each try, 
 * the current fitness value is pushed into a list and the "current" total fitness 
 * value is calculated based on this list.	
 */

class FitnessFunction : public ParameterizedObject {

	public:
		enum FitnessCalculationMode {MEAN_FITNESS, MIN_FITNESS, MAX_FITNESS};

	public:
		FitnessFunction(const QString &name);
		FitnessFunction(const FitnessFunction &copy);
	
		virtual ~FitnessFunction();

		virtual FitnessFunction* createCopy() const = 0;
		virtual double calculateCurrentFitness() = 0;
		virtual void attachToSystem();
		virtual void detachFromSystem();
		virtual void prepareNextTry() = 0;

		virtual void update();
		virtual double getFitness();
		virtual void reset();
		virtual void resetTry();
		virtual void finishTry();
		virtual void finishIndividual();
		virtual void finishGeneration();

		virtual void setName(const QString &name);
		

		virtual double getCurrentFitness();
		virtual void calculateFitness();

		void setPrototypeName(const QString &prototypeName);
		QString getPrototypeName() const;

		bool isAttachedToSystem() const;

		//convenience helper functions
		double getValue(const QString &valueName) const;
		QString getValueAsString(const QString &valueName) const;
		void defineDoubleParam(const QString &valueName, double initialValue = 0.0);
		void defineIntParam(const QString &valueName, int initialValue = 0.0);
		double getParam(const QString &valueName) const;
		void setParam(const QString &valueName, double value);

		void terminateTry();
		int getNumberOfSteps();
		int getNumberOfTries();

		DoubleValue* getMinFitnessValue() const;
		DoubleValue* getMaxFitnessValue() const;
		DoubleValue* getMeanFitnessValue() const;
		DoubleValue* getFitnessVarianceValue() const;

	protected:
		bool mAttachedToSystem;
		
		QList<double> mFitnessOfTries;
		QList<double> mFitnessOfIndividuals;
		QString mPrototypeName;
		StringValue *mHelpMessage;
		StringValue *mErrorState;
		IntValue *mFitnessCalculationMode;

		StatisticCalculator *mMeanFitnessStatistics;
		StatisticCalculator *mMinFitnessStatistics;
		StatisticCalculator *mMaxFitnessStatistics;
		StatisticCalculator *mVarianceStatistics;

		DoubleValue *mCurrentFitness;
		DoubleValue *mFitnessValue;
		DoubleValue *mMeanFitness;
		DoubleValue *mMinFitness;
		DoubleValue *mMaxFitness;
		DoubleValue *mVariance;

};
}
#endif


