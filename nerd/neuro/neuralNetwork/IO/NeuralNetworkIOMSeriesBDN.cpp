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

#include "NeuralNetworkIOMSeriesBDN.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "IO/NeuralNetworkBDNExporter.h"
#include "Core/Core.h"

namespace nerd {
		
// Name of the synapse function which is used for the bias elements
const QString NeuralNetworkIOMSeriesBDN::STANDARD_BIAS_SYNAPSE_TYPE = "MSeries";

// Name of the robot type which is used for the BDN network input and output elements
const QString NeuralNetworkIOMSeriesBDN::ROBOT_TYPE = "M-Series";

	
/**
 * Creates the mapping of the neuron and synapse types to the bytecode which is used 
 * by the M-Series to represet them.
 * 
 * This function must be adapted to new neuron or synapse types.
 * 
 * @return Table with mapping between neuron/synapse type and bytecode.
 */
QMap<QString, QString> NeuralNetworkIOMSeriesBDN::createByteCodeMapping()
{
	QMap<QString, QString> byteCodeMapping;
	
	// Neurontypes
	// 
	// The type is defined by its TransferFunction and ActivationFunction name.
	// The format is: "<TransferFunctionName>;<ActivationFunctionName>"
	// 
	// If the function is flipable, then a further flip type must be provided.
	// Format: "<TransferFunctionName>;<ActivationFunctionName><NeuralNetworkIOMSeriesBDN::FLIP_TYPE>"
	
// 	// tanh function and flipped function
// 	byteCodeMapping.insert("tanh;AdditiveTD",
// 												  NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
// 															+ ":\ntanh V0, Input\nwrite Output, V0");
// 	
// 	byteCodeMapping.insert(	"tanh;AdditiveTD" + NeuralNetworkBDNExporter::FLIP_TYPE, 	
// 													NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
// 														 + ":\ntanh V0, Input\nLOAD V1, -1.0\nMUL V1, V0, V1\nwrite Output, V1");
// 														
// 	byteCodeMapping.insert("ramp[0,1];AdditiveTD",
// 							NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
// 							+ ":\nload v1, 0.0\nmin V0, Input, V1\nload V1, 1.0\nmin V0, V0, V1\nwrite Output, V0");
// 							
// 	byteCodeMapping.insert("ramp[-1,1];AdditiveTD",
// 							NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
// 							+ ":\nload V0, Input\nsat V0\nwrite Output, V0");
// 							
// 	byteCodeMapping.insert("ramp[-1,1];AdditiveTD" + NeuralNetworkBDNExporter::FLIP_TYPE,
// 							NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
// 							+ ":\nload V0, Input\nload V1, -1.0\nmul V0, V0, V1\nsat V0\nwrite Output, V0");
//
// 	byteCodeMapping.insert("SimpleUpdateFunction", 	
// 												 NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
// 														+ ":\nmul   V0, Input, w\nwrite Output, V0");
	
	// tanh function and flipped function
	byteCodeMapping.insert("MSeriesTanh;MSeries",
							NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
							+ ":\ntanh V0, Input\nwrite Output, V0");
	
	byteCodeMapping.insert(	"MSeriesTanh;MSeries" + NeuralNetworkBDNExporter::FLIP_TYPE, 	
							NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
							+ ":\ntanh V0, Input\nLOAD V1, -1.0\nMUL V1, V0, V1\nwrite Output, V1");
														
	byteCodeMapping.insert("ramp[0,1];MSeries",
							NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
							+ ":\nload V1, 1.0\nmin V0, Input, V1\nload V1, 0.0\nmax V0, V0, V1\nwrite Output, V0");
							
	byteCodeMapping.insert("ramp[-1,1];MSeries",
							NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
							+ ":\nload V0, Input\nsat V0\nwrite Output, V0");
							
	byteCodeMapping.insert("ramp[-1,1];MSeries" + NeuralNetworkBDNExporter::FLIP_TYPE,
							NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
							+ ":\nload V0, Input\nload V1, -1.0\nmul V0, V0, V1\nsat V0\nwrite Output, V0");
		
	// Synapsetypes
	// 
	// The type is defined by its SynapseFunction name.
	// The format is: "<SynapseFunctionName>"
	byteCodeMapping.insert("MSeries", 	
							NeuralNetworkBDNExporter::BYTECODE_POS_FLAG 
								+ ":\nmul   V0, Input, w\nwrite Output, V0");
	
	return byteCodeMapping;
}
	
/**
 * Try to export a neural network into a MSeries BDN (Brain Designer Network) File.
 * 
 * @param fileName the file to write to.
 * @param net the net to export.
 * @param errorMsg returns an errorMsg if the function fails with some user-relevant information.
 * @return true if the function is successful, otherwise false.
 */
bool NeuralNetworkIOMSeriesBDN::createFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg) {

	NeuralNetworkBDNExporter *bdnExporter = new NeuralNetworkBDNExporter( 
			STANDARD_BIAS_SYNAPSE_TYPE,
	 		ROBOT_TYPE,
			createByteCodeMapping());
	
	bool ok = bdnExporter->createFileFromNetwork(fileName, net, errorMsg);
	
	delete bdnExporter;
	
	return ok;
}

}
