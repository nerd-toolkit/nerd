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

#include "NeuralNetworkIOEvosunXml.h"
#include "NeuralNetworkIO.h"
#include <QFile>
#include <QTextStream>
#include "TransferFunction/TransferFunctionSigmoid.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"

namespace nerd {

QDomElement NeuralNetworkIOEvosunXml::createNetElement(QDomDocument &doc, bool pure) {
	QDomElement netElement;
	if(pure) {
		netElement = doc.createElement("Net");
		doc.appendChild(netElement);
	} 
	else {
		QDomElement evoElement = doc.createElement("Evolution");
		doc.appendChild(evoElement);
		QDomElement genElement = doc.createElement("Generation");
		genElement.setAttribute("Index", "0");
		evoElement.appendChild(genElement);
		QDomElement popElement = doc.createElement("Population");
		popElement.setAttribute("Name", "pop1");
		popElement.setAttribute("Index", "0");
		genElement.appendChild(popElement);
		QDomElement commElement = doc.createElement("Comment");
		popElement.appendChild(commElement);
		QDomText comment = doc.createTextNode(" no comment ");
		commElement.appendChild(comment);
		QDomElement evoObElement = doc.createElement("EvoObject");
		evoObElement.setAttribute("Index", "0");
		evoObElement.setAttribute("OutPerf", "0.0");
		evoObElement.setAttribute("SysPerf", "0.0");
		evoObElement.setAttribute("Age", "0");
		popElement.appendChild(evoObElement);
		netElement = doc.createElement("Net");
		evoObElement.appendChild(netElement);
	}
	return netElement;
}

bool NeuralNetworkIOEvosunXml::createFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg, QList<QString> *warnings, bool pure) {

	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		if(errorMsg != NULL) {
			*errorMsg = QString("Cannot create file ").append(fileName).append(".");
		}
		file.close();
		return false;
	}

	QDomDocument doc;

	QDomProcessingInstruction header = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"LATIN1\"");
	doc.appendChild(header); 

	QDomElement netElement = createNetElement(doc, pure);

	netElement.setAttribute("SynapseMode", "conventional");
	
	QString nerdTfName = net->getDefaultTransferFunction()->getName();
	QString evosunTfName = NeuralNetworkIO::getEvosunTransferFunctionNameFromNERD(nerdTfName);
	if(evosunTfName.isEmpty()) {
		if(warnings != NULL) {
			warnings->append(QString("DefaultTransferFunction not supported by Evosun. Set to \"tanh\"!"));
		}
		netElement.setAttribute("Transferfunction", "tanh");
	} 
	else {
		netElement.setAttribute("Transferfunction", evosunTfName);
	}

	bool childSynapseWarning = false;
	bool functionWarning = false;

	QList<Neuron*> neurons = net->getNeurons();
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();

		if(!functionWarning &&
			(  (neuron->getTransferFunction()->getName() != net->getDefaultTransferFunction()->getName())
			|| (neuron->getActivationFunction()->getName() != net->getDefaultActivationFunction()->getName()))) {
			if(warnings != NULL) {
				warnings->append(QString("TransferFunctions and ActivationFunctions assigned to single neurons not supported by Evosun. All functions set to default functions of the Network!"));
			}
			functionWarning = true;
		}

		QString type = "hidden";

		if(neuron->hasProperty(Neuron::NEURON_TYPE_INPUT)) {
			type = "input";
		} 
		else if(neuron->hasProperty(Neuron::NEURON_TYPE_OUTPUT)) {
			type = "output";
		}

		QDomElement neuronElem = doc.createElement("Neuron");
		netElement.appendChild(neuronElem);
		neuronElem.setAttribute("Layer", type);
		neuronElem.setAttribute("Process", "dynamic");
		neuronElem.setAttribute("Bias", neuron->getBiasValue().getValueAsString());

		QList<Synapse*> synapses = neuron->getSynapses();
		for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
			Synapse *synapse = j.next();
			QDomElement synapseElem = doc.createElement("Synapse");
			neuronElem.appendChild(synapseElem);
			synapseElem.setAttribute("Source", QString::number(NeuralNetworkIO::getNeuronNumber(net, synapse->getSource())));
			synapseElem.setAttribute("Process", "dynamic");
			synapseElem.setAttribute("Strength", synapse->getStrengthValue().getValueAsString());
			if(!childSynapseWarning && (synapse->getSynapses().size() > 0)) {
				if(warnings != NULL) {
					warnings->append(QString("Net has synapses connected to synapses. This is not supported by Evosun. Synapses will be lost!"));
				}
				childSynapseWarning = false;
			}
		}
	}

	// write QDomDocument to file
	QTextStream outStream(&file);
	doc.save(outStream, 1);

	file.close();
	return true;
}

NeuralNetwork* NeuralNetworkIOEvosunXml::createNetFromFile(QString fileName, QString *errorMsg, QList<QString> *warnings) {

	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)) {
		if(errorMsg != NULL) {
			*errorMsg = QString("Cannot open file ").append(fileName).append(".");
		}
		file.close();
		return NULL;
	}

	QDomDocument document;
	QString errorStr;
	int errorLine, errorColumn;
	if(!document.setContent(&file, &errorStr, &errorLine, &errorColumn)) {
		if(errorMsg != NULL) {
			*errorMsg = QString("Parse error at line %1, column %2:\n%3")
								.arg(errorLine)
								.arg(errorColumn)
								.arg(errorStr);
		}
		file.close();
		return NULL;
	}

	QDomNodeList nodeList = document.elementsByTagName("Net");
	QDomElement netElement;
	for(uint ui = 0; ui < nodeList.length(); ++ui) {
		QDomNode node = nodeList.item(ui);
		if(node.isElement()) {
			netElement = node.toElement();
			break;
		}
	}
	if(netElement.isNull()) {
		if(errorMsg != NULL) {
      // No root level Net element found
      // --> assume original EvoSun syntax and try to find 
      //     first Net element as child node in tree with path 
      //     /Evolution/Generation/Population/EvoObject/
      QDomNodeList nodeList = document.elementsByTagName("Evolution");
      for(uint ui = 0; ui < nodeList.length(); ++ui) {
        QDomNode evoNode = nodeList.item(ui);
        if(!evoNode.isNull() && evoNode.isElement()) {
          QDomElement genElement = evoNode.firstChildElement("Generation");
          if(!genElement.isNull()) {
            QDomElement popElement = genElement.firstChildElement("Population");
            if(!popElement.isNull()) {
              QDomElement evoObjElement = popElement.firstChildElement("EvoObject");
              if(!evoObjElement.isNull()) {
                netElement = evoObjElement.firstChildElement("Net");
                if(!netElement.isNull()) {
                  break;
                }
              }
            }
          }
        }
      }
      if(netElement.isNull()) {
        if(errorMsg != NULL) {
          *errorMsg = "No element <Net> found!";
        }
		file.close();
        return NULL;
      }
		}
		file.close();
		return NULL;
	}

	NeuralNetwork *net = new ModularNeuralNetwork();

	addDefaultTransferFunctionToNetwork(net, netElement, warnings);
	addNeuronsAndSynapsesToNetwork(net, netElement, warnings);

	net->adjustIdCounter();

	file.close();

	return net;
}

void NeuralNetworkIOEvosunXml::addDefaultTransferFunctionToNetwork(NeuralNetwork *net, const QDomElement &netElement, QList<QString> *warnings) {
	QString transferFunction = netElement.attribute("Transferfunction");

	TransferFunction *tf = NULL;

	if(transferFunction == "tanh") {
		tf = new TransferFunctionTanh();
	}
	else if(transferFunction == "sigm") {
		tf = new TransferFunctionSigmoid();
	}
	if(tf != NULL) {
		net->setDefaultTransferFunction(*tf);
		delete tf;
	} 
	else {
		warnings->append(QString("Unknown name of TransferFunction. "
			"TransferFunction set to \"%1\"").arg(net->getDefaultTransferFunction()->getName()));
	}
}

void NeuralNetworkIOEvosunXml::addNeuronsAndSynapsesToNetwork(NeuralNetwork *net, const QDomElement &netElement, QList<QString> *warnings) {

	QVector<Neuron*> neuronBuffer;
	QHash<Neuron*, QDomNodeList> synapseBuffer;

	// add Neurons
	{
		QDomNodeList neurons = netElement.elementsByTagName("Neuron");

		int inputNeuronCount = 0;
		int outputNeuronCount = 0;
		int hiddenNeuronCount = 0;

		for(uint ui = 0; ui < neurons.length(); ++ui) {
			QDomElement neuronElem;
			QDomNode node = neurons.item(ui);
			if(node.isElement()) {
				neuronElem = node.toElement();

				QString layer = neuronElem.attribute("Layer");
				QString bias = neuronElem.attribute("Bias");

				if(layer.isEmpty() || bias.isEmpty()) {
					warnings->append(QString("One of the attributes \"Layer\" and \"Bias\" not found in neuron from line %1. Neuron ignored!").arg(node.lineNumber()));
					continue;
				}

				QString name;
				if(layer == "input") {
					name = QString("InputNeuron%1").arg(++inputNeuronCount);
				} 
				else if (layer == "output") {
					name = QString("OutputNeuron%1").arg(++outputNeuronCount);
				} 
				else {
					name = QString("HiddenNeuron%1").arg(++hiddenNeuronCount);
				}

				// Create Neuron
				Neuron *neuron = new Neuron(name, *(net->getDefaultTransferFunction()), *(net->getDefaultActivationFunction()));

				//Set layer and bias
				if(layer == "input") {
					neuron->setProperty(Neuron::NEURON_TYPE_INPUT);
				}
				if(layer == "output") {
					neuron->setProperty(Neuron::NEURON_TYPE_OUTPUT);
				}

				neuron->getBiasValue().set(bias.toDouble());
				net->addNeuron(neuron);
				neuronBuffer.append(neuron);

				QDomNodeList synapseList = neuronElem.elementsByTagName("Synapse");
				synapseBuffer.insert(neuron, synapseList);
			}
		}
	}

	// add Synapses
	{
		QList<Neuron*> neurons = synapseBuffer.keys();
		QListIterator<Neuron*> neuronIterator(neurons);
		while(neuronIterator.hasNext()) {
			Neuron *neuron = neuronIterator.next();
			QDomNodeList synapseList = synapseBuffer.value(neuron);

			for(uint uj = 0; uj < synapseList.length(); ++uj) {
				QDomNode node = synapseList.item(uj);
				if(node.isElement()) {
					QDomElement synapseElem = node.toElement();
					QString strength = synapseElem.attribute("Strength");
					QString source = synapseElem.attribute("Source");
					if(strength.isEmpty() || source.isEmpty()) {
						warnings->append(QString("One of the attributes \"Strength\" and \"Source\" not found in synapse from line %1. Synapse ignored!").arg(node.lineNumber()));
						continue;
					}
					qulonglong sourceID = source.toULongLong();
					if(sourceID >= (qulonglong) neuronBuffer.size()) {
						warnings->append(QString("SourceID from synapse in line %1 does not match any "
									"neuron. Synapse ignored!").arg(node.lineNumber()));
						continue;
					}
					Synapse *synapse = new Synapse(neuronBuffer.at(sourceID), NULL, strength.toDouble(), *(net->getDefaultSynapseFunction()));
					neuron->addSynapse(synapse);
				}
			}
		}
	}
}

}
