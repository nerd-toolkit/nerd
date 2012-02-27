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



#ifndef NERDInsertSynapseModularOperatorOperator_H
#define NERDInsertSynapseModular_H

#include <QString>
#include <QHash>
#include "NeuralNetworkManipulationChain/NeuralNetworkManipulationOperator.h"
#include "Value/IntValue.h"
#include "Value/StringValue.h"
#include "Value/NormalizedDoubleValue.h"
#include "Value/BoolValue.h"
#include "ModularNeuralNetwork/NeuroModule.h"

namespace nerd {

	/**
	 * InsertSynapseModularOperator.
	 *
	 */
	class InsertSynapseModularOperator : public NeuralNetworkManipulationOperator {
	public:
		InsertSynapseModularOperator(const QString &name);
		InsertSynapseModularOperator(const InsertSynapseModularOperator &other);
		virtual ~InsertSynapseModularOperator();

	private:
		virtual NeuralNetworkManipulationOperator* createCopy() const;

		virtual bool applyOperator(Individual *individual, CommandExecutor *executor = 0);

		bool connectNewNeurons(ModularNeuralNetwork *net);
		bool randomlyConnect(ModularNeuralNetwork *net);

// 		QList<NeuroModule*> getMatchingModules(const QString &matchingList, NeuroModule *owner);

	protected:
		bool addSynapseToSourceNeuron(Neuron *neuron, ModularNeuralNetwork *net);
		bool addSynapseToTargetNeuron(Neuron *neuron, ModularNeuralNetwork *net);

		QList<Neuron*> getValidSourceNeurons(Neuron *target, ModularNeuralNetwork *net);
		QList<Neuron*> getValidTargetNeurons(Neuron *source, ModularNeuralNetwork *net);
		int getInterfaceLevel(Neuron *neuron, const QString &moduleInterfaceType);

		void updatePermittedPaths(ModularNeuralNetwork *net);
		bool canBeConnected(Neuron *source, Neuron *target, ModularNeuralNetwork *net);
		
		void updateConsideredNeurons();
		
	private:
		NormalizedDoubleValue *mInsertionProbability;
		NormalizedDoubleValue *mInitialInsertionProbability;
		IntValue *mMaximalNumberOfNewSynapses;

		QList<Neuron*> mConsideredTargetNeurons;
		QList<Neuron*> mConsideredSourceNeurons;
		QString mGenerationDate;
		QHash<NeuroModule*, QList<NeuroModule*>*> mPermittedPathsBySource;
		QHash<NeuroModule*, QList<NeuroModule*>*> mForbiddenPathsBySource;
		QHash<NeuroModule*, QList<NeuroModule*>*> mPermittedPathsFromDest;
		QList<NeuroModule*> mModulesWithPermissionTag;
// 		QHash<NeuroModule*, QList<NeuroModule*>*> mPermittedPathsByTarget;
	};

}

#endif



