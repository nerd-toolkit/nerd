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



#include "NeuralNetworkUtil.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include <QStringList>
#include "NeuralNetworkConstants.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"

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

QList<Synapse*> NeuralNetworkUtil::getRecurrenceChain(Neuron *neuron, Neuron *currentNeuron, 
									QList<Neuron*> allConsideredNeurons, QList<Neuron*> visitedNeurons) 
{
	
	QList<Synapse*> chain;
	if(neuron == 0 || currentNeuron == 0 || allConsideredNeurons.empty()) {
		return chain;
	}
	
	QList<Synapse*> synapses = currentNeuron->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		Synapse *synapse = i.next();

		Neuron *source = synapse->getSource();
		if(source == 0) {
			return chain;
		}

		if(source == neuron) {
			chain.append(synapse);
			return chain;
		}

		if(!allConsideredNeurons.contains(source)) {
			continue;
			//return chain; //empty;
		}

		if(visitedNeurons.contains(source)) {
			continue;
		}

		visitedNeurons.append(source);
		
		QList<Synapse*> currentChain = 
				NeuralNetworkUtil::getRecurrenceChain(neuron, source, allConsideredNeurons, visitedNeurons);

		if(!currentChain.empty()) {
			currentChain.append(synapse);
			return currentChain;
		}
	}
	return chain;
}


void NeuralNetworkUtil::moveNeuroModuleTo(NeuroModule *module, double x, double y, double z) {
	if(module == 0) {
		return;
	}
	
	Vector3D modulePos = module->getPosition();
	Vector3D moveVector(x - modulePos.getX(), y - modulePos.getY(), z - modulePos.getZ());
	
	QList<NeuralNetworkElement*> movedObjects = module->getAllEnclosedNetworkElements();
	movedObjects.append(module);
	
	for(QListIterator<NeuralNetworkElement*> i(movedObjects); i.hasNext();) {
		NeuralNetworkElement *elem = i.next();
		Vector3D pos = elem->getPosition();
		elem->setPosition(Vector3D(pos.getX() + moveVector.getX(), 
								   pos.getY() + moveVector.getY(), 
								   pos.getZ() + moveVector.getZ()));
	}
}


QList<NeuronGroup*> NeuralNetworkUtil::getGroupsFromIdList(ModularNeuralNetwork *network, const QString &idList) {
	QList<NeuronGroup*> groups;
	
	if(network == 0 || idList.trimmed() == "") {
		return groups;
	}
	
	QString unifiedList = idList;
	unifiedList.replace("|", ",");
	
	QStringList ids = unifiedList.split(",");
	for(QListIterator<QString> i(ids); i.hasNext();) {
		bool ok = true;
		qulonglong id = i.next().toULongLong(&ok);
		if(ok) {
			NeuronGroup *group = ModularNeuralNetwork::selectNeuronGroupById(id, network->getNeuronGroups());
			if(group != 0) {
				groups.append(group);
			}
		}
	}
	
	return groups;
}


QList<Neuron*> NeuralNetworkUtil::getNeuronsFromIdList(NeuralNetwork *network, const QString &idList) {
	QList<Neuron*> neurons;
	
	if(network == 0 || idList.trimmed() == "") {
		return neurons;
	}
	
	QString unifiedList = idList;
	unifiedList.replace("|", ",");
	
	QList<Neuron*> allNeurons = network->getNeurons();
	
	QStringList ids = unifiedList.split(",");
	for(QListIterator<QString> i(ids); i.hasNext();) {
		bool ok = true;
		qulonglong id = i.next().toULongLong(&ok);
		if(ok) {
			Neuron *neuron = NeuralNetwork::selectNeuronById(id, allNeurons);
			if(neuron != 0) {
				neurons.append(neuron);
			}
		}
	}
	
	return neurons;
}


QList<Synapse*> NeuralNetworkUtil::getSynapsesFromIdList(NeuralNetwork *network, const QString &idList) {
	QList<Synapse*> synapses;
	
	if(network == 0 || idList.trimmed() == "") {
		return synapses;
	}
	
	QString unifiedList = idList;
	unifiedList.replace("|", ",");
	
	QList<Synapse*> allSynapses = network->getSynapses();
	
	QStringList ids = unifiedList.split(",");
	for(QListIterator<QString> i(ids); i.hasNext();) {
		bool ok = true;
		qulonglong id = i.next().toULongLong(&ok);
		if(ok) {
			Synapse *synapse = NeuralNetwork::selectSynapseById(id, allSynapses);
			if(synapse != 0) {
				synapses.append(synapse);
			}
		}
	}
	
	return synapses;
}



}



