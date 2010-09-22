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

#ifndef NERDEvolutionManager_H
#define NERDEvolutionManager_H

#include "Evolution/World.h"
#include "Core/SystemObject.h"
#include "Value/IntValue.h"
#include "Event/Event.h"
#include "Event/EventListener.h"
#include "Value/BoolValue.h"
#include "Value/ValueChangedListener.h"
#include "Value/StringValue.h"
#include "Evaluation/EvaluationMethod.h"


namespace nerd {

	/**
	 * EvolutionManager.
	 */
	class EvolutionManager : public virtual SystemObject, 
			public virtual EventListener, public virtual ValueChangedListener 
	{
	public:
		EvolutionManager();
		virtual ~EvolutionManager();

		virtual QString getName() const;

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value);

		bool registerAsGlobalObject();

		bool addEvolutionWorld(World *world);
		bool removeEvolutionWorld(World *world);
		const QList<World*>& getEvolutionWorlds() const;

		void setDefaultEvaluationMethod(EvaluationMethod *evaluationMethod);
		EvaluationMethod* getDefaultEvaluationMethod() const;

		bool processNextGeneration();
		bool restartEvolution();

		Event* getNextGenerationTriggerEvent() const;

		IntValue* getCurrentGenerationValue() const;
		QString getEvolutionWorkingDirectory() const;
		StringValue* getEvolutionWorkingDirectoryValue() const;
		BoolValue* getRestartGenerationValue() const;

		void setCurrentNumberOfCompletedIndividualsDuringEvolution(int count);
		void setCurrentNumberOfCompletedIndividualsDuringEvaluation(int count);
	
	private:
		void initEvolution();
		
	private:
		QList<World*> mEvolutionWorlds;
		IntValue *mCurrentGenerationNumberValue;
		Event *mTriggerNextGenerationEvent;
		Event *mGenerationStartedEvent;
		Event *mNextGenerationCompletedEvent;
		Event *mEvaluationStartedEvent;
		Event *mEvaluationCompletedEvent;
		Event *mSelectionStartedEvent;
		Event *mSelectionCompletedEvent;
		Event *mEvolutionAlgorithmStartedEvent;
		Event *mEvolutionAlgorithmCompletedEvent;
		Event *mEvolutionTerminatedEvent;
		Event *mEvolutionRestartedEvent;
		bool mInitialized;

		IntValue *mGenerationDuration;
		IntValue *mEvolutionDuration;
		IntValue *mSelectionDuration;
		IntValue *mEvolutionAlgorithmDuration;
		IntValue *mEvaluationDuration;
		BoolValue *mRestartGenerationValue;
		bool mRestartGeneration;
		StringValue *mEvolutionWorkingDirectory;

		EvaluationMethod *mDefaultEvaluationMethod;
		IntValue *mEvoIndividualsCompletedCounter;
		IntValue *mEvalIndividualsCompletedCounter;
	
	};

}

#endif


