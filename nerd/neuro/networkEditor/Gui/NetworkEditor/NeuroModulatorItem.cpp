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



#include "NeuroModulatorItem.h"
#include "NetworkVisualization.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"
#include "Util/NetworkEditorUtil.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NeuroModulatorItem.
 */
NeuroModulatorItem::NeuroModulatorItem(NetworkVisualization *owner)
	: PaintItem(0), mOwner(owner), mShowNeuroModulators(true)
{
}


NeuroModulatorItem::NeuroModulatorItem(const NeuroModulatorItem &other) 
	: PaintItem(other), mOwner(other.mOwner), mShowNeuroModulators(other.mShowNeuroModulators)
{

}

/**
 * Destructor.
 */
NeuroModulatorItem::~NeuroModulatorItem() {
}


NeuralNetworkElement* NeuroModulatorItem::getNetworkElement() const {
	return 0;
}


bool NeuroModulatorItem::isHit(const QPointF &point, Qt::MouseButtons mouseButton, double scaling) {
	return false;
}


void NeuroModulatorItem::mouseMoved(const QPointF &distance, Qt::MouseButtons mouseButton) {
}


void NeuroModulatorItem::paintSelf(QPainter *painter) {
	if(!mShowNeuroModulators || painter == 0) {
		return;
	}
}


void NeuroModulatorItem::setViewMode(int mode, bool enabled) {
	if(mode == PaintItem::SHOW_NEURO_MODULATORS) {
		mShowNeuroModulators = enabled;
	}
	else {
		PaintItem::setViewMode(mode, enabled);
	}
}

bool NeuroModulatorItem::isViewModeEnabled(int mode) {
	if(mode == PaintItem::SHOW_NEURO_MODULATORS) {
		return mShowNeuroModulators;
	}
	else {
		return PaintItem::isViewModeEnabled(mode);
	}
}

Properties* NeuroModulatorItem::getEncapsulatedProperties() const {
	return 0;
}

}




