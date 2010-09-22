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

#include "BasicNeuralNetworkStatistics.h"
#include <QListIterator>
#include "Evolution/Individual.h"
#include "Network/NeuralNetwork.h"
#include <limits>
#include <iostream>



using namespace std;


namespace nerd {

BasicNeuralNetworkStatistics::BasicNeuralNetworkStatistics(Population &targetPopulation)
	: StatisticCalculator("MeanNumberOfNeurons", QString("/Statistics/")
		.append(targetPopulation.getName()).append("/")), 
	  mTargetPopulation(&targetPopulation),
	  mMaxNumberOfSynapsesStatistics(0), mMinNumberOfSynapsesStatistics(0),
	  mMaxNumberOfNeuronsStatistics(0), mMinNumberOfNeuronsStatistics(0),
	  mMeanNumberOfSynapsesStatistics(0)
{
	QString path = QString("/Statistics/").append(targetPopulation.getName())
		.append("/");

	mMaxNumberOfSynapsesStatistics = new StatisticCalculator("MaxNumberOfSynapses", path);
	mMinNumberOfSynapsesStatistics = new StatisticCalculator("MinNumberOfSynapses", path);
	mMaxNumberOfNeuronsStatistics = new StatisticCalculator("MaxNumberOfNeurons", path);
	mMinNumberOfNeuronsStatistics = new StatisticCalculator("MinNumberOfNeurons", path);
	mMeanNumberOfSynapsesStatistics = new StatisticCalculator("MeanNumberOfSynapses", path);
	mBestIndividualNumberOfNeuronsStatistics = new StatisticCalculator("BestNumberOfNeurons", path);
	mBestIndividualNumberOfSynapsesStatistics = new StatisticCalculator("BestNumberOfSynapses", path);

	mChildStatistics.append(mMaxNumberOfSynapsesStatistics);
	mChildStatistics.append(mMinNumberOfSynapsesStatistics);
	mChildStatistics.append(mMaxNumberOfNeuronsStatistics);
	mChildStatistics.append(mMinNumberOfNeuronsStatistics);
	mChildStatistics.append(mMeanNumberOfSynapsesStatistics);
	mChildStatistics.append(mBestIndividualNumberOfNeuronsStatistics);
	mChildStatistics.append(mBestIndividualNumberOfSynapsesStatistics);
}

BasicNeuralNetworkStatistics::~BasicNeuralNetworkStatistics() {
}

bool BasicNeuralNetworkStatistics::calculateNextValue(int index) {
	if(mTargetPopulation == 0) {
		setAllStatisticValues(index, 0.0);
		return false;
	}

	const QList<Individual*> &individuals = mTargetPopulation->getIndividuals();

	int minNumberOfNeurons = numeric_limits<int>::max();
	int maxNumberOfNeurons = 0;
	int neuronSum = 0;
	int minNumberOfSynapses = numeric_limits<int>::max();
	int maxNumberOfSynapses = 0;
	int synapseSum = 0;
// 	double minBias = numeric_limits<double>::max();
// 	double maxBias = numeric_limits<int>::min();
// 	double biasSim = 0;
// 	double minSynapseStrength = numeric_limits<double>::max();
// 	double maxSynapseStrength = numeric_limits<int>::min();
// 	double synapseStrengthSim = 0.0;
// 	int minNumberOfHigherOrderSynapses = numeric_limits<int>::max();
// 	int maxNumberOfHigherOrderSynapses = 0;
// 	int higherOrderSynapsesSim = 0;
	

	int numberOfIndividuals = individuals.size();

	if(numberOfIndividuals == 0) {
		setAllStatisticValues(index, 0.0);
		return false;
	}

	double bestFitness = numeric_limits<double>::min();
	int bestNumberOfNeurons = 0;
	int bestNumberOfSynapses = 0;

	for(QListIterator<Individual*> i(individuals); i.hasNext();) {
		Individual *individual = i.next();


		NeuralNetwork *network = dynamic_cast<NeuralNetwork*>(individual->getPhenotype());

		if(network == 0) {
			setAllStatisticValues(index, 0.0);
			return false;
		}

		const QList<Neuron*> &neurons = network->getNeurons();
		QList<Synapse*> synapses = network->getSynapses();

		if(!individual->getFitnessFunctions().empty()) {
			double fitness = individual->getFitness(individual->getFitnessFunctions().at(0));
			if(fitness > bestFitness) {
				bestFitness = fitness;
				bestNumberOfNeurons = neurons.size();
				bestNumberOfSynapses = synapses.size();
			}
			else if(fitness == bestFitness) {
				if(synapses.size() < bestNumberOfSynapses) {
					bestNumberOfSynapses = synapses.size();
					bestNumberOfNeurons = neurons.size();
				}
			}
		}

		int numberOfNeurons = neurons.size();
		if(numberOfNeurons < minNumberOfNeurons) {
			minNumberOfNeurons = numberOfNeurons;
		}
		if(numberOfNeurons > maxNumberOfNeurons) {
			maxNumberOfNeurons = numberOfNeurons;
		}
		neuronSum += numberOfNeurons;

		

		int numberOfSynapses = synapses.size();
		if(numberOfSynapses < minNumberOfSynapses) {
			minNumberOfSynapses = numberOfSynapses;
		}
		if(numberOfSynapses > maxNumberOfSynapses) {
			maxNumberOfSynapses = numberOfSynapses;
		}
		synapseSum += numberOfSynapses;	
	}

	//mean number of neurons
	setValue(index, ((double) neuronSum) / ((double) numberOfIndividuals));

	mMeanNumberOfSynapsesStatistics->setValue(
			 index, ((double) synapseSum) / ((double) numberOfIndividuals));

	mMinNumberOfNeuronsStatistics->setValue(index, (double) minNumberOfNeurons);
	mMaxNumberOfNeuronsStatistics->setValue(index, (double) maxNumberOfNeurons);
	mMinNumberOfSynapsesStatistics->setValue(index, (double) minNumberOfSynapses);
	mMaxNumberOfSynapsesStatistics->setValue(index, (double) maxNumberOfSynapses);
	mBestIndividualNumberOfNeuronsStatistics->setValue(index, (double) bestNumberOfNeurons);
	mBestIndividualNumberOfSynapsesStatistics->setValue(index, (double) bestNumberOfSynapses);

	return true;
}


void BasicNeuralNetworkStatistics::reset() {
	
}

}



