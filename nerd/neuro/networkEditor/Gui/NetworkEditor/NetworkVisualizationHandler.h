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



#ifndef NERDNetworkVisualizationHandler_H
#define NERDNetworkVisualizationHandler_H

#include <QString>
#include <QHash>
#include "Core/ParameterizedObject.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"

namespace nerd {

	class SynapseItem;
	class NeuronItem;
	class Neuron;
	class Synapse;
	class ModuleItem;
	class NeuroModule;
	class GroupItem;
	class NeuronGroup;

	/**
	 * NetworkVisualizationHandler.
	 *
	 */
	class NetworkVisualizationHandler : public ParameterizedObject
	{
	public:
		NetworkVisualizationHandler(const QString &name, NetworkVisualization *owner);
		NetworkVisualizationHandler(const NetworkVisualizationHandler &other);
		virtual ~NetworkVisualizationHandler();

		virtual QString getName() const;

		virtual bool setNeuralNetwork(ModularNeuralNetwork *network);
		virtual ModularNeuralNetwork* getNeuralNetwork() const;

		virtual void rebuildView();

		virtual void conserveSelection() = 0;
		virtual void restoreSelection() = 0;


		virtual bool updateNetworkView() = 0;

		virtual NeuronItem* getNeuronItem(Neuron *neuron) = 0;
		virtual QList<NeuronItem*> getNeuronItems() const = 0;

		virtual SynapseItem* getSynapseItem(Synapse *synapse) = 0;
		virtual QList<SynapseItem*> getSynapseItems() const = 0;

		virtual ModuleItem* getModuleItem(NeuroModule *module) = 0;
		virtual QList<ModuleItem*> getModuleItems() const = 0;

		virtual GroupItem* getGroupItem(NeuronGroup *group) = 0;
		virtual QList<GroupItem*> getGroupItems() const = 0;

		virtual NeuronItem* getNeuronItemPrototype() const = 0;
		virtual SynapseItem* getSynapseItemPrototype() const = 0;
		virtual ModuleItem* getModuleItemPrototype() const = 0;
		virtual GroupItem* getGroupItemPrototype() const = 0;

		virtual void markElementsAsSelected(QList<NeuralNetworkElement*> elements) = 0;

	protected:
		NetworkVisualization *mOwner;
		ModularNeuralNetwork *mNeuralNetwork;
	};

}

#endif




