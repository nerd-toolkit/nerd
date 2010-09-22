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



#include "NeuralNetworkUtil.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include <QStringList>
#include "NeuralNetworkConstants.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"

using namespace std;

namespace nerd {





void NeuralNetworkUtil::setNetworkElementLocationProperty(NeuralNetworkElement *elem, 
													  double x, double y, double z)
{
	Properties *prop = dynamic_cast<Properties*>(elem);

	if(prop == 0) {
		return;
	}
	
	prop->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION,
 						QString::number(x).append(",").append(QString::number(y))
						.append(",").append(QString::number(z)));
}

void NeuralNetworkUtil::setNetworkElementLocationProperty(NeuralNetworkElement *elem, const QPointF &pos) {
	setNetworkElementLocationProperty(elem, pos.x(), pos.y(), 0.0);
}

void NeuralNetworkUtil::setNeuroModuleLocationSizeProperty(NeuroModule *module, 
														double width, double height) 
{
	Properties *prop = dynamic_cast<Properties*>(module);

	if(prop == 0) {
		return;
	}
	
	prop->setProperty(NeuralNetworkConstants::TAG_MODULE_SIZE, 
						QString::number(width).append(",").append(QString::number(height)));
}

void NeuralNetworkUtil::setNeuroModuleLocationSizeProperty(NeuroModule *module, const QSizeF &size) {
	setNeuroModuleLocationSizeProperty(module, size.width(), size.height());
}


QPointF NeuralNetworkUtil::getPosition(const QString &locationProperty, bool *ok) {
	if(locationProperty.size() < 5) {
		if(ok != 0) {
			*ok = false;
		}
		return QPoint();
	}
	QStringList coordinates = locationProperty.split(",");
	if(coordinates.size() != 3) {
		if(ok != 0) {
			*ok = false;
		}
		return QPoint();
	}
	bool okx = true;
	bool oky = true;
	double x = coordinates.at(0).toDouble(&okx);
	double y = coordinates.at(1).toDouble(&oky);

	if(ok != 0) {
		*ok = okx && oky;
	}
	return QPointF(x, y);
}

QPointF NeuralNetworkUtil::getPosition(const NeuralNetworkElement *element) {
	const Properties *prop = dynamic_cast<const Properties*>(element);
	if(prop != 0) {
		return NeuralNetworkUtil::getPosition(prop->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
	}
	return QPointF();
}

QSizeF NeuralNetworkUtil::getModuleSize(const NeuroModule *module, bool *ok) {
	if(module == 0) {
		return QSizeF(0.0, 0.0);
	}

	QString sizeProp = module->getProperty(NeuralNetworkConstants::TAG_MODULE_SIZE);
	if(sizeProp.size() < 3) {
		if(ok != 0) {
			*ok = false;
		}
		return QSizeF(0.0, 0.0);
	}

	QStringList size = sizeProp.split(",");
	if(size.size() != 2) {
		if(ok != 0) {
			*ok = false;
		}
		return QSizeF(0.0, 0.0);
	}
	bool okw = true;
	bool okh = true;
	double w = size.at(0).toDouble(&okw);
	double h = size.at(1).toDouble(&okh);

	if(ok != 0) {
		*ok = okw && okh;
	}
	return QSizeF(w, h);
}


/**
 * Returns a list with all neurons that are connected by this synapse and eventually related 
 * higher order synapses.
 */
QList<Neuron*> NeuralNetworkUtil::getConnectedNeurons(Synapse *synapse) {
	if(synapse == 0) {
		return QList<Neuron*>();
	}
	
	QList<Neuron*> neurons;
	if(synapse->getSource() != 0) {
		neurons.append(synapse->getSource());
	}

	Synapse *targetSynapse = dynamic_cast<Synapse*>(synapse->getTarget());

	if(targetSynapse == 0) {
		Neuron *neuron = dynamic_cast<Neuron*>(synapse->getTarget());
		
		if(neuron != 0 && !neurons.contains(neuron)) {
			neurons.append(neuron);
		}
	}
	else {
		QList<Neuron*> synapseNeurons = NeuralNetworkUtil::getConnectedNeurons(targetSynapse);
		for(QListIterator<Neuron*> i(synapseNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
			if(!neurons.contains(neuron)) {
				neurons.append(neuron);
			}
		}
	}
	return neurons;
}

/**
 * Returns a list with all synapses that are conected by this synapse and eventually related
 * higher order synapses.
 */
QList<Synapse*> NeuralNetworkUtil::getDependentSynapses(Synapse *synapse) {
	if(synapse == 0) {
		return QList<Synapse*>();
	}	
	QList<Synapse*> dependentSynapses;

	QList<Synapse*> synapses = synapse->getSynapses();
	dependentSynapses << synapses;

	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *s = i.next();
		
		dependentSynapses << NeuralNetworkUtil::getDependentSynapses(s);
	}

	return dependentSynapses;
}

}



