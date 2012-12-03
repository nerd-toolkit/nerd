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


#ifndef NERDNeuralNetworkManager_H
#define NERDNeuralNetworkManager_H

#include "Event/EventListener.h"
#include "Core/SystemObject.h"
#include "Event/Event.h"
#include "TransferFunction/TransferFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include "SynapseFunction/SynapseFunction.h"
#include "Network/NeuralNetwork.h"
#include <QMutex>
#include "Value/IntValue.h"

namespace nerd {

	/**
	 * NeuralNetworkManager.
	 */
	class NeuralNetworkManager : public virtual SystemObject, public virtual EventListener,
								 public virtual ValueChangedListener 
	{
	public:
		NeuralNetworkManager();
		virtual ~NeuralNetworkManager();

		virtual QString getName() const;

		virtual bool registerAsGlobalObject();
		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		virtual bool addNeuralNetwork(NeuralNetwork *neuralNetwork);
		virtual bool removeNeuralNetwork(NeuralNetwork *neuralNetwork);
		virtual QList<NeuralNetwork*> getNeuralNetworks() const;
		virtual void destroyNeuralNetworks();

		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value);

		void triggerCurrentNetworksReplacedEvent();
		void triggerNetworkStructureChangedEvent();	
		void triggerNetworkParametersChangedEvent();
		void triggerNetworkIterationCompleted();

		bool addTransferFunctionPrototype(const TransferFunction &tf);
		bool addActivationFunctionPrototype(const ActivationFunction &af);
		bool addSynapseFunctionPrototype(const SynapseFunction &sf);

		QList<TransferFunction*> getTransferFunctionPrototypes() const;
		QList<ActivationFunction*> getActivationFunctionPrototypes() const;
		QList<SynapseFunction*> getSynapseFunctionPrototypes() const;

		BoolValue* getBypassNetworksValue() const;
		BoolValue* getDisablePlasticityValue() const;

		QMutex* getNetworkExecutionMutex();
		
		Event* getResetEvent() const;
		Event* getResetNetworksEvent() const;
		Event* getIterationCompletedEvent() const;
		
	protected:
		virtual void executeNeuralNetworks();
		virtual void resetNeuralNetworks();

	private:
		Event *mStepStartedEvent;
		Event *mStepCompletedEvent;
		Event *mResetEvent;
		Event *mResetNetworksEvent;
	
		Event *mCurrentNetworksReplacedEvent;
		Event *mNetworkEvaluationStarted;
		Event *mNetworkEvaluationCompleted;
		Event *mNetworkStructuresChanged;
		Event *mNetworkParametersChanged;
		Event *mNetworkIterationCompleted;

		QList<NeuralNetwork*> mNeuralNetworks;

		QList<TransferFunction*> mTransferFunctionPrototypes;
		QList<ActivationFunction*> mActivationFunctionPrototypes;
		QList<SynapseFunction*>	mSynapseFunctionPrototypes;

		QMutex mNetworkExecutionMutex;
		BoolValue *mBypassNetworkValue;
		IntValue *mNumberOfNetworkUpdatesPerStep;
		BoolValue *mDisablePlasticity;

	};

}

#endif


