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



#ifndef NERDNeuralNetworkUtil_H
#define NERDNeuralNetworkUtil_H

#include <QString>
#include <QHash>
#include "Network/NeuralNetworkElement.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include <QPointF>
#include <QSizeF>

namespace nerd {

	class Synapse;

	/**
	 * NeuralNetworkUtil.
	 *
	 */
	class NeuralNetworkUtil {
	public:
		static void setNetworkElementLocationProperty(NeuralNetworkElement *elem, 
													  double x, double y, double z);
		static void setNetworkElementLocationProperty(NeuralNetworkElement *elem, const QPointF &pos);
		static void setNeuroModuleLocationSizeProperty(NeuroModule *module, double width, double height);
		static void setNeuroModuleLocationSizeProperty(NeuroModule *module, const QSizeF &size);

		static QPointF getPosition(const QString &locationProperty, bool *ok = 0);
		static QPointF getPosition(const NeuralNetworkElement *element);
		static QSizeF getModuleSize(const NeuroModule *module, bool *ok = 0);

		static QList<Neuron*> getConnectedNeurons(Synapse *synapse);
		static QList<Synapse*> getDependentSynapses(Synapse *synapse);
	private:
	};

}

#endif


