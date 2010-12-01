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


#ifndef NERDSymmetryConstraint_H
#define NERDSymmetryConstraint_H

#include "Constraints/GroupConstraint.h"
#include "Network/NeuralNetworkElement.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "Value/IntValue.h"
#include "Value/StringValue.h"
#include "Constraints/NetworkElementPair.h"
#include "Value/ULongLongValue.h"
#include "ModularNeuralNetwork/NeuroModule.h"

namespace nerd {

	/**
	 * SymmetryConstraint.
	 */
	class SymmetryConstraint : public GroupConstraint {
	public:
		static const int FLIP_MODE_NORMAL;
		static const int FLIP_MODE_HORIZONTAL;
		static const int FLIP_MODE_VERTICAL;
		static const int CONNECTION_MODE_INPUT;
		static const int CONNECTION_MODE_OUTPUT;
		static const int CONNECTION_MODE_MUTUAL;
		static const int CONNECTION_MODE_INPUT_AS;
		static const int CONNECTION_MODE_OUTPUT_AS;
		static const int CONNECTION_MODE_MUTUAL_AS;
		static const int CONNECTION_MODE_STRUCTURAL;

	public:
		SymmetryConstraint(qulonglong id = 0);
		SymmetryConstraint(const SymmetryConstraint &other);
		virtual ~SymmetryConstraint();

		virtual GroupConstraint* createCopy() const;

		virtual bool attachToGroup(NeuronGroup *group);
		virtual bool detachFromGroup(NeuronGroup *group);

		bool setRequiredElements(QList<NeuralNetworkElement*> elements);

		virtual bool isValid(NeuronGroup *owner);
		virtual bool applyConstraint(NeuronGroup *owner, CommandExecutor *executor,
									 QList<NeuralNetworkElement*> &trashcan);

		virtual bool groupIdsChanged(QHash<qulonglong, qulonglong> changedIds);
		
		virtual bool equals(GroupConstraint *constraint);

		bool getGroupOrigins(Vector3D &origin1, Vector3D &origin2);
		bool setElementPosition(NeuralNetworkElement *element, const Vector3D &origin, const Vector3D &pos);
		bool setModulePositionsRecursively(NeuroModule *module, Vector3D referenceOrigin, 
											Vector3D newOrigin);

		NeuroModule* getMatchingModule(NeuroModule *module);
		Neuron* getMatchingNeuron(Neuron *neuron);
		Synapse* getMatchingSynapse(Synapse *synapse, int connectionMode);
		
		NeuronGroup* getNeuronOwnerGroup(Neuron *neuron, NeuronGroup *baseGroup);

		QList<Synapse*> getSynapsesOfSynapseTarget(SynapseTarget *target);

		void setSlaveMark(bool set);

		void updateFlipMode();
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
		StringValue *mLayoutValue;
		StringValue *mConnectionModeValue;
		QList<NetworkElementPair> mNetworkElementPairs;
		QList<NetworkElementPair> mModulePairs;
		int mFlipMode;
	};

}

#endif


