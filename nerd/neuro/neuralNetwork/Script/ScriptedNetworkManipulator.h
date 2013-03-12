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



#ifndef NERDScriptedNetworkManipulator_H
#define NERDScriptedNetworkManipulator_H

#include <QString>
#include <QHash>
#include <QScriptEngine>
#include <QObject>
#include "PlugIns/CommandLineArgument.h"
#include <QVariantList>
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Event/Event.h"

namespace nerd {

class NeuroModulatorElement;

class NeuroModulator;

	/**
	 * ScriptedNetworkManipulator.
	 *
	 */
	class ScriptedNetworkManipulator : public QObject {
	Q_OBJECT
	public:
		ScriptedNetworkManipulator();
		virtual ~ScriptedNetworkManipulator();

	public slots:
		void setNeuralNetwork(ModularNeuralNetwork *network);
		
		//TODO provide a method to setNeuralNetwork in the script, e.g. using a name or an index of the available networks.
		//TODO provide a method to communicate with the NetworkManager (getNumberOfNetworks, etc.)
		
		void setOwnerHint(NeuralNetworkElement *owner);
		NeuralNetworkElement* getOwnerHint() const;
		
		bool saveNetwork(const QString &name);
		bool loadNetwork(const QString &name);

		qulonglong getNeuron(const QString &name);
		qulonglong getNeuroModule(const QString &name);
		qulonglong getNeuronGroup(const QString &name);

		QVariantList getNeurons(qulonglong groupId = 0, bool allEnclosed = false);
		QVariantList getSynapses();
		QVariantList getGroupsAndModules();
		QVariantList getGroups();
		QVariantList getModules();
		QVariantList getSubModules(qulonglong groupId);
		qulonglong getParentModule(qulonglong elementId);

		QVariantList getSynapses(QVariantList elementIds, bool includeFromExternals = false, 
								 bool includeToExternals = false, bool excludeDisabledSynapses = true);
		QVariantList getInSynapses(qulonglong targetId, bool excludeDisabledSynapses = true);
		QVariantList getOutSynapses(qulonglong neuronId, bool excludeDisabledSynapses = true);
		QVariantList getInSynapses(QVariantList targetIds, bool includeFromExternals = false, bool excludeDisabledSynapses = true);
		QVariantList getOutSynapses(QVariantList neuronIds, bool includeToExternals = false, bool excludeDisabledSynapses = true);
		qulonglong getSource(qulonglong synapseId);
		qulonglong getTarget(qulonglong synapseId);

		double getBias(qulonglong neuronId);
		double getWeight(qulonglong synapseId);
		double getSynapseOutput(qulonglong synapseId);
		double execSynapseFunction(qulonglong synapseId);
		double getActivation(qulonglong neuronId);
		double getLastActivation(qulonglong neuronId);
		double getOutput(qulonglong neuronId);
		double getLastOutput(qulonglong neuronId);
		QVariantList getPosition(qulonglong objectId);
		QVariantList getProperties(qulonglong objectId);
		QString getProperty(qulonglong objectId, const QString &name);
		QVariantList getNetworkTagNames(qulonglong objectId);
		QString getNetworkTag(qulonglong objectId, const QString &name);
		QString getName(qulonglong neuronId);

		bool setBias(qulonglong neuronId, double bias);
		bool setOutput(qulonglong neuronId, double output);
		bool setActivation(qulonglong neuronId, double activation);
		bool setWeight(qulonglong synapseId, double weight);
		bool setPosition(qulonglong objectId, double x, double y, double z);
		bool setProperty(qulonglong objectId, const QString &propName, const QString &content, bool severeChange = false);
		bool setNetworkTag(qulonglong objectId, const QString &tagName, const QString &content, bool severeChange = false);
		bool setName(qulonglong neuronId, const QString &name);

		bool addToGroup(qulonglong elementId, qulonglong groupId);
		bool removeFromGroup(qulonglong elementId, qulonglong groupId);

		qulonglong createNeuron(const QString &name = "");
		qulonglong createSynapse(qulonglong sourceId, qulonglong targetId, double weight);
		qulonglong createNeuroModule(const QString &name = "");
		qulonglong createNeuronGroup(const QString &name = "Group");

		bool removeNeuron(qulonglong neuronId);
		bool removeSynapse(qulonglong synapseId);
		bool removeNeuronGroup(qulonglong groupId);
		bool removeProperty(qulonglong objectId, const QString &propertyName, bool severeChange = false);
		bool removeNetworkTag(qulonglong objectId, const QString &tagName, bool severeChange = false);
		
		bool setTransferFunction(qulonglong neuronId, const QString &transferFunctionName);
		QString getTransferFunctionName(qulonglong neuronId);
		bool setActivationFunction(qulonglong neuronId, const QString &activationFunctionName);
		QString getActivationFunctionName(qulonglong neuronId);
		bool setSynapseFunction(qulonglong synapseId, const QString &synapseFunctionName);
		QString getSynapseFunctionName(qulonglong synapseId);
		
		bool setTransferFunctionParameter(qulonglong neuronId, const QString &parameterName, const QString &value);
		QString getTransferFunctionParameter(qulonglong neuronId, const QString &parameterName);
		bool setActivationFunctionParameter(qulonglong neuronId, const QString &parameterName, const QString &value);
		QString getActivationFunctionParameter(qulonglong neuronId, const QString &parameterName);
		bool setSynapseFunctionParameter(qulonglong synapseId, const QString &parameterName, const QString &value);
		QString getSynapseFunctionParameter(qulonglong synapseId, const QString &parameterName);
		
		double applyTransferFunction(qulonglong neuronId, double activity);
		double tf(qulonglong neuronId, double activity);
		double getDistance(QVariantList pos1, QVariantList pos2);
		
		//TODO
		//set and get TF, AF, SF
		//insert module from library
		//resize of modules
		
		//Neuromodulator Framework
		void enableNeuroModulators(qulonglong elementId, bool enable);
		bool isNeuroModulationEnabled(qulonglong elementId);
 		void setModulatorArea(qulonglong elementId, int type, double width, double height, bool circle = true);
		void setModulatorAreaReferenceModule(qulonglong elementId, int type, qulonglong moduleId);
		QVariantList getModulatorArea(qulonglong elementId, int type);
		void setMaxConcentration(qulonglong elementId, int type, double maxConcentration);
		double getMaxConcentration(qulonglong elementId, int type);
		void setModulatorConcentration(qulonglong elementId, int type, double concentration);
		double getModulatorConcentration(qulonglong elementId, int type);
		double getModulatorConcentrationAt(qulonglong elementId, int type, double x, double y, double z);
		double getModulatorConcentrationAt(int type, double x, double y, double z);
		double getModulatorConcentrationInNetwork(int type);
		void setModulatorDistributionModus(qulonglong elementId, int type, int modus);
		int getModulatorDistributionModus(qulonglong elementId, int type = -1);
		void setModulatorUpdateModus(qulonglong elementId, int type, int modus);
		int getModulatorUpdateModus(qulonglong elementId, int type = -1);
		bool setModulatorUpdateParameters(qulonglong elementId, int modus, QVariantList params);
		QVariantList getModulatorUpdateParameters(qulonglong elementId, int type);
		QVariantList getModulatorUpdateParameterNames(qulonglong elementId, int type);
		QVariantList getModulatorUpdateVariables(qulonglong elementId, int type);
		QVariantList getModulatorUpdateVariableNames(qulonglong elementId, int type);
		
	protected:
		NeuroModulator* getNeuroModulator(qulonglong elementId, NeuralNetworkElement **element = 0, 
										  NeuroModulatorElement **modElem = 0);
		NeuroModulatorElement* getNeuroModulatorElement(NeuralNetworkElement *elem);

	private:
		ModularNeuralNetwork *mNetwork;
		Event *mClearNetworkModificationStacks;
		NeuralNetworkElement *mOwner;
	};

}

#endif



