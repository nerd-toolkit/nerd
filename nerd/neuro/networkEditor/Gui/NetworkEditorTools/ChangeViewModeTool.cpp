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


#include "ChangeViewModeTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include <QKeySequence>
#include <QMenu>

using namespace std;

namespace nerd {


/**
 * Constructs a new ChangeViewModeTool.
 */
ChangeViewModeTool::ChangeViewModeTool(NeuralNetworkEditor *owner, int mode, 
					const QString &message, Qt::Key pressedKey, bool reverseActivation,
					bool initiallySelect)
	: QObject(owner), mOwner(owner), mModeActive(false), mMode(mode), mStatusMessage(message),
	  mPressedKey(pressedKey), mReverseActivation(reverseActivation), mMenuAction(0),
	  mKeyPressed(false)
{	
	if(mOwner != 0) {
		QMenu *viewMenu = mOwner->getViewModeMenu();
		if(mode == MENU_SEPARATOR) {
			if(viewMenu != 0) {
				viewMenu->addSeparator();
			}
		}
		else {
			connect(mOwner, SIGNAL(tabSelectionChanged(int)),
					this, SLOT(updateKeyListenerRegistration(int)));
			
			if(viewMenu != 0) {
					mMenuAction = viewMenu->addAction(message + " (" 
									+ QKeySequence(Qt::SHIFT + pressedKey).toString() + ")");
					mMenuAction->setCheckable(true);
					if(initiallySelect) {
						mMenuAction->setChecked(true);
					}
					connect(mMenuAction, SIGNAL(triggered(bool)),
							this, SLOT(menuActionTriggered(bool)));
			}
			updateKeyListenerRegistration(0);
		}
	}
}



/**
 * Destructor.
 */
ChangeViewModeTool::~ChangeViewModeTool() {
}

void ChangeViewModeTool::keyPressed(QKeyEvent *event) {
	if(mMenuAction == 0 || mKeyPressed) {
		return;
	}
	if(event->key() != mPressedKey || !(event->modifiers() & Qt::ShiftModifier)) {
		return;
	}
	
	mKeyPressed = true;

	if(mMenuAction->isChecked()) {
		disableViewMode();
		mMenuAction->setChecked(false);
	}
	else {
		enableViewMode();
		mMenuAction->setChecked(true);
	}
}


void ChangeViewModeTool::keyReleased(QKeyEvent *event) {
	if(mMenuAction == 0) {
		return;
	}
	if(event->key() == mPressedKey) {
		mKeyPressed = false;
	}
}

void ChangeViewModeTool::enableViewMode() {
	if(mOwner == 0 || mMenuAction == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();

	if(visu == 0) {
		return;
	}

	SimpleNetworkVisualizationHandler *handler = 	
			dynamic_cast<SimpleNetworkVisualizationHandler*>(visu->getVisualizationHandler());

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


void ChangeViewModeTool::disableViewMode() {
	if(mOwner == 0 || mMenuAction == 0) {
			return;
		}
		NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	
		if(visu == 0) {
			return;
		}

		SimpleNetworkVisualizationHandler *handler = 	
				dynamic_cast<SimpleNetworkVisualizationHandler*>(visu->getVisualizationHandler());
	
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


void ChangeViewModeTool::updateKeyListenerRegistration(int) {
	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->addKeyListener(this);
	}
}

void ChangeViewModeTool::menuActionTriggered(bool checked) {
	if(mMenuAction->isChecked()) {
		enableViewMode();
	}
	else {
		disableViewMode();
	}
}


}



