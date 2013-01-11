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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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


#ifndef NERDInsertSynapseCommand_H
#define NERDInsertSynapseCommand_H

#include <QString>
#include <QHash>
#include "Command/Command.h"
#include "Network/Neuron.h"
#include <QPointF>
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Network/Neuron.h"
#include "Network/SynapseTarget.h"

namespace nerd {

	/**
	 * InsertSynapseCommand.
	 * 
	 * Adds one of more synapses to the network. 
	 * The source and target for each synapse have to be given.
	 * Positions are optional. If no position is given, then the default position (middle between
	 * source and target) is used.
	 *
	 */
	class InsertSynapseCommand : public Command {
	public:
		InsertSynapseCommand(NetworkVisualization *context, Synapse *synapse, 
						Neuron *sourceNeuron, SynapseTarget *target, QPointF position);
		InsertSynapseCommand(NetworkVisualization *context, QList<Synapse*> synapses, 
							 QList<Neuron*> sourceNeurons, QList<SynapseTarget*> targets, QList<Vector3D> positions = QList<Vector3D>());
		virtual ~InsertSynapseCommand();

		virtual bool isUndoable() const;

		virtual bool doCommand();
		virtual bool undoCommand();

	private:
		NetworkVisualization *mVisualizationContext;	
		QList<Synapse*> mNewSynapses;
		QList<Neuron*> mSourceNeurons;
		QList<SynapseTarget*> mTargets;
		QList<Vector3D> mInsertPositions;
	};

}

#endif




