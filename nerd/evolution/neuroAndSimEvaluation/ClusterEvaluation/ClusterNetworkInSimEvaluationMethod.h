/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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
 *   The NERD Toolkit is part of the EU project ALEAR                      *
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
 *   clearly by citing the NERD homepage and the NERD overview paper.      *  
 ***************************************************************************/

#ifndef ClusterNetworkInSimEvaluationMethod_H_
#define ClusterNetworkInSimEvaluationMethod_H_
#include "ClusterEvaluation/ClusterEvaluationMethod.h"
#include "Event/EventListener.h"
#include <QStringList>

namespace nerd{


/**
 * ClusterNetworkInSimEvaluationMethod. Specific implementation of a  
 * ClusterEvaluationMethod, to be used for evolutions on a cluster, that use neural 
 * networks and an NERD simulation for the evaluation.
**/

class Population;

class ClusterNetworkInSimEvaluationMethod : public ClusterEvaluationMethod, public virtual EventListener {

	public:
		ClusterNetworkInSimEvaluationMethod(const QString &name);
		ClusterNetworkInSimEvaluationMethod(const ClusterNetworkInSimEvaluationMethod &other);
		virtual ~ClusterNetworkInSimEvaluationMethod();
	
		virtual EvaluationMethod* createCopy();
		virtual bool reset();
		virtual void stopEvaluation();

		virtual QString getName() const;
		virtual void eventOccured(Event *event);

	protected:
		virtual bool readEvaluationResults();
		virtual bool prepareEvaluation();
		virtual bool createConfigList();
		virtual bool createJobScript();

	private:
		bool createFitnessInformation();
		void performNeccessaryReSubmits();		

	private:
		QString mFitnessParameter;
		QString mFitnessFileName;
		QStringList mAgentInterfaces;
		QString mApplicationParameter;

		StringValue *mAgentInterfaceNames;
		StringValue *mApplication;
		StringValue *mApplicationFixedParameters;
		StringValue *mHostWithGuiName;

		Event *mNextStep;
		Event *mStepCompleted;
		Event *mGenerationStartedEvent;
		IntValue *mNumberOfSteps;
		IntValue *mCurrentStep;
		IntValue *mGenerationSimulationSeed;
		BoolValue *mRandomizeSeed;
		BoolValue *mPauseSimulation;
		BoolValue *mIncludeBacktraceCodeInScripts;
	
};
}
#endif

