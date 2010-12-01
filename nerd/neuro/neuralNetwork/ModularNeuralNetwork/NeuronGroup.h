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


#ifndef NERDNeuronGroup_H
#define NERDNeuronGroup_H

#include "Network/Neuron.h"
#include <QList>
#include "Core/ParameterizedObject.h"
#include "Network/NeuralNetworkElement.h"

namespace nerd {

	class ModularNeuralNetwork;
	class GroupConstraint;
	class NeuroModule;

	/**
	 * NeuronGroup.
	 */
	class NeuronGroup : public ParameterizedObject,
						public NeuralNetworkElement 
	{
	public:
		NeuronGroup(const QString &name, qulonglong id = 0);
		NeuronGroup(const NeuronGroup &other);
		virtual ~NeuronGroup();

		virtual NeuronGroup* createCopy() const;

		virtual qulonglong getId() const;	
		virtual void setId(qulonglong id);

		virtual bool setOwnerNetwork(ModularNeuralNetwork *network);
		virtual ModularNeuralNetwork* getOwnerNetwork() const;

		virtual bool addNeuron(Neuron *neuron);
		virtual bool removeNeuron(Neuron *neuron);
		virtual QList<Neuron*> getNeurons() const;
		virtual void removeAllNeurons();

		virtual bool addSubModule(NeuroModule *module);
		virtual bool removeSubModule(NeuroModule *module);
		virtual void removeAllSubModules();
		virtual QList<NeuroModule*> getSubModules() const;

		virtual QList<Neuron*> getAllEnclosedNeurons() const;
		virtual QList<NeuroModule*> getAllEnclosedModules() const;

		virtual bool addConstraint(GroupConstraint *constraint);
		virtual bool removeConstraint(GroupConstraint *constraint);
		virtual void removeAllConstraints();
		virtual QList<GroupConstraint*> getConstraints() const;

		virtual bool notifyMemberIdsChanged(QHash<qulonglong, qulonglong> changedIds);

		virtual bool moveGroupToNetworkConst(ModularNeuralNetwork const *oldNetwork, 
										ModularNeuralNetwork *newNetwork);
		virtual bool moveGroupToNetwork(ModularNeuralNetwork *oldNetwork, 
										ModularNeuralNetwork *newNetwork);
		virtual bool verifySubModulePointers(ModularNeuralNetwork *net, QList<NeuroModule*> availableModules);

		virtual bool equals(NeuronGroup *group);
		
	protected:
		qulonglong mId;
		QList<Neuron*> mMemberNeurons;
		QList<GroupConstraint*> mConstraints;
		ModularNeuralNetwork *mOwnerNetwork;
		QList<NeuroModule*> mSubModules;
		
	};

}

#endif


