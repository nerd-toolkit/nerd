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
#include <Math/Math.h>
#include <ModularNeuralNetwork/ModularNeuralNetwork.h>
#include <QStringList>

namespace nerd {

DoubleValue * DynamicsPlotterUtil::getElementValue(QString const &string, QList<NeuralNetworkElement*> const &elemList) {
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
	
	Neuron *neuron = dynamic_cast<Neuron*>(elem);
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
				ObservableNetworkElement *tf = 
						dynamic_cast<ObservableNetworkElement*>(neuron->getTransferFunction());
				if(tf != 0) {
					return dynamic_cast<DoubleValue*>(tf->getObservableOutput(param));
				}
				else {
					return 0;
				}
			}
			if(stringList.at(1) == "af") {
				ObservableNetworkElement *af = 
						dynamic_cast<ObservableNetworkElement*>(neuron->getActivationFunction());
				if(af != 0) {
					return dynamic_cast<DoubleValue*>(af->getObservableOutput(param));
				}
				else {
					return 0;
				}
			}
		}
		return 0;
	}
	
	Synapse *synapse = dynamic_cast<Synapse*>(elem);
	if(synapse != 0) {
		if(stringList.size() == 1 || stringList.at(1) == "w") {
			return &(synapse->getStrengthValue());
		}
		if(stringList.size() == 3 && stringList.at(2) == "sf") {
			ObservableNetworkElement *sf = 
					dynamic_cast<ObservableNetworkElement*>(synapse->getSynapseFunction());
			if(sf != 0) {
				return dynamic_cast<DoubleValue*>(sf->getObservableOutput(stringList.at(2)));
			}
			else {
				return 0;
			}
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
			DoubleValue *value = getElementValue(stringList.at(j), elemList);
			if(value == 0) {
				//return empty list, so that the caller can react on the problem
				Core::log("DynamicsPlotterUtil:getElementValues: Could not find "
						  "an element for [ " + stringList.at(j) + "]", true);
				return QList< QList< DoubleValue *> >();
			}
			
			inner.append(value);
		}
		outer.append(inner);
	}
	return outer;
}
	
QList<QStringList> DynamicsPlotterUtil::parseElementString(QString const &string) {
	// Syntax: ID:FUNC:PARAM (eg I1:F1:P1,I2:F2:P2|I3:F3:P3)	
	QList<QStringList> parts;
	// Create list of plots
	QStringList partlist = string.split("|", QString::SkipEmptyParts);
	for(int i = 0; i < partlist.size(); ++i) {
		QStringList elems = partlist.at(i).split(",", QString::SkipEmptyParts);
		parts.append(elems);
	}
	return parts;
}

QList<double> DynamicsPlotterUtil::getDoublesFromString(const QString &list,
const QString &separator, const QString &replace) {
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
			Core::log("Conversion to double failed, check values please", true);
			return QList<double>();
		}
		
		output.append(d);
	}
	return output;
}

// returns a list of references to relevant network element values
QList<DoubleValue*> DynamicsPlotterUtil::getNetworkValues(const
QList<NeuralNetworkElement*> networkElements) {
	QList<DoubleValue*> list;
	for(int i = 0; i < networkElements.size(); ++i) {
		NeuralNetworkElement *e = networkElements.at(i);
		
		if(e == 0) {
			Core::log("DynamicsPlotterUtil::getNetworkState : "
						"NeuralNetworkElement is NULL!", true);
			return QList<DoubleValue*>();
		}
		
		Neuron *n = dynamic_cast<Neuron*>(e);
		if(n != 0) {
			DoubleValue *d = &(n->getActivationValue());
			list.append(d);
			DoubleValue *b = &(n->getBiasValue());
			list.append(b);
			
			ObservableNetworkElement *tf =
			dynamic_cast<ObservableNetworkElement*>(n->getTransferFunction());
			if(tf != 0) {
				QList<Value*> tfVals = tf->getObservableOutputs();
				for(int j = 0; j < tfVals.size(); ++j) {
					DoubleValue *v = dynamic_cast<DoubleValue*>(tfVals.at(j));
					if(v != 0) {
						list.append(v);
					}
				}
			}
			
			ObservableNetworkElement *af =
			dynamic_cast<ObservableNetworkElement*>(n->getActivationFunction());
			if(af != 0) {
				QList<Value*> afVals = af->getObservableOutputs();
				for(int j = 0; j < afVals.size(); ++j) {
					DoubleValue *v = dynamic_cast<DoubleValue*>(afVals.at(j));
					if(v != 0) {
						list.append(v);
					}
				}
			}
		}
		
		Synapse *s = dynamic_cast<Synapse*>(e);
		if(s != 0) {
			DoubleValue *d = &(s->getStrengthValue());
			list.append(d);
			
			ObservableNetworkElement *sf =
			dynamic_cast<ObservableNetworkElement*>(s->getSynapseFunction());
			if(sf != 0) {
				QList<Value*> sfVals = sf->getObservableOutputs();
				for(int j = 0; j < sfVals.size(); ++j) {
					DoubleValue *v = dynamic_cast<DoubleValue*>(sfVals.at(j));
					if(v != 0) {
						list.append(v);
					}
				}
			}
		}
	}
	return list;
}

// uses the list generated by getNetworkValues to get current actual state
QList<double> DynamicsPlotterUtil::getNetworkState(const QList<DoubleValue*>
networkValues) {
	if(networkValues.isEmpty()) {
		return QList<double>();
	}
	
	QList<double> networkState;
	for(int i = 0; i < networkValues.size(); ++i) {
		double tmp = networkValues.at(i)->get();
		networkState.append(tmp);
	}
	
	return networkState;
}

bool DynamicsPlotterUtil::compareNetworkStates(const QList<double>
&state1, const QList<double> &state2, double accuracy) {
	if(state1.size() != state2.size()) {
		Core::log("DynamicsPlotterUtil: Cannot compare network states, they "
					"appear to be from different networks", true);
		return false;
	}
	
	for(int i = 0; i < state1.size(); ++i) {
		if( !Math::compareDoubles(state1.at(i), state2.at(i), accuracy) ) {
			return false;
		}
	}
	
	return true;
}

}
