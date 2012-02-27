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


#ifndef NERDNeuron_H
#define NERDNeuron_H

#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include "Network/Synapse.h"
#include <QList>
#include <QString>
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"

namespace nerd {

	class NeuralNetwork;


	/**
	 * Neuron
	 */
	class Neuron : public SynapseTarget
	{
	public:
		static const QString NEURON_TYPE_INPUT;
		static const QString NEURON_TYPE_OUTPUT;

	public:
		Neuron(const QString &name, const TransferFunction &tf, const ActivationFunction &af, qulonglong id = 0);
		Neuron(const Neuron &other);
		virtual ~Neuron();

		virtual Neuron* createCopy() const;

		virtual void prepare();
		virtual double getLastActivation() const;
		virtual double getLastOutputActivation() const;
		virtual void reset();
		virtual void updateActivation();

		virtual qulonglong getId() const;	
		virtual void setId(qulonglong id);
		StringValue& getNameValue();
		DoubleValue& getActivationValue();
		DoubleValue& getOutputActivationValue();
		DoubleValue& getBiasValue();

		bool addInputValue(DoubleValue *input);
		bool removeInputValue(DoubleValue *input);
		QList<DoubleValue*> getInputValues() const;

		void setTransferFunction(const TransferFunction &tf);
		void setTransferFunction(TransferFunction *tf);
		TransferFunction* getTransferFunction() const;
		void setActivationFunction(const ActivationFunction &af);
		void setActivationFunction(ActivationFunction *af);
		ActivationFunction* getActivationFunction() const;

		
		virtual bool addSynapse(Synapse *synapse);
		virtual bool removeSynapse(Synapse *synapse);
		virtual QList<Synapse*> getSynapses() const;
		virtual bool registerOutgoingSynapse(Synapse *synapse);
		virtual bool deregisterOutgoingSynapse(Synapse *synapse);
		virtual QList<Synapse*> getOutgoingSynapses() const;
		
		virtual void setOwnerNetwork(NeuralNetwork *network);
		virtual NeuralNetwork* getOwnerNetwork() const;

		virtual bool equals(Neuron *neuron);

		void flipActivation(bool flip);
		bool isActivationFlipped() const;

		bool requiresUpdate(int iteration);

		virtual void propertyChanged(Properties *owner, const QString &property);

	private:	
		qulonglong mId;	
		StringValue mNameValue;
		DoubleValue mActivationValue;
		DoubleValue mOutputActivationValue;
		DoubleValue mBiasValue;
		QList<Synapse*> mIncommingSynapses;
		QList<Synapse*> mOutgoingSynapses;
		QList<DoubleValue*> mInputValues;
		TransferFunction *mTransferFunction;
		ActivationFunction *mActivationFunction;
		double mLastActivation;
		double mLastOutputActivation;
		bool mActivationCalculated;
		bool mFlipActivation;
		NeuralNetwork *mOwnerNetwork;

	};

}

#endif


