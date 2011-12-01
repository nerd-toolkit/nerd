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



#ifndef NERDConnectionSymmetryConstraint_H
#define NERDConnectionSymmetryConstraint_H

#include <QString>
#include <QHash>
#include "Constraints/GroupConstraint.h"
#include "Network/NeuralNetworkElement.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "Value/IntValue.h"
#include "Value/StringValue.h"
#include "Constraints/NetworkElementPair.h"
#include "Value/ULongLongValue.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Network/NeuralNetwork.h"

namespace nerd {

	/**
	 * ConnectionSymmetryConstraint.
	 *
	 */
	class ConnectionSymmetryConstraint : public GroupConstraint {
	public:
		static const int CONNECTION_MODE_NORMAL;
		static const int CONNECTION_MODE_INPUT;
		static const int CONNECTION_MODE_OUTPUT;
		static const int CONNECTION_MODE_MUTUAL;
		static const int CONNECTION_MODE_ASYNC;
		static const int CONNECTION_MODE_RANDOM;
		static const int CONNECTION_MODE_STRUCTURAL;

	public:
		ConnectionSymmetryConstraint(qulonglong id = 0);
		ConnectionSymmetryConstraint(const ConnectionSymmetryConstraint &other);
		virtual ~ConnectionSymmetryConstraint();

		virtual GroupConstraint* createCopy() const;

		virtual bool attachToGroup(NeuronGroup *group);
		virtual bool detachFromGroup(NeuronGroup *group);
		
		virtual void reset();

		virtual bool isValid(NeuronGroup *owner);
		virtual bool applyConstraint(NeuronGroup *owner, CommandExecutor *executor,
									 QList<NeuralNetworkElement*> &trashcan);

		virtual bool groupIdsChanged(QHash<qulonglong, qulonglong> changedIds);
		
		virtual bool equals(GroupConstraint *constraint);

		Neuron* getMatchingOwnerNeuron(Neuron *neuron);
		Neuron* getMatchingReferenceNeuron(Neuron *neuron);
		Synapse* getMatchingSynapse(Synapse *synapse, int connectionMode);

		QList<Synapse*> getSynapsesOfSynapseTarget(SynapseTarget *target);

		void setSlaveMark(bool set);

		void updateNetworkElementPairValue();
		bool updateNetworkElementPairs(NeuronGroup *owner, NeuronGroup *target);
		QList<NetworkElementPair> getNetworkElementPairs() const;

		StringValue* getNetworkElementPairValue() const;
		ULongLongValue* getTargetGroupIdValue() const;

	private:
		NeuronGroup *mGroup1;
		NeuronGroup *mGroup2;
		ULongLongValue *mTargetGroupId;
		StringValue *mNetworkElementPairValue;
		StringValue *mConnectionModeValue;
		BoolValue *mAutoSelectPairs;
		BoolValue *mIgnoreSlaveElements;
		QList<NetworkElementPair> mNetworkElementPairs;
		QList<NetworkElementPair> mModulePairs;
		bool mFirstIteration;

	};

}

#endif



