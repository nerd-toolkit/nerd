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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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

#include "NeuralNetworkIONerdV1Xml.h"
#include "Network/Neuro.h"
#include "Constraints/Constraints.h"
#include "NeuralNetworkIO.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "SynapseFunction/SynapseFunction.h"
#include "Network/Neuron.h"
#include "Core/Core.h"
#include "Value/Value.h"
#include <QDir>
#include <QTextStream>
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Constraints/GroupConstraint.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include <iostream>
#include "NeuralNetworkConstants.h"


using namespace std;

namespace nerd {

QString NeuralNetworkIONerdV1Xml::createXmlFromNetwork(NeuralNetwork *net) {

	if(net == 0) {
		return "";
	}

	QDomDocument doc;

	{
		//Add processing header
		QDomProcessingInstruction header = doc.createProcessingInstruction(
					"xml", "version=\"1.0\" encoding=\"UTF-8\"");
		doc.appendChild(header); 
	}

	//Add root net tag and version information.
	QDomElement netRoot = doc.createElement("net");
	netRoot.setAttribute("version", "NERD XML V1.0");
	doc.appendChild(netRoot);
	addPropertiesToXML(doc, *net, netRoot);

	{
		//store default TransferFunction and its parameters.
		TransferFunction *defaultTf = net->getDefaultTransferFunction();
	
		QDomElement defaultTfElem = doc.createElement("defaultTransferFunction");
		defaultTfElem.setAttribute("name", defaultTf->getName());
		netRoot.appendChild(defaultTfElem);

		addParametersToXML(doc, *defaultTf, defaultTfElem);
		
	}

	{
		//store default ActivationFunction and its parameters.
		ActivationFunction *defaultAf = net->getDefaultActivationFunction();

		QDomElement defaultAfElem = doc.createElement("defaultActivationFunction");
		defaultAfElem.setAttribute("name", defaultAf->getName());
		netRoot.appendChild(defaultAfElem);

		addParametersToXML(doc, *defaultAf, defaultAfElem);
	}


	//store default SynapseFunction and its parameters.
	{
		SynapseFunction *defaultSf = net->getDefaultSynapseFunction();

		QDomElement defaultSfElem = doc.createElement("defaultSynapseFunction");
		defaultSfElem.setAttribute("name", defaultSf->getName());
		netRoot.appendChild(defaultSfElem);

		addParametersToXML(doc, *defaultSf, defaultSfElem);
	}

	//store neurons
	QList<Neuron*> neurons = net->getNeurons();
	{
		storeNeuronsToXML(neurons, doc, netRoot);
	}

	ModularNeuralNetwork *modularNet = dynamic_cast<ModularNeuralNetwork*>(net);

	if(modularNet != 0) {

		QList<NeuronGroup*> availableGroups = modularNet->getNeuronGroups();

		{
			storeGroupsAndModulesToXML(availableGroups, doc, netRoot);
		}

	}

	return doc.toString(4);

}

NeuralNetwork* NeuralNetworkIONerdV1Xml::createNetFromXml(
				QString xml, QString *errorMsg, QList<QString> *warnings)
{
	QDomDocument doc;

	{
		QString errorStr;
		int errorLine, errorColumn;
	
		if(!doc.setContent(xml, &errorStr, &errorLine, &errorColumn)) {
			if(errorMsg != 0) {
				*errorMsg = QString("Parse error at line %1, column %2:\n%3")
									.arg(errorLine)
									.arg(errorColumn)
									.arg(errorStr);
			}
			return 0;
		}
	}


	QDomElement netElement;
	QDomNodeList nodeList = doc.elementsByTagName("net");

	//Select the first <net ...> node available and ignore all others.
	for(uint i = 0; i < nodeList.length(); ++i) {
		QDomNode node = nodeList.item(i);
		if(node.isElement()) {
			netElement = node.toElement();
			break;
		}
	}
	if(netElement.isNull()) {
		if(errorMsg != 0) {
			*errorMsg = "Root element <net> could not be found!";
		}
		return 0;
	}

	QString version = netElement.attribute("version");
	if(version != "ORCS XML V1.0" && version != "NERD XML V1.0") {
		if(errorMsg != NULL) {
			*errorMsg = QString("Version number [").append(version)
						.append("] was not NERD XML V1.0!");
		}
		return 0;
	}


	//Reference required managers
	NeuralNetworkManager *networkManager = Neuro::getNeuralNetworkManager();
	ConstraintManager *constraintManager = Constraints::getConstraintManager();

	if(networkManager == 0 || constraintManager == 0) {
		*errorMsg = "Could not find all required managers "
					"(NeuralNetworkManager, ConstraintManager).";
		return 0;
	}

	//Determine default functions
	TransferFunction *defaultTransferFunction = createTransferFunctionFromXML(
			getFirstDomElement(netElement, "defaultTransferFunction"), warnings);

	ActivationFunction *defaultActivationFunction = createActivationFunctionFromXML(
			getFirstDomElement(netElement, "defaultActivationFunction"), warnings);

	SynapseFunction *defaultSynapseFunction = createSynapseFunctionFromXML(
			getFirstDomElement(netElement, "defaultSynapseFunction"), warnings);

	
	//create network
	ModularNeuralNetwork *net = new ModularNeuralNetwork(*defaultActivationFunction,
				*defaultTransferFunction, *defaultSynapseFunction);

	//add network properties
	addPropertiesFromXML(netElement, *net, warnings);

	//create neurons
	QList<QDomElement> neuronElements;
	getDomElements(netElement, "neuron", neuronElements);

	//Store all available synapses for later use.
	QList<Synapse*> availableSynapses;

	for(QListIterator<QDomElement> i(neuronElements); i.hasNext();) {
		QDomElement neuronElement = i.next();

		QString idString = neuronElement.attribute("id");
		QString biasString = neuronElement.attribute("bias");
		QString name = neuronElement.attribute("name");

		//parse id
		bool ok = true;
		qulonglong id = idString.toULongLong(&ok);
		if(!ok) {
			if(warnings != 0) {
				warnings->append(QString("Could not read neuron id (Line ")
					.append(QString::number(neuronElement.lineNumber()))
					.append(") [SKIPPING]"));
			}
			continue;
		}

		//get transferfunction and activationfunction
		TransferFunction *tf = createTransferFunctionFromXML(
				getFirstDomElement(neuronElement, "transferFunction"), warnings);

		ActivationFunction *af = createActivationFunctionFromXML(
				getFirstDomElement(neuronElement, "activationFunction"), warnings);

		Neuron *neuron = new Neuron(name, *tf, *af, id);
		delete tf;
		delete af;

		//set bias
		neuron->getBiasValue().setValueFromString(biasString);

		//add properties
		addPropertiesFromXML(neuronElement, *neuron, warnings);

		//add synapses
		QList<Synapse*> synapses = createSynapsesFromXML(neuronElement, warnings);
		for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
			Synapse *synapse = j.next();
			if(!synapse->hasProperty("__target")) {
				synapse->setProperty("__target", idString);
			}
			availableSynapses.append(synapse);
		}

		
		net->addNeuron(neuron);
	}


	//connect synapses to their source neurons
	{
		QList<Neuron*> neurons = net->getNeurons();
		QList<Synapse*> synapses = availableSynapses;
		QList<Synapse*> synapsesToRemove;

		for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
			Synapse *synapse = i.next();
			bool ok = true;
			qulonglong id = synapse->getProperty("__source").toULongLong(&ok);
			synapse->removeProperty("__source");

			if(!ok) {
				if(warnings != 0) {
					warnings->append(QString("Could not parse source neuron id of synapse (id=")
							.append(QString::number(synapse->getId())).append(") [REMOVING]"));
				}
				synapsesToRemove.append(synapse);
				continue;
			}
			
			Neuron *source = NeuralNetwork::selectNeuronById(id, neurons);

			if(source == 0) {
				if(warnings != 0) {
					warnings->append(QString("Could not find the source neuron with id=")
							.append(QString::number(id)).append(" of synapse (id=")
							.append(QString::number(synapse->getId())).append(") [REMOVING]"));
				}
				synapsesToRemove.append(synapse);
				continue;
			}

			synapse->setSource(source);
		}
		for(QListIterator<Synapse*> i(synapsesToRemove); i.hasNext();) {
			Synapse *synapse = i.next();
			availableSynapses.removeAll(synapse);
			delete synapse; //TODO check how to ensure that this is save.
		}
	}

	//set targets
	{
		QList<Synapse*> synapses = availableSynapses;
		QList<Neuron*> neurons = net->getNeurons();

		for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
			Synapse *synapse = i.next();
			
			QString idString = synapse->getProperty("__target");
			synapse->removeProperty("__target");

			bool ok = true;
			qulonglong id = idString.toULongLong(&ok);
		
			if(!ok) {
				if(warnings != 0) {
					warnings->append(QString("Could not parse synapse target id [")
							.append(idString).append("] "));
				}
				continue;
			}

			SynapseTarget *target = NeuralNetwork::selectNeuronById(id, neurons);
			if(target == 0) {
				target = NeuralNetwork::selectSynapseById(id, synapses);
			}

			if(target == 0) {
				if(warnings != 0) {
					warnings->append(QString("Could not find synapseTarget with id [")
							.append(idString).append("] for synapse [")
							.append(QString::number(synapse->getId())).append("]!"));
				}
				continue;
			}

			target->addSynapse(synapse);
		}
	}

	{
		//create modules
		createGroupsAndModulesFromXML(netElement, "module", net, warnings);

		//create groups
		createGroupsAndModulesFromXML(netElement, "group", net, warnings);
	}


	delete defaultTransferFunction;
	delete defaultActivationFunction;
	delete defaultSynapseFunction;

	net->adjustIdCounter();

	return net;
}


QString NeuralNetworkIONerdV1Xml::createXmlFromNeuroModule(NeuroModule *module) {
	if(module == 0) {
		return "";
	}

	QDomDocument doc;

	{
		//Add processing header
		QDomProcessingInstruction header = doc.createProcessingInstruction(
					"xml", "version=\"1.0\" encoding=\"UTF-8\"");
		doc.appendChild(header); 
	}

	//Add root net tag and version information.
	QDomElement netRoot = doc.createElement("module");
	netRoot.setAttribute("version", "NERD XML V1.0");
	netRoot.setAttribute("root", QString::number(module->getId()));
	doc.appendChild(netRoot);


	//store neurons
	QList<Neuron*> neurons = module->getAllEnclosedNeurons();
	{
		storeNeuronsToXML(neurons, doc, netRoot);
	}

	{
		QList<NeuronGroup*> availableGroups;
		availableGroups.append(module);

		QList<NeuroModule*> submodules = module->getAllEnclosedModules();
		for(QListIterator<NeuroModule*> i(submodules); i.hasNext();) {
			availableGroups.append(i.next());
		}

		storeNeuronsToXML(neurons, doc, netRoot);

	}

	return doc.toString(4);
}


NeuroModule* NeuralNetworkIONerdV1Xml::createNeuroModuleFromXml(const QString &xml, 
				QString *errorMsg, QList<QString> *warnings)
{
	ModularNeuralNetwork *net = dynamic_cast<ModularNeuralNetwork*>(
					createNetFromXml(xml, errorMsg, warnings));

	if(net == 0)  {
		return 0;
	}


	QDomDocument doc;

	{
		if(!doc.setContent(xml)) {
			return 0;
		}
	}

	QDomElement netElement;
	QDomNodeList nodeList = doc.elementsByTagName("net");

	//Select the first <net ...> node available and ignore all others.
	for(uint i = 0; i < nodeList.length(); ++i) {
		QDomNode node = nodeList.item(i);
		if(node.isElement()) {
			netElement = node.toElement();
			break;
		}
	}
	if(netElement.isNull()) {
		return 0;
	}

	QString type = netElement.attribute("type");
	if(type != "module") {
		if(errorMsg != 0) {
			*errorMsg = QString("XML was not a NeuroModule");
		}
		return 0;
	}
	QString rootId = netElement.attribute("root");
	bool ok = false;
	qulonglong id = rootId.toULongLong(&ok);
	if(!ok) {
		if(errorMsg != 0) {
			*errorMsg = QString("Could not find root id of NeuroModule");
		}
		return 0;
	}

	NeuroModule *module = ModularNeuralNetwork::selectNeuroModuleById(
								id, net->getNeuroModules());

	if(module == 0) {
		if(errorMsg != 0) {
			*errorMsg = QString("Could not find the specified root NeuroModule");
		}
		return 0;
	}

	//detach module, neurons and submodules from network
	net->removeNeuronGroup(module);
	delete net;
	
	return module;
}

void NeuralNetworkIONerdV1Xml::addParametersToXML(QDomDocument &doc, 
						ParameterizedObject &obj, QDomElement &xmlElement) 
{

	QList<QString> parameterNames = obj.getParameterNames();
	for(int i = 0; i < parameterNames.size(); ++i) {

		QString name = parameterNames.at(i);
		Value* value = obj.getParameter(name);
		
		if(value == 0) {
			continue;
		}

		QDomElement param = doc.createElement("param");
		param.setAttribute("name", name);
		param.setAttribute("value", value->getValueAsString());

		xmlElement.appendChild(param);
	}
}

void NeuralNetworkIONerdV1Xml::addPropertiesToXML(QDomDocument &doc, 
						Properties &obj, QDomElement &xmlElement) 
{

	QList<QString> propertyNames = obj.getPropertyNames();

	if(propertyNames.empty()) {
		return;
	}

	QDomElement propSection = doc.createElement("properties");
	xmlElement.appendChild(propSection);

	for(int i = 0; i < propertyNames.size(); ++i) {
		
		QString name = propertyNames.at(i);
		
		QDomElement prop = doc.createElement("prop");
		prop.setAttribute("name", name);
		prop.setAttribute("value", obj.getProperty(name));

		propSection.appendChild(prop);
	}

}


void NeuralNetworkIONerdV1Xml::addSynapseToXML(QDomDocument &doc, 
						Synapse &synapse, QDomElement &xmlElement) 
{	
	if(synapse.getSource() == 0 || synapse.getSynapseFunction() == 0) {
		return;
	}

	QDomElement synapseElem = doc.createElement("synapse");
	xmlElement.appendChild(synapseElem);

	synapseElem.setAttribute("source", QString::number(synapse.getSource()->getId()));
	synapseElem.setAttribute("strength", synapse.getStrengthValue().getValueAsString());
	synapseElem.setAttribute("id", synapse.getId());
	if(!synapse.getEnabledValue().get()) {
		synapseElem.setAttribute("enabled", "false");
	}
	
	{
		//store properties
		addPropertiesToXML(doc, synapse, synapseElem);
	}
	{
		//store current SynapseFunction of the neuron and its parameters.
		SynapseFunction *sf = synapse.getSynapseFunction();

		QDomElement sfElem = doc.createElement("synapseFunction");
		sfElem.setAttribute("name", sf->getName());
		synapseElem.appendChild(sfElem);
		
		addParametersToXML(doc, *sf, sfElem);
	}

	QList<Synapse*> synapses = synapse.getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		addSynapseToXML(doc, *(i.next()), synapseElem);
	}
}


void NeuralNetworkIONerdV1Xml::getDomElements(const QDomElement &parent, 
						const QString &elementName, QList<QDomElement> &container) 
{
	QDomNodeList domNodes = parent.childNodes();

	for(uint i = 0; i < domNodes.length(); ++i) {
		QDomNode node = domNodes.item(i);
		if(node.isElement() && node.nodeName() == elementName) {
			container.append(node.toElement());
		}
	}
}

QDomElement NeuralNetworkIONerdV1Xml::getFirstDomElement(const QDomElement &parent, 
				const QString &elementName) 
{
	//QDomNodeList domNodes = parent.elementsByTagName(elementName);
	QDomNodeList domNodes = parent.childNodes();

	for(uint i = 0; i < domNodes.length(); ++i) {
		QDomNode node = domNodes.item(i);
		if(node.isElement() && node.nodeName() == elementName) {
			return node.toElement();
		}
	}
	return QDomElement();
}

void NeuralNetworkIONerdV1Xml::addParametersFromXML(const QDomElement &parent, 
					ParameterizedObject &obj, QList<QString> *warnings) 
{

	QList<QDomElement> params;
	getDomElements(parent, "param", params);

	for(QListIterator<QDomElement> i(params); i.hasNext();) {
		QDomElement elem = i.next();
		QString name = elem.attribute("name");
		QString valueContent = elem.attribute("value");

		Value *value = obj.getParameter(name);

		if(value == 0) {
			if(warnings != 0) {
				warnings->append(QString("Could not find parameter [").append(name)
						.append("] (Line ").append(QString::number(elem.lineNumber()))
						.append(", Column ").append(QString::number(elem.columnNumber()))
						.append(")! [IGNORING]"));
			}
		}
		else {
			if(!value->setValueFromString(valueContent) && warnings != 0) {
				warnings->append(QString("Could not apply content [")
						.append(valueContent).append("] to parameter [")
						.append(name).append("] (Line ").append(QString::number(elem.lineNumber()))
						.append(", Column ").append(QString::number(elem.columnNumber()))
						.append(")!"));
			}
		}
	}
}


void NeuralNetworkIONerdV1Xml::addPropertiesFromXML(const QDomElement &parent, Properties &obj, 
										QList<QString> *warnings)
{
	QDomElement propSection = getFirstDomElement(parent, "properties");

	if(propSection.isNull()) {
		return;
	}

	QList<QDomElement> props;
	getDomElements(propSection, "prop", props);

	for(QListIterator<QDomElement> i(props); i.hasNext();) {
		QDomElement elem = i.next();
		QString name = elem.attribute("name");
		QString content = elem.attribute("value");

		//Note: This is to allow the loading of old network XML where the location tags 
		//had a different name.
		if(name == "Location") {
			name = NeuralNetworkConstants::TAG_ELEMENT_LOCATION;
		}
		else if(name == "LocationSize") {
			name = NeuralNetworkConstants::TAG_MODULE_SIZE;
		}

		if(name == "") {
			if(warnings != 0) {
				warnings->append(QString("Found invalid property name in line ")
							.append(QString::number(elem.lineNumber())).append("."));
			}
			continue;
		}

		obj.setProperty(name, content);
	}
}

QList<Synapse*> NeuralNetworkIONerdV1Xml::createSynapsesFromXML(
				const QDomElement &parent, QList<QString> *warnings) 
{
	QList<Synapse*> synapses;

	QList<QDomElement> synapseElements;
	getDomElements(parent, "synapse", synapseElements);

	for(QListIterator<QDomElement> i(synapseElements); i.hasNext();) {
		QDomElement synapseElement = i.next();

		QString idString = synapseElement.attribute("id");
		QString strengthString = synapseElement.attribute("strength");
		QString sourceIdString = synapseElement.attribute("source");
		QString enabled = synapseElement.attribute("enabled");

		//parse id
		bool ok = true;
		qulonglong id = idString.toULongLong(&ok);
		if(!ok) {
			if(warnings != 0) {
				warnings->append(QString("Could not read synapse id (Line ")
					.append(QString::number(synapseElement.lineNumber()))
					.append(") [SKIPPING]"));
			}
			continue;
		}
		
		//parse strength
		double strength = strengthString.toDouble(&ok);
		if(!ok) {
			if(warnings != 0) {
				warnings->append(QString("Could not read synapse strength (Line ")
					.append(QString::number(synapseElement.lineNumber()))
					.append(") [IGNORING]"));
			}
			strength = 0.0;
		}

		//get synapsefunction
		SynapseFunction *sf = createSynapseFunctionFromXML(
				getFirstDomElement(synapseElement, "synapseFunction"), warnings);

		Synapse *synapse = new Synapse(0, 0, strength, *sf, id);
		delete sf;

		//add parameters
		addPropertiesFromXML(synapseElement, *synapse, warnings);

		//add id markers for the source
		synapse->setProperty("__source", sourceIdString);

		//check for own synapses
		QList<Synapse*> ownSynapses = createSynapsesFromXML(synapseElement, warnings);

		for(QListIterator<Synapse*> j(ownSynapses); j.hasNext();) {
			Synapse *synapse = j.next();
			if(!synapse->hasProperty("__target")) {
				synapse->setProperty("__target", idString);
			}
			synapses.append(synapse);
		}

		//check if this synapse is disabled
		if(enabled != "") {
			synapse->getEnabledValue().setValueFromString(enabled);
		}

		synapses.append(synapse);
	}
	return synapses;
}


void NeuralNetworkIONerdV1Xml::createGroupsAndModulesFromXML(
				const QDomElement &parent, const QString &type, 
				ModularNeuralNetwork *net, QList<QString> *warnings)
{
	bool createModules = (type == "module") ? true : false;

	QList<Neuron*> neurons = net->getNeurons();

	QList<QDomElement> groupElements;
	getDomElements(parent, type, groupElements);

	QList<NeuroModule*> neuroModules;
	QHash<NeuronGroup*, QList<qulonglong>*> subModuleIds;

	for(QListIterator<QDomElement> i(groupElements); i.hasNext();) {
		QDomElement groupElement = i.next();

		QString idString = groupElement.attribute("id");
		QString name = groupElement.attribute("name");
		
		bool ok = true;
		qulonglong id = idString.toULongLong(&ok);
	
		if(!ok) {
			if(warnings != 0) {
				warnings->append(QString("Could not parse group/module id [")
						.append(idString).append("]  (Line ")
							.append(QString::number(groupElement.lineNumber()))
							.append(") [REMOVING]"));
			}
			continue;
		}

		//create group / module
		NeuronGroup *group = 0;
		if(createModules) {
			NeuroModule *n = new NeuroModule(name, id);
			neuroModules.append(n);
			group = n;
		}
		else {
			if(name == "Default") {
				group = net->getDefaultNeuronGroup();
				if(group != 0) {
					group->setId(id);
				}
			}
			if(group == 0) {
				group = new NeuronGroup(name, id);
			}
		}

		//add properties
		addPropertiesFromXML(groupElement, *group, warnings);

		//add neurons
		QList<QDomElement> neuronIdElements;
		getDomElements(getFirstDomElement(groupElement, "members"), "member", neuronIdElements);

		for(QListIterator<QDomElement> j(neuronIdElements); j.hasNext();) {
			QDomElement neuronIdElement = j.next();

			QString type = neuronIdElement.attribute("type");

			qulonglong id = neuronIdElement.attribute("id").toULongLong(&ok);
	
			if(!ok) {
				if(warnings != 0) {
					warnings->append(QString("Could not parse neuron id in group [")
							.append(idString).append("] (Line ")
							.append(QString::number(neuronIdElement.lineNumber()))
							.append(") [REMOVING]"));
				}
				continue;
			}

			Neuron *neuron = NeuralNetwork::selectNeuronById(id, neurons);

			if(neuron == 0) {
				if(warnings != 0) {
					warnings->append(QString("Could not find the member neuron with id=")
							.append(QString::number(id)).append(" of group (id=")
							.append(QString::number(group->getId())).append(") [IGNORING]"));
				}
				continue;
			}

			group->addNeuron(neuron);
		}
// 
// 		{
// 			if(createModules) {
// 			NeuroModule *module = dynamic_cast<NeuroModule*>(group);
		{
			//adds submodule ids
			QList<QDomElement> subModuleElements;
			getDomElements(getFirstDomElement(groupElement, "subModules"), 
							"subModule", subModuleElements);

			QList<qulonglong> *idList = new QList<qulonglong>();
			subModuleIds.insert(group, idList);

			for(QListIterator<QDomElement> j(subModuleElements); j.hasNext();) {
				QDomElement subModuleIdElement = j.next();

				bool ok = false;
				qulonglong id = subModuleIdElement.attribute("id").toULongLong(&ok);
		
				if(!ok) {
					if(warnings != 0) {
						warnings->append(QString("Could not parse submodule id in group [")
								.append(idString).append("] (Line ")
								.append(QString::number(subModuleIdElement.lineNumber()))
								.append(") [REMOVING]"));
					}
					continue;
				}

				idList->append(id);
			}
		}

		{
			//add constraints
			ConstraintManager *constraintManager = Constraints::getConstraintManager();

			QList<QDomElement> constraintElements;
			getDomElements(getFirstDomElement(groupElement, "constraints"), 
							"constraint", constraintElements);

			for(QListIterator<QDomElement> c(constraintElements); c.hasNext();) {
				QDomElement constraintElement = c.next();

				QString name = constraintElement.attribute("name");
				QString idString = constraintElement.attribute("id");

				bool ok = true;
				qlonglong id = idString.toULongLong(&ok);
				if(!ok) {
					if(warnings != 0) {
						warnings->append(QString("Could not read constraint id (Line ")
							.append(QString::number(constraintElement.lineNumber()))
							.append(") [SKIPPING]"));
					}
					continue;
				}

				GroupConstraint *constraintPrototype = 
						constraintManager->getConstraintPrototype(name);

				if(constraintPrototype == 0) {
					if(warnings != 0) {
						warnings->append(QString("Could not find constraint prototype with name [")
								.append(name).append("] (Line ")
								.append(QString::number(constraintElement.lineNumber()))
								.append(") [IGNORING]"));
					}
					continue;
				}

				GroupConstraint *constraint = constraintPrototype->createCopy();
				constraint->setId(id);

				addParametersFromXML(constraintElement, *constraint, warnings);

				group->addConstraint(constraint);
			}
		}
		if(!net->addNeuronGroup(group) && group != net->getDefaultNeuronGroup()) {
			warnings->append(QString("Could not add neuron module with name [")
							.append(group->getName())
							.append("] to net. At least one of the neurons is a member of another module. "
									" [IGNORING]"));
		} 
	}

	//set pointers to submodules
// 	if(createModules) {
	{
		QList<NeuroModule*> availableModules = net->getNeuroModules();

		for(QHash<NeuronGroup*, QList<qulonglong>*>::iterator i = subModuleIds.begin(); 
			i != subModuleIds.end(); ++i) 
		{
			NeuronGroup *currentGroup = i.key();
			QList<qulonglong> *ids = i.value();
	
			for(QListIterator<qulonglong> i(*ids); i.hasNext();) {
				qulonglong id = i.next();
				NeuroModule *subModule = ModularNeuralNetwork::selectNeuroModuleById(id, availableModules);

				if(subModule == 0) {
					warnings->append(QString("Could not find a submodule with id [")
							.append(QString::number(id)).append("] for module [")
							.append(currentGroup->getName())
							.append("]. [IGNORING]"));
					continue;
				}

				//remove from network to allow it being added.
				//net->removeNeuronGroup(subModule);
				currentGroup->addSubModule(subModule);
			}
		}
	}
	//destroy id containers
	QList<QList<qulonglong>*> idList = subModuleIds.values();
	while(!idList.isEmpty()) {
		QList<qulonglong> *list = idList.first();
		idList.removeAll(list);
		delete list;
	}
}

TransferFunction* NeuralNetworkIONerdV1Xml::createTransferFunctionFromXML(
				const QDomElement &parent, QList<QString> *warnings)
{
	QList<TransferFunction*> tfPrototypes =
			Neuro::getNeuralNetworkManager()->getTransferFunctionPrototypes();

	TransferFunction *transferFunction = 0;

	if(parent.isNull()) {
		if(warnings != 0) {
			warnings->append(QString("There was no TransferFunction specification as required. [")
					.append(parent.tagName()).append("]"));
		}
		return new TransferFunctionTanh(); //this is the default.
	}

	QString name = parent.attribute("name");

	for(QListIterator<TransferFunction*> i(tfPrototypes); i.hasNext();) {
		TransferFunction *tf = i.next();
		if(tf->getName() == name) {
			transferFunction = tf->createCopy();
			break;
		}
	}
	if(transferFunction == 0) {
		if(warnings != 0) {
			warnings->append(QString("Unknown name [").append(name)
					.append("] of TransferFunction.!"));
			transferFunction = new TransferFunctionTanh();
		}
	}
	else {
		addParametersFromXML(parent, *transferFunction, warnings);
	}
	return transferFunction;
}

ActivationFunction* NeuralNetworkIONerdV1Xml::createActivationFunctionFromXML(
						const QDomElement &parent, QList<QString> *warnings)
{
	QList<ActivationFunction*> afPrototypes =
			Neuro::getNeuralNetworkManager()->getActivationFunctionPrototypes();

	ActivationFunction *activationFunction = 0;

	if(parent.isNull()) {
		if(warnings != 0) {
			warnings->append(QString("There was no ActivationFunction specification as required. [")
					.append(parent.tagName()).append("]"));
		}
		return new AdditiveTimeDiscreteActivationFunction();
	}

	QString name = parent.attribute("name");

	for(QListIterator<ActivationFunction*> i(afPrototypes); i.hasNext();) {
		ActivationFunction *af = i.next();
		if(af->getName() == name) {
			activationFunction = af->createCopy();
			break;
		}
	}
	if(activationFunction == 0) {
		if(warnings != 0) {
			warnings->append(QString("Unknown name [").append(name)
					.append("] of ActivationFunction. (Line ")
					.append(QString::number(parent.lineNumber())).append(") !"));
			activationFunction = new AdditiveTimeDiscreteActivationFunction();
		}
	}
	else {
		addParametersFromXML(parent, *activationFunction, warnings);
	}
	return activationFunction;
}


SynapseFunction* NeuralNetworkIONerdV1Xml::createSynapseFunctionFromXML(
						const QDomElement &parent, QList<QString> *warnings)
{
	QList<SynapseFunction*> sfPrototypes =
			Neuro::getNeuralNetworkManager()->getSynapseFunctionPrototypes();

	SynapseFunction *synapseFunction = 0;

	if(parent.isNull()) {
		if(warnings != 0) {
			warnings->append(QString("There was no SynapseFunction specification as required. [")
					.append(parent.tagName()).append("]"));
		}
		return new SimpleSynapseFunction();
	}

	QString name = parent.attribute("name");

	for(QListIterator<SynapseFunction*> i(sfPrototypes); i.hasNext();) {
		SynapseFunction *sf = i.next();
		if(sf->getName() == name) {
			synapseFunction = sf->createCopy();
			break;
		}
	}
	if(synapseFunction == 0) {
		if(warnings != 0) {
			warnings->append(QString("Unknown name [").append(name)
					.append("] of SynapseFunction.!"));
			synapseFunction = new SimpleSynapseFunction();
		}
	}
	else {
		addParametersFromXML(parent, *synapseFunction, warnings);
	}
	return synapseFunction;
}


void NeuralNetworkIONerdV1Xml::storeNeuronsToXML(QList<Neuron*> neurons,	
								QDomDocument &doc, QDomElement &netRoot) 
{
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron* neuron = i.next();
				
		QDomElement neuronElem = doc.createElement("neuron");
		netRoot.appendChild(neuronElem);

		neuronElem.setAttribute("id", neuron->getId());
		neuronElem.setAttribute("bias", neuron->getBiasValue().getValueAsString());
		neuronElem.setAttribute("name", neuron->getNameValue().getValueAsString());

		addPropertiesToXML(doc, *neuron, neuronElem);
				
		{
			//store TransferFunction of the neuron and its parameters.
			TransferFunction *tf = neuron->getTransferFunction();

			QDomElement tfElem = doc.createElement("transferFunction");
			tfElem.setAttribute("name", tf->getName());
			neuronElem.appendChild(tfElem);

			addParametersToXML(doc, *tf, tfElem);
		}

		{
			//store current ActivationFunction of the neuron and its parameters.
			ActivationFunction *af = neuron->getActivationFunction();

			QDomElement afElem = doc.createElement("activationFunction");
			afElem.setAttribute("name", af->getName());
			neuronElem.appendChild(afElem);
			
			addParametersToXML(doc, *af, afElem);
		}
		
		QList<Synapse*> synapses = neuron->getSynapses();
		for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
			addSynapseToXML(doc, *(j.next()), neuronElem);
		}
	}
}


void NeuralNetworkIONerdV1Xml::storeGroupsAndModulesToXML(
								QList<NeuronGroup*> availableGroups,
								QDomDocument &doc, QDomElement &netRoot)
{
	//add groups and modules
	for(QListIterator<NeuronGroup*> i(availableGroups); i.hasNext();) {
		NeuronGroup *group = i.next();
		NeuroModule *module = dynamic_cast<NeuroModule*>(group);


		QDomElement groupElem;

		if(module == 0) {
			groupElem = doc.createElement("group");
		}
		else {
			groupElem = doc.createElement("module");
		}
		groupElem.setAttribute("name", group->getName());
		groupElem.setAttribute("id", group->getId());
		netRoot.appendChild(groupElem);

		//add properties of group
		addPropertiesToXML(doc, *group, groupElem);

		//add member neurons
		QList<Neuron*> neurons = group->getNeurons();

		QDomElement neuronSection = doc.createElement("members");
		groupElem.appendChild(neuronSection);

		for(QListIterator<Neuron*> j(neurons); j.hasNext();) {
			Neuron *neuron = j.next();

			QDomElement neuronElem = doc.createElement("member");
			neuronElem.setAttribute("id", neuron->getId());

			neuronSection.appendChild(neuronElem);
		}

		//add submodules
// 		if(module != 0) {
		{
			QList<NeuroModule*> subModules = group->getSubModules();

			QDomElement subModuleSection = doc.createElement("subModules");
			groupElem.appendChild(subModuleSection);

			if(!subModules.empty()) {

				for(QListIterator<NeuroModule*> j(subModules); j.hasNext();) {
					NeuroModule *subModule = j.next();

					QDomElement subModuleElem = doc.createElement("subModule");
					subModuleElem.setAttribute("id", subModule->getId());	
					subModuleSection.appendChild(subModuleElem);
				}
			}
		}

		//add constraints
		QList<GroupConstraint*> constraints = group->getConstraints();

		if(!constraints.empty()) {
			QDomElement constraintSection = doc.createElement("constraints");
			groupElem.appendChild(constraintSection);

			for(QListIterator<GroupConstraint*> j(constraints); j.hasNext();) {
				GroupConstraint *constraint = j.next();

				QDomElement constraintElem = doc.createElement("constraint");
				constraintElem.setAttribute("name", constraint->getName());
				constraintElem.setAttribute("id", constraint->getId());
				constraintSection.appendChild(constraintElem);

				addParametersToXML(doc, *constraint, constraintElem);
			}
		}
	}
}

}


