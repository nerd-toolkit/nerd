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

#ifndef NERDCreateNetworkOperator_H
#define NERDCreateNetworkOperator_H

#include "NeuralNetworkManipulationChain/NeuralNetworkManipulationOperator.h"
#include "Value/IntValue.h"
#include "Value/StringValue.h"
#include "Value/FileNameValue.h"
#include "Network/NeuralNetwork.h"
#include "Value/BoolValue.h"
#include "Event/Event.h"
#include "Event/EventListener.h"

namespace nerd {

	/**
	 * CreateNetworkOperator.
	 */
	class CreateNetworkOperator : public NeuralNetworkManipulationOperator, public virtual EventListener {
	public:
		CreateNetworkOperator(const QString &name);
		CreateNetworkOperator(const CreateNetworkOperator &other);
		virtual ~CreateNetworkOperator();

		virtual NeuralNetworkManipulationOperator* createCopy() const;

		virtual bool applyOperator(Individual *individual, CommandExecutor *executor = 0);

		virtual QString getName() const;
		virtual void eventOccured(Event *event);
		
	private:
		NeuralNetwork* loadNetwork(const QString fileName);

	private:
		Event *mEvolutionRestartedEvent;
		IntValue *mNumberOfInputNeurons;
		IntValue *mNumberOfOutputNeurons;
		StringValue *mTransferFunctionName;
		StringValue *mActivationFunctionName;
		StringValue *mSynapseFunctionName;
		FileNameValue *mInitialNetworkXML;
		BoolValue *mPreserveInitialNetworks;
		QList<NeuralNetwork*> mInitialNetworks;
		int mNetworkIndex;
		bool mFirstNetworkCycle;
	};

}

#endif


