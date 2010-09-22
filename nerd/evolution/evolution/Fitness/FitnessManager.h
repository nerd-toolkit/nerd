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

#ifndef FitnessManager_H_
#define FitnessManager_H_

#include "Core/SystemObject.h"
#include "Event/EventListener.h"
#include "FitnessFunction.h"
#include "Value/StringValue.h"
#include "Value/IntValue.h"

namespace nerd {

class Event;

/**
 * FitnessManager.
 *
 * All fitness function are registered at the FitnessManager.
 *
 * Fitness function prototypes can be registered here and can be used to 
 * create new fitness functions using their names. This allows to use fitness 
 * functions without knowing their exact class.
 *
 * The FitnessManager also reacts on events as starting a new try and calls the 
 * appropriate methods of all fitness functions that are registered with the manager.
 *
 * The FitnessManager handles the destruction of the FitnessFunctions and prototypes.
 **/
class FitnessManager: public virtual SystemObject, public virtual EventListener {

	public:
		FitnessManager();
		virtual ~FitnessManager();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		virtual QString getName() const;

		virtual void eventOccured(Event *event);
		
		bool addFitnessFunction(FitnessFunction *fitnessFunction);
		bool removeFitnessFunction(const QString &fitnessFunctionName);
		bool removeFitnessFunction(FitnessFunction *fitnessFunction);
		QList<FitnessFunction*> getFitnessFunctions();
		QList<QString> getFitnessFunctionNames();

		bool addFitnessFunctionPrototype(const QString &name, 
			FitnessFunction* fitnessFunction);
		bool removeFitnessFunctionPrototype(const QString &name);
		QHash<QString, FitnessFunction*> getFitnessFunctionPrototypes();
		FitnessFunction* createFitnessFunctionFromPrototype(const QString &prototypeName,
			const QString &fitnessFunctionName = 0);

		bool writeFitnessValuesToFile(const QString &fileName, const QString &comment = "");
	
		FitnessFunction* getFitnessFunction(const QString &fitnessName);

		//helper functions to simplify fitness function implementations
		void suggestTryTermination();
		int getNumberOfSteps();
		int getNumberOfTries();

		void enableFitnessFunctionExecution(bool enable);
		bool isFitnessFunctionExecutionEnabled() const;
		
	private:
		QHash<QString, FitnessFunction*> mFitnessFunctionPrototypes;
		QHash<QString, FitnessFunction*> mFitnessFunctions;
	
		Event *mNextTryEvent;
		Event *mNextIndividualEvent;
		Event *mStepCompletedEvent;
		Event *mTryCompletedEvent;
		Event *mIndividualCompletedEvent;
		Event *mGenerationCompletedEvent;
		Event *mResetEvent;

		IntValue *mNumberOfSteps;
		IntValue *mNumberOfTries;
		Event *mTerminateTryEvent;
		bool mEnableFitnessFunctionExecution;
};
}
#endif

