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

// GNU General Public License Agreement
// Copyright (C) 2004-2007 CodeCogs, Zyba Ltd, Broadwood, Holford, TA5 1DU, England.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by CodeCogs. 
// You must retain a copy of this licence in all copies. 
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// ---------------------------------------------------------------------------------

#ifndef NERD_MuSlashRhoLambdaESOptimizer_H
#define MuSlashRhoLambdaESOptimizer_H


#include "ModelOptimizer.h"
#include <QVector>

class QTextStream;
namespace nerd { class TrainSet; }
namespace nerd { class StringValue; }  
namespace nerd { class IntValue; } 
namespace nerd { class DoubleValue; } 
namespace nerd { class ESMarriage; } 
namespace nerd { class ESRecombination; } 
namespace nerd { class ESObjectParametersMutation; } 
namespace nerd { class ESStrategyParametersMutation; } 
namespace nerd { class ESSelection; } 
namespace nerd { class ESFitnessEvaluation; } 
namespace nerd { class ESInitialize; }
namespace nerd { class MuSlashRhoLambdaES; }
namespace nerd { class ErrorFunction; }
namespace nerd { class ESIndividual; }


namespace nerd {
 
 /**
 * MuSlashRhoLambdaESOptimizer
 *   
 */
class MuSlashRhoLambdaESOptimizer : public virtual ModelOptimizer {
  public:
		MuSlashRhoLambdaESOptimizer(const QString &model, TrainSet *trainSet);
    
    ~MuSlashRhoLambdaESOptimizer();

    virtual QString getName() const;
    
    virtual bool init();
		
		virtual bool initMuSlashRhoLambdaESComponents();
		
		virtual bool initESSelectionFromConfig();
		
		virtual bool initESStrategyParaRecombinationFromConfig();
		
		virtual bool initESObjectParaRecombinationFromConfig();
		
		virtual bool initESParameterMutationsFromConfig();
		
		virtual bool initESInitializeFromConfig();
		
    virtual bool optimizationStart();

    virtual bool doNextOptimizationStep();
    
    virtual bool optimizationEnd();
    
  protected:
	
		// Config values
		IntValue *mMu;
		IntValue *mRho;
		IntValue *mLambda;
		StringValue *mSelectionType;
		StringValue *mStrategyParaRecombinationType;
		StringValue *mObjectParaRecombinationType;
		StringValue *mParameterMutationsType;
		DoubleValue *mLearnRateKoeff;
		IntValue *mCheckInterval;
		DoubleValue *mInitStrategyParameter;
		IntValue *mNumberOfResultFiles;
		
		// ES components
		ESMarriage *mESMarriage;
		ESRecombination *mESObjectParametersRecombination;
		ESRecombination *mESStrategyParametersRecombination;
		ESObjectParametersMutation *mESObjectParametersMutation; 
		ESStrategyParametersMutation *mESStrategyParametersMutation; 
		ESSelection *mESSelection;
		ESFitnessEvaluation *mESFitnessEvaluation; 
		ESInitialize *mESInitialize;
		MuSlashRhoLambdaES *mMuSlashRhoLambdaES; 
		ErrorFunction *mErrorFunction;
		
		// statistic values
		DoubleValue *mSmallestError;
		DoubleValue *mHighestError;
		DoubleValue *mMeanError;
		DoubleValue *mErrorSTD;
		
		
    QVector<double> getParametersAsDoubleVector();
         
    void setParametersFromDoubleVector(QVector<double> vector);
    
    virtual bool writeReportStartEntry();
		
		void updateFitnessValues();
		
		bool saveBestIndividuals();
		bool saveIndividual(ESIndividual* individual, const QString &filename);
};

} // namespace nerd
#endif
