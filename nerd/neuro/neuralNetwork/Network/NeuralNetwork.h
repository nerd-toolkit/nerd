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


#ifndef NERDNeuralNetwork_H
#define NERDNeuralNetwork_H

#include "Network/Neuron.h"
#include <QList>
#include <QHash>
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "SynapseFunction/SynapseFunction.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "Network/SynapseTarget.h"
#include "Control/Controller.h"
#include "Value/InterfaceValue.h"
#include "Core/Object.h"
#include "Core/Properties.h"
#include "Core/PropertyChangedListener.h"

namespace nerd {

	class ControlInterface;

	class NeuronInterfaceValuePair {
	public:
		NeuronInterfaceValuePair(Neuron *n, InterfaceValue *v) 
			: mInterfaceValue(v), mNeuron(n) {}

		InterfaceValue* mInterfaceValue;
		Neuron *mNeuron;
	};

	/**
	 * NeuralNetwork
	 */
	class NeuralNetwork : public Properties, public virtual Controller, public virtual Object,
						  public PropertyChangedListener
	{
	public:
		NeuralNetwork(const ActivationFunction &defaultActivationFunction 
						= AdditiveTimeDiscreteActivationFunction(),
					  const TransferFunction &defaultTransferFunction 
						= TransferFunctionTanh(),
					  const SynapseFunction &defaultSynapseFunction 
						= SimpleSynapseFunction());

		NeuralNetwork(const NeuralNetwork &other);
		virtual ~NeuralNetwork();

		virtual NeuralNetwork* createCopy();

		virtual QString getName() const;

		virtual void setControlInterface(ControlInterface *controlInterface);
		virtual ControlInterface* getControlInterface() const;

		virtual void executeStep();
		virtual void reset();

		virtual bool addNeuron(Neuron *neuron);
		virtual bool removeNeuron(Neuron *neuron);
		virtual QList<Neuron*> getNeurons() const;
		virtual QList<Neuron*> getInputNeurons() const;
		virtual QList<Neuron*> getOutputNeurons() const;

		virtual void propertyChanged(Properties *owner, const QString &property);
	
		virtual QList<Synapse*> getSynapses() const;
		virtual void getSynapses(const Synapse *synapse, QList<Synapse*> &synapses) const;

		virtual void getNetworkElements(QList<NeuralNetworkElement*> &elementList) const;
		virtual QList<NeuralNetworkElement*> savelyRemoveNetworkElement(NeuralNetworkElement *element);

		void setDefaultTransferFunction(const TransferFunction &tf);
		TransferFunction* getDefaultTransferFunction() const;
		void setDefaultActivationFunction(const ActivationFunction &af);
		ActivationFunction* getDefaultActivationFunction() const;
		void setDefaultSynapseFunction(const SynapseFunction &sf);
		SynapseFunction* getDefaultSynapseFunction() const;
		
		void validateSynapseConnections();

		virtual bool equals(NeuralNetwork *network);

		virtual void adjustIdCounter();
		virtual void freeElements(bool destroyElements);

		void bypassNetwork(bool bypass);
		bool isBypassingNetwork() const;

		virtual int getHighestRequiredIterationNumber() const;
		virtual QList<Neuron*> getNeuronsWithIterationRequirement(int requirement);
		virtual int getMinimalStartIteration() const;
		

		Neuron* getNeuronById(qulonglong id) const;
		void synchronizeNeuronIds();

		static qulonglong generateNextId();
		static void resetIdCounter(qulonglong currentId = 0);
// 		static void renewIds(QList<SynapseTarget*> objects);

		static Neuron* selectNeuronById(qulonglong id, QList<Neuron*> neurons);
		static SynapseTarget* selectSynapseTargetById(qulonglong id, QList<SynapseTarget*> targets);
		static Synapse* selectSynapseById(qulonglong id, QList<Synapse*> synapses);
		static NeuralNetworkElement* selectNetworkElementById(qulonglong id, 
							QList<NeuralNetworkElement*> elements);

		

	protected:
		QList<NeuronInterfaceValuePair> getInputPairs() const;
		QList<NeuronInterfaceValuePair> getOutputPairs() const;
		
		
	protected:
		QList<Neuron*> mNeurons;
		QHash<qulonglong, Neuron*> mNeuronsById;

	private:		
		static qulonglong mIdPool;
		
		ActivationFunction *mDefaultActivationFunction;
		TransferFunction *mDefaultTransferFunction;
		SynapseFunction *mDefaultSynapseFunction;
		
		ControlInterface *mControlInterface;
		QList<NeuronInterfaceValuePair> mInputPairs;
		QList<NeuronInterfaceValuePair> mOutputPairs;
		QList<Neuron*> mInputNeurons;
		QList<Neuron*> mOutputNeurons;
		QList<Neuron*> mProcessibleNeurons;
		bool mBypassNetwork;
		int mMinimalIterationNumber;
	};

}

#endif



