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

#ifndef NERDNeuralNetworkIO_H
#define NERDNeuralNetworkIO_H

#include <QString>
#include <QDomElement>
#include "Network/NeuralNetwork.h"

namespace nerd {

	/**
	 * NeuralNetworkIO
	 *
	 */
	class NeuralNetworkIO {
	public:
		enum FileType { SimbaV3Xml, EvosunXml, PureEvosunXml, Bytecode, OrcsV1Xml, NerdV1Xml };

	public:
		static int getNeuronNumber(NeuralNetwork *net, Neuron *neuron);

		static bool createFileFromNetwork(const QString &fileName, NeuralNetwork *net, 
					FileType type, QString *errorMsg = 0, QList<QString> *warnings = 0);
		static bool createFileFromNetwork(const QString &fileName, NeuralNetwork *net, 
					QString *errorMsg = 0, QList<QString> *warnings = 0);


		static NeuralNetwork* createNetworkFromFile(const QString &fileName, FileType type, 
					QString *errorMsg = 0, QList<QString> *warnings = 0);
		static NeuralNetwork* createNetworkFromFile(const QString &fileName, 
					QString *errorMsg = 0, QList<QString> *warnings = 0);

		static QString getEvosunTransferFunctionNameFromNERD(QString nerdName);

		static QString getSimbaV3TransferFunctionNameFromNERD(QString nerdName);
		static QString getSimbaV3ActivationFunctionNameFromNERD(QString nerdName);
		static QString getSimbaV3SynapseFunctionNameFromNERD(QString nerdName);

		static QString getNERDTransferFunctionNameFromSimbaV3(QString simbaV3Name);
		static QString getNERDActivationFunctionNameFromSimbaV3(QString simbaV3Name);
		static QString getNERDSynapseFunctionNameFromSimbaV3(QString simbaV3Name);
	};

}

#endif
