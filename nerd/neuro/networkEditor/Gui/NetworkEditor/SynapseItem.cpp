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



#include "SynapseItem.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/PaintItem.h"
#include <QPainterPath>
#include <math.h>
#include "Math/Math.h"
#include "Util/NetworkEditorUtil.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SynapseItem.
 */
SynapseItem::SynapseItem(NetworkVisualization *owner)
	: PaintItem(800), mOwner(owner)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the SynapseItem object to copy.
 */
SynapseItem::SynapseItem(const SynapseItem &other) 
	: PaintItem(other), mOwner(other.mOwner), mSynapse(0)
{
}

/**
 * Destructor.
 */
SynapseItem::~SynapseItem() {
}



bool SynapseItem::setSynapse(Synapse *synapse) {
	mSynapse = synapse;

	if(synapse == 0) {
		mSourceNeuron = 0;
		mTarget = 0;
	}
	else {
		if(mOwner != 0 && mOwner->getVisualizationHander() != 0) {
			NetworkVisualizationHandler *handler = mOwner->getVisualizationHander();
			mSourceNeuron = handler->getNeuronItem(synapse->getSource());
			if(dynamic_cast<Neuron*>(synapse->getTarget()) != 0) {
				mTarget = handler->getNeuronItem(dynamic_cast<Neuron*>(synapse->getTarget()));
			}
			else {
				mTarget = handler->getSynapseItem(dynamic_cast<Synapse*>(synapse->getTarget()));
			}

			NetworkEditorUtil::setPaintItemLocation(this, 
					synapse->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
		}
	}
	if(mSynapse == 0 || mSourceNeuron == 0 || mTarget == 0  ) {
		return false;
	}
	return true;
}


Synapse* SynapseItem::getSynapse() const {
	return mSynapse;
}


void SynapseItem::paintSelf(QPainter *painter) {
	if(mHidden || !mActive || painter == 0 || mSynapse == 0) {
		return;
	}
}

Properties* SynapseItem::getEncapsulatedProperties() const {
	return mSynapse;
}


}




