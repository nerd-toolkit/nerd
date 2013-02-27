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



#include "NeuronItem.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"
#include "Util/NetworkEditorUtil.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NeuronItem.
 */
NeuronItem::NeuronItem(NetworkVisualization *owner)
	: PaintItem(1000), mOwner(owner), mNeuron(0), 
	  mShowName(true), mShowInputOutputMarker(false), mShowBias(false),
	  mShowActivationFlipped(false), mShowBiasAsString(false),
	  mShowModuleInputOutputMarker(true)
{
}


NeuronItem::NeuronItem(const NeuronItem &other) 
	: PaintItem(other), mOwner(other.mOwner), mNeuron(0), 
	  mShowName(other.mShowName), mShowInputOutputMarker(other.mShowInputOutputMarker),
	  mShowBias(other.mShowBias), mShowActivationFlipped(other.mShowActivationFlipped),
	  mShowBiasAsString(other.mShowBiasAsString),
	  mShowModuleInputOutputMarker(other.mShowModuleInputOutputMarker)
{

}

/**
 * Destructor.
 */
NeuronItem::~NeuronItem() {
}


NeuralNetworkElement* NeuronItem::getNetworkElement() const {
	return mNeuron;
}

bool NeuronItem::setNeuron(Neuron *neuron) {
	mNeuron = neuron;
	if(neuron != 0) {
		NetworkEditorUtil::setPaintItemLocation(this, 
				neuron->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
	}
	return true;
}


Neuron* NeuronItem::getNeuron() const {
	return mNeuron;
}



void NeuronItem::paintSelf(QPainter *painter) {
	if(mHidden || !mActive || painter == 0) {
		return;
	}
}


void NeuronItem::setViewMode(int mode, bool enabled) {
	if(mode == PaintItem::SHOW_NEURON_INPUT_OUTPUT) {
		mShowInputOutputMarker = enabled;
	}
	else if(mode == PaintItem::SHOW_NEURON_NAME) {
		mShowName = enabled;
	}
	else if(mode == PaintItem::SHOW_NEURON_BIAS) {
		mShowBias = enabled;
	}
	else if(mode == PaintItem::SHOW_NEURON_ACTIVATION_FLIPPED) {
		mShowActivationFlipped = enabled;
	}
	else if(mode == PaintItem::SHOW_NEURON_BIAS_AS_NUMBER) {
		mShowBiasAsString = enabled;
	}
	else if(mode == PaintItem::SHOW_NEURON_MODULE_INPUT_OUTPUT) {
		mShowModuleInputOutputMarker = enabled;
	}
	else {
		PaintItem::setViewMode(mode, enabled);
	}
}

bool NeuronItem::isViewModeEnabled(int mode) {
	if(mode == PaintItem::SHOW_NEURON_INPUT_OUTPUT) {
		return mShowInputOutputMarker;
	}
	else if(mode == PaintItem::SHOW_NEURON_NAME) {
		return mShowName;
	}
	else if(mode == PaintItem::SHOW_NEURON_BIAS) {
		return mShowBias;
	}
	else if(mode == PaintItem::SHOW_NEURON_ACTIVATION_FLIPPED) {
		return mShowActivationFlipped;
	}
	else if(mode == PaintItem::SHOW_NEURON_BIAS_AS_NUMBER) {
		return mShowBiasAsString;
	}
	else if(mode == PaintItem::SHOW_NEURON_MODULE_INPUT_OUTPUT) {
		return mShowModuleInputOutputMarker;
	}
	else {
		return PaintItem::isViewModeEnabled(mode);
	}
}

Properties* NeuronItem::getEncapsulatedProperties() const {
	return mNeuron;
}

}




