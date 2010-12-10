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

#ifndef NERDNeuralNetworkIOSimbaV3Xml_H
#define NERDNeuralNetworkIOSimbaV3Xml_H

#include <QString>
#include <QDomElement>
#include "Network/NeuralNetwork.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Synapse.h"

namespace nerd {

	/**
	 * NeuralNetworkIOSimbaV3Xml
	 */
	class NeuralNetworkIOSimbaV3Xml {
	public:
		static bool createFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg);
		static NeuralNetwork* createNetFromFile(QString fileName, QString *errorMsg, QList<QString> *warnings);
		static ModularNeuralNetwork* createASeriesNetFromFile(QString fileName, QString *errorMsg, QList<QString> *warnings);
		static bool createASeriesFileFromNetwork(QString fileName, ModularNeuralNetwork *modNet, QString *errorMsg);
	private:
		static NeuralNetwork* copyNeuronsAndSynapsesToNewNet(ModularNeuralNetwork *modNet, QString *errorMsg);
		static void mapInputAndOutputNeuronsToSimbaNames(ModularNeuralNetwork *modNet);
		static void prepareHiddenNeuronNames(ModularNeuralNetwork *modNet);
		static bool transferNeuronsAndSynapses(NeuralNetwork *net, ModularNeuralNetwork *modNet, QString *errorMsg, QList<QString> *warnings);
		static bool createAccelboardModulesAndAssignNeurons(ModularNeuralNetwork *modNet, QString *errorMsg);
		static bool assignSpinalChordAddressesToNeurons(ModularNeuralNetwork *modNet, QString *errorMsg);
		static bool moduleAddressAndMemoryCheck(ModularNeuralNetwork *modNet, QString *errorMsg);
		static void mapNeuronNames(ModularNeuralNetwork *modNet);

		static NeuralNetwork* createNetFromNetElement(const QDomElement &netElement, QString *errorMsg, QList<QString> *warnings);
		static void addDefaultTransferFunctionToNetwork(NeuralNetwork *net, const QDomElement &netElement, NeuralNetworkManager *neuralNetworkManager, QList<QString> *warnings);
		static void addDefaultActivationFunctionToNetwork(NeuralNetwork *net, const QDomElement &netElement, NeuralNetworkManager *neuralNetworkManager, QList<QString> *warnings);
		static void addDefaultSynapseFunctionToNetwork(NeuralNetwork *net, const QDomElement &netElement, NeuralNetworkManager *neuralNetworkManager, QList<QString> *warnings);
		static void addNeuronsAndSynapsesToNetwork(NeuralNetwork *net, const QDomElement &netElement, NeuralNetworkManager *neuralNetworkManager, QList<QString> *warnings);
		static void addSynapsesToNetwork(QVector<Neuron*> neurons, SynapseTarget *target, const QDomNodeList &synapseList, const QDomNode &currentNode, NeuralNetworkManager *neuralNetworkManager, QList<QString> *warnings);
		static void addSynapseToFile(QDomDocument &doc, NeuralNetwork *net, Synapse *synapse, QDomElement &node);	
	};
}

#endif
