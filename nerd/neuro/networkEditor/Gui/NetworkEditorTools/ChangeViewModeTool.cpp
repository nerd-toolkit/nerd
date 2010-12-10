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


#include "ChangeViewModeTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Gui/NetworkEditor/GroupItem.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new ChangeViewModeTool.
 */
ChangeViewModeTool::ChangeViewModeTool(NeuralNetworkEditor *owner, int mode, 
					const QString &message, Qt::Key pressedKey, bool reverseActivation)
	: QObject(owner), mOwner(owner), mModeActive(false), mMode(mode), mStatusMessage(message),
	  mPressedKey(pressedKey), mReverseActivation(reverseActivation)
{
	if(mOwner != 0) {
		connect(mOwner, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(updateKeyListenerRegistration(int)));
	}
	updateKeyListenerRegistration(0);
}



/**
 * Destructor.
 */
ChangeViewModeTool::~ChangeViewModeTool() {
}

void ChangeViewModeTool::keyPressed(QKeyEvent *event) {
	if(mModeActive) {
		return;
	}
	if(event->key() != mPressedKey || !(event->modifiers() & Qt::ShiftModifier)) {
		return;
	}

	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();

	if(visu == 0) {
		return;
	}

	SimpleNetworkVisualizationHandler *handler = 	
			dynamic_cast<SimpleNetworkVisualizationHandler*>(visu->getVisualizationHander());

	if(handler == 0) {
		return;
	}

	QList<PaintItem*> items = handler->getPaintItems();
	for(QListIterator<PaintItem*> i(items); i.hasNext();) {
		i.next()->setViewMode(mMode, !mReverseActivation);
	}
	handler->getNeuronItemPrototype()->setViewMode(mMode, !mReverseActivation);
	handler->getSynapseItemPrototype()->setViewMode(mMode, !mReverseActivation);
	handler->getModuleItemPrototype()->setViewMode(mMode, !mReverseActivation);
	handler->getGroupItemPrototype()->setViewMode(mMode, !mReverseActivation);
	mModeActive = true;
	
	visu->addStatusMessage(mStatusMessage);
}


void ChangeViewModeTool::keyReleased(QKeyEvent *event) {

	if(!mModeActive) {
		return;
	}
	if(event->key() == mPressedKey) {

		if(mOwner == 0) {
			return;
		}
		NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	
		if(visu == 0) {
			return;
		}

		SimpleNetworkVisualizationHandler *handler = 	
				dynamic_cast<SimpleNetworkVisualizationHandler*>(visu->getVisualizationHander());
	
		if(handler == 0) {
			return;
		}
	
		QList<PaintItem*> items = handler->getPaintItems();
		for(QListIterator<PaintItem*> i(items); i.hasNext();) {
			i.next()->setViewMode(mMode, mReverseActivation);
		}
		handler->getNeuronItemPrototype()->setViewMode(mMode, mReverseActivation);
		handler->getSynapseItemPrototype()->setViewMode(mMode, mReverseActivation);
		handler->getModuleItemPrototype()->setViewMode(mMode, mReverseActivation);
		handler->getGroupItemPrototype()->setViewMode(mMode, mReverseActivation);
		mModeActive = false;

		visu->removeStatusMessage(mStatusMessage);
	}
}


void ChangeViewModeTool::updateKeyListenerRegistration(int) {
	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->addKeyListener(this);
	}
}


}



