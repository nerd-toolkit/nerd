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

#include "NeuralNetworkIOASeriesBDN.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Core/Core.h"
#include "IO/NeuralNetworkBDNExporter.h"


namespace nerd {
	
	
// Name of the synapse function which is used for the bias elements
const QString NeuralNetworkIOASeriesBDN::STANDARD_BIAS_SYNAPSE_TYPE = 	"ASeries";

// Name of the robot type which is used for the BDN network input and output elements
const QString NeuralNetworkIOASeriesBDN::ROBOT_TYPE = "A-Series";
	
	
/**
 * Creates the mapping of the neuron and synapse types to the bytecode which is used 
 * by the A-Series to represet them.
 * 
 * This function must be adapted to new neuron or synapse types.
 * 
 * @return Table with mapping between neuron/synapse type and bytecode.
 */
QMap<QString, QString> NeuralNetworkIOASeriesBDN::createByteCodeMapping()
{
	QMap<QString, QString> byteCodeMapping;
	
	// Neurontypes
	// 
	// The type is defined by its TransferFunction and ActivationFunction name.
	// The format is: "<TransferFunctionName>;<ActivationFunctionName>"
	byteCodeMapping.insert("ASeriesTanh;ASeries", 	
												 NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
														 + ":\ntanh V0, Input\nwrite Output, V0");
	
	byteCodeMapping.insert(	"ASeriesTanh;ASeries" + NeuralNetworkBDNExporter::FLIP_TYPE, 	
													NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
															+ ":\ntanh V0, Input\nLOAD V1, -1.0\nMUL V1, V0, V1\nwrite Output, V1");

	// Synapsetypes
	// 
	// The type is defined by its SynapseFunction name.
	// The format is: "<SynapseFunctionName>"
	byteCodeMapping.insert("SimpleUpdateFunction", 	
												 NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
														 + ":\nmul   V0, Input, w\nwrite Output, V0");
	byteCodeMapping.insert("ASeries", 							
												 NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
														 + ":\nmul   V0, Input, w\nwrite Output, V0");
	byteCodeMapping.insert("ASeriesMulti", 					
												 NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
														 + ":\nmul   V0, Input, w\nwrite Output, V0");

	return byteCodeMapping;
}


/**
 * The motoroutput neurons for the ASeries in the BrainDesigner are specified by their
 * SpinalCord position. Instead, they have another position (See the SpinalCord Documentation). 
 * 
 * This method maps the SpinalChord Position into the motor position and must be adjusted 
 * if changes occure on the spinalcord mapping.
 * @param spinalCordPos SpinalCordPosition for which a mapping should be found.
 * @return Mapping from SpinalCordPosition to the position which is used in the BrainDesigner.
 */
QMap<QString, QString> NeuralNetworkIOASeriesBDN::getSpinalCordMapping()
{
	QMap<QString, QString> spinalCordMapping;

	spinalCordMapping.insert("16","21");
	spinalCordMapping.insert("17","40");
	spinalCordMapping.insert("18","41");

	spinalCordMapping.insert("29","0");
	spinalCordMapping.insert("30","19");
	spinalCordMapping.insert("31","20");
	
	// Left Arm
	spinalCordMapping.insert("32","23");
	spinalCordMapping.insert("33","25");
	spinalCordMapping.insert("34","27");
	
	spinalCordMapping.insert("45","2");
	spinalCordMapping.insert("46","4");
	spinalCordMapping.insert("47","6");
	
	// Right Arm
	spinalCordMapping.insert("48","22");
	spinalCordMapping.insert("49","24");
	spinalCordMapping.insert("50","26");
	
	spinalCordMapping.insert("61","1");
	spinalCordMapping.insert("62","3");
	spinalCordMapping.insert("63","5");
	
	// Hip Left
	spinalCordMapping.insert("64","29");
	spinalCordMapping.insert("65","31");
	spinalCordMapping.insert("66","33");
	
	spinalCordMapping.insert("77","8");
	spinalCordMapping.insert("78","10");
	spinalCordMapping.insert("79","12");

	// Hip Right
	spinalCordMapping.insert("80","28");
	spinalCordMapping.insert("81","30");
	spinalCordMapping.insert("82","32");

	spinalCordMapping.insert("93","7");
	spinalCordMapping.insert("94","9");
	spinalCordMapping.insert("95","11");
		
	// Foot Left
	spinalCordMapping.insert("96","35");
	spinalCordMapping.insert("97","37");
	spinalCordMapping.insert("98","39");

	spinalCordMapping.insert("109","14");
	spinalCordMapping.insert("110","16");
	spinalCordMapping.insert("111","18");

	// Foot Right
	spinalCordMapping.insert("112","34");
	spinalCordMapping.insert("113","36");
	spinalCordMapping.insert("114","38");

	spinalCordMapping.insert("125","13");
	spinalCordMapping.insert("126","15");
	spinalCordMapping.insert("127","17");
	
	return spinalCordMapping;
}

/**
 * Try to export a neural network to the ASeries Bytecode.
 * 
 * @param fileName the file to write to.
 * @param net the net to export.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkIOASeriesBDN::createFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg) {
	
	NeuralNetworkBDNExporter *bdnExporter = new NeuralNetworkBDNExporter( 
			STANDARD_BIAS_SYNAPSE_TYPE,
	 		ROBOT_TYPE,
			createByteCodeMapping());
	
	// set extra spinal cord mapping
	bdnExporter->setExtraSpinalCordMapping(getSpinalCordMapping());
	
	bool ok = bdnExporter->createFileFromNetwork(fileName, net, errorMsg);
	
	delete bdnExporter;
	
	return ok;
}

}
