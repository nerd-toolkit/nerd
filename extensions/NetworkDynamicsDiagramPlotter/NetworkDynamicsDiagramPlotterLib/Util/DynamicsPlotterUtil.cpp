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


#include "DynamicsPlotterUtil.h"
#include <Core/Core.h>
#include <QStringList>

namespace nerd {
	
DoubleValue *DynamicsPlotterUtil::getElementValue(QString const &string, QList<NeuralNetworkElement*> const &elemList) {
	// string Syntax: ID[[:FUNC]:PARAM]
	QStringList stringList = string.split(":", QString::SkipEmptyParts);
	if(stringList.isEmpty() || stringList.size() > 3) {
		return 0;
	}
	bool ok;
	qulonglong id = stringList.first().toULongLong(&ok);
	if(!ok) {
		return 0;
	}
	NeuralNetworkElement *elem = NeuralNetwork::selectNetworkElementById(id, elemList);
	if(elem == 0) {
		return 0;
	}
	Neuron *neuron = static_cast<Neuron*>(elem);
	if(neuron != 0) {
		if(stringList.size() == 1 || stringList.at(1) == "o") {
			return &(neuron->getOutputActivationValue());
		}
		if(stringList.at(1) == "a") {
			return &(neuron->getActivationValue());
		}
		if(stringList.at(1) == "b") {
			return &(neuron->getBiasValue());
		}
		if(stringList.size() == 3) {
			QString param = stringList.at(2);
			if(stringList.at(1) == "tf") {
				return static_cast<DoubleValue*>(neuron->getTransferFunction()->getParameter(param));
			}
			if(stringList.at(1) == "af") {
				return static_cast<DoubleValue*>(neuron->getActivationFunction()->getParameter(param));
			}
		}
		return 0;
	}
	Synapse *synapse = static_cast<Synapse*>(elem);
	if(synapse != 0) {
		if(stringList.size() == 1 || stringList.at(1) == "w") {
			return &(synapse->getStrengthValue());
		}
		if(stringList.size() == 3 && stringList.at(2) == "sf") {
			return static_cast<DoubleValue*>(synapse->getSynapseFunction()->getParameter(stringList.at(2)));
		}
		return 0;
	}
	return 0;
}

// Wrapper for above method to get values for multiple strings in a list of lists
QList< QList< DoubleValue *> > DynamicsPlotterUtil::getElementValues(QList< QStringList > const &listList, QList<NeuralNetworkElement*> const &elemList) {
	QList< QList< DoubleValue *> > outer;
	for(int i = 0; i < listList.size(); ++i) {
		QList< DoubleValue *> inner;
		QStringList stringList = listList.at(i);
		for(int j = 0; j < stringList.size(); ++j) {
			inner.append(getElementValue(stringList.at(j), elemList));
		}
		outer.append(inner);
	}
	return outer;
}
	
QList<QStringList> DynamicsPlotterUtil::parseElementString(QString const &string) {
	// Syntax: ID:FUNC:PARAM (eg I1:F1:P1,I2:F2:P2|I3:F3:P3)	
	QList<QStringList> plots;
	// Create list of plots
	QStringList plotlist = string.split("|", QString::SkipEmptyParts);
	for(int i = 0; i < plotlist.size(); ++i) {
		QStringList elems = plotlist.at(i).split(",", QString::SkipEmptyParts);
		plots.append(elems);
	}
	return plots;
}

QList< double > DynamicsPlotterUtil::getDoublesFromString(const QString &list, const QString &separator, const QString &replace) {
	QString tmp(list);
	if(!replace.isEmpty()) {
		QStringList replist = replace.split(separator, QString::SkipEmptyParts);
		for(int i = 0; i < replist.size(); ++i) {
			tmp.replace(replist.at(i), separator);
		}
	}
	QStringList doublelist = tmp.split(separator, QString::SkipEmptyParts);
	QList<double> output;
	bool ok;
	
	for(int i = 0; i < doublelist.size(); ++i) {
		double d = doublelist.at(i).toDouble(&ok);
		
		if(!ok) {
			Core::log("Conversion to double failed, check values please");
		}
		
		output.append(d);
	}
	return output;
}


// DEPRECATED
QList<DoubleValue *> DynamicsPlotterUtil::getElementValuesFromIDs(QList<qulonglong> &idlist, NeuralNetwork *&network, int type) {
	
	QListIterator<qulonglong> iterator(idlist);
	QList<Neuron *> neurons = network->getNeurons();
	QList<Synapse *> synapses = network->getSynapses();
	QList<DoubleValue *> values;
	DoubleValue *value;
	qulonglong id;

	while(iterator.hasNext()) {
		id = iterator.next();
		
		Neuron *n = NeuralNetwork::selectNeuronById(id, neurons);
		if(n != 0) {
			switch(type) {
				case 1:
					value = &n->getActivationValue();
					break;
				case 2:
					value = &n->getOutputActivationValue();
					break;
				case 0:
				default:
					value = &n->getBiasValue();
					break;
			}
			values.append(value);
			continue;
		}

		Synapse *s = NeuralNetwork::selectSynapseById(id, synapses);

		if(s != 0) {
			values.append(&s->getStrengthValue());
			continue;
		}

		Core::log("Could not find network element with id " + id, true);
	}

	return values;
}

/*
 * Returns a QList of IDs found in QString string separated by QString separator
 * beforehand, every occurence of a String in string which is given in QString
 * replace as a list of items separated by the same String given in separator
 */
QList<qulonglong> DynamicsPlotterUtil::getIDsFromString(const QString &string, const QString &separator, const QString &replace) {
	QString tmp = QString(string);
	if(!replace.isEmpty()) {
		QStringList replist = replace.split(separator, QString::SkipEmptyParts);
		for(int i = 0; i < replist.size(); ++i) {
			tmp.replace(replist.at(i), separator);
		}
	}
	QStringList idlist = tmp.split(separator, QString::SkipEmptyParts);
	QList<qulonglong> output;
	bool ok;
	
	for(int i = 0; i < idlist.size(); ++i) {
		qulonglong id = idlist.at(i).toULongLong(&ok);
		
		if(!ok) {
			Core::log("Conversion to ULongLong failed at position"+QString(i)+"!", true);
			continue;
		}
		
		output.append(id);
	}
	return output;
}

QList< QPair<double, double> > DynamicsPlotterUtil::getPairsFromString(const QString &list, const QString &elem_separator, const QString &pair_separator) {
	QStringList pairlist = list.split(pair_separator, QString::SkipEmptyParts);
	QList< QPair<double, double> > output;
	bool ok;

	for(int i = 0; i < pairlist.size(); ++i) {
		QStringList pair = pairlist.at(i).split(elem_separator, QString::SkipEmptyParts);

		if(pair.size() != 2) {
			Core::log("Not a pair, continuing w/o error", true);
			continue;
		}

		double first = pair.at(0).toDouble(&ok);
		double second = pair.at(1).toDouble(&ok);

		if(!ok) {
			Core::log("Conversion to double failed, check values please", true);
			continue;
		}

		output.append(QPair<double, double>(first, second));
	}

	return output;
}

}
