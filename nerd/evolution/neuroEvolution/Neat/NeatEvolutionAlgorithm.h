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



#ifndef NERDNeatEvolutionAlgorithm_H
#define NERDNeatEvolutionAlgorithm_H

#include <QString>
#include <QHash>
#include <QList>
#include "Evolution/EvolutionAlgorithm.h"
#include "Evolution/Individual.h"
#include "Value/NormalizedDoubleValue.h"
#include "Neat/NeatGenome.h"

namespace nerd {

	struct NeatNodeInnovation {
		NeatNodeInnovation(NeatNodeGene *node, NeatConnectionGene *splittedConnection, 
						NeatConnectionGene *link1, NeatConnectionGene *link2);
		NeatNodeGene *mNode;
		NeatConnectionGene *mSplittedConnection;
		NeatConnectionGene *mLink1;
		NeatConnectionGene *mLink2;
	};
	struct NeatLinkInnovation {
		NeatLinkInnovation(NeatConnectionGene *connection);
		NeatConnectionGene *mConnection;
	};

	/**
	 * NeatEvolutionAlgorithm.
	 *
	 */
	class NeatEvolutionAlgorithm : public EvolutionAlgorithm {
	public:
		NeatEvolutionAlgorithm();
		NeatEvolutionAlgorithm(const NeatEvolutionAlgorithm &other);
		virtual ~NeatEvolutionAlgorithm();

		virtual int getRequiredNumberOfParentsPerIndividual() const;

		virtual EvolutionAlgorithm* createCopy();

		virtual bool createNextGeneration(QList<Individual*> &trashcan);
		virtual bool reset();

	private:
		NeatGenome* mate(NeatGenome *parent1, NeatGenome *parent2, bool preferSmallerGenome);
		void mutate(NeatGenome *genome);

		NeatGenome* mateMultiPoint(NeatGenome *parent1, NeatGenome *parent2, 
									bool preferSmallerGenome, bool average);

		void mutateAddNode(NeatGenome *genome);
		void mutateAddLink(NeatGenome *genome);
		void mutateLinkWeights(NeatGenome *genome);
		void mutateToggleEnable(NeatGenome *genome);
		void mutateReenable(NeatGenome *genome);

		NeatNodeGene* getNodeGene(int id, NeatGenome *genome);
		void addGenomeProperty(NeatGenome *genome, Individual *individual);

		void saveGenerationToFile();

// 		void addConnectionGene(NeatGenome *genome, NeatConnectionGene *link);
// 		void addNodeGene(NeatGenome *genome, NeatNodeGene *node);

	private:
		DoubleValue *mLinkWeightMutationPower;
		NormalizedDoubleValue *mLinkWeightMutationProb;
		NormalizedDoubleValue *mMutateOnlyProb;
		NormalizedDoubleValue *mMutateLinkWeightsProb;
		NormalizedDoubleValue *mMutateToggleEnableProb;
		NormalizedDoubleValue *mMutateReenableProb;
		NormalizedDoubleValue *mMutateAddNodeProb;
		NormalizedDoubleValue *mMutateAddLinkProb;
		NormalizedDoubleValue *mMutateAddRecurrentLinkProb;
		NormalizedDoubleValue *mMateMultiPointProb;
		NormalizedDoubleValue *mMateMultiPointAvgProb;
		NormalizedDoubleValue *mMateSinglePointProb;
		NormalizedDoubleValue *mMateOnlyProb;
		IntValue *mNumberOfInputNeurons;
		IntValue *mNumberOfOutputNeurons;
		BoolValue *mStartFullyConnected;
		NeatGenome *mGenomeTemplate;
		QList<NeatNodeInnovation> mNodeInnovations;
		QList<NeatLinkInnovation> mLinkInnovations;
	};

}

#endif



