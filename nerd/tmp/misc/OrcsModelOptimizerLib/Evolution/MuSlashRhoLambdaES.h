/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   OrcsModelOptimizer by Chris Reinke (creinke@uni-osnabrueck.de         *
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

#ifndef NERD_MUSLASHRHOLAMBDAES_H
#define NERD_MUSLASHRHOLAMBDAES_H

#include <QList>
#include "ESInformation.h"

namespace nerd { class ESIndividual; } 
namespace nerd { class ESMarriage; } 
namespace nerd { class ESRecombination; } 
namespace nerd { class ESObjectParametersMutation; } 
namespace nerd { class ESStrategyParametersMutation; } 
namespace nerd { class ESSelection; } 
namespace nerd { class ESFitnessEvaluation; } 
namespace nerd { class ESInitialize; } 

namespace nerd {

class MuSlashRhoLambdaES {
  public:
		static bool descendingOrder(const ESIndividual *individual1,
		 														const ESIndividual *individual2);
		
		MuSlashRhoLambdaES(	ESMarriage *marriage,
												ESRecombination *strategyParametersRecombination,
												ESRecombination *objectParametersRecombination,
												ESStrategyParametersMutation *strategyParametersMutation,
												ESObjectParametersMutation *objectParametersMutation,
												ESSelection *selection,
												ESFitnessEvaluation *fitnessEvaluation);

    ~MuSlashRhoLambdaES();

		bool startEvolutionRun(int mu, int rho, int lambda, ESInitialize *init);

		bool doNextGeneration();

		QList<ESIndividual*> getPopulation() const;

		ESIndividual* getBestIndividual() const;

		int getGenerationCount();
		
		const ESInformation & getInformation();

  protected:
		QList<ESIndividual*> mPopulation;

    ESMarriage *mMarriage;

    ESRecombination *mStrategyParametersRecombination;

    ESRecombination *mObjectParametersRecombination;

    ESStrategyParametersMutation *mStrategyParametersMutation;

		ESObjectParametersMutation *mObjectParametersMutation;
		
		ESSelection *mSelection;

    ESFitnessEvaluation *mFitnessEvaluation;
		
		ESInformation mInformation;
		
		void deletePopulation();
		
		void deletePopulation(QList<ESIndividual*> whiteList);
		
		void calculateStatistics();
};

} // namespace nerd
#endif
