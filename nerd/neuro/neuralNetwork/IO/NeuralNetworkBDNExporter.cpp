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

#include "NeuralNetworkBDNExporter.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Core/Core.h"
#include "Math/Math.h"
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QDebug>

#include <QDomElement>

#include <limits> 

namespace nerd {
	
// Name of the property which holds the position of a neuron on the spinal cord
const QString NeuralNetworkBDNExporter::STANDARD_POSITION_PROPERTY_NAME = "SPINAL_CORD_ADDRESS";

// Postfix of bytecodemappings for functions which can be flipped
const QString NeuralNetworkBDNExporter::FLIP_TYPE = "_FLIP";

// Flag which gets replaced in the byte code mappings by the execution position value
const QString NeuralNetworkBDNExporter::BYTECODE_POS_FLAG = "<$POS>";

// Flag which gets replaced in the byte code mappings of mirror neurons with the minimum value
// the input 
const QString NeuralNetworkBDNExporter::BYTECODE_MIN_FLAG = "<$MIN>";

// Flag which gets replaced in the byte code mappings of mirror neurons with the maximum value
// the input 
const QString NeuralNetworkBDNExporter::BYTECODE_MAX_FLAG = "<$MAX>";
		
// Name of the neurontype which is used as input neuron mirror
const QString NeuralNetworkBDNExporter::INPUT_MIRROR_GUID = "InputMirror";

// Bytecode of the neurontype which is used as input neuron mirror
const QString NeuralNetworkBDNExporter::INPUT_MIRROR_BYTE_CODE = QString::number(BDN_MIN_EXECUTION_POS + 1) + ":\nwrite Output, Input";

// Name of the neurontype which is used as input neuron mirror
const QString NeuralNetworkBDNExporter::INPUT_MIRROR_FLIP_GUID = "InputMirror" + FLIP_TYPE;

const QString NeuralNetworkBDNExporter::INPUT_MIRROR_FLIP_BYTE_CODE =
		QString::number(BDN_MIN_EXECUTION_POS + 1) + ":\nLOAD V1, -1.0\nMUL V0, Input, V1\nwrite Output, V0";

// Length of the drawing area of the braindesigner in x direction.
// Exported network get scaled to this maximum.
const int NeuralNetworkBDNExporter::BDN_DRAWING_AREA_X_LENGTH = 4000;

// Length of the drawing area of the braindesigner in y direction
// Exported network get scaled to this maximum.
const int NeuralNetworkBDNExporter::BDN_DRAWING_AREA_Y_LENGTH = 4000;

// distance for additional neurons in the BDN network to the base neuron
const int NeuralNetworkBDNExporter::ADDITIONAL_BDN_NEURON_DISTANCE = 50;

// minimal execution position for the byte code modules
const int NeuralNetworkBDNExporter::BDN_MIN_EXECUTION_POS = 101;

// maximal execution position for the byte code modules
const int NeuralNetworkBDNExporter::BDN_MAX_EXECUTION_POS = 300;


NeuralNetworkBDNExporter::NeuralNetworkBDNExporter(QString standardBiasSynapseType, QString robotType, QMap<QString, QString> byteCodeMapping)
{
	m_standardBiasSynapseType = standardBiasSynapseType;
	m_robotType = robotType;
	m_byteCodeMapping = byteCodeMapping;
	
	m_extraSpinalCordMapping = QMap<QString, QString>();
	m_xmlIdTable = QHash<NeuralNetworkElement*, int>();
	m_executionPositionTable = QHash<NeuralNetworkElement*, int>();
	m_byteCodeModulPositions = QMap<QString, QList<QString> >();
	m_BDNNeuronInfoList 	= QList<BDNNeuronInfo*>();
	m_BDNSynapseInfoList 	= QList<BDNSynapseInfo*>();
	m_nextXmlId = 0;
	m_bdnXPosOffset = 0;
	m_bdnYPosOffset = 0;
	m_bdnXPosScaling = 0.0;
	m_bdnYPosScaling = 0.0;
}

NeuralNetworkBDNExporter::~NeuralNetworkBDNExporter()
{
	m_extraSpinalCordMapping.clear();
	m_byteCodeMapping.clear();
	
	deleteWorkingVariables();
}

void NeuralNetworkBDNExporter::deleteWorkingVariables()
{
	m_xmlIdTable.clear();
	m_executionPositionTable.clear();
	m_byteCodeModulPositions.clear();
	
	QListIterator<BDNNeuronInfo*> BDNNeuronInfoIt(m_BDNNeuronInfoList);
	while(BDNNeuronInfoIt.hasNext()) {
		delete BDNNeuronInfoIt.next();
	}
	m_BDNNeuronInfoList.clear();
	
	QListIterator<BDNSynapseInfo*> BDNSynapseInfoIt(m_BDNSynapseInfoList);
	while(BDNSynapseInfoIt.hasNext()) {
		delete BDNSynapseInfoIt.next();
	}
	m_BDNSynapseInfoList.clear();
			
	m_nextXmlId = 0;
	m_bdnXPosOffset = 0;
	m_bdnYPosOffset = 0;
	m_bdnXPosScaling = 0.0;
	m_bdnYPosScaling = 0.0;
}

void NeuralNetworkBDNExporter::setExtraSpinalCordMapping(QMap<QString, QString> extraSpinalCordMapping)
{
	m_extraSpinalCordMapping = extraSpinalCordMapping;
}

/**
 * Try to export a neural network into a BDN (Brain Designer Network) File.
 * 
 * @param fileName the file to write to.
 * @param net the net to export.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkBDNExporter::createFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg) {

	// check if network fullfills constraints for export
	if(checkIfNetworkIsExportable(net, errorMsg) == false){
		return false;
	}
	
	QFileInfo fileInfo(fileName);
	QString networkName = fileInfo.fileName().remove(".bdn");
	
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
bool NeuralNetworkBDNExporter::checkIfNetworkIsExportable(NeuralNetwork *net, QString *errorMsg) {
	
	// try to cast Network to ModularNeuralNetwork
	ModularNeuralNetwork *modNet = dynamic_cast<ModularNeuralNetwork*>(net);
	if(modNet == NULL) {
		*errorMsg = QString("Network has to be modular!");
		return false;
	}

	///////////////////////////////////////////////////////
	// Check if FAST - Property of Neurons and Synapses has for all elements 
	// only an required iteration count of 1	
	
	QString badNeurons = "";
	
	QList<NeuralNetworkElement*> elementList;
	net->getNetworkElements(elementList);
	
	QListIterator<NeuralNetworkElement*> netElemIt(elementList);
	while(netElemIt.hasNext()) {		
		NeuralNetworkElement *netElem = netElemIt.next();
		
		if(netElem->getRequiredIterations() != 1){
			
			Neuron* netNeuron = dynamic_cast<Neuron*>(netElem);
			if(netNeuron != NULL){
				badNeurons += " - Neuron: " + netNeuron->getNameValue().get() + "\n";
				continue;
			}
			
			Synapse* netSynapse = dynamic_cast<Synapse*>(netElem);
			if(netSynapse != NULL){
				badNeurons += " - Synapse\n";
				continue;
			}	
					
			badNeurons += " - Unknown NetworkElementType\n";
		}
	}
	
	if(badNeurons.length() > 0){
		*errorMsg = QString("The exporter can handle only networkelements with a required iteration count of 1! The following elements have a different configuration:\n") + badNeurons;
		return false;
	}
	
	/////////////////////////////////////////////////////
	
	return true;
}



/**
 * Exports the given MSeries NeuralNetwork into a BrainDesigner Library.
 * @param networkName The name of the network to export.
 * @param net The net to export.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return A string with the BrainDesigner Library. In the case of an error QString::null will be returned.
 */
QString NeuralNetworkBDNExporter::exportNetwork(QString networkName, ModularNeuralNetwork *net, QString *errorMsg)
{
	bool ok = true;
	
	//////////////////////////////////////////////////////////////////////
	// Prepare export
	
	deleteWorkingVariables();
	
	ok &= updateXmlIdTable(net, errorMsg);
	if(ok == false){return QString::null;}
	
	ok &= calcNeuronPositionValues(net, errorMsg);
	if(ok == false){return QString::null;}
	
	ok &= calcExecutionPositions(net, errorMsg);
	if(ok == false){return QString::null;}
	
	//////////////////////////////////////////////////////////////////////
	// Create BDN Information about neurons and synapses
	
	const QList<Neuron*>& netNeurons = net->getNeurons();
	QListIterator<Neuron*> netNeuronIt(netNeurons);
	while(netNeuronIt.hasNext()) {
		Neuron *netNeuron = netNeuronIt.next();
		
		ok &= addBDNNeuronInfo(netNeuron,errorMsg);
		if(ok == false){return QString::null;}		
	}
	
	const QList<Synapse*>& netSynapses = net->getSynapses();
	QListIterator<Synapse*> netSynapseIt(netSynapses);
	while(netSynapseIt.hasNext()) {
		Synapse *netSynapse = netSynapseIt.next();
		
		ok &= addBDNSynapseInfo(netSynapse,errorMsg);
		if(ok == false){return QString::null;}	
	}
	
	//////////////////////////////////////////////////////////////////////
	// Create the XML Document from the collected information
	
	QDomDocument xmlDoc;
	QDomProcessingInstruction process = xmlDoc.createProcessingInstruction(
			"xml", "version=\"1.0\" encoding=\"utf-8\"");
	xmlDoc.appendChild(process);
	
	// Braindesigner element
	QDomElement xmlRoot = xmlDoc.createElement( "BrainDesigner" );
	xmlRoot.setAttribute( "version", 1 );
	xmlRoot.setAttribute( "libraryname", networkName );
	xmlDoc.appendChild( xmlRoot );
	
	ok &= createBDNHeadInformation(xmlRoot);
	if(ok == false){return QString::null;}	
	
	// add module with networkdescription
	QDomElement xmlNetModule = xmlDoc.createElement( "Module" );
	xmlNetModule.setAttribute( "name", networkName );
	xmlNetModule.setAttribute( "type", "Structure" );
	xmlNetModule.setAttribute( "guid", networkName );
	xmlNetModule.setAttribute( "opened", "true" );
	xmlRoot.appendChild( xmlNetModule );
	
	// add information about neurons and synapses
	QListIterator<BDNNeuronInfo*> BDNNeuronInfoIt(m_BDNNeuronInfoList);
	while(BDNNeuronInfoIt.hasNext()) {
		BDNNeuronInfo *bdnNeuronInfo = BDNNeuronInfoIt.next();
		
		xmlNetModule.appendChild(bdnNeuronInfo->toXML(xmlDoc));
	}
	
	QListIterator<BDNSynapseInfo*> BDNSynapseInfoIt(m_BDNSynapseInfoList);
	while(BDNSynapseInfoIt.hasNext()) {
		BDNSynapseInfo *bdnSynapseInfo = BDNSynapseInfoIt.next();
		
		xmlNetModule.appendChild(bdnSynapseInfo->toXML(xmlDoc));
	}
	
	//////////////////////////////////////////////////////////////////////
	// clean up working variables
		
	deleteWorkingVariables();
	
	return xmlDoc.toString();
}

/**
 * Fills Hashtable m_xmlIdTable with all neuron and synapse id's from the given network.
 */
bool NeuralNetworkBDNExporter::updateXmlIdTable(ModularNeuralNetwork *net, QString *errorMsg)
{
	m_xmlIdTable.clear();

	// create XML ID's for neurons
	const QList<Neuron*>& netNeurons = net->getNeurons();
	QListIterator<Neuron*> netNeuronIt(netNeurons);
	while(netNeuronIt.hasNext()) {		
		Neuron *netNeuron = netNeuronIt.next();
		
		m_xmlIdTable.insert((NeuralNetworkElement*)netNeuron, m_nextXmlId);
		m_nextXmlId++;
	}
	
	// create XML ID's for synapses
	const QList<Synapse*>& netSynapses = net->getSynapses();
	QListIterator<Synapse*> netSynapseIt(netSynapses);
	while(netSynapseIt.hasNext()) {
		
		Synapse *netSynapse = netSynapseIt.next();
	
		m_xmlIdTable.insert((NeuralNetworkElement*)netSynapse, m_nextXmlId);
		m_nextXmlId++;
	}	
	
	return true;
}

/**
 * Calculate offsets (m_bdnXPosOffset, m_bdnYPosOffset) for neuron positions.
 * Needed because the braindesigner uses only positive coordinates whereas the networkeditor 
 * uses also negative coordinates.
 * 
 * Furthermore, the scaling factors (m_bdnXPosScaling, m_bdnYPosScaling) are calculated.
 * Needed because the braindesigner has only limited drawing area which is defined in
 * BDN_DRAWING_AREA_X_LENGTH and BDN_DRAWING_AREA_Y_LENGTH. 
 */
bool NeuralNetworkBDNExporter::calcNeuronPositionValues(ModularNeuralNetwork *net, QString *errorMsg)
{
	int minX = std::numeric_limits<int>::max(); 
	int minY = std::numeric_limits<int>::max();
	int maxX = std::numeric_limits<int>::min();
	int maxY = std::numeric_limits<int>::min();
	
	const QList<Neuron*>& netNeurons = net->getNeurons();
	QListIterator<Neuron*> netNeuronIt(netNeurons);
	while(netNeuronIt.hasNext()) {
		Neuron *netNeuron = netNeuronIt.next();
		
		int xPos = static_cast<int>(netNeuron->getPosition().getX());
		int yPos = static_cast<int>(netNeuron->getPosition().getY());
		
		minX = Math::min(minX, xPos);
		minY = Math::min(minY, yPos);
		maxX = Math::max(maxX, xPos);
		maxY = Math::max(maxY, yPos);
	}
	
	m_bdnXPosOffset = 0 - minX + 120;
	m_bdnYPosOffset = 0 - minY + 60;
	
	maxX += m_bdnXPosOffset;
	maxY += m_bdnYPosOffset;
	
	m_bdnXPosScaling = 1.0;
	if(maxX > BDN_DRAWING_AREA_X_LENGTH){
		m_bdnXPosScaling = BDN_DRAWING_AREA_X_LENGTH / static_cast<double>(maxX);
	}
		
	m_bdnYPosScaling = 1.0;
	if(maxY > BDN_DRAWING_AREA_Y_LENGTH){
		m_bdnYPosScaling = BDN_DRAWING_AREA_Y_LENGTH / static_cast<double>(maxY);
	}
	
	return true;
}


/**
 * Calclates the execution position of the single neurons which is defined by the
 * FAST property.
 */
bool NeuralNetworkBDNExporter::calcExecutionPositions(ModularNeuralNetwork *net, QString *errorMsg)
{
	m_executionPositionTable.clear();
	
	int currentNerdIteration = net->getMinimalStartIteration();
	
	// start execution position for BDN
	// keep 3 places as buffer for input mirror synapses and neurons
	// and synapses from mirror neurons to predecessor neurons 
	int currentBdnIteration = BDN_MIN_EXECUTION_POS + 3;

	QList<Neuron*> remainingNeurons = net->getNeurons();
	QList<Neuron*> currentExecuteNeurons;
	do {
		
		currentExecuteNeurons = net->getNeuronsWithIterationRequirement(currentNerdIteration);
		
		QListIterator<Neuron*> netNeuronIt(currentExecuteNeurons);
		while(netNeuronIt.hasNext()) {
			Neuron *netNeuron = netNeuronIt.next();

			m_executionPositionTable.insert(netNeuron, currentBdnIteration);
			
			remainingNeurons.removeAll(netNeuron);
		}

		// increase BDN position by two, because a free position for the
		// update position of the incoming synapses of the current neurons
		// is needed
		if(currentExecuteNeurons.length() > 0){
			currentBdnIteration += 2;
		}
		
		currentNerdIteration++;

	} while(!remainingNeurons.empty());
	
	// highest execution position in the current network
	int maxNetBDNExecutionPos = currentBdnIteration - 2;
	
	// error if the needed execution positions exceeds the possible number
	// keep 3 as buffer for:
	// 	- synapse to output computational neuron 
	// 	- output computational neuron
	// 	- synapse from output computational neuron to output neuron
	if(maxNetBDNExecutionPos > BDN_MAX_EXECUTION_POS - 3)
	{
		*errorMsg = QString("The number of execution positions (n = %1) exceeds the possible number of execution positions (n = %2) for BDN files)!").arg(	
												(maxNetBDNExecutionPos / 2),
												 (BDN_MAX_EXECUTION_POS - 4) / 2); 
		
		return false;
	}
	
	return true;
}


/**
 * Adds the position information for the given byteCode Modul
 * into the QMap m_byteCodeModulPositions if it does not exists.
 */
void NeuralNetworkBDNExporter::addByteCodePosition(const QString &modulName, const QString &position)
{
	// add new element if needed
	if(m_byteCodeModulPositions.contains(modulName) == false)
	{
		QList<QString> newPosList;
		newPosList.append(position);
		
		m_byteCodeModulPositions[modulName] = newPosList;
	}
	else
	{ // update existing element if the position does not exist
		
		if( m_byteCodeModulPositions[modulName].contains(position) == false ){
			m_byteCodeModulPositions[modulName].append(position);
		}
	}
}


bool NeuralNetworkBDNExporter::createBDNHeadInformation(QDomElement &xmlRoot)
{
	// Input Element
	QDomElement xmlInputDescr =  xmlRoot.ownerDocument().createElement( "Module" );
	xmlInputDescr.setAttribute( "name", "Input" );
	xmlInputDescr.setAttribute( "type", "System" );
	xmlInputDescr.setAttribute( "systemtype", "Input" );
	xmlInputDescr.setAttribute( "guid", "Input" );
	xmlRoot.appendChild( xmlInputDescr );
	
	QDomElement xmlInputDescrIoLabelList = xmlRoot.ownerDocument().createElement( "IoLabelList" );
	xmlInputDescrIoLabelList.setAttribute("name", "Parameters");
	xmlInputDescr.appendChild(xmlInputDescrIoLabelList);
	
	QDomElement xmlInputDescrIoLabelInput = xmlRoot.ownerDocument().createElement( "IoLabel" );
	xmlInputDescrIoLabelInput.setAttribute("name", "Input Type");
	xmlInputDescrIoLabelInput.setAttribute("standard", "Normal");
	xmlInputDescrIoLabelList.appendChild(xmlInputDescrIoLabelInput);	

	QDomElement xmlInputDescrIoLabelName = xmlRoot.ownerDocument().createElement( "IoLabel" );
	xmlInputDescrIoLabelName.setAttribute("name", "Name");
	xmlInputDescrIoLabelName.setAttribute("standard", "Input");
	xmlInputDescrIoLabelList.appendChild(xmlInputDescrIoLabelName);	
	
	QDomElement xmlInputDescrIoLabelPos = xmlRoot.ownerDocument().createElement( "IoLabel" );
	xmlInputDescrIoLabelPos.setAttribute("name", "Position");
	xmlInputDescrIoLabelPos.setAttribute("standard", "");
	xmlInputDescrIoLabelList.appendChild(xmlInputDescrIoLabelPos);	
		
	// Output Element
	QDomElement xmlOutputDescr = xmlRoot.ownerDocument().createElement( "Module" );
	xmlOutputDescr.setAttribute( "name", "Output" );
	xmlOutputDescr.setAttribute( "type", "System" );
	xmlOutputDescr.setAttribute( "systemtype", "Output" );
	xmlOutputDescr.setAttribute( "guid", "Output" );
	xmlRoot.appendChild( xmlOutputDescr );
	
	QDomElement xmlOutputDescrIoLabelList = xmlRoot.ownerDocument().createElement( "IoLabelList" );
	xmlOutputDescrIoLabelList.setAttribute("name", "Parameters");
	xmlOutputDescrIoLabelList.setAttribute("standard", "Normal");
	xmlOutputDescr.appendChild(xmlOutputDescrIoLabelList);
	
	QDomElement xmlOutputDescrIoLabelOutput = xmlRoot.ownerDocument().createElement( "IoLabel" );
	xmlOutputDescrIoLabelOutput.setAttribute("name", "Output Type");
	xmlOutputDescrIoLabelOutput.setAttribute("standard", "Normal");
	xmlOutputDescrIoLabelList.appendChild(xmlOutputDescrIoLabelOutput);	

	QDomElement xmlOutputDescrIoLabelName = xmlRoot.ownerDocument().createElement( "IoLabel" );
	xmlOutputDescrIoLabelName.setAttribute("name", "Name");
	xmlOutputDescrIoLabelName.setAttribute("standard", "Output");
	xmlOutputDescrIoLabelList.appendChild(xmlOutputDescrIoLabelName);	
	
	QDomElement xmlOutputDescrIoLabelPos = xmlRoot.ownerDocument().createElement( "IoLabel" );
	xmlOutputDescrIoLabelPos.setAttribute("name", "Position");
	xmlOutputDescrIoLabelPos.setAttribute("standard", "");
	xmlOutputDescrIoLabelList.appendChild(xmlOutputDescrIoLabelPos);	

	// Bias element
	QDomElement xmlBiasDescr = xmlRoot.ownerDocument().createElement( "Module" );
	xmlBiasDescr.setAttribute( "name", "Bias" );
	xmlBiasDescr.setAttribute( "type", "System" );
	xmlBiasDescr.setAttribute( "systemtype", "Bias" );
	xmlBiasDescr.setAttribute( "guid", "Bias" );
	xmlRoot.appendChild( xmlBiasDescr );
	
	// Neuron and Synapse Descriptions
	
	// MirrorNeuron description
	QDomElement xmlNeuronDescr = xmlRoot.ownerDocument().createElement( "Module" );
	xmlNeuronDescr.setAttribute( "name", INPUT_MIRROR_GUID );
	xmlNeuronDescr.setAttribute( "type", "Unit" );
	xmlNeuronDescr.setAttribute( "guid", INPUT_MIRROR_GUID );
	xmlRoot.appendChild( xmlNeuronDescr );
	
	QDomElement bytecode = xmlRoot.ownerDocument().createElement( "bytecode" );
	QDomText	bytecodeText = xmlRoot.ownerDocument().createTextNode( INPUT_MIRROR_BYTE_CODE );
	bytecode.appendChild( bytecodeText );
	xmlNeuronDescr.appendChild(bytecode);	
	
	// MirrorNeuron flipped description
	xmlNeuronDescr = xmlRoot.ownerDocument().createElement( "Module" );
	xmlNeuronDescr.setAttribute( "name", INPUT_MIRROR_FLIP_GUID );
	xmlNeuronDescr.setAttribute( "type", "Unit" );
	xmlNeuronDescr.setAttribute( "guid", INPUT_MIRROR_FLIP_GUID );
	xmlRoot.appendChild( xmlNeuronDescr );
	
	bytecode = xmlRoot.ownerDocument().createElement( "bytecode" );
	bytecodeText = xmlRoot.ownerDocument().createTextNode( INPUT_MIRROR_FLIP_BYTE_CODE );
	bytecode.appendChild( bytecodeText );
	xmlNeuronDescr.appendChild(bytecode);	
	
	// user defined descriptions
	
	// byte code mappings
	QMapIterator<QString, QList<QString> > modulPosIter(m_byteCodeModulPositions);
	while (modulPosIter.hasNext()) {
		
		modulPosIter.next();
		
		// execution positions for each byte code mapping
		for(int i = 0; i < modulPosIter.value().count(); i++ )
		{
			QString byteCodeMapping = m_byteCodeMapping[modulPosIter.key()];
			byteCodeMapping.replace(BYTECODE_POS_FLAG, modulPosIter.value()[i]); 
			
			QString modulName = modulPosIter.key() + modulPosIter.value()[i];
		
		// if the key contains an ";", it is a neuron type, 
		// otherwise it is a s synapse type
			if(modulName.contains(";"))
			{
			// Neuron
				QDomElement xmlNeuronDescr = xmlRoot.ownerDocument().createElement( "Module" );
				xmlNeuronDescr.setAttribute( "name", modulName );
				xmlNeuronDescr.setAttribute( "type", "Unit" );
				xmlNeuronDescr.setAttribute( "guid", modulName );
				xmlRoot.appendChild( xmlNeuronDescr );
	
				QDomElement bytecode = xmlRoot.ownerDocument().createElement( "bytecode" );
				QDomText bytecodeText = xmlRoot.ownerDocument().createTextNode( byteCodeMapping );
				bytecode.appendChild( bytecodeText );
				xmlNeuronDescr.appendChild(bytecode);
			}else
			{
			// Synapse
				QDomElement xmlSynapseDescr = xmlRoot.ownerDocument().createElement( "Module" );
				xmlSynapseDescr.setAttribute( "name", modulName );
				xmlSynapseDescr.setAttribute( "type", "Synapse" );
				xmlSynapseDescr.setAttribute( "guid", modulName );
				xmlRoot.appendChild( xmlSynapseDescr );
	
				QDomElement bytecode = xmlRoot.ownerDocument().createElement( "bytecode" );
				QDomText bytecodeText = xmlRoot.ownerDocument().createTextNode( byteCodeMapping );
				bytecode.appendChild( bytecodeText );
				xmlSynapseDescr.appendChild(bytecode);
	
				QDomElement xmlSynapseDescrIoLabelList = xmlRoot.ownerDocument().createElement( "IoLabelList" );
				xmlSynapseDescrIoLabelList.setAttribute("name", "Parameters");
				xmlSynapseDescr.appendChild(xmlSynapseDescrIoLabelList);
	
				QDomElement xmlSynapseDescrIoLabel = xmlRoot.ownerDocument().createElement( "IoLabel" );
				xmlSynapseDescrIoLabel.setAttribute("name", "w");
				xmlSynapseDescrIoLabel.setAttribute("standard", "1");
				xmlSynapseDescrIoLabelList.appendChild(xmlSynapseDescrIoLabel);	
			}			
		}
	}
		
	return true;
}



bool NeuralNetworkBDNExporter::addBDNNeuronInfo(Neuron *netNeuron, QString *errorMsg)
{
	if(netNeuron->hasProperty("TYPE_INPUT")) {
		return addInputBDNNeuronInfo(netNeuron, errorMsg);
	}
	else if(netNeuron->hasProperty("TYPE_OUTPUT")){
		return addOutputBDNNeuronInfo(netNeuron, errorMsg);
	}
	else {
		return addHiddenBDNNeuronInfo(netNeuron, errorMsg);
	}
}

bool NeuralNetworkBDNExporter::addInputBDNNeuronInfo(Neuron *inNeuron, QString *errorMsg)
{
	if(getBDNInSynapseInformation(inNeuron, errorMsg).length() > 0){
		*errorMsg = QString("The input neuron \"" + inNeuron->getNameValue().get() + "\" has incoming synapses! This is not supported.");
		
		return false;
	}
	
	BDNNeuronInfo *inputInfo = new BDNNeuronInfo();
	
	inputInfo->ID = m_xmlIdTable.value(inNeuron);
	inputInfo->Type = "Input";
	inputInfo->Guid = "Input";
	inputInfo->XPosition = getBDNXPos(inNeuron);
	inputInfo->YPosition = getBDNYPos(inNeuron);
	inputInfo->RobotType = m_robotType;
	inputInfo->SpinalCordAddress = getSpinalCordAddress(inNeuron, errorMsg);
	if(inputInfo->SpinalCordAddress == QString::null) {return false;}
	
	// create mirror neuron if a bias is set or if the input is flipped
	if(inNeuron->getBiasValue().get() != 0.0
			|| inNeuron->isActivationFlipped())
	{
		inputInfo->ID = m_nextXmlId++;
		
		BDNNeuronInfo *mirrorInfo = new BDNNeuronInfo();
		mirrorInfo->ID = m_xmlIdTable.value(inNeuron);
		mirrorInfo->Type = "Unit";
		
		if(inNeuron->getTransferFunction()->getLowerBound() != -1.0){
			*errorMsg = QString("The input neuron \"" + inNeuron->getNameValue().get() + "\" has an invalid lower value of " + QString::number(inNeuron->getTransferFunction()->getLowerBound())+ ". The lower value must be -1.0.");
					
			return false;
		}
		
		if(inNeuron->getTransferFunction()->getUpperBound() != 1.0){
			*errorMsg = QString("The input neuron \"" + inNeuron->getNameValue().get() + "\" has an invalid upper value of " + QString::number(inNeuron->getTransferFunction()->getUpperBound())+ ". The upper value must be 1.0.");
			
			return false;
		}
		
		mirrorInfo->Guid = INPUT_MIRROR_GUID;
		if(inNeuron->isActivationFlipped()){
			mirrorInfo->Guid = INPUT_MIRROR_FLIP_GUID;
		}
		
		mirrorInfo->XPosition = inputInfo->XPosition + ADDITIONAL_BDN_NEURON_DISTANCE;
		mirrorInfo->YPosition = inputInfo->YPosition;
		mirrorInfo->RobotType = m_robotType;
		
		mirrorInfo->InSynapseIDs.append(getBDNInSynapseInformation(inNeuron, errorMsg));
		mirrorInfo->OutSynapseIDs.append(getBDNOutSynapseInformation(inNeuron, errorMsg));
		
		if(inNeuron->getBiasValue().get() != 0.0 ){
			addBDNBiasInfo(mirrorInfo, inNeuron->getBiasValue().get(), BDN_MIN_EXECUTION_POS);
		}
		
		int inputMirrorSynapseID = m_nextXmlId++;
		addBDNSynapseInfo(inputMirrorSynapseID, 
											inputInfo->ID, 
											mirrorInfo->ID, 
											1.0, 
											m_standardBiasSynapseType,
										  BDN_MIN_EXECUTION_POS);
		
		mirrorInfo->InSynapseIDs.append(inputMirrorSynapseID);
		inputInfo->OutSynapseIDs.append(inputMirrorSynapseID);
		
		m_BDNNeuronInfoList.append(mirrorInfo);
	}
	else
	{
		inputInfo->InSynapseIDs.append(getBDNInSynapseInformation(inNeuron, errorMsg));
		inputInfo->OutSynapseIDs.append(getBDNOutSynapseInformation(inNeuron, errorMsg));
	}
	
	m_BDNNeuronInfoList.append(inputInfo);
	
	return true;
}

bool NeuralNetworkBDNExporter::addOutputBDNNeuronInfo(Neuron *outNeuron, QString *errorMsg)
{
	BDNNeuronInfo *outputInfo = new BDNNeuronInfo();
	
	outputInfo->ID = m_xmlIdTable.value(outNeuron);
	outputInfo->Type = "Output";
	outputInfo->Guid = "Output";
	outputInfo->XPosition = getBDNXPos(outNeuron);
	outputInfo->YPosition = getBDNYPos(outNeuron);
	outputInfo->RobotType = m_robotType;	
	outputInfo->SpinalCordAddress = getSpinalCordAddress(outNeuron, errorMsg);
	if(outputInfo->SpinalCordAddress == QString::null) {return false;}
	
	m_BDNNeuronInfoList.append(outputInfo);
	
	// create preOutput if needed
	if(	outNeuron->getBiasValue().get() != 0.0 
			|| outNeuron->getSynapses().length() > 0
			|| outNeuron->getOutgoingSynapses().length() > 0)
	{
		// change ID of output to assign all synapses to the pre output neuron
		outputInfo->ID = m_nextXmlId++;
		
		BDNNeuronInfo *preOutputInfo = new BDNNeuronInfo();
		preOutputInfo->ID = m_xmlIdTable.value(outNeuron);
		preOutputInfo->Type = "Unit";
		preOutputInfo->Guid = getBDNNeuronType(outNeuron, errorMsg) 
				+ QString::number(BDN_MAX_EXECUTION_POS - 1);
		preOutputInfo->XPosition = outputInfo->XPosition - ADDITIONAL_BDN_NEURON_DISTANCE;
		preOutputInfo->YPosition = outputInfo->YPosition;
		preOutputInfo->RobotType = m_robotType;
			
		preOutputInfo->InSynapseIDs.append(getBDNInSynapseInformation(outNeuron, errorMsg));
		preOutputInfo->OutSynapseIDs.append(getBDNOutSynapseInformation(outNeuron, errorMsg));
		
		// create Synapse from preNeuron to output
		int preOutputSynapseID = m_nextXmlId++;
		addBDNSynapseInfo(	preOutputSynapseID,
												preOutputInfo->ID, 
												outputInfo->ID, 
												1.0, 
												m_standardBiasSynapseType,
										 		BDN_MAX_EXECUTION_POS);
		
		preOutputInfo->OutSynapseIDs.append(preOutputSynapseID);
		outputInfo->InSynapseIDs.append(preOutputSynapseID);
		
		// create a bias neuron if needed
		if(outNeuron->getBiasValue().get() != 0.0 ){
			addBDNBiasInfo(preOutputInfo, outNeuron->getBiasValue().get(),BDN_MAX_EXECUTION_POS - 2);
		}
		
		m_BDNNeuronInfoList.append(preOutputInfo);
		
		addByteCodePosition(getBDNNeuronType(outNeuron, errorMsg),
												QString::number(BDN_MAX_EXECUTION_POS - 1));
	}
	
	return true;
}

bool NeuralNetworkBDNExporter::addHiddenBDNNeuronInfo(Neuron *hiddenNeuron, QString *errorMsg)
{
	BDNNeuronInfo *hiddenInfo = new BDNNeuronInfo();
	
	hiddenInfo->ID = m_xmlIdTable.value(hiddenNeuron);
	hiddenInfo->Type = "Unit";
	hiddenInfo->Guid = getBDNNeuronType(hiddenNeuron,errorMsg) 
			+ QString::number(m_executionPositionTable[hiddenNeuron]);
	hiddenInfo->XPosition = getBDNXPos(hiddenNeuron);
	hiddenInfo->YPosition = getBDNYPos(hiddenNeuron);
	hiddenInfo->RobotType = m_robotType;
	hiddenInfo->InSynapseIDs.append(getBDNInSynapseInformation(hiddenNeuron, errorMsg));
	hiddenInfo->OutSynapseIDs.append(getBDNOutSynapseInformation(hiddenNeuron, errorMsg));
	
	// create bias if needed
	if(hiddenNeuron->getBiasValue().get() != 0.0 ){
		addBDNBiasInfo(	hiddenInfo, 
										hiddenNeuron->getBiasValue().get(), 
									 	m_executionPositionTable[hiddenNeuron] - 1 );
	}
	
	m_BDNNeuronInfoList.append(hiddenInfo);
	
	// add neuron position
	addByteCodePosition(getBDNNeuronType(hiddenNeuron,errorMsg),
											QString::number(m_executionPositionTable[hiddenNeuron]));
	
	return true;
}

QString NeuralNetworkBDNExporter::getBDNNeuronType(Neuron *netNeuron, QString *errorMsg)
{
	// select neurontype and check if a bytecodemapping exists
	QString neuronType = 	netNeuron->getTransferFunction()->getName() 
			+ ";" 
			+ netNeuron->getActivationFunction()->getName();
		
	if(netNeuron->isActivationFlipped()){
		neuronType = neuronType + FLIP_TYPE;
	}
		
		// check if neuron type exists in bytecodemapping, if not --> error
	if(!m_byteCodeMapping.contains(neuronType))
	{			
		*errorMsg = QString("The neuron \"%1\" has an unknown activation and transfer function pair \"%2\"! Use a known pair or add a new bytecode mapping to the function \"createByteCodeMapping()\".\nThe following pairs are known:\n%3").arg(	
												netNeuron->getNameValue().get(), 
												neuronType,
												getNeuronByteCodeMappingTypes());
			
		return *errorMsg;
	}
	
	return neuronType;
}


bool NeuralNetworkBDNExporter::addBDNBiasInfo(BDNNeuronInfo *target, double weight, int position)
{
	BDNNeuronInfo *biasNeuronInfo = new BDNNeuronInfo();
	
	biasNeuronInfo->ID = m_nextXmlId++;
	biasNeuronInfo->Type = "Bias";
	biasNeuronInfo->Guid = "Bias";
	biasNeuronInfo->XPosition = target->XPosition - ADDITIONAL_BDN_NEURON_DISTANCE;
	biasNeuronInfo->YPosition = target->YPosition - ADDITIONAL_BDN_NEURON_DISTANCE;
	biasNeuronInfo->RobotType = m_robotType;
	
	int synapseID = m_nextXmlId++;
	addBDNSynapseInfo(
			synapseID,
			biasNeuronInfo->ID,
			target->ID,
			weight,
			m_standardBiasSynapseType,
	 		position);
	
	biasNeuronInfo->OutSynapseIDs.append(synapseID);
	target->InSynapseIDs.append(synapseID);
	
	m_BDNNeuronInfoList.append(biasNeuronInfo);
	
	return true;
}


bool NeuralNetworkBDNExporter::addBDNSynapseInfo(Synapse *netSynapse, QString *errorMsg)
{
	if(netSynapse->getEnabledValue().get() == false) {								
		return false;
	}
	
	int sourceID = m_xmlIdTable[netSynapse->getSource()];
	int targetID = m_xmlIdTable[netSynapse->getTarget()];
	double weight = netSynapse->getStrengthValue().get();
	QString type = netSynapse->getSynapseFunction()->getName();
	
	
		// check if the synapse type exists in bytecodemapping, if not --> error
	if(!m_byteCodeMapping.contains(type))
	{
		*errorMsg = QString("A synapse of the neuron \"%1\" has an unknown synaptic function \"%2\"! Use a known function or add a new bytecode mapping to the function \"createByteCodeMapping()\".\nThe following functions are known:\n%3").arg(	
												netSynapse->getSource()->getNameValue().get(), 
												netSynapse->getSynapseFunction()->getName(),
												getSynapseByteCodeMappingTypes());
		
		return false;
	}
	
	return addBDNSynapseInfo(	m_xmlIdTable[netSynapse], 
														sourceID, 
														targetID, 
														weight, 
														type,
														m_executionPositionTable[netSynapse->getTarget()] - 1);
}


bool NeuralNetworkBDNExporter::addBDNSynapseInfo(int synapseID, int source, int target, double weight, QString type, int executionPosition)
{
	BDNSynapseInfo *synapseInfo = new BDNSynapseInfo();
	
	synapseInfo->ID = synapseID;
	synapseInfo->TargetID = target;
	synapseInfo->SourceID = source;
	synapseInfo->Weight = weight;
	synapseInfo->Type = type + QString::number(executionPosition);
	
	m_BDNSynapseInfoList.append(synapseInfo);
	
	addByteCodePosition(type, QString::number(executionPosition));
	
	return true;
}

QList<int> NeuralNetworkBDNExporter::getBDNInSynapseInformation(Neuron *netNeuron, QString *errorMsg)
{
	QList<int> inSynapseInformation = QList<int>();
	
	// create synapse information for neuron if needed
	const QList<Synapse*>& netNeuronSynapsesIn = netNeuron->getSynapses();
					
	// information for input synapses
	QListIterator<Synapse*> netNeuronSynapsesInIt(netNeuronSynapsesIn);
	while(netNeuronSynapsesInIt.hasNext()) {
		Synapse *netNeuronSynapse = netNeuronSynapsesInIt.next();
		
		if(netNeuronSynapse->getEnabledValue().get() == false) {								
			continue;
		}
		
		inSynapseInformation.append(m_xmlIdTable.value(netNeuronSynapse));
	}
	
	return inSynapseInformation;
}

QList<int> NeuralNetworkBDNExporter::getBDNOutSynapseInformation(Neuron *netNeuron, QString *errorMsg)
{
	QList<int> outSynapseInformation = QList<int>();
	
	const QList<Synapse*>& netNeuronSynapsesOut = netNeuron->getOutgoingSynapses();
		
	// information for output synapses
	QListIterator<Synapse*> netNeuronSynapsesOutIt(netNeuronSynapsesOut);
	while(netNeuronSynapsesOutIt.hasNext()) {
		Synapse *netNeuronSynapse = netNeuronSynapsesOutIt.next();
		
		if(netNeuronSynapse->getEnabledValue().get() == false) {								
			continue;
		}
		
		outSynapseInformation.append(m_xmlIdTable.value(netNeuronSynapse));
	}
	
	return outSynapseInformation;
}


int NeuralNetworkBDNExporter::getBDNXPos(Neuron *netNeuron)
{
	int orgXPos = static_cast<int>(netNeuron->getPosition().getX());
	
	return (orgXPos + m_bdnXPosOffset) * m_bdnXPosScaling;
}


int NeuralNetworkBDNExporter::getBDNYPos(Neuron *netNeuron)
{
	int orgXPos = static_cast<int>(netNeuron->getPosition().getY());
	
	return (orgXPos + m_bdnYPosOffset) * m_bdnYPosScaling;
}

QString NeuralNetworkBDNExporter::getSpinalCordAddress(Neuron *netNeuron, QString *errorMsg)
{
	if(!netNeuron->hasProperty(STANDARD_POSITION_PROPERTY_NAME)) {
		*errorMsg = QString("The neuron \"%1\" has not the needed property \"%2\"!. ").arg(
												netNeuron->getNameValue().get(),
												STANDARD_POSITION_PROPERTY_NAME);
		return QString::null;
	}
			
	QString spinalCordAddress = netNeuron->getProperty(STANDARD_POSITION_PROPERTY_NAME);
	
	// check if an extra spinal cord mapping was provided
	if(m_extraSpinalCordMapping.contains(spinalCordAddress)){
		spinalCordAddress = m_extraSpinalCordMapping[spinalCordAddress];
	}
	
	return spinalCordAddress;
}


/**
 * Returns the type names of the bytecode mappings for neurons. Used for error messages.
 */
QString NeuralNetworkBDNExporter::getNeuronByteCodeMappingTypes()
{
	QString retval = "";
	
	QMapIterator<QString, QString> byteCodeMappingIter(m_byteCodeMapping);
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
QString NeuralNetworkBDNExporter::getSynapseByteCodeMappingTypes()
{
	QString retval = "";
	
	QMapIterator<QString, QString> byteCodeMappingIter(m_byteCodeMapping);
	while (byteCodeMappingIter.hasNext()) {
		byteCodeMappingIter.next();
		
		if(!byteCodeMappingIter.key().contains(";"))
		{
			retval += " - " + byteCodeMappingIter.key()  + "\n";
		}
	}
	
	return retval;
}


QDomElement BDNNeuronInfo::toXML(QDomDocument &xmlDoc)
{
	QDomElement xmlNeuron = xmlDoc.createElement( "node" ); 
	
	xmlNeuron.setAttribute( "id", this->ID );
	xmlNeuron.setAttribute( "type", this->Type );
	xmlNeuron.setAttribute( "guid", this->Guid );
	
	if(this->Type == "Input" || this->Type == "Output")
	{	
		QDomElement xmlNeuronParameters = xmlDoc.createElement( "parameters" );
		xmlNeuron.appendChild(xmlNeuronParameters);
			
		QDomElement xmlNeuronTypeParameter = xmlDoc.createElement( "parameter" );
		if(this->Type == "Input"){
			xmlNeuronTypeParameter.setAttribute( "name", "Input Type");
		} else {
			xmlNeuronTypeParameter.setAttribute( "name", "Output Type");
		}
		xmlNeuronTypeParameter.setAttribute( "value", this->RobotType);
		xmlNeuronTypeParameter.setAttribute( "show", "Standard");
		xmlNeuronParameters.appendChild(xmlNeuronTypeParameter);
						
		QDomElement xmlNeuronPosParameter = xmlDoc.createElement( "parameter" );
		xmlNeuronPosParameter.setAttribute( "name", "Position");
		xmlNeuronPosParameter.setAttribute( "value", this->SpinalCordAddress );
		xmlNeuronPosParameter.setAttribute( "show", "Standard");
		xmlNeuronParameters.appendChild(xmlNeuronPosParameter);
	}
	
	QDomElement xmlPosition = xmlDoc.createElement( "position" );
	xmlPosition.setAttribute( "x", this->XPosition );
	xmlPosition.setAttribute( "y", this->YPosition );
	xmlNeuron.appendChild(xmlPosition);
	
	QDomElement xmlConnectedEdges = xmlDoc.createElement("connectedEdges");
	
	QListIterator<int> neuronSynapsesInIt(this->InSynapseIDs);
	while(neuronSynapsesInIt.hasNext()) {
			
		int inSynapseID = neuronSynapsesInIt.next();
					
		QDomElement xmlConnectionPoint = xmlDoc.createElement("connectionPoint");
		
		int connectionPos = 0;
		
		if(this->Type == "Unit"){
			connectionPos = 2;
		}else if(this->Type == "Output"){
			connectionPos = 1;
		}
		
		xmlConnectionPoint.setAttribute( "id", connectionPos );
		xmlConnectedEdges.appendChild(xmlConnectionPoint);
				
		QDomElement xmlConnectedEdge = xmlDoc.createElement("connectedEdge");
		xmlConnectedEdge.setAttribute( "id", inSynapseID );
		xmlConnectedEdge.setAttribute( "start", "False" );
		xmlConnectionPoint.appendChild(xmlConnectedEdge);
	}
	
	QListIterator<int> neuronSynapsesOutIt(this->OutSynapseIDs);
	while(neuronSynapsesOutIt.hasNext()) {
			
		int outSynapseID = neuronSynapsesOutIt.next();
					
		QDomElement xmlConnectionPoint = xmlDoc.createElement("connectionPoint");
		
		int connectionPos = 0;
		
		if(this->Type == "Unit"){
			connectionPos = 6;
		}else if(this->Type == "Bias" || this->Type == "Input"){
			connectionPos = 1;
		}
		
		xmlConnectionPoint.setAttribute( "id", connectionPos );
		
		xmlConnectedEdges.appendChild(xmlConnectionPoint);
				
		QDomElement xmlConnectedEdge = xmlDoc.createElement("connectedEdge");
		xmlConnectedEdge.setAttribute( "id", outSynapseID );
		xmlConnectedEdge.setAttribute( "start", "True" );
		xmlConnectionPoint.appendChild(xmlConnectedEdge);
	}
	
	// append Synapse information if needed
	if(xmlConnectedEdges.hasChildNodes() == true){
		xmlNeuron.appendChild(xmlConnectedEdges);
	}
	
	return xmlNeuron;
}


QDomElement BDNSynapseInfo::toXML(QDomDocument &xmlDoc)
{
	QDomElement xmlSynapse = xmlDoc.createElement( "edge" ); 
	
	xmlSynapse.setAttribute( "id", this->ID );
	xmlSynapse.setAttribute( "guid", this->Type);
		
	QDomElement xmlSynapseStart = xmlDoc.createElement( "start" );	
	xmlSynapseStart.setAttribute( "nodeId", this->SourceID );
	xmlSynapse.appendChild(xmlSynapseStart);
		
	QDomElement xmlSynapseEnd = xmlDoc.createElement( "end" );	
	xmlSynapseEnd.setAttribute( "nodeId", this->TargetID );
	xmlSynapse.appendChild(xmlSynapseEnd);
		
	QDomElement xmlSynapseRoundness = xmlDoc.createElement( "roundness" );	
	xmlSynapseRoundness.setAttribute( "height", 0 );
	xmlSynapseRoundness.setAttribute( "width", 0 );
	xmlSynapse.appendChild(xmlSynapseRoundness);
		
	QDomElement xmlSynapseParas = xmlDoc.createElement( "parameters" );	
	xmlSynapse.appendChild(xmlSynapseParas);
			
	QDomElement xmlSynapsePara = xmlDoc.createElement( "parameter" );	
	xmlSynapsePara.setAttribute("name", "w");
	xmlSynapsePara.setAttribute("value", this->Weight);
	xmlSynapseParas.appendChild(xmlSynapsePara);
	
	return xmlSynapse;
}


}
