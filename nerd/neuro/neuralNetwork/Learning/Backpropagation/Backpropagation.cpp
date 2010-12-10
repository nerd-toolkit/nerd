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



#include "Backpropagation.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Synapse.h"
#include "Math/Math.h"
#include "Math/Random.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new Backpropagation.
 */
Backpropagation::Backpropagation()
	: mMomentum(0.0), mLearningRate(0.1), mNetworkValid(false)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the Backpropagation object to copy.
 */
Backpropagation::Backpropagation(const Backpropagation &other) 
	: mMomentum(other.mMomentum), mLearningRate(other.mLearningRate), mNetworkValid(false)
{
}

/**
 * Destructor.
 */
Backpropagation::~Backpropagation() {
}

bool Backpropagation::setNeurons(QList<Neuron*> neurons) {


	mNeurons = neurons;
	mLayers.clear();

	mNetworkValid = true;

	mMaxLayerSize = 0;

	//determine layers
	QList<Neuron*> remainingNeurons = neurons;
	while(!remainingNeurons.empty()) {
		QList<Neuron*> layer;
		for(QListIterator<Neuron*> i(remainingNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
			QList<Synapse*> synapses = neuron->getOutgoingSynapses();
			bool valid = true;
			for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
				Synapse *synapse = j.next();
				Neuron *target = dynamic_cast<Neuron*>(synapse->getTarget());
				if(target == 0) {
					continue;
				}
				if(remainingNeurons.contains(target)) {
					valid = false;
					break;
				}
			}
			if(valid) {
				layer.append(neuron);
			}
		}
		if(layer.empty()) {
			Core::log("Warning: The Backpropagation network is not valid (recurrences?)", true);
			mNetworkValid = false;
			return false;
		}
		mLayers.append(layer);
		mMaxLayerSize = Math::max(mMaxLayerSize, layer.size());
		for(QListIterator<Neuron*> i(layer); i.hasNext();) {
			remainingNeurons.removeAll(i.next());
		}
	}
	//printLayers();
	return mNetworkValid;
}


QList<Neuron*> Backpropagation::getNeurons() const {
	return mNeurons;
}

QList<Neuron*> Backpropagation::getOutputLayer() const {
	if(mLayers.empty()) {
		return QList<Neuron*>();
	}
	return mLayers.first();
}


double Backpropagation::backpropagate(QVector<double> desiredValues) {

	QHash<Neuron*, double> deltas;

	if(mLayers.size() <= 1) {
		Core::log("Backpropagation: Too few layers to execute algorithm!", true);
		return 0.0;
	}

	QList<Synapse*> allSynapses;

	double error = 0.0;

	//first layer
	QList<Neuron*> outputLayer = mLayers.first();
	//cerr << "outputLayer: " << outputLayer.size() << "  dsiredValues: " << desiredValues.size() << endl;
	for(int i = 0; i < outputLayer.size() && i < desiredValues.size(); ++i) {
		Neuron *n1 = outputLayer[i];
		double output = n1->getOutputActivationValue().get();
		double delta = output * (1 - output) * (desiredValues[i] - output);
		//double delta = (desiredValues[i] - output);
		deltas.insert(n1, delta);
		//cerr << "Delta1 " << output << " - " << desiredValues[i] << " : " << delta << endl;
		error += Math::abs(desiredValues[i] - output);
	}

	for(int i = 1; i < mLayers.size(); ++i) {
		QList<Neuron*> layer = mLayers[i];
		for(QListIterator<Neuron*> j(layer); j.hasNext();) {
			Neuron *neuron = j.next();

			double sum = 0.0;
			QList<Synapse*> synapses = neuron->getOutgoingSynapses();
			for(QListIterator<Synapse*> k(synapses); k.hasNext();) {
				Synapse *synapse = k.next();
				Neuron *n = dynamic_cast<Neuron*>(synapse->getTarget());
				if(n != 0 && deltas.contains(n)) {
					sum += deltas.value(n) * synapse->getStrengthValue().get();

					allSynapses.append(synapse);
				}
			}
			double output = neuron->getOutputActivationValue().get();
			double delta = output * (1 - output) * sum;
			deltas.insert(neuron, delta);

			//cerr << "Delta [" << neuron << "]: " << sum << " , " << output << " , " << delta << endl;
		}
	}

	for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
		Synapse *synapse = i.next();

		if(synapse->getStrengthValue().get() == 0.0) {
			synapse->getStrengthValue().set(Random::nextDoubleBetween(-1.5, 1.5));
			continue;
		}

		Neuron *out = synapse->getSource();
		Neuron *in = dynamic_cast<Neuron*>(synapse->getTarget());
		if(out == 0 || in == 0) {
			cerr << "Warning" << endl;
			continue;
		}
		double weightChange = mLearningRate * deltas.value(in) * out->getOutputActivationValue().get();
		synapse->getStrengthValue().set(synapse->getStrengthValue().get() + weightChange);
		//cerr << "WeightChange " << weightChange << endl;
	}

	return error;
}

void Backpropagation::clear() {
	mNeurons.clear();
	mLayers.clear();
}

void Backpropagation::setLearningRate(double rate) {
	mLearningRate = rate;
}


double Backpropagation::getLearningRate() const {
	return mLearningRate;
}


void Backpropagation::setMomentum(double momentum) {
	mMomentum = momentum;
}


double Backpropagation::getMomentum() const {
	return mMomentum;
}


void Backpropagation::printLayers() {
	cerr << "Printing layers: " << endl;
	for(QListIterator<QList<Neuron*> > i(mLayers); i.hasNext();) {
		QList<Neuron*> layer = i.next();
		Core::log("Layer: ", true);
		QString layerElems = "  ";
		for(QListIterator<Neuron*> j(layer); j.hasNext();) {
			Neuron *neuron = j.next();
			layerElems = layerElems + QString::number(neuron->getId()) + ",";
		}
		Core::log(layerElems, true);
	}
}

}



