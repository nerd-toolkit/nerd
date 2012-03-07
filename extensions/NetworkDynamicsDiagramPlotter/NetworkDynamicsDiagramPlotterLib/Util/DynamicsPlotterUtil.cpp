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

QList<qulonglong> DynamicsPlotterUtil::getIDsFromString(const QString &list, const QString &separator, const QString &replace) {
	QString tmp = QString(list);
	if(!replace.isEmpty()) {
		QStringList replist = replace.split(" ", QString::KeepEmptyParts);
		for(int i = 0; i < replist.size(); ++i) {
			tmp.replace(replace.at(i), separator);
		}
	}
	QStringList idlist = tmp.split(separator, QString::SkipEmptyParts);
	QList<qulonglong> output;
	bool ok;
	
	for(int i = 0; i < idlist.size(); ++i) {
		qulonglong id = idlist.at(i).toULongLong(&ok);
		
		if(!ok) {
			Core::log("Conversion to ULongLong failed!", true);
			continue;
		}
		
		output.append(id);
	}
	return output;
}
		

QList< double > DynamicsPlotterUtil::getDoublesFromString(const QString &list, const QString &separator) {
	QStringList doublelist = list.split(separator, QString::SkipEmptyParts);
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
		}

		output.append(QPair<double, double>(first, second));
	}

	return output;
}

}