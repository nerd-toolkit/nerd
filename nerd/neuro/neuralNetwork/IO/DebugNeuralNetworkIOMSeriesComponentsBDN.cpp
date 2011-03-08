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

#include "DebugNeuralNetworkIOMSeriesComponentsBDN.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Core/Core.h"
#include "Math/Math.h"
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QDebug>
#include <QStringList>

#include <QDomDocument>
#include <QDomElement>

#include <limits> 
#include <iostream>

using namespace std; 

namespace nerd {
	
	
// Name of the synapse function which is used for the bias elements
const QString DebugNeuralNetworkIOMSeriesComponentsBDN::STANDARD_BIAS_SYNAPSE_TYPE = 	"SimpleUpdateFunction";

// Name of the property which holds the position of a neuron on the spinal cord
const QString DebugNeuralNetworkIOMSeriesComponentsBDN::STANDARD_POSITION_PROPERTY_NAME = "SPINAL_CORD_ADDRESS";

// Postfix of bytecodemappings for functions which can be flipped
const QString DebugNeuralNetworkIOMSeriesComponentsBDN::FLIP_TYPE = "_FLIP";
	
	
	
/**
 * Creates the mapping of the neuron and synapse types to the bytecode which is used 
 * by the M-Series to represet them.
 * 
 * This function must be adapted to new neuron or synapse types.
 * 
 * @return Table with mapping between neuron/synapse type and bytecode.
 */
QMap<QString, QString> DebugNeuralNetworkIOMSeriesComponentsBDN::createByteCodeMapping()
{
	QMap<QString, QString> byteCodeMapping;
	
	// Neurontypes
	// 
	// The type is defined by its TransferFunction and ActivationFunction name.
	// The format is: "<TransferFunctionName>;<ActivationFunctionName>"
	// 
	// If the function is flipable, then a further flip type must be provided.
	// Format: "<TransferFunctionName>;<ActivationFunctionName><DebugNeuralNetworkIOMSeriesComponentsBDN::FLIP_TYPE>"
	
	
	// tanh function and flipped function
	byteCodeMapping.insert("tanh;AdditiveTD", 			"300:\ntanh V0, Input\nwrite Output, V0");
	
	byteCodeMapping.insert(	"tanh;AdditiveTD" + DebugNeuralNetworkIOMSeriesComponentsBDN::FLIP_TYPE, 	
													"300:\ntanh V0, Input\nLOAD V1, -1.0\nMUL V1, V0, V1\nwrite Output, V1");
	
	
	// Synapsetypes
	// 
	// The type is defined by its SynapseFunction name.
	// The format is: "<SynapseFunctionName>"
	byteCodeMapping.insert("SimpleUpdateFunction", 	"200:\nmul   V0, Input, w\nwrite Output, V0");
	
	return byteCodeMapping;
}
	
	

/**
 * Try to export a neural network to the ASeries Bytecode.
 * 
 * @param fileName the file to write to.
 * @param net the net to export.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the function is successful, otherwise false.
 */
bool DebugNeuralNetworkIOMSeriesComponentsBDN::createFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg) {

	// check if network fullfills constraints for export
	if(checkIfNetworkIsExportable(net, errorMsg) == false){
		return false;
	}
	
	QFileInfo fileInfo(fileName);
	QString networkName = fileInfo.fileName().remove(".bdp");
	
		// try to open or create file
	QString code = exportNetwork(networkName, dynamic_cast<ModularNeuralNetwork*>(net), errorMsg);

	if(code == "" || code == QString::null) {
		return false;
	}

	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		if(errorMsg != NULL) {
			*errorMsg = QString("Cannot create file ").append(fileName).append(".");
		}
		file.close();
		return false;
	}
	file.write(code.toUtf8());
	file.close();

	return true;
}

/**
 * Checks, if a neural network is exportable to a M-Series Braindesigner Library.
 * @param net the net to check or to export.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the network is exportable, otherwise false.
 */
bool DebugNeuralNetworkIOMSeriesComponentsBDN::checkIfNetworkIsExportable(NeuralNetwork *net, QString *errorMsg) {
	
	// try to cast Network to ModularNeuralNetwork
	ModularNeuralNetwork *modNet = dynamic_cast<ModularNeuralNetwork*>(net);
	if(modNet == NULL) {
		*errorMsg = QString("Network has to be modular!");
		return false;
	}

	return true;
}

/**
 * Exports the given MSeries NeuralNetwork into a BrainDesigner Library.
 * @param networkName The name of the network to export.
 * @param net The net to export.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return A string with the BrainDesigner Library. In the case of an error QString::null will be returned.
 */
QString DebugNeuralNetworkIOMSeriesComponentsBDN::exportNetwork(QString networkName, ModularNeuralNetwork *net, QString *errorMsg)
{
	QHash<NeuralNetworkElement*, int> xmlIdTable;
	int nextXmlId = 0;
	
	// FILL HASHTABLE WITH ALL NEURON AND SYNAPSE ID'S /////////////
	
	// create XML ID's for neurons
	QList<Neuron*> netNeurons = net->getNeurons();
	QListIterator<Neuron*> netNeuronIt(netNeurons);
	while(netNeuronIt.hasNext()) {		
		Neuron *netNeuron = netNeuronIt.next();
		
		xmlIdTable.insert((NeuralNetworkElement*)netNeuron, nextXmlId);
		nextXmlId++;
	}
	
	// create XML ID's for synapses
	QList<Synapse*> netSynapses = net->getSynapses();
	QListIterator<Synapse*> netSynapseIt(netSynapses);
	while(netSynapseIt.hasNext()) {
		
		Synapse *netSynapse = netSynapseIt.next();
	
		xmlIdTable.insert((NeuralNetworkElement*)netSynapse, nextXmlId);
		nextXmlId++;
	}	
	
	
	// CREATE XML DOCUMENT /////////////////////////////////////////
	
	QDomDocument xmlDoc;
	QDomProcessingInstruction process = xmlDoc.createProcessingInstruction(
			"xml", "version=\"1.0\" encoding=\"utf-8\"");
	xmlDoc.appendChild(process);
	
	// Braindesigner element
	QDomElement xmlRoot = xmlDoc.createElement( "BrainDesigner" );
	xmlRoot.setAttribute( "version", 1 );
	xmlRoot.setAttribute( "libraryname", networkName );
	xmlDoc.appendChild( xmlRoot );
	
	// ELEMENT DESCRIPTIONS
	
	// Input Element
	QDomElement xmlInputDescr = xmlDoc.createElement( "Module" );
	xmlInputDescr.setAttribute( "name", "Input" );
	xmlInputDescr.setAttribute( "type", "System" );
	xmlInputDescr.setAttribute( "systemtype", "Input" );
	xmlInputDescr.setAttribute( "guid", "Input" );
	xmlRoot.appendChild( xmlInputDescr );
	
	QDomElement xmlInputDescrIoLabelList = xmlDoc.createElement( "IoLabelList" );
	xmlInputDescrIoLabelList.setAttribute("name", "Parameters");
	xmlInputDescr.appendChild(xmlInputDescrIoLabelList);
	
	QDomElement xmlInputDescrIoLabelInput = xmlDoc.createElement( "IoLabel" );
	xmlInputDescrIoLabelInput.setAttribute("name", "Input Type");
	xmlInputDescrIoLabelInput.setAttribute("standard", "Normal");
	xmlInputDescrIoLabelList.appendChild(xmlInputDescrIoLabelInput);	

	QDomElement xmlInputDescrIoLabelName = xmlDoc.createElement( "IoLabel" );
	xmlInputDescrIoLabelName.setAttribute("name", "Name");
	xmlInputDescrIoLabelName.setAttribute("standard", "Input");
	xmlInputDescrIoLabelList.appendChild(xmlInputDescrIoLabelName);	
	
	QDomElement xmlInputDescrIoLabelPos = xmlDoc.createElement( "IoLabel" );
	xmlInputDescrIoLabelPos.setAttribute("name", "Position");
	xmlInputDescrIoLabelPos.setAttribute("standard", "");
	xmlInputDescrIoLabelList.appendChild(xmlInputDescrIoLabelPos);	
		
	// Output Element
	QDomElement xmlOutputDescr = xmlDoc.createElement( "Module" );
	xmlOutputDescr.setAttribute( "name", "Output" );
	xmlOutputDescr.setAttribute( "type", "System" );
	xmlOutputDescr.setAttribute( "systemtype", "Output" );
	xmlOutputDescr.setAttribute( "guid", "Output" );
	xmlRoot.appendChild( xmlOutputDescr );
	
	QDomElement xmlOutputDescrIoLabelList = xmlDoc.createElement( "IoLabelList" );
	xmlOutputDescrIoLabelList.setAttribute("name", "Parameters");
	xmlOutputDescrIoLabelList.setAttribute("standard", "Normal");
	xmlOutputDescr.appendChild(xmlOutputDescrIoLabelList);
	
	QDomElement xmlOutputDescrIoLabelOutput = xmlDoc.createElement( "IoLabel" );
	xmlOutputDescrIoLabelOutput.setAttribute("name", "Output Type");
	xmlOutputDescrIoLabelOutput.setAttribute("standard", "Normal");
	xmlOutputDescrIoLabelList.appendChild(xmlOutputDescrIoLabelOutput);	

	QDomElement xmlOutputDescrIoLabelName = xmlDoc.createElement( "IoLabel" );
	xmlOutputDescrIoLabelName.setAttribute("name", "Name");
	xmlOutputDescrIoLabelName.setAttribute("standard", "Output");
	xmlOutputDescrIoLabelList.appendChild(xmlOutputDescrIoLabelName);	
	
	QDomElement xmlOutputDescrIoLabelPos = xmlDoc.createElement( "IoLabel" );
	xmlOutputDescrIoLabelPos.setAttribute("name", "Position");
	xmlOutputDescrIoLabelPos.setAttribute("standard", "");
	xmlOutputDescrIoLabelList.appendChild(xmlOutputDescrIoLabelPos);	

	// Bias element
	QDomElement xmlBiasDescr = xmlDoc.createElement( "Module" );
	xmlBiasDescr.setAttribute( "name", "Bias" );
	xmlBiasDescr.setAttribute( "type", "System" );
	xmlBiasDescr.setAttribute( "systemtype", "Bias" );
	xmlBiasDescr.setAttribute( "guid", "Bias" );
	xmlRoot.appendChild( xmlBiasDescr );
	
	// Neuron and Synapse Descriptions
	QMap<QString, QString> byteCodeMapping = createByteCodeMapping();
	QMapIterator<QString, QString> byteCodeMappingIter(byteCodeMapping);
	while (byteCodeMappingIter.hasNext()) {
		byteCodeMappingIter.next();
		
		// if the key contains an ";", it is a neuron type, 
		// otherwise it is a s synapse type
		if(byteCodeMappingIter.key().contains(";"))
		{
			// Neuron
			QDomElement xmlNeuronDescr = xmlDoc.createElement( "Module" );
			xmlNeuronDescr.setAttribute( "name", byteCodeMappingIter.key() );
			xmlNeuronDescr.setAttribute( "type", "Unit" );
			xmlNeuronDescr.setAttribute( "guid", byteCodeMappingIter.key() );
			xmlRoot.appendChild( xmlNeuronDescr );
	
			QDomElement bytecode = xmlDoc.createElement( "bytecode" );
			QDomText bytecodeText = xmlDoc.createTextNode( byteCodeMappingIter.value() );
			bytecode.appendChild( bytecodeText );
			xmlNeuronDescr.appendChild(bytecode);
		}else
		{
			// Synapse
			QDomElement xmlSynapseDescr = xmlDoc.createElement( "Module" );
			xmlSynapseDescr.setAttribute( "name", byteCodeMappingIter.key() );
			xmlSynapseDescr.setAttribute( "type", "Synapse" );
			xmlSynapseDescr.setAttribute( "guid", byteCodeMappingIter.key() );
			xmlRoot.appendChild( xmlSynapseDescr );
	
			QDomElement bytecode = xmlDoc.createElement( "bytecode" );
			QDomText bytecodeText = xmlDoc.createTextNode( byteCodeMappingIter.value() );
			bytecode.appendChild( bytecodeText );
			xmlSynapseDescr.appendChild(bytecode);
	
			QDomElement xmlSynapseDescrIoLabelList = xmlDoc.createElement( "IoLabelList" );
			xmlSynapseDescrIoLabelList.setAttribute("name", "Parameters");
			xmlSynapseDescr.appendChild(xmlSynapseDescrIoLabelList);
	
			QDomElement xmlSynapseDescrIoLabel = xmlDoc.createElement( "IoLabel" );
			xmlSynapseDescrIoLabel.setAttribute("name", "w");
			xmlSynapseDescrIoLabel.setAttribute("standard", "1");
			xmlSynapseDescrIoLabelList.appendChild(xmlSynapseDescrIoLabel);	
		}
	}
	
	// NETWORK DESCRIPTION
	
	/*
	QDomElement xmlNetModule = xmlDoc.createElement( "Module" );
	xmlNetModule.setAttribute( "name", networkName );
	xmlNetModule.setAttribute( "type", "Structure" );
	xmlNetModule.setAttribute( "guid", networkName );
	xmlNetModule.setAttribute( "opened", "true" );
	xmlRoot.appendChild( xmlNetModule );
	*/
	
	QList<NeuroModule*> netModules = net->getNeuroModules();
	QListIterator<NeuroModule*> netModuleIt(netModules);
	while(netModuleIt.hasNext()) {
		
		NeuroModule *netModule = netModuleIt.next();

		QDomElement xmlNetModule = xmlDoc.createElement( "Module" );
		xmlNetModule.setAttribute( "name", netModule->getName() );
		xmlNetModule.setAttribute( "type", "Structure" );
		xmlNetModule.setAttribute( "guid", netModule->getName() );
		xmlNetModule.setAttribute( "opened", "true" );
		xmlRoot.appendChild( xmlNetModule );
		
		cout << "\n" << qPrintable(netModule->getName()) << ":\n";
		
		
		// create List of Neurons in the module
		QList<Neuron*> netModuleNeurons = netModule->getAllEnclosedNeurons();
		QListIterator<Neuron*> netModuleNeuronIt(netModuleNeurons);

		// create List of synapses in the module
		//QList<Synapse*> netModuleSynapses = netModule->getAllEnclosedSynapses();
		QListIterator<Synapse*> netModuleSynapseIt(netSynapses);

		
		// Neurons		
		
		// calculate offset for neuron positions
		// needed because the braindesigner uses only positive coordinates 
		// whereas the networkeditor uses also negative coordinates
		
		int minX = std::numeric_limits<int>::max(); 
		int minY = std::numeric_limits<int>::max();
		
		netModuleNeuronIt.toFront();
		while(netModuleNeuronIt.hasNext()) {
			Neuron *netNeuron = netModuleNeuronIt.next();
			
			int xPos = static_cast<int>(netNeuron->getPosition().getX());
			int yPos = static_cast<int>(netNeuron->getPosition().getY());
			
			minX = Math::min(minX, xPos);
			minY = Math::min(minY, yPos);
		}
		
		int xOffSet = 0 - minX + 120;
		int yOffSet = 0 - minY + 60;
		
		QStringList neuronStrLst;
		
		// create information for every neuron
		netModuleNeuronIt.toFront();
		while(netModuleNeuronIt.hasNext()) {
					
			Neuron *netNeuron = netModuleNeuronIt.next();
		
			if(netNeuron->hasProperty(STANDARD_POSITION_PROPERTY_NAME)) 
			{
			neuronStrLst << netNeuron->getNameValue().get() + ";" + netNeuron->getProperty(STANDARD_POSITION_PROPERTY_NAME);
			}
			else
			{
				neuronStrLst << netNeuron->getNameValue().get();
			}
			//cout << qPrintable(netNeuron->getNameValue().get()) << "\n";
			
			// select neurontype and check if a bytecodemapping exists
			QString neuronType = 	netNeuron->getTransferFunction()->getName() 
														+ ";" 
														+ netNeuron->getActivationFunction()->getName();
			
			if(netNeuron->isActivationFlipped())
			{
				neuronType = neuronType + DebugNeuralNetworkIOMSeriesComponentsBDN::FLIP_TYPE;
			}
			
			// check if neuron type exists in bytecodemapping, if not --> error
			if(!byteCodeMapping.contains(neuronType))
			{			
				*errorMsg = QString("The neuron \"%1\" has an unknown activation and transfer function pair \"%2\"! Use a known pair or add a new bytecode mapping to the function \"DebugNeuralNetworkIOMSeriesComponentsBDN::createByteCodeMapping()\".\nThe following pairs are known:\n%3").arg(	
													netNeuron->getNameValue().get(), 
													neuronType,
													getNeuronByteCodeMappingTypes(byteCodeMapping));
				
				return QString::null;
			}
			
			QDomElement xmlNeuron = xmlDoc.createElement( "node" );	
			xmlNeuron.setAttribute( "id", xmlIdTable.value(netNeuron) );
			xmlNetModule.appendChild(xmlNeuron);
			
			// select type of neuron: input, output, hidden
			if(netNeuron->hasProperty("TYPE_INPUT"))
			{
				// Input Neuron
				xmlNeuron.setAttribute( "type", "Input" );
				xmlNeuron.setAttribute( "guid", "Input" );
				
				QDomElement xmlNeuronParameters = xmlDoc.createElement( "parameters" );
				xmlNeuron.appendChild(xmlNeuronParameters);
				
				QDomElement xmlNeuronTypeParameter = xmlDoc.createElement( "parameter" );
				xmlNeuronTypeParameter.setAttribute( "name", "Input Type");
				xmlNeuronTypeParameter.setAttribute( "value", "M-Series");
				xmlNeuronTypeParameter.setAttribute( "show", "Standard");
				xmlNeuronParameters.appendChild(xmlNeuronTypeParameter);
							
				QDomElement xmlNeuronPosParameter = xmlDoc.createElement( "parameter" );
				xmlNeuronPosParameter.setAttribute( "name", "Position");
				
				if(!netNeuron->hasProperty(STANDARD_POSITION_PROPERTY_NAME)) {
					*errorMsg = QString("The input neuron \"%1\" has not the needed property \"%2\"!. ").arg(
															netNeuron->getNameValue().get(),
															STANDARD_POSITION_PROPERTY_NAME);
					return QString::null;
				}
				
				xmlNeuronPosParameter.setAttribute( "value", netNeuron->getProperty(STANDARD_POSITION_PROPERTY_NAME) );
				xmlNeuronPosParameter.setAttribute( "show", "Standard");
				xmlNeuronParameters.appendChild(xmlNeuronPosParameter);			
			}
			else if(netNeuron->hasProperty("TYPE_OUTPUT")) {
				// Output Neuron
				xmlNeuron.setAttribute( "type", "Output" );
				xmlNeuron.setAttribute( "guid", "Output" );
				
				QDomElement xmlNeuronParameters = xmlDoc.createElement( "parameters" );
				xmlNeuron.appendChild(xmlNeuronParameters);
				
				QDomElement xmlNeuronTypeParameter = xmlDoc.createElement( "parameter" );
				xmlNeuronTypeParameter.setAttribute( "name", "Output Type");
				xmlNeuronTypeParameter.setAttribute( "value", "M-Series");
				xmlNeuronTypeParameter.setAttribute( "show", "Standard");
				xmlNeuronParameters.appendChild(xmlNeuronTypeParameter);
							
				if(!netNeuron->hasProperty(STANDARD_POSITION_PROPERTY_NAME)) {
					*errorMsg = QString("The output neuron \"%1\" has not the needed property \"%2\"!. ").arg(
															netNeuron->getNameValue().get(),
															STANDARD_POSITION_PROPERTY_NAME);
					return QString::null;
				}
				
				QDomElement xmlNeuronPosParameter = xmlDoc.createElement( "parameter" );
				xmlNeuronPosParameter.setAttribute( "name", "Position");
				
				if(!netNeuron->hasProperty(STANDARD_POSITION_PROPERTY_NAME)) {
					*errorMsg = QString("The output neuron \"%1\" has not the needed property \"%2\"!. ").arg(
															netNeuron->getNameValue().get(),
															STANDARD_POSITION_PROPERTY_NAME);
					return QString::null;
				}
				
				xmlNeuronPosParameter.setAttribute( "value", netNeuron->getProperty(STANDARD_POSITION_PROPERTY_NAME) );
				xmlNeuronPosParameter.setAttribute( "show", "Standard");
				xmlNeuronParameters.appendChild(xmlNeuronPosParameter);			
			}
			else{
				// Hidden Neuron
				xmlNeuron.setAttribute( "type", "Unit" );
				xmlNeuron.setAttribute( "guid", neuronType );
			}
			
			// Neuron Position
			int xmlNeuronXPos = static_cast<int>(netNeuron->getPosition().getX()) + xOffSet;
			int xmlNeuronYPos = static_cast<int>(netNeuron->getPosition().getY()) + yOffSet;
			
			QDomElement xmlPosition = xmlDoc.createElement( "position" );
			xmlPosition.setAttribute( "x", xmlNeuronXPos);
			xmlPosition.setAttribute( "y", xmlNeuronYPos);
			xmlNeuron.appendChild(xmlPosition);
		
			// create bias for neuron if needed
			bool biasExists = false;
			int xmlBiasSynapseId = -1;
			
			if(netNeuron->getBiasValue().get() != 0.0)
			{		
				xmlBiasSynapseId = addXmlBias(	xmlNetModule, 
																				xmlIdTable.value(netNeuron),
																				xmlNeuronXPos - 60,
																				xmlNeuronYPos - 30,
																				netNeuron->getBiasValue().get(), 
																				nextXmlId);
				biasExists = true;
			}
			
			// create synapse information for neuron if needed
			QList<Synapse*> netNeuronSynapsesIn = netNeuron->getSynapses();
			QList<Synapse*> netNeuronSynapsesOut = netNeuron->getOutgoingSynapses();
			if(netNeuronSynapsesIn.count() > 0 || netNeuronSynapsesOut.count() > 0 || biasExists == true)
			{		
				QDomElement xmlConnectedEdges = xmlDoc.createElement ("connectedEdges");
				xmlNeuron.appendChild(xmlConnectedEdges);
				
				// information for input synapses
				QListIterator<Synapse*> netNeuronSynapsesInIt(netNeuronSynapsesIn);
				while(netNeuronSynapsesInIt.hasNext()) {
					
					Synapse *netNeuronSynapse = netNeuronSynapsesInIt.next();
					
					QDomElement xmlConnectionPoint = xmlDoc.createElement ("connectionPoint");
					xmlConnectionPoint.setAttribute( "id", "0" );
					xmlConnectedEdges.appendChild(xmlConnectionPoint);
					
					QDomElement xmlConnectedEdge = xmlDoc.createElement ("connectedEdge");
					xmlConnectedEdge.setAttribute( "id", xmlIdTable.value(netNeuronSynapse) );
					xmlConnectedEdge.setAttribute( "start", "False" );
					xmlConnectionPoint.appendChild(xmlConnectedEdge);
				}
				
				// information for output synapses
				QListIterator<Synapse*> netNeuronSynapsesOutIt(netNeuronSynapsesOut);
				while(netNeuronSynapsesOutIt.hasNext()) {
					
					Synapse *netNeuronSynapse = netNeuronSynapsesOutIt.next();
					
					QDomElement xmlConnectionPoint = xmlDoc.createElement ("connectionPoint");
					xmlConnectionPoint.setAttribute( "id", "0" );
					xmlConnectedEdges.appendChild(xmlConnectionPoint);
					
					QDomElement xmlConnectedEdge = xmlDoc.createElement ("connectedEdge");
					xmlConnectedEdge.setAttribute( "id", xmlIdTable.value(netNeuronSynapse) );
					xmlConnectedEdge.setAttribute( "start", "True" );
					xmlConnectionPoint.appendChild(xmlConnectedEdge);
				}
				
				//information for bias synapse
				if(biasExists)
				{
					QDomElement xmlConnectionPointBias = xmlDoc.createElement ("connectionPoint");
					xmlConnectionPointBias.setAttribute( "id", "2" );
					xmlConnectedEdges.appendChild(xmlConnectionPointBias);
					
					QDomElement xmlConnectedEdge = xmlDoc.createElement ("connectedEdge");
					xmlConnectedEdge.setAttribute( "id", xmlBiasSynapseId );
					xmlConnectedEdge.setAttribute( "start", "False" );
					xmlConnectionPointBias.appendChild(xmlConnectedEdge);
				}
			}
		}
		
		// print debugoutput for mappingtest
		neuronStrLst.sort();
			
		for(int index = 0; index < neuronStrLst.size(); index++)
		{
			cout << qPrintable(neuronStrLst.at(index)) << "\n";
		}
		
		
		// Synapses
		netModuleSynapseIt.toFront();
		while(netModuleSynapseIt.hasNext()) {
			
			Synapse *netSynapse = netModuleSynapseIt.next();
			
			// check if the synapse type exists in bytecodemapping, if not --> error
			if(!byteCodeMapping.contains(netSynapse->getSynapseFunction()->getName()))
			{
				
				*errorMsg = QString("A synapse of the neuron \"%1\" has an unknown synaptic function \"%2\"! Use a known function or add a new bytecode mapping to the function \"DebugNeuralNetworkIOMSeriesComponentsBDN::createByteCodeMapping()\".\nThe following functions are known:\n%3").arg(	
														netSynapse->getSource()->getNameValue().get(), 
														netSynapse->getSynapseFunction()->getName(),
														getSynapseByteCodeMappingTypes(byteCodeMapping));
				
				return QString::null;
			}
			
			addXmlSynapse(	xmlNetModule, 
											xmlIdTable.value(netSynapse), 
											xmlIdTable.value(netSynapse->getSource()),
											xmlIdTable.value(netSynapse->getTarget()),
											netSynapse->getStrengthValue().get(),
											netSynapse->getSynapseFunction()->getName());
		}
		
	}
	
	return xmlDoc.toString();
}
 
void DebugNeuralNetworkIOMSeriesComponentsBDN::addXmlSynapse(QDomElement &baseElement, int xmlSynapseID, int srcXmlNeuronID, int destXmlNeuronID, double strength, QString guid)
{
	QDomElement xmlSynapse = baseElement.ownerDocument().createElement( "edge" );	
	xmlSynapse.setAttribute( "id", xmlSynapseID );
	xmlSynapse.setAttribute( "guid", guid);
	baseElement.appendChild(xmlSynapse);
		
	QDomElement xmlSynapseStart = baseElement.ownerDocument().createElement( "start" );	
	xmlSynapseStart.setAttribute( "nodeId", srcXmlNeuronID );
	xmlSynapse.appendChild(xmlSynapseStart);
		
	QDomElement xmlSynapseEnd = baseElement.ownerDocument().createElement( "end" );	
	xmlSynapseEnd.setAttribute( "nodeId", destXmlNeuronID );
	xmlSynapse.appendChild(xmlSynapseEnd);
		
	QDomElement xmlSynapseRoundness = baseElement.ownerDocument().createElement( "roundness" );	
	xmlSynapseRoundness.setAttribute( "height", 0 );
	xmlSynapseRoundness.setAttribute( "width", 0 );
	xmlSynapse.appendChild(xmlSynapseRoundness);
		
	QDomElement xmlSynapseParas = baseElement.ownerDocument().createElement( "parameters" );	
	xmlSynapse.appendChild(xmlSynapseParas);
			
	QDomElement xmlSynapsePara = baseElement.ownerDocument().createElement( "parameter" );	
	xmlSynapsePara.setAttribute("name", "w");
	xmlSynapsePara.setAttribute("value", strength);
	xmlSynapseParas.appendChild(xmlSynapsePara);
}

int DebugNeuralNetworkIOMSeriesComponentsBDN::addXmlBias(QDomElement &baseElement,const int xmlNeuronID, int xPos, int yPos, double bias, int &nextXmlId)
{
	// generate XML - ID's for bias elements
	int xmlBiasNeuronId = nextXmlId++;
	int xmlBiasSynapseId = nextXmlId++;
	
	// create bias neuron
	QDomElement xmlBias = baseElement.ownerDocument().createElement( "node" );	
	xmlBias.setAttribute( "id", xmlBiasNeuronId );
	xmlBias.setAttribute( "type", "Bias" );
	xmlBias.setAttribute( "guid", "Bias" ); 
	baseElement.appendChild(xmlBias);
		
	QDomElement xmlBiasPosition = baseElement.ownerDocument().createElement( "position" );
	xmlBiasPosition.setAttribute( "x", xPos );
	xmlBiasPosition.setAttribute( "y", yPos );
	xmlBias.appendChild(xmlBiasPosition);
		
	QDomElement xmlBiasConnectedEdges = baseElement.ownerDocument().createElement ("connectedEdges");
	xmlBias.appendChild(xmlBiasConnectedEdges);
		
	QDomElement xmlBiasConnectionPoint = baseElement.ownerDocument().createElement ("connectionPoint");
	xmlBiasConnectionPoint.setAttribute( "id", "1" );
	xmlBiasConnectedEdges.appendChild(xmlBiasConnectionPoint);
		
	QDomElement xmlBiasConnectedEdge = baseElement.ownerDocument().createElement ("connectedEdge");
	xmlBiasConnectedEdge.setAttribute( "id", xmlBiasSynapseId );
	xmlBiasConnectedEdge.setAttribute( "start", "True" );
	xmlBiasConnectionPoint.appendChild(xmlBiasConnectedEdge);	
	
	// add Xml Synapse for bias
	addXmlSynapse(	baseElement, 
									xmlBiasSynapseId, 
				 					xmlBiasNeuronId, 
									xmlNeuronID, 
				 					bias, 
									STANDARD_BIAS_SYNAPSE_TYPE);
	
	return xmlBiasSynapseId;
}

/**
 * Returns the type names of the bytecode mappings for neurons. Used for error messages.
 */
QString DebugNeuralNetworkIOMSeriesComponentsBDN::getNeuronByteCodeMappingTypes(QMap<QString, QString> byteCodeMapping)
{
	QString retval = "";
	
	QMapIterator<QString, QString> byteCodeMappingIter(byteCodeMapping);
	while (byteCodeMappingIter.hasNext()) {
		byteCodeMappingIter.next();
		
		if(byteCodeMappingIter.key().contains(";"))
		{
			retval += " - " + byteCodeMappingIter.key()  + "\n";
		}
	}
	
	return retval;
}

/**
 * Returns the type names of the bytecode mappings for synapses. Used for error messages.
 */
QString DebugNeuralNetworkIOMSeriesComponentsBDN::getSynapseByteCodeMappingTypes(QMap<QString, QString> byteCodeMapping)
{
	QString retval = "";
	
	QMapIterator<QString, QString> byteCodeMappingIter(byteCodeMapping);
	while (byteCodeMappingIter.hasNext()) {
		byteCodeMappingIter.next();
		
		if(!byteCodeMappingIter.key().contains(";"))
		{
			retval += " - " + byteCodeMappingIter.key()  + "\n";
		}
	}
	
	return retval;
}

}
