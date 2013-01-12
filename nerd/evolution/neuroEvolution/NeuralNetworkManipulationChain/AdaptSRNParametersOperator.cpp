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



#include "AdaptSRNParametersOperator.h"
#include <iostream>
#include <QList>
#include "Network/NeuralNetwork.h"
#include "Core/Core.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Math/Random.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include "Core/Properties.h"
#include <iostream>
#include "Evolution/Evolution.h"
#include "EvolutionConstants.h"
#include <ActivationFunction/LearningRules/SelfRegulatingNeuronActivationFunction.h>
#include <Math/Math.h>

using namespace std;

namespace nerd {


/**
 * Constructs a new AdaptSRNParametersOperator.
 */
AdaptSRNParametersOperator::AdaptSRNParametersOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name)
{
	mChangeProbability = new NormalizedDoubleValue(0.1, 0.0, 1.0, 0.0, 1.0);
	mGlobalSettings = new BoolValue(true);
	
	mChangeProbability->setDescription("The probability for a parameter change to occur at all.\n"
									   "If used with ApplyGlobally = false, then this probability\n"
									   "is applied to each SRN neuron separately.");
	
	mGlobalSettings->setDescription("If true, then all parameters of all SRN neurons are changed\n"
									"at once.");
	
	for(int i = 0; i < 5; ++i) {
		mProbabilities.append(new NormalizedDoubleValue(0.5, 0.0, 1.0, 0.0, 1.0));
		mDeviations.append(new NormalizedDoubleValue(0.05, 0.0, 1.0, 0.0, 1.0));
		mMins.append(new DoubleValue(0.0));
		mMaxs.append(new DoubleValue(1.0));
	}
	addParameters("Alpha", 0);
	addParameters("Beta", 1);
	addParameters("Gamma", 2);
	addParameters("Delta", 3);
	addParameters("AStar", 4);
	
	addParameter("ProbabilityForChange", mChangeProbability);
	addParameter("ApplyGlobally", mGlobalSettings);
	
	getEnableOperatorValue()->set(false);
	getHiddenValue()->set(true);

}


/**
 * Copy constructor. 
 * 
 * @param other the AdaptSRNParametersOperator object to copy.
 */
AdaptSRNParametersOperator::AdaptSRNParametersOperator(const AdaptSRNParametersOperator &other) 
	: NeuralNetworkManipulationOperator(other)
{
	mChangeProbability = dynamic_cast<NormalizedDoubleValue*>(
			getParameter("ProbabilityForChange"));
	mGlobalSettings = dynamic_cast<BoolValue*>(getParameter("ApplyGlobally"));
	
	fetchParameters("Alpha");
	fetchParameters("Beta");
	fetchParameters("Gamma");
	fetchParameters("Delta");
	fetchParameters("AStar");
}

/**
 * Destructor.
 */
AdaptSRNParametersOperator::~AdaptSRNParametersOperator() {
}


NeuralNetworkManipulationOperator* AdaptSRNParametersOperator::createCopy() const {
	return new AdaptSRNParametersOperator(*this);
}


bool AdaptSRNParametersOperator::applyOperator(Individual *individual, CommandExecutor*) {

	NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individual->getGenome());

	if(net == 0) {
		Core::log("AdaptSRNParametersOperator: Could not apply operator because individual did not "
				  "provide a NeuralNetwork as genome!");
		return false;
	}

	double changeProbability = mChangeProbability->get();
	
	QList<Neuron*> networkNeurons = net->getNeurons();
	QList<SelfRegulatingNeuronActivationFunction*> consideredAF;
	for(QListIterator<Neuron*> i(networkNeurons); i.hasNext();) {
		//remove all neurons that can not be changed.
		Neuron *neuron = i.next();
		SelfRegulatingNeuronActivationFunction *srnaf = 
				dynamic_cast<SelfRegulatingNeuronActivationFunction*>(neuron->getActivationFunction());
		if(srnaf == 0) {
			continue;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
			continue;
		}
		if(neuron->hasProperty(NeuralNetworkConstants::TAG_NEURON_PROTECT_ACTIVATION_FUNCTION)) {
			continue;
		}
		consideredAF.append(srnaf);
	}
	
	if(mGlobalSettings->get()) {
		if(consideredAF.empty()) {
			//there are no matching activation functions to change.
			return true;
		}
		//check if mutations take place
		if(Random::nextDouble() >= changeProbability) {
			return true;
		}
		SelfRegulatingNeuronActivationFunction *srnaf = consideredAF.at(0);
		double newAlpha = mutateParameter(srnaf->getAlpha(), 0);
		double newBeta = mutateParameter(srnaf->getBeta(), 1);
		double newGamma = mutateParameter(srnaf->getGamma(), 2);
		double newDelta = mutateParameter(srnaf->getDelta(), 3);
		double newAStar = mutateParameter(srnaf->getAStar(), 4);
		
		//check if the default AF should be mutated, too.
		SelfRegulatingNeuronActivationFunction *defaultAF = 
				dynamic_cast<SelfRegulatingNeuronActivationFunction*>(net->getDefaultActivationFunction());
		if(defaultAF != 0) {
			consideredAF.append(defaultAF);
		}
		
		for(QListIterator<SelfRegulatingNeuronActivationFunction*> i(consideredAF); i.hasNext();) {
			SelfRegulatingNeuronActivationFunction *af = i.next();
			af->getAlpha()->set(newAlpha);
			af->getBeta()->set(newBeta);
			af->getGamma()->set(newGamma);
			af->getDelta()->set(newDelta);
			af->getAStar()->set(newAStar);
		}
	}
	else {
		for(QListIterator<SelfRegulatingNeuronActivationFunction*> i(consideredAF); i.hasNext();) {
			SelfRegulatingNeuronActivationFunction *af = i.next();
			
			if(Random::nextDouble() >= changeProbability) {
				continue;
			}
			
			//change each parameter separately.
			double newAlpha = mutateParameter(af->getAlpha(), 0);
			double newBeta = mutateParameter(af->getBeta(), 1);
			double newGamma = mutateParameter(af->getGamma(), 2);
			double newDelta = mutateParameter(af->getDelta(), 3);
			double newAStar = mutateParameter(af->getAStar(), 4);
		
			af->getAlpha()->set(newAlpha);
			af->getBeta()->set(newBeta);
			af->getGamma()->set(newGamma);
			af->getDelta()->set(newDelta);
			af->getAStar()->set(newAStar);
		}
	}
// 
// 	double probability = mInsertionProbability->get();
// 
// 	if(consideredNeurons.empty()) {
// 		return true;
// 	}
// 
// 	//prepare the generation date as string.
// 	QString generationDate = QString::number(Evolution::getEvolutionManager()
// 				->getCurrentGenerationValue()->get());
// 	
// 	for(int i = 0; i < maxNumberOfNewBiases; ++i) {
// 		
// 		if(Random::nextDouble() >= probability) {
// 			continue;
// 		}
// 
// 		//select neuron
// 		Neuron *neuron = consideredNeurons.value(Random::nextInt(consideredNeurons.size()));
// 
// 		if(neuron == 0) {
// 			continue;
// 		}
// 		consideredNeurons.removeAll(neuron);
// 
// 
// 		neuron->setProperty(NeuralNetworkConstants::TAG_NEURON_REINIT_BIAS);
// 		neuron->setProperty(NeuralNetworkConstants::PROP_ELEMENT_MODIFIED);
// 
// 		//mark the individual as significantly modified
// 		individual->setProperty(EvolutionConstants::TAG_GENOME_SIGNIFICANT_CHANGE,
// 						generationDate);
// 		
// 	}

	return true;

}

double AdaptSRNParametersOperator::mutateParameter(DoubleValue *param, int index)
{
	if(param == 0 || index < 0 || index > mProbabilities.size()) {
		return 0.0;
	}
	
	double probability = mProbabilities.at(index)->get();
	double deviation = mDeviations.at(index)->get();
	double min = mMins.at(index)->get();
	double max = mMaxs.at(index)->get();

	if(Random::nextDouble() >= probability) {
		return param->get();
	}
	
	return Math::min(max, Math::max(min, 
				param->get() + Math::getNextGaussianDistributedValue(deviation)));
}

void AdaptSRNParametersOperator::addParameters(const QString &prefix, int index) {
	addParameter(prefix + "Probability", dynamic_cast<NormalizedDoubleValue*>(mProbabilities.at(index)));
	addParameter(prefix + "Deviation", dynamic_cast<NormalizedDoubleValue*>(mDeviations.at(index)));
	addParameter(prefix + "Min", mMins.at(index));
	addParameter(prefix + "Max", mMaxs.at(index));
}


void AdaptSRNParametersOperator::fetchParameters(const QString &prefix) {
	mProbabilities.append(dynamic_cast<DoubleValue*>(getParameter(prefix + "Probability")));
	mDeviations.append(dynamic_cast<DoubleValue*>(getParameter(prefix + "Deviation")));
	mMins.append(dynamic_cast<DoubleValue*>(getParameter(prefix + "Min")));
	mMaxs.append(dynamic_cast<DoubleValue*>(getParameter(prefix + "Max")));
}



}



