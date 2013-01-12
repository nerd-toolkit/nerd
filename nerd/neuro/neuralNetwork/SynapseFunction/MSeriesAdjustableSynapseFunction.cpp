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

#include "MSeriesAdjustableSynapseFunction.h"
#include "Math/MSeriesFunctions.h"
#include <Network/Neuron.h>
#include <Network/NeuralNetwork.h>

namespace nerd {

/**
 * Constructor.
 */
MSeriesAdjustableSynapseFunction::MSeriesAdjustableSynapseFunction()
	: SynapseFunction("MSeriesAdjustable"), mMasterNeuron(0)
{
	mMasterNeuronId = new ULongLongValue();
	mRangeMin = new DoubleValue(-10);
	mRangeMax = new DoubleValue(10);
	
	addParameter("MasterId", mMasterNeuronId);
	addParameter("RangeMin", mRangeMin);
	addParameter("RangeMax", mRangeMax);
}

/**
 * Copy constructor.
 */
MSeriesAdjustableSynapseFunction::MSeriesAdjustableSynapseFunction(const MSeriesAdjustableSynapseFunction &other)
	: Object(), ValueChangedListener(), SynapseFunction(other), mMasterNeuron(0)
{
	mMasterNeuronId = dynamic_cast<ULongLongValue*>(getParameter("MasterId"));
	mRangeMin = dynamic_cast<DoubleValue*>(getParameter("RangeMin"));
	mRangeMax = dynamic_cast<DoubleValue*>(getParameter("RangeMax"));
}

/**
 * Destructor.
 */
MSeriesAdjustableSynapseFunction::~MSeriesAdjustableSynapseFunction() {
}

SynapseFunction* MSeriesAdjustableSynapseFunction::createCopy() const {
	return new MSeriesAdjustableSynapseFunction(*this);
}

/**
 * Does nothing in this implementation.
 */
void MSeriesAdjustableSynapseFunction::reset(Synapse*) {
}

/**
 * Returns the strength of the owner synapse truncated to the
 * precision of the A-Series weight value.
 * 
 * @param owner the owner of this SynapseFunction.
 * @return the truncated strength of the owner.
 */
double MSeriesAdjustableSynapseFunction::calculate(Synapse *owner) {
	if(owner == 0 || owner->getSource() == 0) {
		return 0.0;
	}
	Neuron *neuron = owner->getSource();
	TransferFunction *tf = neuron->getTransferFunction();
	NeuralNetwork *network = neuron->getOwnerNetwork();
	
	if(tf == 0 || network == 0 || (tf->getLowerBound() == tf->getUpperBound())) {
		owner->getStrengthValue().set(0.0);
		return 0.0;
	}
	Neuron *master = network->selectNeuronById(mMasterNeuronId->get(), network->getNeurons());
	if(master == 0) {
		owner->getStrengthValue().set(0.0);
		return 0.0;
	}
	double activity = master->getOutputActivationValue().get();
	
	double strength = (mRangeMin->get() + (((mRangeMax->get() - mRangeMin->get())
						/ (tf->getUpperBound() - tf->getLowerBound())) 
						* (activity - tf->getLowerBound())));
	
	owner->getStrengthValue().set(strength);
	
	//calculate weight in MSeries format.
	strength = owner->getStrengthValue().get();
	return MSeriesFunctions::fixedPoint_17_15_ToDouble(
			MSeriesFunctions::doubleToFixedPoint_17_15(strength));
}


bool MSeriesAdjustableSynapseFunction::equals(SynapseFunction *synapseFunction) const {
	if(SynapseFunction::equals(synapseFunction) == false) {
		return false;
	}

	MSeriesAdjustableSynapseFunction *af = dynamic_cast<MSeriesAdjustableSynapseFunction*>(synapseFunction);

	if(af == 0) {
		return false;
	}
	if(af->mMasterNeuronId != mMasterNeuronId
		|| af->mRangeMin != mRangeMin
		|| af->mRangeMax != mRangeMax)
	{
		return false;
	}
	return true;
}

}


