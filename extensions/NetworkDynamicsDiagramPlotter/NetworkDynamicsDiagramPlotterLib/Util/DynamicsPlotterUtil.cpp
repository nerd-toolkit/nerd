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
#include <math.h>
#include <Gui/GuiManager.h>
#include <ModularNeuralNetwork/ModularNeuralNetwork.h>
#include <Gui/NetworkEditorTools/EditorMessageWidget.h>
#include <NetworkEditorConstants.h>
#include <QStringList>
#include <QTime>


namespace nerd {

	
	EditorMessageWidget* DynamicsPlotterUtil::sMessageWidget = 0;
	Core* DynamicsPlotterUtil::sCore = 0;

	
	DoubleValue* DynamicsPlotterUtil::getElementValue(QString const &specifier,
													  NeuralNetworkElement* networkElement,
													  QList<Neuron*> *neuronsWithActivationChange)
	{
		QString parameter, variable;
		QStringList nParams, sParams;
		nParams << "o" << "a" << "b" << "tf" << "af";
		sParams << "w" << "sf";

		if(specifier.isEmpty()) {
			reportProblem("DynamicsPlotterUtil::getElementValue(1) : Empty parameter specification");
			return 0;
		}

		if(specifier.contains(":")) {
			QStringList paramParts = specifier.split(":");
			if(paramParts.size() > 2) {
				reportProblem("DynamicsPlotterUtil::getElementValue(1) : Invalid parameter specification");
				return 0;
			}
			parameter = paramParts.first();
			variable = paramParts.last();
		} else {
			parameter = specifier;
		}
		
		// requested parameter is neuron-specific
		if(nParams.contains(parameter)) {
			Neuron *neuron = dynamic_cast<Neuron*>(networkElement);

			if(neuron != 0) {
				if(parameter == specifier) { // no variable requested

					if(parameter == nParams.at(0)) { // output "o"
						return &(neuron->getOutputActivationValue());
					}

					if(parameter == nParams.at(1)) { // activation "a"
						if(neuronsWithActivationChange != 0) {
							neuronsWithActivationChange->append(neuron);
						}
						return &(neuron->getActivationValue());
					}

					if(parameter == nParams.at(2)) { // bias "b"
						return &(neuron->getBiasValue());
					}

				} else { // variable requested

					if(parameter == nParams.at(3)) { // TransferFunction "tf"
						TransferFunction *tf = neuron->getTransferFunction();
						if(tf != 0) {
							DoubleValue *tfo = dynamic_cast<DoubleValue*>(tf->getObservableOutput(variable));
							if(tfo != 0) {
								return tfo;
							}
							DoubleValue *tfp = dynamic_cast<DoubleValue*>(tf->getParameter(variable));
							if(tfp != 0) {
								return tfp;
							}
						}
					}

					if(parameter == nParams.at(4)) { // ActivationFunction "af"
						ActivationFunction *af = neuron->getActivationFunction();
						if(af != 0) {
							DoubleValue *afo = dynamic_cast<DoubleValue*>(af->getObservableOutput(variable));
							if(afo != 0) {
								return afo;
							}
							DoubleValue *afp = dynamic_cast<DoubleValue*>(af->getParameter(variable));
							if(afp != 0) {
								return afp;
							}
						}
					}
				}
				
				// requested parameter has not been found, report
				reportProblem("DynamicsPlotterUtil::getElementValue(1) : "
							  "Found neuron ["+QString::number(neuron->getId())+"] "
							  "but not the specified observable or function parameter "
							  "["+specifier+"]");
			}
		}

		// requested parameter is synapse-specific
		if(sParams.contains(parameter)) {
			Synapse *synapse = dynamic_cast<Synapse*>(networkElement);

			if(synapse != 0) {
				if(parameter == specifier) { // no variable requested

					if(parameter == sParams.at(0)) { // weight w
						return &(synapse->getStrengthValue());
					}

				} else { // variable requested

					if(parameter == sParams.at(1)) {
						SynapseFunction *sf = synapse->getSynapseFunction();
						if(sf != 0) {
							DoubleValue *sfo = dynamic_cast<DoubleValue*>(sf->getObservableOutput(variable));
							if(sfo != 0) {
								return sfo;
							}
							DoubleValue *sfp = dynamic_cast<DoubleValue*>(sf->getParameter(variable));
							if(sfp != 0) {
								return sfp;
							}
						}
					}
				}

				// requested parameter has not been found, report
				reportProblem("DynamicsPlotterUtil::getElementValue(1) : "
							  "Found synapse ["+QString::number(synapse->getId())+"] "
							  "but not the specified observable or function parameter "
							  "["+specifier+"]");
			}
		}
		
		// neuron/synapse or specified parameter not found
		// do not report here, since getElementValues
		// correctly calls this function with non-matching
		// specifier-element pairs
		return 0;
	}


	/**
	 * 
	 * The neuronsWithActivationChange parameter can be used to collect all DoubelValues that correspond
	 * to activation values of neurons. These neurons have to be collected in each DymnamcisPlotter in
	 * variable DynamcisPlotter::mNeuronsWithActivationsToTransfer to be handled correctly, if the 
	 * activation of a neuron should be varied during an analyzer run. Otherwise the varied activations
	 * are immediately overwritten by the newly calculated activations at the first network update. 
	 * 
	 * If this method is used to collect elements that will NOT be changed during analyzer runs,
	 * then the last parameter MUST be empty. Othewise activations of neurons collected in this way
	 * will be treated differently compared to the other neurons in the network. This may lead to 
	 * unexpected and erroneous behavior.
	 * 
	 * @param specifier the specification of the desired DoubleValue object.
	 * @param networkElements the list of objects that are considered to find the specified DoubleValue object.
	 * @param neuronsWithActivationChange (optional) list to collect all activation values that are going to be changed during a run.
	 */
	DoubleValue* DynamicsPlotterUtil::getElementValue(QString const &specifier,
													  QList<NeuralNetworkElement*> const &networkElements,
													  QList<Neuron*> *neuronsWithActivationChange)
	{
		if(specifier.isEmpty()) {
			reportProblem("DynamicsPlotterUtil::getElementValue(2) : "
						  "Empty specifier!");
			return 0;
		}

		QStringList specifierParts = specifier.split(":");
		if(specifierParts.size() < 2 || specifierParts.size() > 3) {
			reportProblem("DynamicsPlotterUtil::getElementValue(2) : "
						  "Invalid specifier ["+specifier+"]!");
			return 0;
		}

		bool idValid;
		QString idString = specifierParts.first();
		QString parameter = specifier;
		parameter = parameter.remove(0, idString.size()+1);
		qulonglong id = idString.toULongLong(&idValid);
		if(!idValid) {
			reportProblem("DynamicsPlotterUtil::getElementValue(2) : "
						  "Invalid ID ["+idString+"]!");
			return 0;
		}

		NeuralNetworkElement* networkElement;
		networkElement = NeuralNetwork::selectNetworkElementById(id, networkElements);
		if(networkElement == 0) {
			reportProblem("DynamicsPlotterUtil::getElementValue(2) : "
						  "No element with ID ["+idString+"] "
						  "found in given list!");
			return 0;
		}

		DoubleValue* elementValue;
		elementValue = getElementValue(parameter, networkElement, neuronsWithActivationChange);
		if(elementValue == 0) {
			reportProblem("DynamicsPlotterUtil::getElementValue(2) : "
						  "Could not find specified parameter value at element!");
			return 0;
		}

		return elementValue;
	}


	/**
	 * 
	 * The neuronsWithActivationChange parameter can be used to collect all DoubelValues that correspond
	 * to activation values of neurons. These neurons have to be collected in each DymnamcisPlotter in
	 * variable DynamcisPlotter::mNeuronsWithActivationsToTransfer to be handled correctly, if the 
	 * activation of a neuron should be varied during an analyzer run. Otherwise the varied activations
	 * are immediately overwritten by the newly calculated activations at the first network update. 
	 * 
	 * If this method is used to collect elements that will NOT be changed during analyzer runs,
	 * then the last parameter MUST be empty. Othewise activations of neurons collected in this way
	 * will be treated differently compared to the other neurons in the network. This may lead to 
	 * unexpected and erroneous behavior.
	 * 
	 * @param specifierLists a list of stringlists, containing the single specifications of DoubleValues.
	 * @param networkElements the list of objects that are considered to find the specified DoubleValue object.
	 * @param neuronsWithActivationChange (optional) list to collect all activation values that are going to be changed during a run.
	 */
	QList< QList<DoubleValue*> > DynamicsPlotterUtil::getElementValues(QList<QStringList> const &specifierLists, 
																	   QList<NeuralNetworkElement*> const &networkElements, 
																	   QList<Neuron*> *neuronsWithActivationChange)
	{
		QList< QList<DoubleValue*> > plotElements, emptyList;
		emptyList = QList< QList<DoubleValue*> >();
		
		for(int listNr = 0; listNr < specifierLists.size(); ++listNr) {
			QList<DoubleValue*> elementValues;
			
			QStringList specifierList = specifierLists.at(listNr);
			for(int specifierNr = 0; specifierNr < specifierList.size(); ++specifierNr) {

				QString specifier = specifierList.at(specifierNr);
				if(specifier.isEmpty()) {
					reportProblem("DynamicsPlotterUtil::getElementValues : Empty specifier!");
					return emptyList;
				}

				QStringList parameters = specifier.split(":");
				if(parameters.size() < 2 || parameters.size() > 3) {
					reportProblem("DynamicsPlotterUtil::getElementValues : Invalid specifier ["+specifier+"]!");
					return emptyList;
				}

				if(parameters.first() == "all") {
					QList<DoubleValue*> netValues;
					DoubleValue* elementValue;
					QString parameter = specifier.remove("all:");

					for(int elemNr = 0; elemNr < networkElements.size(); ++elemNr) {
						NeuralNetworkElement* networkElement = networkElements.at(elemNr);
						elementValue = getElementValue(parameter, networkElement, neuronsWithActivationChange);

						if(elementValue != 0) {
							netValues.append(elementValue);
						}
					}

					elementValues.append(netValues);

				} else {
					DoubleValue* elementValue;
					elementValue = getElementValue(specifier, networkElements, neuronsWithActivationChange);

					if(elementValue == 0) {
						reportProblem("DynamicsPlotterUtil::getElementValues : Could not find a value "
									  "for element specifier ["+specifier+"]!");
						return emptyList;
					}
					
					elementValues.append(elementValue);

				}
			}

			plotElements.append(elementValues);
		}

		return plotElements;
	}


	QList<QStringList> DynamicsPlotterUtil::parseElementString(QString const &elementString) {
		if(elementString.isEmpty()) {
			return QList<QStringList>();
		}
		QList<QStringList> parts;
		QStringList partList = elementString.split("|", QString::SkipEmptyParts);
		for(int i = 0; i < partList.size(); ++i) {
			QStringList elements = partList.at(i).split(",", QString::SkipEmptyParts);
			parts.append(elements);
		}
		return parts;
	}


	double DynamicsPlotterUtil::getMeanValue(const QList<DoubleValue*> &valuesList) {
		if(valuesList.isEmpty()) {
			reportProblem("DynamicsPlotterUtil::getMeanValue : Empty list. Nothing to do.");
			return 0;
		}

		int nrValues = valuesList.size();
		double meanValue = 0;

		for(int currVal = 0; currVal < nrValues; ++currVal) {
			DoubleValue* dVal = valuesList.at(currVal);

			if(dVal == 0) {
				reportProblem("DynamicsPlotterUtil::getMeanValue : Encountered NULL element.");
				return 0;
			}

			meanValue += dVal->get();
		}

		return meanValue / nrValues;
	}


	QList<double> DynamicsPlotterUtil::getMeanValues(const QList< QList<DoubleValue*> > &valuesListList) {
		QList<double> meanValues, emptyList;
		emptyList = QList<double>();

		if(valuesListList.isEmpty()) {
			reportProblem("DynamicsPlotterUtil::getMeanValues : Empty list. Nothing to do.");
			return emptyList;
		}
		
		for(int currList = 0; currList < valuesListList.size(); ++currList) {
			meanValues.append(getMeanValue(valuesListList.at(currList)));
		}

		return meanValues;
	}		 


	// separator is a string containing the symbol/character separating entries
	// and replace contains a separator-separated list of characters to replace
	// by the separator beforehand
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
				reportProblem("DynamicsPlotterUtil::getDoublesFromString : "
							  "Unable to convert ["+doublelist.at(i)+"] to double");
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
				reportProblem("DynamicsPlotterUtil::getNetworkState : "
							"NeuralNetworkElement is NULL!");
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
			reportProblem("DynamicsPlotterUtil: Cannot compare network states, they "
						"appear to be from different networks");
			return false;
		}
		
		for(int i = 0; i < state1.size(); ++i) {
			if(!Math::compareDoubles(state1.at(i), state2.at(i), accuracy) ) {
				return false;
			}
		}
		
		return true;
	}


	double DynamicsPlotterUtil::getDistance(const QList<double> &state1, const QList<double> &state2) {
		if(state1.size() != state2.size()) {
			reportProblem("DynamicsPlotterUtil::getDistance : State dimensions do not match.");
			return 0;
		}

		double sum = 0;
		for(int i = 0; i < state1.size(); ++i) {
			sum += pow(state1.at(i)-state2.at(i),2);
		}
		return sqrt(sum);
	}


	void DynamicsPlotterUtil::transferNeuronActivationToOutput(NeuralNetwork *network) {
			
		if(network != 0) {
			
			//Set the output of every neuron to the transferred activation
			DynamicsPlotterUtil::transferNeuronActivationToOutput(network->getNeurons());
		}
	}

	void DynamicsPlotterUtil::transferNeuronActivationToOutput(const QList<Neuron*> neurons) {
		
		//Set the output of every neuron to the transferred activation
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *neuron = i.next();
			neuron->getOutputActivationValue().set(
				neuron->getTransferFunction()->transferActivation(
					neuron->getActivationValue().get(), neuron));
		}
	}
	
	void DynamicsPlotterUtil::reportProblem(const QString &errorMessage) {
		
		if(DynamicsPlotterUtil::sMessageWidget == 0 
				|| DynamicsPlotterUtil::sCore != Core::getInstance()) 
		{
			DynamicsPlotterUtil::sCore = Core::getInstance();
			DynamicsPlotterUtil::sMessageWidget = dynamic_cast<EditorMessageWidget*>(
					GuiManager::getGlobalGuiManager()->getWidget(NetworkEditorConstants::WIDGET_MESSAGE_WINDOW));
		}
		if(DynamicsPlotterUtil::sMessageWidget != 0) {
			sMessageWidget->addMessage(QTime::currentTime().toString("hh:mm:ss") + ": " + errorMessage);
			//Core::log(QTime::currentTime().toString("hh:mm:ss") + ": " + errorMessage, true);
		}
		else {
			Core::log(QTime::currentTime().toString("hh:mm:ss") + ": " + errorMessage, true);
		}
	}
	
	void DynamicsPlotterUtil::clearProblemMessageArea() {
		if(DynamicsPlotterUtil::sMessageWidget == 0 
				|| DynamicsPlotterUtil::sCore != Core::getInstance()) 
		{
			DynamicsPlotterUtil::sCore = Core::getInstance();
			DynamicsPlotterUtil::sMessageWidget = dynamic_cast<EditorMessageWidget*>(
					GuiManager::getGlobalGuiManager()->getWidget(NetworkEditorConstants::WIDGET_MESSAGE_WINDOW));
		}
		if(DynamicsPlotterUtil::sMessageWidget != 0) {
			//sMessageWidget->clear();
			sMessageWidget->clear();
		}
	}

}
