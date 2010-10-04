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

		qlonglong getNeuron(const QString &name);
		qlonglong getNeuroModule(const QString &name);
		qlonglong getNeuronGroup(const QString &name);

		QVariantList getNeurons(qlonglong groupId = 0, bool allEnclosed = false);
		QVariantList getSynapses();
		QVariantList getGroupsAndModules();
		QVariantList getGroups();
		QVariantList getModules();
		QVariantList getSubModules(qlonglong groupId);
		qlonglong getParentModule(qlonglong elementId);

		QVariantList getInSynapses(qlonglong neuronId);
		QVariantList getOutSynapses(qlonglong neuronId);
		qlonglong getSource(qlonglong synapseId);
		qlonglong getTarget(qlonglong synapseId);

		double getBias(qlonglong neuronId);
		double getWeight(qlonglong synapseId);
		double getActivation(qlonglong neuronId);
		double getOutput(qlonglong neuronId);
		QVariantList getPosition(qlonglong objectId);
		QVariantList getProperties(qlonglong objectId);
		QString getProperty(qlonglong objectId, const QString &name);

		bool setBias(qlonglong neuronId, double bias);
		bool setOutput(qlonglong neuronId, double output);
		bool setActivation(qlonglong neuronId, double activation);
		bool setWeight(qlonglong synapseId, double weight);
		bool setPosition(qlonglong objectId, double x, double y, double z);
		bool setProperty(qlonglong objectId, const QString &propName, const QString &content);

		bool addToGroup(qlonglong elementId, qlonglong groupId);
		bool removeFromGroup(qlonglong elementId, qlonglong groupId);

		qlonglong createNeuron(const QString &name = "Hidden");
		qlonglong createSynapse(qlonglong sourceId, qlonglong targetId, double weight);
		qlonglong createNeuroModule(const QString &name = "Module");
		qlonglong createNeuronGroup(const QString &name = "Group");

		bool removeNeuron(qlonglong neuronId);
		bool removeSynapse(qlonglong synapseId);
		bool removeNeuronGroup(qlonglong groupId);
		
		//TODO
		//set and get TF, AF, SF
		//insert module from library
		//resize of modules

	private:
		ModularNeuralNetwork *mNetwork;
		Event *mClearNetworkModificationStacks;
	};

}

#endif


