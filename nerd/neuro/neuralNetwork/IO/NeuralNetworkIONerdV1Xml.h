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


#ifndef NERDNeuralNetworkIONerdV1Xml_H
#define NERDNeuralNetworkIONerdV1Xml_H

#include <QString>
#include <QList>
#include "Network/NeuralNetwork.h"
#include <QDomDocument>
#include "ModularNeuralNetwork/NeuronGroup.h"

namespace nerd {

	class NeuroModule;

	/**
	 * NeuralNetworkIONerdV1Xml.
	 */
	class NeuralNetworkIONerdV1Xml {
	public:

		static QString createXmlFromNetwork(NeuralNetwork *net);
		static NeuralNetwork* createNetFromXml(QString xml, QString *errorMsg, 
													QList<QString> *warnings);

		static QString createXmlFromNeuroModule(NeuroModule *module);
		static NeuroModule* createNeuroModuleFromXml(const QString &xml, QString *errorMsg,
													QList<QString> *warnings);
		

		static void addParametersToXML(QDomDocument &doc, ParameterizedObject &obj, 
										QDomElement &xmlElement);
		static void addPropertiesToXML(QDomDocument &doc, Properties &obj, QDomElement &xmlElement);

		static void addSynapseToXML(QDomDocument &doc, Synapse &synapse, QDomElement &xmlElement);
		


		static void getDomElements(const QDomElement &parent, const QString &elementName, 
										QList<QDomElement> &container);
		static QDomElement getFirstDomElement(const QDomElement &parent, const QString &elementName);


		static void addParametersFromXML(const QDomElement &parent, ParameterizedObject &obj, 
										QList<QString> *warnings);
		static void addPropertiesFromXML(const QDomElement &parent, Properties &obj, 
										QList<QString> *warnings);
		static QList<Synapse*> createSynapsesFromXML(const QDomElement &parent, QList<QString> *warnings);
		static void createGroupsAndModulesFromXML(const QDomElement &parent, 
										const QString &type, ModularNeuralNetwork *net, 
										QList<QString> *warnings);

		static TransferFunction* createTransferFunctionFromXML(const QDomElement &parent, 
										QList<QString> *warnings);
		static ActivationFunction* createActivationFunctionFromXML(const QDomElement &parent, 
										QList<QString> *warnings);
		static SynapseFunction* createSynapseFunctionFromXML(const QDomElement &parent, 
										QList<QString> *warnings);
	private:		
		static void storeNeuronsToXML(const QList<Neuron*> &neurons, 
										QDomDocument &doc, QDomElement &netRoot);
		static void storeGroupsAndModulesToXML(const QList<NeuronGroup*> &availableGroups,
										QDomDocument &doc, QDomElement &netRoot);
	};

}

#endif

