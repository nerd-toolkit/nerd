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

#ifndef NeuralNetworkBDNExporter_H
#define NeuralNetworkBDNExporter_H

#include "Network/NeuralNetwork.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include <QString>
#include <QMap>
#include <QHash>
#include <QFile>
#include <QTextStream>
#include <QDomDocument>


class QDomElement;

namespace nerd {

	class BDNNeuronInfo {
		public:
			int ID;
			QList<int> InSynapseIDs;
			QList<int> OutSynapseIDs;
			int XPosition;
			int YPosition;
			QString Type;
			QString Guid;
			QString SpinalCordAddress;
			QString RobotType;
			QString Name;
			
		public:
			BDNNeuronInfo() {ID = 0;InSynapseIDs=QList<int>(); OutSynapseIDs=QList<int>();XPosition=-1;YPosition=-1;Type=QString::null;Guid=QString::null;SpinalCordAddress=QString::null;RobotType=QString::null;Name=QString::null;}
			
			QDomElement toXML(QDomDocument &xmlDoc);
	};
	
	class BDNSynapseInfo {
		public:
			int ID;
			int TargetID;
			int SourceID;
			double Weight;
			QString Type;
			
		public:
			BDNSynapseInfo() {ID=-1;TargetID=-1;SourceID=-1;Weight=0.0;Type=QString::null;}
			
			QDomElement toXML(QDomDocument &xmlDoc);
	};
	
	
	/**
	 * NeuralNetworkBDNExporter
	 * Converts a ModularNeuralNetwork into a library file for the BrainDesigner.   
	 */
	class NeuralNetworkBDNExporter {
		public:
			static const QString FLIP_TYPE;
			static const QString BYTECODE_POS_FLAG;
			static const QString BYTECODE_MIN_FLAG;
			static const QString BYTECODE_MAX_FLAG;
			static const QString TAG_BDN_INTERFACE_INPUT;
			static const QString TAG_BDN_INTERFACE_OUTPUT;
			
		private:
			static const QString STANDARD_POSITION_PROPERTY_NAME;
			static const QString INPUT_MIRROR_GUID;
			static const QString INPUT_MIRROR_BYTE_CODE;
			static const QString INPUT_MIRROR_FLIP_GUID;
			static const QString INPUT_MIRROR_FLIP_BYTE_CODE;
			static const QString MODULE_INPUT_GUID;
			static const QString MODULE_OUTPUT_GUID;
			static const int BDN_DRAWING_AREA_X_LENGTH;
			static const int BDN_DRAWING_AREA_Y_LENGTH;
			static const int ADDITIONAL_BDN_NEURON_DISTANCE;
			static const int BDN_MIN_EXECUTION_POS;
			static const int BDN_MAX_EXECUTION_POS;
				
		public:
			NeuralNetworkBDNExporter(QString standardBiasSynapseType, QString robotType, QMap<QString, QString> byteCodeMapping);
			
			~NeuralNetworkBDNExporter();
			
			bool createFileFromNetwork(QString fileName, NeuralNetwork *net, QString *errorMsg);
			
			bool checkIfNetworkIsExportable(NeuralNetwork *net, QString *errorMsg);

			QString exportNetwork(QString networkName, ModularNeuralNetwork *net, QString *errorMsg);
			
			void setExtraSpinalCordMapping(QMap<QString, QString> extraSpinalCordMapping);
			
		private:
			bool updateXmlIdTable(ModularNeuralNetwork *net, QString *errorMsg);
			bool calcNeuronPositionValues(ModularNeuralNetwork *net, QString *errorMsg);
			bool calcExecutionPositions(ModularNeuralNetwork *net, QString *errorMsg);
			bool determineBrainDesignerInterfaceNeurons(ModularNeuralNetwork *net, QString *errorMsg);
			bool createBDNHeadInformation(QDomElement &xmlRoot);
			bool createSurroundingModule(QDomDocument &xmlDoc, QDomElement &xmlRoot, const QString &networkName);
			void addByteCodePosition(const QString &modulName, const QString &position);
			
			bool addBDNNeuronInfo(Neuron *netNeuron, QString *errorMsg);
			bool addInputBDNNeuronInfo(Neuron *inNeuron, QString *errorMsg);
			bool addOutputBDNNeuronInfo(Neuron *outNeuron, QString *errorMsg);
			bool addHiddenBDNNeuronInfo(Neuron *hiddenNeuron, QString *errorMsg);
			
			bool addBDNBiasInfo(BDNNeuronInfo *target, double weight, int position);
			
			bool addBDNSynapseInfo(Synapse *netSynapse, QString *errorMsg);
			bool addBDNSynapseInfo(int synapseID, int source, int target, double weight, QString type, int executionPosition);
			
			QList<int> getBDNInSynapseInformation(Neuron *netNeuron, QString *errorMsg);
			QList<int> getBDNOutSynapseInformation(Neuron *netNeuron, QString *errorMsg);
			
			int getBDNXPos(Neuron *netNeuron);
			int getBDNYPos(Neuron *netNeuron);
			
			QString getBDNNeuronType(Neuron *netNeuron, QString *errorMsg);
			QString getSpinalCordAddress(Neuron *netNeuron, QString *errorMsg);
			
			QString getNeuronByteCodeMappingTypes();
			QString getSynapseByteCodeMappingTypes();
			
			void deleteWorkingVariables();
			
		private:
			QString m_standardBiasSynapseType;
			QString m_robotType;
			QMap<QString, QString> m_byteCodeMapping;
			QMap<QString, QString> m_extraSpinalCordMapping;
			
			QHash<NeuralNetworkElement*, int> m_xmlIdTable;
			QHash<NeuralNetworkElement*, int> m_executionPositionTable;
			QMap<QString, QList<QString> > m_byteCodeModulPositions;			
			QList<BDNNeuronInfo*> m_BDNNeuronInfoList;
			QList<BDNSynapseInfo*> m_BDNSynapseInfoList;
			int m_nextXmlId;
			int m_bdnXPosOffset;
			int m_bdnYPosOffset;
			double m_bdnXPosScaling;
			double m_bdnYPosScaling;
			int mNumberOfModuleInterfaceInputs;
			int mNumberOfModuleInterfaceOutputs;
	};

}

#endif
