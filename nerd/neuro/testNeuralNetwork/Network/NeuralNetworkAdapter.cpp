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

#include "NeuralNetworkAdapter.h"

namespace nerd {

NeuralNetworkAdapter::NeuralNetworkAdapter(const ActivationFunction &defaultActivationFunction,
					  const TransferFunction &defaultTransferFunction,
					  const SynapseFunction &defaultSynapseFunction)
	: NeuralNetwork(defaultActivationFunction, defaultTransferFunction, defaultSynapseFunction),
	  mResetCounter(0), mExecuteStepCounter(0), mValidateSynapseConnectionsCounter(0),
	  mInterceptFunctionCalls(false), mDestroyedFlag(0)
{
}

NeuralNetworkAdapter::NeuralNetworkAdapter(const NeuralNetworkAdapter &other) 
	: Controller(), Object(), NeuralNetwork(other), mDestroyedFlag(0)
{
}

NeuralNetworkAdapter::~NeuralNetworkAdapter() {
	if(mDestroyedFlag != 0) {
		(*mDestroyedFlag) = true;
	}
}


NeuralNetwork* NeuralNetworkAdapter::createCopy() {
	return new NeuralNetworkAdapter(*this);
}


void NeuralNetworkAdapter::executeStep() {
	mExecuteStepCounter++;
	if(!mInterceptFunctionCalls) {
		NeuralNetwork::executeStep();
	}
}


void NeuralNetworkAdapter::reset() {
	mResetCounter++;
	if(!mInterceptFunctionCalls) {
		NeuralNetwork::reset();
	}
}


void NeuralNetworkAdapter::validateSynapseConnections() {
	mValidateSynapseConnectionsCounter++;
	if(!mInterceptFunctionCalls) {
		NeuralNetwork::validateSynapseConnections();
	}
}

const QList<NeuronInterfaceValuePair>& NeuralNetworkAdapter::getRawInputPairs() const {
	return getInputPairs();
}


const QList<NeuronInterfaceValuePair>& NeuralNetworkAdapter::getRawOutputPairs() const {
	return getOutputPairs();
}



}

