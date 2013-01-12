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

#include <QDomDocument>
#include <QFile>
#include "NeuralNetworkIO.h"
#include "NeuralNetworkIOSimbaV3Xml.h"
#include "NeuralNetworkIOEvosunXml.h"
#include "NeuralNetworkIOBytecode.h"
#include "Core/Core.h"
#include "IO/NeuralNetworkIONerdV1Xml.h"

namespace nerd {

/**
 * Loads and creates a NeuralNetwork from a file.
 *
 * @param fileName the name of the file from which the NeuralNetwork should be loaded.
 * @param type the type of the file.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @param warnings returns warnings if the NeuralNetwork was created with some restrictions.
 * @return the created NeuralNetwork or NULL if the function fails.
 */
NeuralNetwork* NeuralNetworkIO::createNetworkFromFile(const QString &fileName, 
				FileType type, QString *errorMsg, QList<QString> *warnings) 
{
	switch(type) {
		case SimbaV3Xml:
			return NeuralNetworkIOSimbaV3Xml::createNetFromFile(fileName, errorMsg, warnings);
		case EvosunXml:
		case PureEvosunXml:
			return NeuralNetworkIOEvosunXml::createNetFromFile(fileName, errorMsg, warnings);
//		case Bytecode:
//			return NeuralNetworkIOBytecode::createNetFromFile(fileName, errorMsg, warnings);
		default:
			Core::log("NeuralNetworkIO::loadNetworkFromFile : Unsupported file type!");
			return 0;
	}
	return 0;
}

NeuralNetwork* NeuralNetworkIO::createNetworkFromFile(const QString &fileName, 
					QString *errorMsg, QList<QString> *warnings)
{
	if(fileName.endsWith(".smb")) {
		return NeuralNetworkIOSimbaV3Xml::createNetFromFile(fileName, errorMsg, warnings);
	}
	else if(fileName.endsWith(".onn")) {
		QFile file(fileName);
		if(!file.open(QIODevice::ReadOnly)) {
			if(errorMsg != NULL) {
				*errorMsg = QString("Cannot open file ").append(fileName).append(".");
			}
			file.close();
			return NULL;
		}
		QString xml;
		QTextStream input(&file);
		while (!input.atEnd()) {
			xml.append(input.readLine()).append("\n");
		}
		file.close();
		return NeuralNetworkIONerdV1Xml::createNetFromXml(xml, errorMsg, warnings);
	}
	else if(fileName.endsWith(".xml") || fileName.endsWith(".sxml")) {
		return NeuralNetworkIOEvosunXml::createNetFromFile(fileName, errorMsg, warnings);
	}
	*errorMsg = "Unsupported file type!";
	Core::log("NeuralNetworkIO::loadNetworkFromFile : Unsupported file type!");
	return 0;
}



/**
 * Saves a NeuralNetwork to a file.
 *
 * @param fileName the name of the file to which the network should be saved.
 * @param net the NeuralNetwork to be saved.
 * @param type the format in which the NeuralNetwork should be saved.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @param warnings returns warnings if the NeuralNetwork was saved with some restrictions.
 * @return true if the NeuralNetwork was saved, otherwise false
 */
bool NeuralNetworkIO::createFileFromNetwork(const QString &fileName, NeuralNetwork *net, 
					FileType type, QString *errorMsg, QList<QString> *warnings) 
{
	switch(type) {
		case SimbaV3Xml:
			return NeuralNetworkIOSimbaV3Xml::createFileFromNetwork(fileName, net, errorMsg);
		case EvosunXml:
			return NeuralNetworkIOEvosunXml::createFileFromNetwork(fileName, net, errorMsg, warnings, false);
		case PureEvosunXml:
			return NeuralNetworkIOEvosunXml::createFileFromNetwork(fileName, net, errorMsg, warnings, true);
		case Bytecode:
			return NeuralNetworkIOBytecode::createFileFromNetwork(fileName, net, errorMsg);
		default:
			Core::log("NeuralNetworkIO::createFileFromNetwork : Unsupported file type!");
			return false;
	}
	return false;
}

bool NeuralNetworkIO::createFileFromNetwork(const QString &fileName, NeuralNetwork *net, 
					QString *errorMsg, QList<QString> *warnings)
{
	if(fileName.endsWith(".smb")) {
		return NeuralNetworkIOSimbaV3Xml::createFileFromNetwork(fileName, net, errorMsg);
	}
	else if(fileName.endsWith(".onn")) {
		QString xml = NeuralNetworkIONerdV1Xml::createXmlFromNetwork(net);
		QFile file(fileName);
		if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			if(errorMsg != NULL) {
				*errorMsg = QString("Cannot create file ").append(fileName).append(".");
			}
			file.close();
			return false;
		}
		QTextStream out(&file);
		out << xml << "\n";
		file.close();	
		return true;
	}
	else if(fileName.endsWith(".xml")) {
		return NeuralNetworkIOEvosunXml::createFileFromNetwork(fileName, net, errorMsg, warnings, false);
	}
	else if(fileName.endsWith(".sxml")) {
		return NeuralNetworkIOEvosunXml::createFileFromNetwork(fileName, net, errorMsg, warnings, true);
	}
	
	Core::log("NeuralNetworkIO::createFileFromNetwork : Unsupported file type! [" + fileName + "]");
	return false;
}

/**
 * Returns the name of the TransferFunction used in Evosun corresponding to the name used in NERD
 * @param nerdName the name of the TransferFunction used in NERD
 * @return the name of the TransferFunction used in Evosun or an empty String if unknown
 */
QString NeuralNetworkIO::getEvosunTransferFunctionNameFromNERD(QString nerdName) {
// TODO: ...
	if(nerdName == "tanh")
		return "tanh";
	return "";
}

/**
 * Returns the name of the TransferFunction used in SimbaV3 corresponding to the name used in NERD
 * @param nerdName the name of the TransferFunction used in NERD
 * @return the name of the TransferFunction used in SimbaV3 or an empty String if unknown
 */
QString NeuralNetworkIO::getSimbaV3TransferFunctionNameFromNERD(QString nerdName) {
// TODO: ...
	if(nerdName == "ASeriesTanh") {
		return "tanh";
	}

	return nerdName;
}

/**
 * Returns the name of the ActivationFunction used in SimbaV3 corresponding to the name used in NERD
 * @param nerdName the name of the ActivationFunction used in NERD
 * @return the name of the ActivationFunction used in SimbaV3 or an empty String if unknown
 */
QString NeuralNetworkIO::getSimbaV3ActivationFunctionNameFromNERD(QString nerdName) {
// TODO: ...
	if(nerdName == "ASeries") {
		return "AdditiveTD";
	}

	return nerdName;
}

/**
 * Returns the name of the SynapseFunction used in SimbaV3 corresponding to the name used in NERD
 * @param nerdName the name of the SynapseFunction used in NERD
 * @return the name of the SynapseFunction used in SimbaV3 or an empty String if unknown
 */
QString NeuralNetworkIO::getSimbaV3SynapseFunctionNameFromNERD(QString nerdName) {
	if(nerdName == "ASeries") {
		return "SimpleUpdateFunction";
	}

	return nerdName;
}

/**
 * Returns the name of the TransferFunction used in NERD corresponding to the name used in SimbaV3
 * @param simbaV3Name the name of the SynapseFunction used in SimbaV3
 * @return the name of the SynapseFunction used in NERD or an empty String if unknown
 */
QString NeuralNetworkIO::getNERDTransferFunctionNameFromSimbaV3(QString simbaV3Name) {
// TODO: ...
//	if(simbaV3Name.compare("tanh") == 0) {
//		return "tanh";
//	}
//	if(simbaV3Name.compare("sigmoid") == 0) {
//		return "sigmoid";
//	}
	return simbaV3Name;
}

/**
 * Returns the name of the ActivationFunction used in NERD corresponding to the name used in SimbaV3
 * @param simbaV3Name the name of the ActivationFunction used in SimbaV3
 * @return the name of the ActivationFunction used in NERD or an empty String if unknown
 */
QString NeuralNetworkIO::getNERDActivationFunctionNameFromSimbaV3(QString simbaV3Name) {
	return simbaV3Name;
}

/**
 * Returns the name of the SynapseFunction used in NERD corresponding to the name used in SimbaV3
 * @param simbaV3Name the name of the SynapseFunction used in SimbaV3
 * @return the name of the SynapseFunction used in NERD or an empty String if unknown
 */
QString NeuralNetworkIO::getNERDSynapseFunctionNameFromSimbaV3(QString simbaV3Name) {
	return simbaV3Name;
}

/**
 * Returns the number of the neuron in the NeuronList of the NeuralNetwork.
 *
 * @param net the NeuralNetwork.
 * @param neuron the Neuron.
 * @return the number of the neuron or a negative number if something went wrong
 */
int NeuralNetworkIO::getNeuronNumber(NeuralNetwork *net, Neuron *neuron) {
	if(net == NULL || neuron == NULL) {
		Core::log("NeuralNetworkIO::getNeuronNumber : net == NULL or neuron == NULL!");
		return -2;
	}

	QList<Neuron*> neurons = net->getNeurons();

	for(int i = 0; i < neurons.size(); ++i) {
		if(neuron == neurons.at(i)) {
			return i;
		}
	}
	return -1;
}

}
