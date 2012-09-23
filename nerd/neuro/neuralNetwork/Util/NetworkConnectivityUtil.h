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



#ifndef NERDNetworkConnectivityUtil_H
#define NERDNetworkConnectivityUtil_H

#include <QString>
#include <QHash>
#include "Network/NeuralNetwork.h"
#include "Network/NeuralNetworkElement.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "Network/Synapse.h"
#include <QPointF>
#include <QSizeF>

namespace nerd {
	
	struct SynapseSet {
		QList<Synapse*> mSynapses;
		QList<Neuron*> mSources;
		QList<SynapseTarget*> mTargets;
		QList<Vector3D> mPositions;
	};
	
	/**
	 * NetworkConnectivityUtil.
	 *
	 */
	class NetworkConnectivityUtil {
	public:
		
		static const int MODUS_UNIDIRECTIONAL = 1;
		static const int MODUS_BIDIRECTIONAL = 2;
		static const int MODUS_IGNORE_INTERFACES = 4;
		static const int MODUS_IGNORE_MODULE_BOUNDARIES = 8;
		static const int MODUS_IGNORE_PROPERTIES = 16;
		
	public:
// 		static SynapseSet fullyConnectGroups(NeuronGroup *sourceGroup, NeuronGroup *targetGroup, int modus);
		static SynapseSet fullyConnectElements(NeuralNetwork *network, QList<Neuron*> sources, QList<SynapseTarget*> targets, int modus);
// 		static SynapseSet fullyConnectGroup(NeuronGroup *group, int modus);
		
		static SynapseSet connectElementsUnidirectional(NeuralNetwork *network, QList<Neuron*> sources, 
							QList<SynapseTarget*> targets, int modus, double defaultWeight = 0.0, 
							SynapseFunction *defaultSynapseFunction = 0);
		
		static QList<Neuron*> getValidSourceNeurons(Neuron *target, ModularNeuralNetwork *net);
		static QList<Neuron*> getValidTargetNeurons(Neuron *source, ModularNeuralNetwork *net);
		static int getInterfaceLevel(Neuron *neuron, const QString &moduleInterfaceType);
	};
	
}

#endif


