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


#ifndef NERDModularNeuralNetwork_H
#define NERDModularNeuralNetwork_H


#include "Network/NeuralNetwork.h"
#include "NeuroModule.h"

namespace nerd {

	/**
	 * ModularNeuralNetwork
	 */
	class ModularNeuralNetwork : public NeuralNetwork {
	public:
		ModularNeuralNetwork(const ActivationFunction &defaultActivationFunction 
						= AdditiveTimeDiscreteActivationFunction(),
					  const TransferFunction &defaultTransferFunction 
						= TransferFunctionTanh(),
					  const SynapseFunction &defaultSynapseFunction 
						= SimpleSynapseFunction());
		ModularNeuralNetwork(const ModularNeuralNetwork &other);
		virtual ~ModularNeuralNetwork();

		virtual NeuralNetwork* createCopy();

		virtual void reset();

		virtual bool equals(NeuralNetwork *network);

		virtual bool addNeuron(Neuron *neuron);
		virtual bool removeNeuron(Neuron *neuron);

		bool addNeuronGroup(NeuronGroup *group);
		bool removeNeuronGroup(NeuronGroup *group);
		NeuronGroup* getNeuronGroup(const QString &name) const;
		NeuronGroup* getDefaultNeuronGroup() const;
		QList<NeuronGroup*> getNeuronGroups() const;
		QList<NeuroModule*> getNeuroModules() const;
		NeuroModule* getSubModuleOwner(NeuroModule *module) const;

		virtual QList<NeuralNetworkElement*> savelyRemoveNetworkElement(NeuralNetworkElement *element);

		NeuroModule* getOwnerModule(Neuron *neuron) const;
		QList<NeuroModule*> getRootModules() const;
		QList<Neuron*> getRootNeurons() const;

		virtual bool notifyMemberIdsChanged(QHash<qulonglong, qulonglong> changedIds);
		
		virtual void getNetworkElements(QList<NeuralNetworkElement*> &elementList) const;

		virtual void adjustIdCounter();
		virtual void freeElements(bool destroyElements);

		static NeuronGroup* selectNeuronGroupById(qulonglong id, QList<NeuronGroup*> groups);
		static NeuroModule* selectNeuroModuleById(qulonglong id, QList<NeuroModule*> modules);
		static GroupConstraint* selectConstraintById(qulonglong id, QList<GroupConstraint*> constraints);
		
	private:
		QList<NeuronGroup*> mNeuronGroups;
		NeuronGroup *mDefaultNeuronGroup;
	};

}

#endif


