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



#ifndef NERDNetworkSimulationRecorder_H
#define NERDNetworkSimulationRecorder_H

#include <QString>
#include <QFile>
#include <QHash>
#include <QTextStream>
#include "Value/InterfaceValue.h"
#include "Physics/SimObjectGroup.h"
#include "Core/SystemObject.h"
#include "Event/EventListener.h"
#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include "Value/FileNameValue.h"
#include "Value/BoolValue.h"
#include "Gui/SimulationRecorder/SimulationRecorder.h"
#include "Network/NeuralNetworkManager.h"

namespace nerd {

	/**
	 * NetworkSimulationRecorder
	 */
	class NetworkSimulationRecorder : public SimulationRecorder
	{
	public:
		NetworkSimulationRecorder();
		virtual ~NetworkSimulationRecorder();

		virtual QString getName() const;
		
		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		
		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value) ;
		
		virtual bool startPlayback();
		virtual bool stopPlayback();
		
	protected:
		virtual void updateListOfRecordedValues();
		virtual void syncWithListOfRecordedValues();
		virtual void updateRecordedData(QDataStream &dataStream);
		virtual void updatePlaybackData(QDataStream &dataStream);
		
	private:
		NeuralNetwork* getNetworkByIdString(QList<NeuralNetwork*> networks, const QString &netId);
		QList<Neuron*> getNeurons(NeuralNetwork *net, const QString &neuronIds);
		QList<Synapse*> getSynapses(NeuralNetwork *net, const QString &synapseIds);
		
	protected:
		Event *mNetworksReplacedEvent;
		Event *mNetworkStructureChangedEvent;
		
		BoolValue *mDisablePlasticityValue;
		BoolValue *mDisableNetworkUpdateValue;
		bool mPlasticityWasDisabled;
		bool mNetworkUpdateWasDisabled;
		bool mNetworksChanged;
		
		BoolValue *mIncludeNetworks;
		
		NeuralNetworkManager *mNetworkManager;
		QList<Neuron*> mNeurons;
		QList<Synapse*> mSynapses;
	};

}

#endif


