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



#include "NeuralNetworkManipulationOperatorAdapter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/NeuralNetwork.h"
#include "NeuralNetworkManipulationChain/NeuralNetworkManipulationChainAlgorithm.h"

using namespace std;

namespace nerd {


int NeuralNetworkManipulationOperatorAdapter::mApplicationOrderCounter = 0;


/**
 * Constructs a new NeuralNetworkManipulationOperatorAdapter.
 */
NeuralNetworkManipulationOperatorAdapter::NeuralNetworkManipulationOperatorAdapter(
											const QString &name, bool *destroyFlag)
	: NeuralNetworkManipulationOperator(name), mCountApplyOperator(0),
 	  mNumberOfApplicationsUntilTrue(0), mDestroyFlag(destroyFlag), mLastIndividual(0),
	  mApplicationOrderIdentifier(0)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the NeuralNetworkManipulationOperatorAdapter object to copy.
 */
NeuralNetworkManipulationOperatorAdapter::NeuralNetworkManipulationOperatorAdapter(
							const NeuralNetworkManipulationOperatorAdapter &other)
	: NeuralNetworkManipulationOperator(other), mDestroyFlag(0), mLastIndividual(0),
	  mApplicationOrderIdentifier(0)
{
}

/**
 * Destructor.
 */
NeuralNetworkManipulationOperatorAdapter::~NeuralNetworkManipulationOperatorAdapter() {
	if(mDestroyFlag != 0) {
		*mDestroyFlag = true;
	}
}


NeuralNetworkManipulationOperator* NeuralNetworkManipulationOperatorAdapter::createCopy() const
{
	return new NeuralNetworkManipulationOperatorAdapter(*this);
}

bool NeuralNetworkManipulationOperatorAdapter::applyOperator(Individual *individual, CommandExecutor*) 
{
	mApplicationOrderCounter++;
	mApplicationOrderIdentifier = mApplicationOrderCounter;
	mCountApplyOperator++;
	mLastIndividual = individual;
	if(mNumberOfApplicationsUntilTrue > 0) {
		mNumberOfApplicationsUntilTrue--;
		return false;
	}
	return true;
}



}



