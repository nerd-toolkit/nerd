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

#include "NeuralNetworkIOMSeriesBDN.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "IO/NeuralNetworkBDNExporter.h"
#include "Core/Core.h"

namespace nerd {
		
// Name of the synapse function which is used for the bias elements
const QString NeuralNetworkIOMSeriesBDN::STANDARD_BIAS_SYNAPSE_TYPE = 	"SimpleUpdateFunction";

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
	
	
	// tanh function and flipped function
	byteCodeMapping.insert("tanh;AdditiveTD", "300:\ntanh V0, Input\nwrite Output, V0");
	
	byteCodeMapping.insert(	"tanh;AdditiveTD" + NeuralNetworkBDNExporter::FLIP_TYPE, 	
													"300:\ntanh V0, Input\nLOAD V1, -1.0\nMUL V1, V0, V1\nwrite Output, V1");
		
	// Synapsetypes
	// 
	// The type is defined by its SynapseFunction name.
	// The format is: "<SynapseFunctionName>"
	byteCodeMapping.insert("SimpleUpdateFunction", 	"200:\nmul   V0, Input, w\nwrite Output, V0");
	
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
