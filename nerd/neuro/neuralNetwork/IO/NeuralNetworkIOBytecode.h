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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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

#ifndef NERDNeuralNetworkIOBytecode_H
#define NERDNeuralNetworkIOBytecode_H

#include "Network/NeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include <QString>
#include <QFile>
#include <QTextStream>

namespace nerd {

	/**
	 * NeuralNetworkIOBytecode
	 */
	class NeuralNetworkIOBytecode {
	public:
		static const char* A_SERIES_ACCELBOARD_NAMES[8];
		static const char* SPINAL_CORD_ADDRESS_PROPERTY;
		static const char* REMOVE_SPINAL_CORD_ADDRESS_PROPERTY;
		static const char* MEMORY_NUMBER_PROPERTY;
	public:
		static bool createFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg);
		static QString createByteCodeFromNetwork(NeuralNetwork *net, QString *errorMsg);
		static bool createMotionEditorFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg);
		static bool checkIfNetworkIsExportable(NeuralNetwork *net, QString *errorMsg);
//		static NeuralNetwork* createNetFromFile(QString fileName, QString *errorMsg, QList<QString> *warnings);
	private:
		static bool checkOrExportNetwork(QTextStream &output, NeuralNetwork *net, QString *errorMsg);
		static void writeBytecode(QMap<QString, NeuroModule*> &accelboardModules, QTextStream &out);
		static bool isValidAccelboardName(QString name);
		static bool accelboardModuleHasRightInputOutputNeurons(NeuroModule *module, QString *errorMsg);
		static bool accelboardModuleHiddenNeuronCheck(NeuralNetwork *net, NeuroModule *module, QString *errorMsg, bool check);
		static bool isNeuronWithOutgoingConnections(NeuralNetwork *net, NeuroModule *module, Neuron *neuron);
		static void removeSpinalChordAddressesAndMemoryNumbers(NeuralNetwork *net);
	};

}

#endif
