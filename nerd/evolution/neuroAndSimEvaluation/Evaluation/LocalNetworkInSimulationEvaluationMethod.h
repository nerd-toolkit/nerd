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


#ifndef NERDLocalNetworkInSimulationEvaluationMethod_H
#define NERDLocalNetworkInSimulationEvaluationMethod_H

#include "Evaluation/EvaluationMethod.h"
#include "Value/StringValue.h"
#include "Evaluation/EvaluationLoop.h"
#include "Event/EventListener.h"
#include "Core/SystemObject.h"
#include "Value/IntValue.h"
#include "Control/ControlInterface.h"
#include "Evolution/Population.h"
#include "Network/NeuralNetwork.h"

namespace nerd {

	/**
	 * LocalNetworkInSimulationEvaluationMethod.
	 */
	class LocalNetworkInSimulationEvaluationMethod : public EvaluationMethod,
				public virtual EventListener, public virtual SystemObject
	{
	public:
		LocalNetworkInSimulationEvaluationMethod();
		LocalNetworkInSimulationEvaluationMethod(
				const LocalNetworkInSimulationEvaluationMethod &other);
		virtual ~LocalNetworkInSimulationEvaluationMethod();

		virtual EvaluationMethod* createCopy();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		virtual void valueChanged(Value *value);

		virtual void eventOccured(Event *event);
		virtual QString getName() const;

		virtual bool evaluateIndividuals();
		virtual bool reset();

		virtual void stopEvaluation();

	protected:
		bool setupController();
		void setupFitnessFunctions();
		void setupEvaluationPairs();
		void setNetworksForNextTry(int currentTry);
		void updateBestNetworks();
		
	private:
		QList<StringValue*> mControllerNames;
		BoolValue *mRunStasisMode;
		EvaluationLoop *mEvaluationLoop;
		Event *mIndividualStartedEvent;
		Event *mIndividualCompletedEvent;
		Event *mShutDownEvent;
		Event *mSimEnvironmentChangedEvent;
		Event *mRandomizeEnvironmentEvent;
		bool mDoShutDown;
		IntValue *mCurrentIndividualValue;
		bool mRestartIndividual;
		bool mStopEvaluation;
		BoolValue *mRandomizeSeed;
		IntValue *mDesiredSeed;
		IntValue *mSimulationSeed;
		IntValue *mCurrentTryValue;
		QList<ControlInterface*> mControlInterfaces;
		QList<Population*> mPopulations;
		IntValue *mMaximumNumberOfPartnersPerEvaluation;
		QList<QList<QList<Individual*> > > mEvaluationPairs;
		QList<QList<Individual*> > mCurrentPermutation;
		QList<FitnessFunction*> mCurrentFitnessFunctions;
		QList<NeuralNetwork*> mCurrentNeuralNetworks;
		Individual *mCurrentIndividual;
		Population *mCurrentPopulation;
		int mCurrentPopulationId;
		QList<NeuralNetwork*> mLastGenerationsBestControllers;
		QList<NeuralNetwork*> mBestNetworkDestroyBuffer;
	};

}

#endif


